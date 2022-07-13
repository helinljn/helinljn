#include "base/ev_stream_buffer.h"
#include "base/ev_byte_order.h"
#include "util-internal.h"
#include "mm-internal.h"

#include <cstring>

namespace evpp {

//////////////////////////////////////////////////////////////////////////
// ev_stream_buffer
//////////////////////////////////////////////////////////////////////////
ev_stream_buffer::ev_stream_buffer(void)
	: capacity_(g_bufferconfig_prepend_size + g_bufferconfig_initial_size)
	, read_index_(g_bufferconfig_prepend_size)
	, write_index_(g_bufferconfig_prepend_size)
	, buffer_(static_cast<char*>(mm_malloc(capacity_)))
{
}

ev_stream_buffer::~ev_stream_buffer(void)
{
	if (buffer_)
	{
		mm_free(buffer_);
	}
	
	capacity_    = 0;
	read_index_  = 0;
	write_index_ = 0;
	buffer_      = NULL;
}

bool ev_stream_buffer::read(int8& val, bool is_delete)
{
	return read_internal(&val, sizeof(val), is_delete);
}

bool ev_stream_buffer::read(uint8& val, bool is_delete)
{
	return read_internal(&val, sizeof(val), is_delete);
}

bool ev_stream_buffer::read(int16& val, bool is_delete)
{
	if (!read_internal(&val, sizeof(val), is_delete))
	{
		return false;
	}

	val = ev_byte_order::ntoh16(val);

	return true;
}

bool ev_stream_buffer::read(uint16& val, bool is_delete)
{
	if (!read_internal(&val, sizeof(val), is_delete))
	{
		return false;
	}

	val = ev_byte_order::ntoh16(val);

	return true;
}

bool ev_stream_buffer::read(int32& val, bool is_delete)
{
	if (!read_internal(&val, sizeof(val), is_delete))
	{
		return false;
	}

	val = ev_byte_order::ntoh32(val);

	return true;
}

bool ev_stream_buffer::read(uint32& val, bool is_delete)
{
	if (!read_internal(&val, sizeof(val), is_delete))
	{
		return false;
	}

	val = ev_byte_order::ntoh32(val);

	return true;
}

bool ev_stream_buffer::read(int64& val, bool is_delete)
{
	if (!read_internal(&val, sizeof(val), is_delete))
	{
		return false;
	}

	val = ev_byte_order::ntoh64(val);

	return true;
}

bool ev_stream_buffer::read(uint64& val, bool is_delete)
{
	if (!read_internal(&val, sizeof(val), is_delete))
	{
		return false;
	}

	val = ev_byte_order::ntoh64(val);

	return true;
}

bool ev_stream_buffer::read(bool& val, bool is_delete)
{
	return read_internal(&val, sizeof(val), is_delete);
}

bool ev_stream_buffer::read(float& val, bool is_delete)
{
	union {
		uint32 val1;
		float  val2;
	}uf;

	if (!read_internal(&uf.val1, sizeof(uf.val1), is_delete))
	{
		return false;
	}

	uf.val1 = ev_byte_order::ntoh32(uf.val1);
	val     = uf.val2;

	return true;
}

bool ev_stream_buffer::read(double& val, bool is_delete)
{
	union {
		uint64 val1;
		double val2;
	}ud;

	if (!read_internal(&ud.val1, sizeof(ud.val1), is_delete))
	{
		return false;
	}

	ud.val1 = ev_byte_order::ntoh64(ud.val1);
	val     = ud.val2;

	return true;
}

bool ev_stream_buffer::read(std::string& val, bool is_delete)
{
	// 先获取字符串的长度
	uint32 str_size = 0;
	if (!read(str_size, is_delete))
	{
		return false;
	}
	
	if (is_delete)
	{
		// 可读的长度小于字符串的长度，返回失败
		if (readable_size() < str_size)
		{
			read_index_ -= 4;
			return false;
		}

		val.append(buffer_ + read_index_, str_size);
		read_index_ += str_size;
	}
	else
	{
		uint32 remain_size = readable_size();
		remain_size        = (remain_size >= 4 ? remain_size - 4 : 0);

		// 可读的长度小于字符串的长度，返回失败
		if (remain_size < str_size)
		{
			return false;
		}

		val.append(buffer_ + read_index_ + 4, str_size);
	}

	return true;
}

bool ev_stream_buffer::write(int8 val)
{
	return write_internal(&val, sizeof(val));
}

bool ev_stream_buffer::write(uint8 val)
{
	return write_internal(&val, sizeof(val));
}

bool ev_stream_buffer::write(int16 val)
{
	int16 temp_val = ev_byte_order::hton16(val);
	return write_internal(&temp_val, sizeof(val));
}

bool ev_stream_buffer::write(uint16 val)
{
	uint16 temp_val = ev_byte_order::hton16(val);
	return write_internal(&temp_val, sizeof(val));
}

bool ev_stream_buffer::write(int32 val)
{
	int32 temp_val = ev_byte_order::hton32(val);
	return write_internal(&temp_val, sizeof(val));
}

bool ev_stream_buffer::write(uint32 val)
{
	uint32 temp_val = ev_byte_order::hton32(val);
	return write_internal(&temp_val, sizeof(val));
}

bool ev_stream_buffer::write(int64 val)
{
	int64 temp_val = ev_byte_order::hton64(val);
	return write_internal(&temp_val, sizeof(val));
}

bool ev_stream_buffer::write(uint64 val)
{
	uint64 temp_val = ev_byte_order::hton64(val);
	return write_internal(&temp_val, sizeof(val));
}

bool ev_stream_buffer::write(bool val)
{
	return write_internal(&val, sizeof(val));
}

bool ev_stream_buffer::write(float val)
{
	union {
		uint32 val1;
		float  val2;
	}uf;

	uf.val2 = val;

	uint32 temp_val = ev_byte_order::hton32(uf.val1);
	return write_internal(&temp_val, sizeof(temp_val));
}

bool ev_stream_buffer::write(double val)
{
	union {
		uint64 val1;
		double val2;
	}ud;

	ud.val2 = val;

	uint64 temp_val = ev_byte_order::hton64(ud.val1);
	return write_internal(&temp_val, sizeof(temp_val));
}

bool ev_stream_buffer::write(const std::string& val)
{
	return write(val.c_str(), static_cast<uint32>(val.size()));
}

bool ev_stream_buffer::write(const char* val)
{
	return write(val, static_cast<uint32>(strlen(val)));
}

bool ev_stream_buffer::write(const char* val, uint32 len)
{
	// 先写入字符串的长度
	if (!write(len))
	{
		return false;
	}

	// 写入字符串
	if (!write_internal(val, len))
	{
		write_index_ -= 4;
		return false;
	}

	return true;
}

bool ev_stream_buffer::prepend(int8 val)
{
	return prepend_internal(&val, sizeof(val));
}

bool ev_stream_buffer::prepend(uint8 val)
{
	return prepend_internal(&val, sizeof(val));
}

bool ev_stream_buffer::prepend(int16 val)
{
	int16 temp_val = ev_byte_order::hton16(val);
	return prepend_internal(&temp_val, sizeof(val));
}

bool ev_stream_buffer::prepend(uint16 val)
{
	uint16 temp_val = ev_byte_order::hton16(val);
	return prepend_internal(&temp_val, sizeof(val));
}

bool ev_stream_buffer::prepend(int32 val)
{
	int32 temp_val = ev_byte_order::hton32(val);
	return prepend_internal(&temp_val, sizeof(val));
}

bool ev_stream_buffer::prepend(uint32 val)
{
	uint32 temp_val = ev_byte_order::hton32(val);
	return prepend_internal(&temp_val, sizeof(val));
}

bool ev_stream_buffer::prepend(int64 val)
{
	int64 temp_val = ev_byte_order::hton64(val);
	return prepend_internal(&temp_val, sizeof(val));
}

bool ev_stream_buffer::prepend(uint64 val)
{
	uint64 temp_val = ev_byte_order::hton64(val);
	return prepend_internal(&temp_val, sizeof(val));
}

bool ev_stream_buffer::append(const ev_stream_buffer& buffer)
{
	return append(buffer.data(), buffer.size());
}

bool ev_stream_buffer::append(const void* data, uint32 datalen)
{
	return write_internal(data, datalen);
}

void ev_stream_buffer::swap(ev_stream_buffer& buffer)
{
	if (this == &buffer)
	{
		return;
	}

	char temp_buf[sizeof(ev_stream_buffer)];
	memcpy(temp_buf, &buffer, sizeof(ev_stream_buffer));
	memcpy(&buffer, this, sizeof(ev_stream_buffer));
	memcpy(this, temp_buf, sizeof(ev_stream_buffer));
}

bool ev_stream_buffer::read_internal(void* buf, uint32 readlen, bool is_delete)
{
	if (readable_size() < readlen)
	{
		return false;
	}

	memcpy(buf, buffer_ + read_index_, readlen);

	if (is_delete)
	{
		read_index_ += readlen;
	}

	return true;
}

bool ev_stream_buffer::write_internal(const void* data, uint32 datalen)
{
	if (writable_size() < datalen && !grow_internal(datalen))
	{
		return false;
	}

	memcpy(buffer_ + write_index_, data, datalen);

	write_index_ += datalen;

	return true;
}

bool ev_stream_buffer::prepend_internal(const void* data, uint32 datalen)
{
	if (prependable_size() < datalen)
	{
		return false;
	}

	read_index_ -= datalen;

	memcpy(buffer_ + read_index_, data, datalen);

	return true;
}

bool ev_stream_buffer::grow_internal(uint32 total_size)
{
	// ???申请这么大内存，搞事情???
	if (total_size > UINT32_MAX / 4 + g_bufferconfig_prepend_size)
	{
		return false;
	}

	// 扩容
	uint32 new_size = (capacity() - g_bufferconfig_prepend_size) * 2 + g_bufferconfig_prepend_size;
	while (new_size < total_size)
	{
		new_size = (new_size - g_bufferconfig_prepend_size) * 2 + g_bufferconfig_prepend_size;
	}

	// ???申请这么大内存，搞事情???
	if (new_size > UINT32_MAX / 4 + g_bufferconfig_prepend_size)
	{
		return false;
	}

	char* new_buf = static_cast<char*>(mm_malloc(new_size));
	if (NULL == new_buf)
	{
		return false;
	}

	// 拷贝流数据至新的缓冲区
	memcpy(new_buf + prependable_size(), buffer_ + prependable_size(), readable_size());

	// 删除旧的缓冲区
	mm_free(buffer_);

	buffer_   = new_buf;
	capacity_ = new_size;

	return true;
}

} // namespace evpp