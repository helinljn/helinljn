#include "base/ev_file.h"
#include "base/ev_common.h"
#include "util-internal.h"
#include "mm-internal.h"

#include <cstdio>

namespace evpp {

//////////////////////////////////////////////////////////////////////////
// file_utils
//////////////////////////////////////////////////////////////////////////
namespace file_utils {

uint32 ev_fsize(const char* filename, bool is_binary_file)
{
	if (NULL == filename)
	{
		return 0;
	}

	// 不存在或者没有读权限则直接返回
	if (!ev_access_exists(filename) || !ev_access_read(filename))
	{
		return 0;
	}

	void* file = ev_fopen(filename, (is_binary_file ? FM_READ_B_ONLY : FM_READ_ONLY));
	if (NULL == file)
	{
		return 0;
	}

	uint32 len = ev_fsize(file);

	ev_fclose(file);

	return len;
}

uint32 ev_fsize(void* file)
{
	if (NULL == file)
	{
		return 0;
	}

	FILE* temp_file = reinterpret_cast<FILE*>(file);
	long cur_pos    = ftell(temp_file);
	long len        = 0;

	fseek(temp_file, 0, SEEK_END);
	len = ftell(temp_file);
	fseek(temp_file, cur_pos, SEEK_SET);

	return static_cast<uint32>(len > 0 ? len : 0);
}

file_data ev_fdata(const char* filename, bool is_binary_file)
{
	file_data data;
	data.data    = NULL;
	data.datalen = 0;

	if (NULL == filename)
	{
		return data;
	}

	// 不存在或者没有读权限则直接返回
	if (!ev_access_exists(filename) || !ev_access_read(filename))
	{
		return data;
	}

	void* file = ev_fopen(filename, (is_binary_file ? FM_READ_B_ONLY : FM_READ_ONLY));
	if (NULL == file)
	{
		return data;
	}

	data = ev_fdata(file);

	ev_fclose(file);

	return data;
}

file_data ev_fdata(void* file)
{
	file_data data;
	data.data    = NULL;
	data.datalen = 0;

	if (NULL == file)
	{
		return data;
	}

	// 获取文件大小
	uint32 filesz = ev_fsize(file);
	if (0 == filesz)
	{
		return data;
	}

	// 申请写入buffer
	void* buf = mm_malloc(sizeof(unsigned char) * filesz);
	if (NULL == buf)
	{
		return data;
	}

	FILE* temp_file = reinterpret_cast<FILE*>(file);
	long cur_pos    = ftell(temp_file);

	fseek(temp_file, 0, SEEK_SET);
	uint32 ret = ev_fread(temp_file, buf, filesz);
	fseek(temp_file, cur_pos, SEEK_SET);

	if (ret != filesz)
	{
		mm_free(buf);
		return data;
	}

	data.data    = buf;
	data.datalen = filesz;

	return data;
}

void ev_fdata_free(file_data& fdata)
{
	if (NULL == fdata.data)
	{
		return;
	}

	mm_free(fdata.data);

	fdata.data    = NULL;
	fdata.datalen = 0;
}

void* ev_fopen(const char* filename, file_mode mode)
{
	if (NULL == filename)
	{
		return NULL;
	}

	const char* str_mode = NULL;
	switch (mode)
	{
	case FM_READ_ONLY:
		str_mode = "r";
		break;
	case FM_READ_B_ONLY:
		str_mode = "rb";
		break;
	case FM_WRITE_ONLY:
		str_mode = "w";
		break;
	case FM_WRITE_B_ONLY:
		str_mode = "wb";
		break;
	case FM_APPEND_ONLY:
		str_mode = "a";
		break;
	case FM_APPEND_B_ONLY:
		str_mode = "ab";
		break;
	default:
		break;
	}

	if (NULL == str_mode)
	{
		return NULL;
	}

	return fopen(filename, str_mode);
}

uint32 ev_fread(void* file, void* buf, uint32 datalen)
{
	if (NULL == file || NULL == buf || datalen < 1)
	{
		return 0;
	}

	return static_cast<uint32>(fread(buf, 1, datalen, reinterpret_cast<FILE*>(file)));
}

uint32 ev_fread_unlocked(void* file, void* buf, uint32 datalen)
{
	if (NULL == file || NULL == buf || datalen < 1)
	{
		return 0;
	}

#if defined(__EV_WINDOWS__)
	return static_cast<uint32>(_fread_nolock(buf, 1, datalen, reinterpret_cast<FILE*>(file)));
#elif defined(__EV_LINUX__)
	return static_cast<uint32>(fread_unlocked(buf, 1, datalen, reinterpret_cast<FILE*>(file)));
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

uint32 ev_fwrite(void* file, const void* data, uint32 datalen)
{
	if (NULL == file || NULL == data || datalen < 1)
	{
		return 0;
	}

	return static_cast<uint32>(fwrite(data, 1, datalen, reinterpret_cast<FILE*>(file)));
}

uint32 ev_fwrite_unlocked(void* file, const void* data, uint32 datalen)
{
	if (NULL == file || NULL == data || datalen < 1)
	{
		return 0;
	}

#if defined(__EV_WINDOWS__)
	return static_cast<uint32>(_fwrite_nolock(data, 1, datalen, reinterpret_cast<FILE*>(file)));
#elif defined(__EV_LINUX__)
	return static_cast<uint32>(fwrite_unlocked(data, 1, datalen, reinterpret_cast<FILE*>(file)));
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)
}

bool ev_fflush(void* file)
{
	if (NULL == file)
	{
		return false;
	}

	return (0 == fflush(reinterpret_cast<FILE*>(file)));
}

bool ev_fflush_unlocked(void* file)
{
	if (NULL == file)
	{
		return false;
	}

	int32 ret = -1;

#if defined(__EV_WINDOWS__)
	ret = _fflush_nolock(reinterpret_cast<FILE*>(file));
#elif defined(__EV_LINUX__)
	ret = fflush_unlocked(reinterpret_cast<FILE*>(file));
#else
	#error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)

	return (0 == ret);
}

bool ev_fclose(void* file)
{
	if (NULL == file)
	{
		return false;
	}

	return (0 == fclose(reinterpret_cast<FILE*>(file)));
}

bool ev_fdelete(const char* filename)
{
	if (NULL == filename)
	{
		return false;
	}

	return (0 == remove(filename));
}

} // namespace file_utils

//////////////////////////////////////////////////////////////////////////
// ev_file
//////////////////////////////////////////////////////////////////////////
file_ptr ev_file::fopen(const char* filename, file_utils::file_mode mode, uint32 file_bufsz)
{
	file_ptr ret(new ev_file);
	if (!ret->init(filename, mode, file_bufsz))
	{
		ret.reset();
		return ret;
	}

	return ret;
}

ev_file::ev_file(void)
	: cur_file_(NULL)
	, cur_file_buf_(NULL)
	, cur_file_size_(0)
	, cur_file_mode_(file_utils::FM_READ_ONLY)
{
}

ev_file::~ev_file(void)
{
	if (cur_file_)
	{
		file_utils::ev_fclose(cur_file_);
	}

	if (cur_file_buf_)
	{
		mm_free(cur_file_buf_);
	}

	cur_file_      = NULL;
	cur_file_buf_  = NULL;
	cur_file_size_ = 0;
}

bool ev_file::init(const char* filename, file_utils::file_mode mode, uint32 file_bufsz)
{
	cur_file_ = file_utils::ev_fopen(filename, mode);
	if (NULL == cur_file_)
	{
		return false;
	}

	int32 buf_mode = _IONBF;
	if (file_bufsz > 0)
	{
		cur_file_buf_ = static_cast<char*>(mm_malloc(file_bufsz));
		buf_mode      = _IOFBF;

		if (NULL == cur_file_buf_)
		{
			return false;
		}
	}

	if (setvbuf(static_cast<FILE*>(cur_file_), cur_file_buf_, buf_mode, file_bufsz) != 0)
	{
		return false;
	}

	cur_file_size_ = file_utils::ev_fsize(cur_file_);
	cur_file_mode_ = mode;

	return true;
}

uint32 ev_file::fread_internal(void* buf, uint32 datalen, bool is_safe)
{
	if (file_utils::FM_READ_ONLY != cur_file_mode_ && file_utils::FM_READ_B_ONLY != cur_file_mode_)
	{
		return 0;
	}

	if (is_safe)
	{
		return file_utils::ev_fread(cur_file_, buf, datalen);
	}
	else
	{
		return file_utils::ev_fread_unlocked(cur_file_, buf, datalen);
	}
}

uint32 ev_file::fwrite_internal(const void* data, uint32 datalen, bool is_safe)
{
	if (file_utils::FM_READ_ONLY == cur_file_mode_ || file_utils::FM_READ_B_ONLY == cur_file_mode_)
	{
		return 0;
	}

	uint32 ret = 0;
	if (is_safe)
	{
		ret = file_utils::ev_fwrite(cur_file_, data, datalen);
	}
	else
	{
		ret = file_utils::ev_fwrite_unlocked(cur_file_, data, datalen);
	}

	cur_file_size_ += ret;
	return ret;
}

bool ev_file::fflush_internal(bool is_safe)
{
	if (file_utils::FM_READ_ONLY == cur_file_mode_ || file_utils::FM_READ_B_ONLY == cur_file_mode_)
	{
		return false;
	}

	if (is_safe)
	{
		return file_utils::ev_fflush(cur_file_);
	}
	else
	{
		return file_utils::ev_fflush_unlocked(cur_file_);
	}
}

} // namespace evpp