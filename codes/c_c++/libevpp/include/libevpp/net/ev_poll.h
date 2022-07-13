//////////////////////////////////////////////////////////////////////////
// poll封装
//////////////////////////////////////////////////////////////////////////
#ifndef __EV_POLL_H__
#define __EV_POLL_H__

#include "base/ev_types.h"
#include "base/ev_noncopyable.h"

#include <map>
#include <vector>

struct pollfd;

namespace evpp {

//////////////////////////////////////////////////////////////////////////
// ev_poll
//////////////////////////////////////////////////////////////////////////
class ev_poll : public ev_noncopyable
{
public:
	// 事件类型
	enum event_type
	{
		ET_READ  = 1U << 0,  // 读事件
		ET_WRITE = 1U << 1,  // 写事件
	};

	// 活跃事件类型
	typedef std::pair<socket_t, uint32> active_event;
	typedef std::vector<active_event>   active_event_array;
	typedef std::map<socket_t, uint32>  fd_pos_container;

public:
	ev_poll(void);
	~ev_poll(void);

	/**
	 * @brief 添加事件
	 * @param fd     socket
	 * @param events 添加的事件类型，读(ET_READ)、写(ET_WRITE)、读写(ET_READ | ET_WRITE)
	 * @return 成功返回true，失败返回false
	 */
	bool add_event(socket_t fd, uint32 events);

	/**
	 * @brief 删除事件
	 * @param fd     socket
	 * @param events 删除的事件类型，读(ET_READ)、写(ET_WRITE)、读写(ET_READ | ET_WRITE)
	 * @return 成功返回true，失败返回false
	 */
	bool del_event(socket_t fd, uint32 events);

	/**
	 * @brief 派发事件(每次调用，函数内部都会先清空上次的活跃事件数组)
	 * @param millisecond_timeout 毫秒超时时间，如果为-1则代表无限等待
	 * @return 返回值对应含义如下所示：
	 *           ret_value >=  1 : 活跃事件的数量
	 *           ret_value ==  0 : 超时
	 *           ret_value == -1 : 其它错误，调用ev_socket_errno()获取错误码
	 *         无其它情况返回值
	 */
	int32 dispatch(int32 millisecond_timeout);

	/**
	 * @brief 获取活跃事件数组
	 * @param
	 * @return
	 */
	active_event_array get_active_event_array(void) const {return active_events_;}

private:
	/**
	 * @brief 重置event_set_大小
	 * @param new_set_size 新重置的pollfd数量
	 * @return 成功返回true，失败返回false
	 */
	bool reset_event_set(uint32 new_set_size);

private:
	active_event_array active_events_;
	fd_pos_container   fd_pos_info_;
	pollfd*            event_set_;
	uint32             max_set_size_;
	uint32             cur_set_size_;
	uint64             weakrand_seed_;
};

} // namespace evpp

#endif // __EV_POLL_H__