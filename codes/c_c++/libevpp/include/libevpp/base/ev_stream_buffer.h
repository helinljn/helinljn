//////////////////////////////////////////////////////////////////////////
// 流缓冲区
//////////////////////////////////////////////////////////////////////////
#ifndef __EV_STREAM_BUFFER_H__
#define __EV_STREAM_BUFFER_H__

#include "base/ev_types.h"
#include "base/ev_noncopyable.h"

#include <string>

namespace evpp {

//////////////////////////////////////////////////////////////////////////
// 缓冲区配置参数
//////////////////////////////////////////////////////////////////////////
// 预留空间，可以非常便捷的写入额外信息(比如：Message header)
const uint32 g_bufferconfig_prepend_size = 8;

// 缓冲区初始化大小
const uint32 g_bufferconfig_initial_size = 1024;

//////////////////////////////////////////////////////////////////////////
// ev_stream_buffer
//////////////////////////////////////////////////////////////////////////
class ev_stream_buffer : public ev_noncopyable
{
public:
	ev_stream_buffer(void);
	~ev_stream_buffer(void);

	/**
	 * @brief 读取int8/uint8
	 * @param val       int8/uint8 value
	 * @param is_delete 是否删除读取的数据，如果为false，则相当于MSG_PEEK的功能
	 * @return 成功返回true，失败返回false
	 */
	bool read(int8&  val, bool is_delete = true);
	bool read(uint8& val, bool is_delete = true);

	/**
	 * @brief 读取int16/uint16，自动转换为本机字节序
	 * @param val       int16/uint16 value
	 * @param is_delete 是否删除读取的数据，如果为false，则相当于MSG_PEEK的功能
	 * @return 成功返回true，失败返回false
	 */
	bool read(int16&  val, bool is_delete = true);
	bool read(uint16& val, bool is_delete = true);

	/**
	 * @brief 读取int32/uint32，自动转换为本机字节序
	 * @param val       int32/uint32 value
	 * @param is_delete 是否删除读取的数据，如果为false，则相当于MSG_PEEK的功能
	 * @return 成功返回true，失败返回false
	 */
	bool read(int32&  val, bool is_delete = true);
	bool read(uint32& val, bool is_delete = true);

	/**
	 * @brief 读取int64/uint64，自动转换为本机字节序
	 * @param val       int64/uint64 value
	 * @param is_delete 是否删除读取的数据，如果为false，则相当于MSG_PEEK的功能
	 * @return 成功返回true，失败返回false
	 */
	bool read(int64&  val, bool is_delete = true);
	bool read(uint64& val, bool is_delete = true);

	/**
	 * @brief 读取bool/float/double
	 * @param val       bool/float/double value
	 * @param is_delete 是否删除读取的数据，如果为false，则相当于MSG_PEEK的功能
	 * @return 成功返回true，失败返回false
	 */
	bool read(bool&   val, bool is_delete = true);
	bool read(float&  val, bool is_delete = true);
	bool read(double& val, bool is_delete = true);

	/**
	 * @brief 读取字符串(缓冲区中的字符串会被追加到val中)
	 * @param val       string value
	 * @param is_delete 是否删除读取的数据，如果为false，则相当于MSG_PEEK的功能
	 * @return 成功返回true，失败返回false
	 */
	bool read(std::string& val, bool is_delete = true);

	/**
	 * @brief 写入int8/uint8
	 * @param val int8/uint8 value
	 * @return 成功返回true，失败返回false
	 */
	bool write(int8  val);
	bool write(uint8 val);

	/**
	 * @brief 写入int16/uint16，自动转换为网络字节序
	 * @param val int16/uint16 value
	 * @return 成功返回true，失败返回false
	 */
	bool write(int16  val);
	bool write(uint16 val);

	/**
	 * @brief 写入int32/uint32，自动转换为网络字节序
	 * @param val int32/uint32 value
	 * @return 成功返回true，失败返回false
	 */
	bool write(int32  val);
	bool write(uint32 val);

	/**
	 * @brief 写入int64/uint64，自动转换为网络字节序
	 * @param val int64/uint64 value
	 * @return 成功返回true，失败返回false
	 */
	bool write(int64  val);
	bool write(uint64 val);

	/**
	 * @brief 写入bool/float/double
	 * @param val bool/float/double value
	 * @return 成功返回true，失败返回false
	 */
	bool write(bool   val);
	bool write(float  val);
	bool write(double val);

