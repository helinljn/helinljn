//////////////////////////////////////////////////////////////////////////
// 定长缓冲区
//////////////////////////////////////////////////////////////////////////
#ifndef __EV_FIXED_BUFFER_H__
#define __EV_FIXED_BUFFER_H__

#include "base/ev_types.h"
#include "base/ev_noncopyable.h"
#include "base/ev_shared_ptr.h"

#include <cstdarg>

namespace evpp {

// 使用ev_shared_ptr进行资源管理
class ev_fixed_buffer;
typedef ev_shared_ptr<ev_fixed_buffer> fixed_buffer_ptr;

//////////////////////////////////////////////////////////////////////////
// ev_fixed_buffer
//////////////////////////////////////////////////////////////////////////
class ev_fixed_buffer : public ev_noncopyable
{
public:
	/**
	 * @brief 创建定长缓冲区
	 * @param init_buffer_size 缓冲区大小
	 * @return 创建成功返回对应的合法指针，创建失败返回空指针
	 */
	static fixed_buffer_ptr create(uint32 init_buffer_size);

public:
	ev_fixed_buffer(void);
	~ev_fixed_buffer(void);

	/**
	 * @brief 获取本缓冲区已经写入的字节数，与readable_size()相同
	 * @param
	 * @return
	 */
	uint32 size(void) const {return readable_size();}

	/**
	 * @brief 获取本缓冲区可写入的最大字节数
	 * @param
	 * @return
	 */
	uint32 capacity(void) const {return max_size_;}

	/**
	 * @brief 获取本缓冲区可读取的字节数
	 * @param
	 * @return
	 */
	uint32 readable_size(void) const{return cur_size_;}

	/**
	 * @brief 获取本缓冲区剩余可写入的字节数
	 * @param
	 * @return
	 */
	uint32 writable_size(void) const {return (cur_size_ >= max_size_ ? 0 : (max_size_ - cur_size_));}

	/**
	 * @brief 获取本缓冲区数据
	 * @param
	 * @return
	 */
	const void* data(void) const {return buffer_;}

	/**
	 * @brief 重置缓冲区
	 * @param
	 * @return
	 */
	void reset(void) {cur_size_ = 0; buffer_[0] = '\0';}

	/**
	 * @brief 将数据写入缓冲区
	 * @param data    写入的数据
	 * @param datalen 写入的数据长度
	 * @return 返回成功写入的字节，有可能小于len(此时缓冲区已满)
	 */
	uint32 write(const void* data, uint32 datalen);

	/**
	 * @brief 格式化写入缓冲区
	 * @param format_str 格式化字符串
	 * @return 返回成功写入的字节(当缓冲区已满，写入的数据会被截断)
	 */
	uint32 writef(const char* format_str, ...);

	/**
	 * @brief 格式化写入缓冲区
	 * @param format_str 格式化字符串
	 * @return 返回成功写入的字节(当缓冲区已满，写入的数据会被截断)
	 */
	uint32 vwritef(const char* format_str, va_list ap);

private:
	/**
	 * @brief 初始化定长缓冲区
	 * @param init_buffer_size 缓冲区大小
	 * @return 成功返回true，失败返回false
	 */
	bool init(uint32 init_buffer_size);

private:
	uint32         cur_size_;
	uint32         max_size_;
	unsigned char* buffer_;
};

} // namespace evpp

#endif // __EV_FIXED_BUFFER_H__