//////////////////////////////////////////////////////////////////////////
// 文件操作
//////////////////////////////////////////////////////////////////////////
#ifndef __EV_FILE_H__
#define __EV_FILE_H__

#include "base/ev_types.h"
#include "base/ev_noncopyable.h"
#include "base/ev_shared_ptr.h"

namespace evpp {

//////////////////////////////////////////////////////////////////////////
// file_utils
//////////////////////////////////////////////////////////////////////////
namespace file_utils {

// 文件打开模式
enum file_mode
{
	FM_READ_ONLY     = 0,  // 以只读模式打开文本文件，文件必须存在
	FM_READ_B_ONLY   = 1,  // 以只读模式打开二进制文件，文件必须存在
	FM_WRITE_ONLY    = 2,  // 以只写模式打开文本文件，文件不存在则创建，文件存在则清空内容
	FM_WRITE_B_ONLY  = 3,  // 以只写模式打开二进制文件，文件不存在则创建，文件存在则清空内容
	FM_APPEND_ONLY   = 4,  // 以追加模式打开文本文件，文件不存在则创建
	FM_APPEND_B_ONLY = 5,  // 以追加模式打开二进制文件，文件不存在则创建
};

// 文件数据
typedef struct 
{
	void*  data;
	uint32 datalen;
} file_data;

/**
 * @brief 获取文件的大小
 * @param filename       文件名
 * @param file           文件指针(FILE*)，为了避免引入stdio.h头文件
 * @param is_binary_file 是否为二进制文件
 * @return 成功返回文件的大小，失败(文件不存在或者文件打开失败)返回0
 */
uint32 ev_fsize(const char* filename, bool is_binary_file);
uint32 ev_fsize(void* file);

/**
 * @brief 获取文件的数据
 *        【注意：当使用完毕之后，需要调用ev_fdata_free(file_data)释放资源】
 * @param filename       文件名
 * @param file           文件指针(FILE*)，为了避免引入stdio.h头文件
 * @param is_binary_file 是否为二进制文件
 * @return 成功返回文件的数据(此时file_data.data != NULL && file_data.datalen > 0)
 *         空文件、文件不存在或者文件打开失败，均返回默认值(file_data.data == NULL && file_data.datalen == 0)
 */
file_data ev_fdata(const char* filename, bool is_binary_file);
file_data ev_fdata(void* file);

/**
 * @brief 释放由ev_fdata()获取的资源
 * @param fdata file_data
 * @return
 */
void ev_fdata_free(file_data& fdata);

/**
 * @brief 打开文件
 * @param filename 文件名
 * @param mode     文件打开模式，详见file_mode
 * @return 成功返回文件指针，失败返回空指针
 */
void* ev_fopen(const char* filename, file_mode mode);

/**
 * @brief 读取文件
 *        ev_fread_unlocked版本为不加锁版本，更加高效，请自行保证无竞争条件
 * @param file    读取的文件指针(FILE*)，为了避免引入stdio.h头文件
 * @param buf     存放数据的缓冲区
 * @param datalen 读取的数据长度
 * @return 返回读取的字节数
 */
uint32 ev_fread(void* file, void* buf, uint32 datalen);
uint32 ev_fread_unlocked(void* file, void* buf, uint32 datalen);

/**
 * @brief 写入文件
 *        ev_fwrite_unlocked版本为不加锁版本，更加高效，请自行保证无竞争条件
 * @param file    写入的文件指针(FILE*)，为了避免引入stdio.h头文件
 * @param data    写入的数据
 * @param datalen 写入的数据长度
 * @return 返回写入的字节数
 */
uint32 ev_fwrite(void* file, const void* data, uint32 datalen);
uint32 ev_fwrite_unlocked(void* file, const void* data, uint32 datalen);

/**
 * @brief 刷新文件缓冲区
 *        ev_fflush_unlocked版本为不加锁版本，更加高效，请自行保证无竞争条件
 * @param file 刷新缓冲区的文件指针(FILE*)，为了避免引入stdio.h头文件
 * @return 刷新成功返回true，失败返回false
 */
bool ev_fflush(void* file);
bool ev_fflush_unlocked(void* file);

/**
 * @brief 关闭文件
 * @param file 关闭的文件指针(FILE*)，为了避免引入stdio.h头文件
 * @return 关闭成功返回true，失败返回false
 */
bool ev_fclose(void* file);

/**
 * @brief 删除文件
 * @param pathname 删除的文件名
 * @return 成功返回true，失败返回false
 */
bool ev_fdelete(const char* filename);

} // namespace file_utils

// 使用ev_shared_ptr进行资源管理
class ev_file;
typedef ev_shared_ptr<ev_file> file_ptr;

//////////////////////////////////////////////////////////////////////////
// ev_file
//////////////////////////////////////////////////////////////////////////
class ev_file : public ev_noncopyable
{
public:
	/**
	 * @brief 打开文件
	 * @param filename   文件名
	 * @param mode       文件打开模式，详见file_mode
	 * @param file_bufsz 文件缓冲区大小，0代表无缓冲(内容会实时写入文件)，默认8K缓冲
	 * @return 创建成功返回对应的合法指针，创建失败返回空指针
	 */
	static file_ptr fopen(const char* filename, file_utils::file_mode mode, uint32 file_bufsz = 1024 * 8);

public:
	ev_file(void);
	~ev_file(void);

