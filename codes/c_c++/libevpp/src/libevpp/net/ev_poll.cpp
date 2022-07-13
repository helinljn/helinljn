#include "net/ev_poll.h"
#include "net/ev_socket.h"
#include "base/ev_thread.h"
#include "util-internal.h"
#include "mm-internal.h"

#if defined(__EV_WINDOWS__)
    #include <WinSock2.h>
#elif defined(__EV_LINUX__)
    #include <poll.h>
#else
    #error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)

#include <cstring>

namespace evpp    {
namespace details {

// 默认事件集的大小
static const uint32 g_default_event_set_size = 1024;

/**
 * @brief 创建new_set_size个struct pollfd需要申请的空间大小
 * @param new_set_size 创建的struct pollfd数量
 * @return 返回需要申请的空间大小
 */
static inline uint32 alloc_size_internal(uint32 new_set_size)
{
	return (sizeof(struct pollfd) * new_set_size);
}

} // namespace details

//////////////////////////////////////////////////////////////////////////
// ev_poll
//////////////////////////////////////////////////////////////////////////
ev_poll::ev_poll(void)
	: active_events_()
	, fd_pos_info_()
	, event_set_(static_cast<pollfd*>(mm_calloc(1, details::alloc_size_internal(details::g_default_event_set_size))))
	, max_set_size_(details::g_default_event_set_size)
	, cur_set_size_(0)
	, weakrand_seed_(0)
{
	active_events_.reserve(details::g_default_event_set_size);
	evutil_weakrand_seed_(reinterpret_cast<evutil_weakrand_state*>(&weakrand_seed_), 0);
}

ev_poll::~ev_poll(void)
{
	active_events_.clear();
	fd_pos_info_.clear();

	if (event_set_)
	{
		mm_free(event_set_);
	}

	event_set_     = NULL;
	max_set_size_  = 0;
	cur_set_size_  = 0;
	weakrand_seed_ = 0;
}

bool ev_poll::add_event(socket_t fd, uint32 events)
{
	if (-1 == fd || !(events & (ET_READ | ET_WRITE)))
	{
		return false;
	}

	// 扩容
	if (cur_set_size_ == max_set_size_ && !reset_event_set(max_set_size_ * 2))
	{
		return false;
	}

	fd_pos_container::iterator iter = fd_pos_info_.find(fd);
	uint32                     pos  = 0;
	if (iter == fd_pos_info_.end())
	{
		// 没有位置信息，记录位置
		pos = cur_set_size_++;

		fd_pos_info_.insert(std::pair<socket_t, uint32>(fd, pos));

		// 初始化该位置的pollfd结构信息
		pollfd& rp = event_set_[pos];
		rp.fd      = fd;
		rp.events  = 0;
		rp.revents = 0;
	}
	else
	{
		// 有位置信息，直接使用
		pos = iter->second;
	}

	// 添加事件
	if (events & ET_READ)
	{
		event_set_[pos].events |= POLLIN;
	}

	if (events & ET_WRITE)
	{
		event_set_[pos].events |= POLLOUT;
	}

	return true;
}

bool ev_poll::del_event(socket_t fd, uint32 events)
{
	if (-1 == fd || !(events & (ET_READ | ET_WRITE)))
	{
		return false;
	}

	fd_pos_container::iterator iter = fd_pos_info_.find(fd);
	if (iter == fd_pos_info_.end())
	{
		// 已经删除
		return true;
	}

	// 删除事件
	uint32  pos = iter->second;
	pollfd& rp  = event_set_[pos];
	if (events & ET_READ)
	{
		rp.events &= (~POLLIN);
	}

	if (events & ET_WRITE)
	{
		rp.events &= (~POLLOUT);
	}

	// 删除了某一事件，还剩余有其它事件，所以到此成功
	if (rp.events)
	{
		return true;
	}

	// 所有的事件都被删除了，那么就移除呗
	fd_pos_info_.erase(iter);

	if (--cur_set_size_ != pos)
	{
		event_set_[pos]                  = event_set_[cur_set_size_];
		fd_pos_info_[event_set_[pos].fd] = pos;
	}

	return true;
}

int32 ev_poll::dispatch(int32 millisecond_timeout)
{
	millisecond_timeout = (millisecond_timeout < 0 ? -1 : millisecond_timeout);

	// 每次都先清空结果集
	active_events_.clear();

#if defined(__EV_WINDOWS__)
	// Windows doesn't like you to call WSAPoll() with no sockets.
	if (0 == cur_set_size_)
	{
		this_thread::sleep_for(static_cast<uint32>(-1 == millisecond_timeout ? 0xFFFFFFFF : millisecond_timeout));
		return 0;
	}

	int32 ret = WSAPoll(event_set_, cur_set_size_, millisecond_timeout);
#else
	int32 ret = poll(event_set_, cur_set_size_, millisecond_timeout);
#endif // defined(__EV_WINDOWS__)
	if (0 == ret)
	{
		return 0;
	}

	if (-1 == ret)
	{
		int32 error_code = socket_utils::ev_socket_errno();
#if defined(__EV_WINDOWS__)
		if (WSAEINTR == error_code || EINTR == error_code)
#elif defined(__EV_LINUX__)
		if (EINTR == error_code)
#else
		#error "Unrecognized os platform!"
#endif // defined(__EV_WINDOWS__)
		{
			return 0;
		}
		else
		{
			return -1;
		}
	}

	// 收集结果，使用伪随机数来尽量的保证对套接字处理的公平性
	uint32 revents = 0;
	uint32 result  = 0;
	uint32 ridx    = evutil_weakrand_range_(reinterpret_cast<evutil_weakrand_state*>(&weakrand_seed_), cur_set_size_);

	for (uint32 size = 0; size < cur_set_size_; ++size)
	{
		if (++ridx >= cur_set_size_)
			ridx = 0;

		revents = static_cast<uint32>(event_set_[ridx].revents);
		result  = 0;

		if (0 == revents)
			continue;

		if (revents & (POLLERR | POLLHUP | POLLNVAL))
			revents |= (POLLIN | POLLOUT);

		if (revents & POLLIN)
			result |= ET_READ;

		if (revents & POLLOUT)
			result |= ET_WRITE;

		if (0 == result)
			continue;

		active_events_.push_back(active_event(static_cast<socket_t>(event_set_[ridx].fd), result));
	}

	return ret;
}

bool ev_poll::reset_event_set(uint32 new_set_size)
{
	// 申请所需空间
	pollfd* new_set = static_cast<pollfd*>(mm_calloc(1, details::alloc_size_internal(new_set_size)));
	if (NULL == new_set)
	{
		return false;
	}

	// 计算旧的空间大小，并拷贝数据到新空间，删除旧空间
	if (max_set_size_ != 0 && event_set_ != NULL)
	{
		memcpy(new_set, event_set_, details::alloc_size_internal(max_set_size_));
		mm_free(event_set_);
	}

	// 设置新空间
	event_set_    = new_set;
	max_set_size_ = new_set_size;

	return true;
}

} // namespace evpp