	/**
	 * @brief 写入字符串，C风格或者C++风格
	 * @param val C/C++ style string value
	 * @param len 写入的长度
	 * @return 成功返回true，失败返回false
	 */
	bool write(const std::string& val);
	bool write(const char* val);
	bool write(const char* val, uint32 len);

	/**
	 * @brief 向预留空间写入int8/uint8
	 * @param val int8/uint8 value
	 * @return 成功返回true，失败返回false
	 */
	bool prepend(int8  val);
	bool prepend(uint8 val);

	/**
	 * @brief 向预留空间写入int16/uint16，自动转换为网络字节序
	 * @param val int16/uint16 value
	 * @return 成功返回true，失败返回false
	 */
	bool prepend(int16  val);
	bool prepend(uint16 val);

	/**
	 * @brief 向预留空间写入int32/uint32，自动转换为网络字节序
	 * @param val int32/uint32 value
	 * @return 成功返回true，失败返回false
	 */
	bool prepend(int32  val);
	bool prepend(uint32 val);

	/**
	 * @brief 向预留空间写入int64/uint64，自动转换为网络字节序
	 * @param val int64/uint64 value
	 * @return 成功返回true，失败返回false
	 */
	bool prepend(int64  val);
	bool prepend(uint64 val);

	/**
	 * @brief 将另一个缓冲区的内容追加至本缓冲区
	 * @param buffer  追加的源缓冲区
	 * @param data    追加的数据
	 * @param datalen 追加的数据长度
	 * @return 成功返回true，失败返回false
	 */
	bool append(const ev_stream_buffer& buffer);
	bool append(const void* data, uint32 datalen);

	/**
	 * @brief 交换两个流缓冲区
	 * @param buffer 缓冲区
	 * @return
	 */
	void swap(ev_stream_buffer& buffer);

	/**
	 * @brief 获取缓冲区的数据
	 * @param
	 * @return
	 */
	const char* data(void) const {return buffer_ + read_index_;}

	/**
	 * @brief 获取本缓冲区可读取的字节数，与readable_size()相同
	 * @param
	 * @return
	 */
	uint32 size(void) const {return readable_size();}

	/**
	 * @brief 获取缓冲区可写入的最大字节数
	 * @param
	 * @return
	 */
	uint32 capacity(void) const {return capacity_;}

	/**
	 * @brief 获取本缓冲区可读取的字节数
	 * @param
	 * @return
	 */
	uint32 readable_size(void) const {return (write_index_ >= read_index_ ? write_index_ - read_index_ : 0);}

	/**
	 * @brief 获取本缓冲区剩余可写入的字节数
	 * @param
	 * @return
	 */
	uint32 writable_size(void) const {return (capacity_ >= write_index_ ? capacity_ - write_index_ : 0);}

	/**
	 * @brief 获取预留空间还可以写入的字节数
	 * @param
	 * @return
	 */
	uint32 prependable_size(void) const {return read_index_;}

	/**
	 * @brief 重置缓冲区
	 * @param
	 * @return
	 */
	void reset(void) {read_index_ = write_index_ = g_bufferconfig_prepend_size;}

private:
	/**
	 * @brief 读取readlen长度的数据到buf中
	 * @param buf       存放读取数据的缓冲区，请自行保证缓冲区的大小足够容纳读取的数据长度
	 * @param readlen   读取的数据长度
	 * @param is_delete 是否删除读取的数据，如果为false，则相当于MSG_PEEK的功能
	 * @return 成功返回true，失败返回false
	 */
	bool read_internal(void* buf, uint32 readlen, bool is_delete);

	/**
	 * @brief 写入datalen长度的data数据到缓冲区中
	 * @param data    写入的数据
	 * @param datalen 写入的数据长度
	 * @return 成功返回true，失败返回false
	 */
	bool write_internal(const void* data, uint32 datalen);

	/**
	 * @brief 向预留空间写入datalen长度的data数据
	 * @param data    写入的数据
	 * @param datalen 写入的数据长度
	 * @return 成功返回true，失败返回false
	 */
	bool prepend_internal(const void* data, uint32 datalen);

	/**
	 * @brief 扩容
	 * @param total_size 扩容后期望的最小长度
	 * @return 成功返回true，失败返回false
	 */
	bool grow_internal(uint32 total_size);

private:
	uint32 capacity_;
	uint32 read_index_;
	uint32 write_index_;
	char*  buffer_;
};

} // namespace evpp

#endif // __EV_STREAM_BUFFER_H__