	/**
	 * @brief 获取文件的大小
	 * @param
	 * @return
	 */
	uint32 fsize(void) const {return cur_file_size_;}

	/**
	 * @brief 读取文件
	 *        fread_unlocked版本为不加锁版本，更加高效，请自行保证无竞争条件
	 * @param buf     存放数据的缓冲区
	 * @param datalen 读取的数据长度
	 * @return 返回读取的字节数
	 */
	uint32 fread(void* buf, uint32 datalen)          {return fread_internal(buf, datalen, true);}
	uint32 fread_unlocked(void* buf, uint32 datalen) {return fread_internal(buf, datalen, false);}

	/**
	 * @brief 写入文件
	 *        fwrite_unlocked版本为不加锁版本，更加高效，请自行保证无竞争条件
	 * @param data    写入的数据
	 * @param datalen 写入的数据长度
	 * @return 返回写入的字节数
	 */
	uint32 fwrite(const void* data, uint32 datalen)          {return fwrite_internal(data, datalen, true);}
	uint32 fwrite_unlocked(const void* data, uint32 datalen) {return fwrite_internal(data, datalen, false);}

	/**
	 * @brief 刷新文件缓冲区
	 *        fflush_unlocked版本为不加锁版本，更加高效，请自行保证无竞争条件
	 * @param
	 * @return 刷新成功返回true，失败返回false
	 */
	bool fflush(void)          {return fflush_internal(true);}
	bool fflush_unlocked(void) {return fflush_internal(false);}

private:
	/**
	 * @brief 初始化
	 * @param filename   文件名
	 * @param mode       文件打开模式，详见file_mode
	 * @param file_bufsz 文件缓冲区大小，0代表无缓冲(内容会实时写入文件)
	 * @return 成功返回true，失败返回false
	 */
	bool init(const char* filename, file_utils::file_mode mode, uint32 file_bufsz);

	/**
	 * @brief 读取文件
	 * @param buf     存放数据的缓冲区
	 * @param datalen 读取的数据长度
	 * @param is_safe 是否调用加锁的安全版本
	 * @return 返回读取的字节数
	 */
	uint32 fread_internal(void* buf, uint32 datalen, bool is_safe);

	/**
	 * @brief 写入文件
	 * @param data    写入的数据
	 * @param datalen 写入的数据长度
	 * @param is_safe 是否调用加锁的安全版本
	 * @return 返回写入的字节数
	 */
	uint32 fwrite_internal(const void* data, uint32 datalen, bool is_safe);

	/**
	 * @brief 刷新文件缓冲区
	 * @param is_safe 是否调用加锁的安全版本
	 * @return 刷新成功返回true，失败返回false
	 */
	bool fflush_internal(bool is_safe);

private:
	void*                 cur_file_;
	char*                 cur_file_buf_;
	uint32                cur_file_size_;
	file_utils::file_mode cur_file_mode_;
};

} // namespace evpp

#endif // __EV_FILE_H__