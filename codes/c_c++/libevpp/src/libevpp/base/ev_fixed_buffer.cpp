#include "base/ev_fixed_buffer.h"
#include "base/ev_common.h"
#include "util-internal.h"
#include "mm-internal.h"

#include <cstring>

namespace evpp {

//////////////////////////////////////////////////////////////////////////
// ev_fixed_buffer
//////////////////////////////////////////////////////////////////////////
fixed_buffer_ptr ev_fixed_buffer::create(uint32 init_buffer_size)
{
	fixed_buffer_ptr ret(new ev_fixed_buffer);
	if (!ret->init(init_buffer_size))
	{
		ret.reset();
		return ret;
	}

	return ret;
}

ev_fixed_buffer::ev_fixed_buffer(void)
	: cur_size_(0)
	, max_size_(0)
	, buffer_(NULL)
{
}

ev_fixed_buffer::~ev_fixed_buffer(void)
{
	if (buffer_)
	{
		mm_free(buffer_);
	}

	cur_size_ = 0;
	max_size_ = 0;
	buffer_   = NULL;
}

uint32 ev_fixed_buffer::write(const void* data, uint32 datalen)
{
	if (NULL == data || 0 == datalen || 0 == writable_size())
	{
		return 0;
	}

	uint32 remain_size = writable_size();
	if (remain_size >= datalen)
	{
		memcpy(buffer_ + cur_size_, data, datalen);
		cur_size_ += datalen;
		return datalen;
	}
	else
	{
		memcpy(buffer_ + cur_size_, data, remain_size);
		cur_size_ += remain_size;
		return remain_size;
	}
}

uint32 ev_fixed_buffer::writef(const char* format_str, ...)
{
	va_list ap;
	va_start(ap, format_str);

	uint32 write_size = vwritef(format_str, ap);

	va_end(ap);

	return write_size;
}

uint32 ev_fixed_buffer::vwritef(const char* format_str, va_list ap)
{
	if (NULL == format_str || 0 == writable_size())
	{
		return 0;
	}

	// char buf[8];
	// int ret = ev_vsnprintf(buf, sizeof(buf), "hello world");
	// 此时ret返回值为：11，但是实际写入的内容为："hello w"，以及末尾的'\0'，共计8个字节
	// 因为该函数保证字符串是以'\0'结束，并且写入大小不超过sizeof(buf) - 1
	// 最后的一个字节总是留给'\0'，其实我们并不需要，所以需要特殊处理一下
	// 即：将剩余大小总是加1，注意，不会出现溢出情况，因为申请内存的时候多了8个字节的预留空间
	int32 remain_size = static_cast<int32>(writable_size()) + 1;
	int32 write_size  = ev_vsnprintf(reinterpret_cast<char*>(buffer_) + cur_size_, static_cast<size_t>(remain_size), format_str, ap);

	// 写入失败或者format_str为空串(写入了0个字节)
	if (write_size <= 0)
	{
		return 0;
	}

	// 缓冲区不足，发生截断
	if (write_size >= remain_size)
	{
		cur_size_ += static_cast<uint32>(--remain_size);
		return static_cast<uint32>(remain_size);
	}

	// 正常写入
	cur_size_ += static_cast<uint32>(write_size);
	return static_cast<uint32>(write_size);
}

bool ev_fixed_buffer::init(uint32 init_buffer_size)
{
	if (0 == init_buffer_size)
	{
		return false;
	}

	max_size_ = init_buffer_size;
	buffer_   = static_cast<unsigned char*>(mm_malloc(init_buffer_size + 8/* 为ev_vsnprintf()准备的预留空间 */));
	if (NULL == buffer_)
	{
		return false;
	}
	
	reset();

	return true;
}

} // namespace evpp