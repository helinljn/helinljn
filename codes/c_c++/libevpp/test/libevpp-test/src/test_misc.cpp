#include "test_misc.h"

#include "base/ev_md5.h"
#include "base/ev_sha.h"
#include "base/ev_aes.h"
#include "base/ev_base64.h"
#include "base/ev_assert.h"
#include "base/ev_common.h"
#include "base/ev_byte_order.h"
#include "base/ev_time.h"
#include "base/ev_time_counter.h"
#include "base/ev_fixed_buffer.h"
#include "base/ev_stream_buffer.h"
#include "base/ev_file.h"
#include "base/ev_async_logging.h"
#include "net/ev_socket.h"
#include "event2/util.h"

#include <cstdio>
#include <cstring>
#include <string>

using namespace evpp;

namespace test_misc {

void test_ev_common(void)
{
	// 测试内存获取
	{
		uint32 free_memory  = ev_get_free_memory();
		uint32 total_memory = ev_get_total_memory();
		ev_assert(free_memory);

		fprintf(stdout, "free  memory:%u(KB)\t%u(MB)\n", free_memory, free_memory / 1024);
		fprintf(stdout, "total memory:%u(KB)\t%u(MB)\n", total_memory, total_memory / 1024);
	}

	// 测试当前执行文件的路径
	{
		char   exepath[2048];
		uint32 exepathlen = sizeof(exepath);
		bool ret_exepath  = ev_get_exepath(exepath, &exepathlen);
		ev_assert(true == ret_exepath && static_cast<uint32>(strlen(exepath)) == exepathlen);
		ev_assert(0 == strcmp(ev_get_exepath().c_str(), exepath));

		char   exedir[2048];
		uint32 exedirlen = sizeof(exedir);
		bool ret_exedir  = ev_get_exedir(exedir, &exedirlen);
		ev_assert(true == ret_exedir && static_cast<uint32>(strlen(exedir)) == exedirlen);
		ev_assert(0 == strcmp(ev_get_exedir().c_str(), exedir));

		fprintf(stdout, "current exe path:%s\n", exepath);
		fprintf(stdout, "current dir path:%s\n", exedir);
	}

	// 测试获取当前主机名称和pid
	{
		char   hostname[512];
		uint32 hostname_len = sizeof(hostname);
		ev_assert(ev_get_hostname(hostname, &hostname_len) && static_cast<uint32>(strlen(hostname)) == hostname_len);

		uint32 pid = ev_get_process_id();

		fprintf(stdout, "current hostname:%s, pid:%u\n", hostname, pid);
	}

	// 测试内存数据与16进制字符串互转
	{
		struct test_hex_string
		{
		public:
			test_hex_string(void)
				: int16_val(0)
				, int32_val(0)
				, int64_val(0)
				, float_val(0.f)
				, double_val(0.0)
			{
			}

			bool operator ==(const test_hex_string& other)
			{
				return (int16_val == other.int16_val && int32_val == other.int32_val && int64_val == other.int64_val
					&& float_val == other.float_val && double_val == other.double_val);
			}

			bool operator !=(const test_hex_string& other)
			{
				return !operator==(other);
			}

		public:
			int16  int16_val;
			int32  int32_val;
			int64  int64_val;
			float  float_val;
			double double_val;
		};

		test_hex_string test_data;
		test_data.int16_val  = 123;
		test_data.int32_val  = 123456;
		test_data.int64_val  = 123456789;
		test_data.float_val  = 123.456f;
		test_data.double_val = 123.456789;

		char hex_string[sizeof(test_hex_string) * 2 + 1];
		ev_assert(true == ev_memory_to_hex_string(&test_data, sizeof(test_data), hex_string, sizeof(hex_string)));
		fprintf(stdout, "hex_string:%s\n", hex_string);

		test_hex_string restore_data;
		ev_assert(true == ev_hex_string_to_memory(hex_string, &restore_data, sizeof(restore_data)) && test_data == restore_data);
	}

	// 测试创建空目录
	ev_assert(ev_mkdir("test_dir"));

	// 测试删除空目录
	ev_assert(ev_rmdir("test_dir"));

	// 测试大小端序
	{
		ev_byte_order::byte_order order = ev_byte_order::ev_get_byte_order();
		if (ev_byte_order::BO_BIG == order)
		{
			fprintf(stdout, "big endian\n");
		}
		else if (ev_byte_order::BO_LITTLE == order)
		{
			fprintf(stdout, "little endian\n");
		}
		else
		{
			ev_assert(false && "unknown endian");
		}

		const uint16 test16 = 1234;
		ev_assert(test16 == ev_byte_order::ntoh16(ev_byte_order::hton16(test16)));

		const uint32 test32 = 12345678;
		ev_assert(test32 == ev_byte_order::ntoh32(ev_byte_order::hton32(test32)));

		const uint64 test64 = 1234567890;
		ev_assert(test64 == ev_byte_order::ntoh64(ev_byte_order::hton64(test64)));

		const int16 stest16 = -1234;
		ev_assert(stest16 == ev_byte_order::ntoh16(ev_byte_order::hton16(stest16)));

		const int32 stest32 = -12345678;
		ev_assert(stest32 == ev_byte_order::ntoh32(ev_byte_order::hton32(stest32)));

		const int64 stest64 = -1234567890;
		ev_assert(stest64 == ev_byte_order::ntoh64(ev_byte_order::hton64(stest64)));

		const int16 s1test16 = 1234;
		ev_assert(s1test16 == ev_byte_order::ntoh16(ev_byte_order::hton16(s1test16)));

		const int32 s1test32 = 12345678;
		ev_assert(s1test32 == ev_byte_order::ntoh32(ev_byte_order::hton32(s1test32)));

		const int64 s1test64 = 1234567890;
		ev_assert(s1test64 == ev_byte_order::ntoh64(ev_byte_order::hton64(s1test64)));
	}
	
	// 测试字符串转换
	{
		const char* num1_str = "123456";
		int32       num1     = 123456;
		ev_assert(num1 == ev_strtoint32(num1_str));

		const char* num2_str = "123456789";
		int64       num2     = 123456789;
		ev_assert(num2 == ev_strtoint64(num2_str));

		const char* num3_str = "123456.78901";
		float       num3     = 123456.78901f;
		ev_assert(num3 == ev_strtof(num3_str));

		const char* num4_str = "123456.7890123456";
		double      num4     = 123456.7890123456;
		ev_assert(num4 == ev_strtod(num4_str));
	}

	// 随机数生成测试
	uint8  random_i8  = 0;
	uint16 random_i16 = 0;
	uint32 random_i32 = 0;
	uint64 random_i64 = 0;
	float  random_f32 = 0.f;
	double random_d64 = 0.0;
	int32  random_val = 0;
	const int32 random_times = 10;
	for (int32 idx = 0; idx != random_times; ++idx)
	{
		random_i8  = ev_random_uint8();
		random_i16 = ev_random_uint16();
		random_i32 = ev_random_uint32();
		random_i64 = ev_random_uint64();
		random_f32 = ev_random_float();
		random_d64 = ev_random_double();
		random_val = ev_random_range(100);
		fprintf(stdout, "[%2d] uint8 = %u, uint16 = %u, uint32 = %u, uint64 = " EV_I64U ", float = %f, double = %lf, int32 = %d\n",
			idx + 1, random_i8, random_i16, random_i32, random_i64, random_f32, random_d64, random_val);
	}

	// 字符相关操作
	ev_assert(ev_isalpha('A') && ev_isalpha('a') && !ev_isalpha('8'));
	ev_assert(ev_isdigit('8') && ev_isdigit('0') && !ev_isdigit('f'));
	ev_assert(ev_isalnum('a') && ev_isalnum('8') && !ev_isalnum('*'));
	ev_assert(ev_isspace(' ') && ev_isspace('\t') && !ev_isspace('a'));
	ev_assert(ev_isprint(' ') && ev_isprint('a') && !ev_isprint('\001'));
	ev_assert(ev_islower('a') && ev_islower('b') && !ev_islower('8') && !ev_islower(' '));
	ev_assert(ev_isupper('A') && ev_isupper('B') && !ev_islower('8') && !ev_islower(' '));
	ev_assert('a' == ev_tolower('A') && 'a' == ev_tolower('a') && ' ' == ev_tolower(' '));
	ev_assert('A' == ev_toupper('a') && 'A' == ev_toupper('A') && ' ' == ev_toupper(' '));

	// 分割字符串测试
	{
		std::vector<std::string> result;
		result.reserve(32);

		result.clear();
		evpp::ev_split_string("#milan , a, b, c, hello# world, this, is# shmilyl#\"nice\" to # meet #you", " ,#", result);

		ev_assert(result[0] == "milan");
		ev_assert(result[1] == "a");
		ev_assert(result[2] == "b");
		ev_assert(result[3] == "c");
		ev_assert(result[4] == "hello");
		ev_assert(result[5] == "world");
		ev_assert(result[6] == "this");
		ev_assert(result[7] == "is");
		ev_assert(result[8] == "shmilyl");
		ev_assert(result[9] == "\"nice\"");
		ev_assert(result[10] == "to");
		ev_assert(result[11] == "meet");
		ev_assert(result[12] == "you");
		ev_assert(13 == result.size());

		result.clear();
		evpp::ev_split_string("1#2#3#4#5#6#7#8#9####   ,,# , #hello ,#,192.168.111.1", " .,#", result);

		ev_assert(result[0] == "1");
		ev_assert(result[1] == "2");
		ev_assert(result[2] == "3");
		ev_assert(result[3] == "4");
		ev_assert(result[4] == "5");
		ev_assert(result[5] == "6");
		ev_assert(result[6] == "7");
		ev_assert(result[7] == "8");
		ev_assert(result[8] == "9");
		ev_assert(result[9] == "hello");
		ev_assert(result[10] == "192");
		ev_assert(result[11] == "168");
		ev_assert(result[12] == "111");
		ev_assert(result[13] == "1");
		ev_assert(14 == result.size());

		result.clear();
		evpp::ev_split_string("This is first line.\nThis is second line.\n\n\nThis is third line.\nEnd line.\n", "\n", result);

		ev_assert(result[0] == "This is first line.");
		ev_assert(result[1] == "This is second line.");
		ev_assert(result[2] == "This is third line.");
		ev_assert(result[3] == "End line.");
		ev_assert(4 == result.size());

		result.clear();
		evpp::ev_split_string("111\t222\t333\t444\t555\t666\t777\t888\t999", "\t", result);

		ev_assert(result[0] == "111");
		ev_assert(result[1] == "222");
		ev_assert(result[2] == "333");
		ev_assert(result[3] == "444");
		ev_assert(result[4] == "555");
		ev_assert(result[5] == "666");
		ev_assert(result[6] == "777");
		ev_assert(result[7] == "888");
		ev_assert(result[8] == "999");
		ev_assert(9 == result.size());
	}

	{
		char   temp_buf[128] = {0};
		char   gbk_str[128]  = {0};
		char   utf8_str[128] = {0};
		size_t gbk_strlen    = 0;
		size_t utf8_strlen   = 0;

#if defined(__EV_WINDOWS__)
		file_ptr gbk_file  = ev_file::fopen("../../../test_gbk_string.txt",  file_utils::FM_READ_ONLY, 0);
		file_ptr utf8_file = ev_file::fopen("../../../test_utf8_string.txt", file_utils::FM_READ_ONLY, 0);
#else
		file_ptr gbk_file  = ev_file::fopen("../test_gbk_string.txt",  file_utils::FM_READ_ONLY, 0);
		file_ptr utf8_file = ev_file::fopen("../test_utf8_string.txt", file_utils::FM_READ_ONLY, 0);
#endif // defined(__EV_WINDOWS__)

		ev_assert(gbk_file && utf8_file);

		gbk_file->fread(gbk_str, sizeof(gbk_str) - 1);
		utf8_file->fread(utf8_str, sizeof(utf8_str) - 1);

		gbk_strlen  = strlen(gbk_str);
		utf8_strlen = strlen(utf8_str);

		ev_assert(gbk_strlen > 0 && utf8_strlen > 0);
		ev_assert(ev_is_gbk(gbk_str, gbk_strlen) && !ev_is_utf8(gbk_str, gbk_strlen));
		ev_assert(ev_is_utf8(utf8_str, utf8_strlen) && !ev_is_gbk(utf8_str, utf8_strlen));

		std::string temp_gbk = ev_utf8_to_gbk(utf8_str);
		ev_assert(ev_is_gbk(temp_gbk.c_str(), temp_gbk.size()) && !ev_is_utf8(temp_gbk.c_str(), temp_gbk.size()));

		memset(temp_buf, 0, sizeof(temp_buf));
		ev_snprintf(temp_buf, sizeof(temp_buf) - 1, gbk_str, temp_gbk.c_str(), 1);
		fprintf(stdout, "GBK string =%s\n", temp_buf);

		std::string temp_utf8 = ev_gbk_to_utf8(gbk_str);
		ev_assert(ev_is_utf8(temp_utf8.c_str(), temp_utf8.size()) && !ev_is_gbk(temp_utf8.c_str(), temp_utf8.size()));

		memset(temp_buf, 0, sizeof(temp_buf));
		ev_snprintf(temp_buf, sizeof(temp_buf) - 1, temp_utf8.c_str(), utf8_str, 1);
		fprintf(stdout, "UTF-8 string =%s\n", temp_buf);
	}
}

void test_md5(void)
{
	const char* test_string = "hello, this is shmilyl!";

	std::string result1        = ev_md5(test_string, static_cast<uint32>(strlen(test_string)));
	std::string result2        = ev_md5(test_string, static_cast<uint32>(strlen(test_string)), false);
	int32       compare_result = ev_strcasecmp(result1.c_str(), result2.c_str());

	ev_assert(!result1.empty() && !result2.empty());
	ev_assert(0 == compare_result);

	if (!result1.empty() && !result2.empty() && 0 == compare_result)
	{
		fprintf(stdout, "test md5 success!\n");
	}
	else
	{
		fprintf(stdout, "test md5 failed!\n");
	}
}

void test_sha(void)
{
	const char* test_string    = "hello, this is shmilyl!";
	const char* key_string     = "milan";
	size_t      test_stringlen = strlen(test_string);
	size_t      key_stringlen  = strlen(key_string);

	{
		std::string result1        = ev_sha1(test_string, static_cast<uint32>(test_stringlen));
		std::string result2        = ev_sha1(test_string, static_cast<uint32>(test_stringlen), false);
		int32       compare_result = ev_strcasecmp(result1.c_str(), result2.c_str());

		ev_assert(!result1.empty() && !result2.empty());
		ev_assert(0 == compare_result);

		if (!result1.empty() && !result2.empty() && 0 == compare_result)
		{
			fprintf(stdout, "test sha1 success!\n");
		}
		else
		{
			fprintf(stdout, "test sha1 failed!\n");
		}
	}
	
	{
		std::string result1        = ev_sha256(test_string, static_cast<uint32>(test_stringlen));
		std::string result2        = ev_sha256(test_string, static_cast<uint32>(test_stringlen), false);
		int32       compare_result = ev_strcasecmp(result1.c_str(), result2.c_str());

		ev_assert(!result1.empty() && !result2.empty());
		ev_assert(0 == compare_result);

		if (!result1.empty() && !result2.empty() && 0 == compare_result)
		{
			fprintf(stdout, "test sha256 success!\n");
		}
		else
		{
			fprintf(stdout, "test sha256 failed!\n");
		}
	}

	{
		std::string result1        = ev_hmacsha256(test_string, static_cast<uint32>(test_stringlen), key_string, static_cast<uint32>(key_stringlen));
		std::string result2        = ev_hmacsha256(test_string, static_cast<uint32>(test_stringlen), key_string, static_cast<uint32>(key_stringlen), false);
		int32       compare_result = ev_strcasecmp(result1.c_str(), result2.c_str());

		ev_assert(!result1.empty() && !result2.empty());
		ev_assert(0 == compare_result);

		if (!result1.empty() && !result2.empty() && 0 == compare_result)
		{
			fprintf(stdout, "test hmacsha256 success!\n");
		}
		else
		{
			fprintf(stdout, "test hmacsha256 failed!\n");
		}
	}
}

void test_aes(void)
{
	// AES-128
	{
		uint8 aes128_key[]         = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
		uint8 aes128_plain_text[]  = {0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x04, 0x04, 0x04, 0x04};

		uint8 aes128_encrypt_result[16] = {0};
		uint8 aes128_decrypt_result[16] = {0};

		ev_aes128_encrypt(aes128_key, aes128_plain_text, aes128_encrypt_result);
		ev_aes128_decrypt(aes128_key, aes128_encrypt_result, aes128_decrypt_result);

		ev_assert(0 == memcmp(aes128_plain_text, aes128_decrypt_result, sizeof(aes128_plain_text)));
	}
	
	// AES-256
	{
		uint8 aes256_key[]         = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
		                                      0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};
		uint8 aes256_plain_text[]  = {0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x04, 0x04, 0x04, 0x04};

		uint8 aes256_encrypt_result[16] = {0};
		uint8 aes256_decrypt_result[16] = {0};

		ev_aes256_encrypt(aes256_key, aes256_plain_text, aes256_encrypt_result);
		ev_aes256_decrypt(aes256_key, aes256_encrypt_result, aes256_decrypt_result);

		ev_assert(0 == memcmp(aes256_plain_text, aes256_decrypt_result, sizeof(aes256_plain_text)));
	}
}

void test_base64(void)
{
	const char* test_string    = "hello, this is shmilyl!";
	std::string encode_string  = ev_base64_encode(test_string, strlen(test_string));
	std::string decode_string  = ev_base64_decode(encode_string.c_str(), encode_string.size());
	int32       compare_result = strcmp(test_string, decode_string.c_str());

	ev_assert(0 == compare_result);

	if (!encode_string.empty() && !decode_string.empty() && 0 == compare_result)
	{
		fprintf(stdout, "test base64 success!\n");
	}
	else
	{
		fprintf(stdout, "test base64 failed!\n");
	}
}

void test_time(uint32 repeat_times)
{
	ev_time_counter counter;

	counter.start();
	for (evpp::uint32 idx = 0; idx != repeat_times; ++idx)
	{
		ev_time cur_time;
		std::string cur_time_str = ev_time::convert_date_to_string(cur_time);
		ev_assert(!cur_time_str.empty());
	}
	counter.end();

	fprintf(stdout, "repeat %u times costs seconds(" EV_I64U "), milliseconds(" EV_I64U "), microseconds(" EV_I64U ").\n",
		repeat_times, counter.seconds(), counter.milliseconds(), counter.microseconds());
}

void test_time_counter(uint32 repeat_times)
{
	ev_time_counter counter;
	std::string     result1;
	std::string     result2;
	int32           compare_result;

	const char* test_string    = "hello, this is shmilyl!";
	uint32      test_stringlen = static_cast<uint32>(strlen(test_string));

	counter.start();
	for (evpp::uint32 idx = 0; idx != repeat_times; ++idx)
	{
		result1 = ev_md5(test_string, test_stringlen);
		result2 = ev_md5(test_string, test_stringlen, false);

		compare_result = ev_strcasecmp(result1.c_str(), result2.c_str());

		ev_assert(!result1.empty() && !result2.empty());
		ev_assert(0 == compare_result);
	}
	counter.end();

	fprintf(stdout, "repeat %u times costs seconds(" EV_I64U "), milliseconds(" EV_I64U "), microseconds(" EV_I64U ").\n",
		repeat_times, counter.seconds(), counter.milliseconds(), counter.microseconds());
}

void test_random(evpp::uint32 repeat_times)
{
	ev_time_counter counter;
	double          random_d64;

	counter.start();
	for (evpp::uint32 idx = 0; idx != repeat_times; ++idx)
	{
		random_d64 = ev_random_double();
		ev_assert(random_d64 >= 0.0 && random_d64 < 1.0 && "ev_random_double() error!");
	}
	counter.end();

	fprintf(stdout, "repeat %u times costs seconds(" EV_I64U "), milliseconds(" EV_I64U "), microseconds(" EV_I64U ").\n",
		repeat_times, counter.seconds(), counter.milliseconds(), counter.microseconds());
}

void test_fixed_buffer(void)
{
	fixed_buffer_ptr buffer = ev_fixed_buffer::create(32);
	ev_assert(buffer && 32 == buffer->capacity());

	uint32 write_size = 0;

	write_size = buffer->writef("0123456789");
	ev_assert(10 == write_size);

	write_size = buffer->writef("0123456789");
	ev_assert(10 == write_size);

	write_size = buffer->writef("0123456789");
	ev_assert(10 == write_size);

	write_size = buffer->writef("0123456789");
	ev_assert(2 == write_size);

	write_size = buffer->writef("0123456789");
	ev_assert(0 == write_size);

	buffer->reset();
	ev_assert(buffer && 32 == buffer->capacity());

	write_size = buffer->writef("0123456789012345678901234567890123456789");
	ev_assert(32 == write_size);
}

void test_stream_buffer(void)
{
	bool   val_b    = true;

	float  val_f    = 123.45678f;
	float  val_f1   = -123.45678f;

	double val_d    = 123456.7890987;
	double val_d1   = -123456.7890987;

	int8   val_i8   = 12;
	int8   val_i81  = -12;
	uint8  val_ui8  = 123;

	int16  val_i16  = 1234;
	int16  val_i161 = -1234;
	uint16 val_ui16 = 12345;

	int32  val_i32  = 123456;
	int32  val_i321 = -123456;
	uint32 val_ui32 = 1234567;

	int64  val_i64  = 12345678;
	int64  val_i641 = -12345678;
	uint64 val_ui64 = 123456789;

	std::string val_str("Hello, this is ev_stream_buffer string content!");

	ev_stream_buffer outbuf;

	// 写入
	{
		uint32            write_size = 0;
		ev_stream_buffer  buffer;

		buffer.write(val_b);
		write_size += sizeof(val_b);
		ev_assert(buffer.readable_size() == write_size);

		buffer.write(val_f);
		buffer.write(val_f1);
		write_size += sizeof(val_f);
		write_size += sizeof(val_f1);
		ev_assert(buffer.readable_size() == write_size);

		buffer.write(val_d);
		buffer.write(val_d1);
		write_size += sizeof(val_d);
		write_size += sizeof(val_d1);
		ev_assert(buffer.readable_size() == write_size);

		buffer.write(val_i8);
		buffer.write(val_i81);
		buffer.write(val_ui8);
		write_size += sizeof(val_i8);
		write_size += sizeof(val_i81);
		write_size += sizeof(val_ui8);
		ev_assert(buffer.readable_size() == write_size);

		buffer.write(val_i16);
		buffer.write(val_i161);
		buffer.write(val_ui16);
		write_size += sizeof(val_i16);
		write_size += sizeof(val_i161);
		write_size += sizeof(val_ui16);
		ev_assert(buffer.readable_size() == write_size);

		buffer.write(val_i32);
		buffer.write(val_i321);
		buffer.write(val_ui32);
		write_size += sizeof(val_i32);
		write_size += sizeof(val_i321);
		write_size += sizeof(val_ui32);
		ev_assert(buffer.readable_size() == write_size);

		buffer.write(val_i64);
		buffer.write(val_i641);
		buffer.write(val_ui64);
		write_size += sizeof(val_i64);
		write_size += sizeof(val_i641);
		write_size += sizeof(val_ui64);
		ev_assert(buffer.readable_size() == write_size);

		buffer.write(val_str);
		write_size += sizeof(uint32);
		write_size += static_cast<uint32>(val_str.size());
		ev_assert(buffer.readable_size() == write_size);

		uint32 arr_size = 256;
		buffer.write(arr_size);
		write_size += sizeof(uint32);
		ev_assert(buffer.readable_size() == write_size);

		for (uint32 idx = 0; idx != arr_size; ++idx)
		{
			buffer.write(idx);
			write_size += sizeof(uint32);
			ev_assert(buffer.readable_size() == write_size);
		}

		// 写入流长度
		buffer.prepend(buffer.size());
		write_size += sizeof(uint32);
		ev_assert(buffer.readable_size() == write_size);

		// 写入包号
		buffer.prepend(static_cast<uint32>(1001));
		write_size += sizeof(uint32);
		ev_assert(buffer.readable_size() == write_size);

		outbuf.append(buffer);
	}

	// 读取
	{
		// 读取包号
		uint32 message_id = 0;
		ev_assert(outbuf.read(message_id, false) && 1001 == message_id);
		ev_assert(outbuf.read(message_id) && 1001 == message_id);

		// 读取流长度
		uint32 msg_size = 0;
		ev_assert(outbuf.read(msg_size, false));
		ev_assert(outbuf.read(msg_size));

		bool rval_b;
		outbuf.read(rval_b, false);
		outbuf.read(rval_b);
		msg_size -= sizeof(rval_b);
		ev_assert(outbuf.readable_size() == msg_size && rval_b == val_b);

		float rval_f;
		float rval_f1;
		outbuf.read(rval_f, false);
		outbuf.read(rval_f);
		outbuf.read(rval_f1, false);
		outbuf.read(rval_f1);
		msg_size -= sizeof(rval_f);
		msg_size -= sizeof(rval_f1);
		ev_assert(outbuf.readable_size() == msg_size && rval_f == val_f && rval_f1 == val_f1);

		double rval_d;
		double rval_d1;
		outbuf.read(rval_d, false);
		outbuf.read(rval_d);
		outbuf.read(rval_d1, false);
		outbuf.read(rval_d1);
		msg_size -= sizeof(rval_d);
		msg_size -= sizeof(rval_d1);
		ev_assert(outbuf.readable_size() == msg_size && rval_d == val_d && rval_d1 == val_d1);

		int8  rval_i8;
		int8  rval_i81;
		uint8 rval_ui8;
		outbuf.read(rval_i8, false);
		outbuf.read(rval_i8);
		outbuf.read(rval_i81, false);
		outbuf.read(rval_i81);
		outbuf.read(rval_ui8, false);
		outbuf.read(rval_ui8);
		msg_size -= sizeof(rval_i8);
		msg_size -= sizeof(rval_i81);
		msg_size -= sizeof(rval_ui8);
		ev_assert(outbuf.readable_size() == msg_size);
		ev_assert(rval_i8 == val_i8 && rval_i81 == val_i81 && rval_ui8 == val_ui8);

		int16  rval_i16;
		int16  rval_i161;
		uint16 rval_ui16;
		outbuf.read(rval_i16, false);
		outbuf.read(rval_i16);
		outbuf.read(rval_i161, false);
		outbuf.read(rval_i161);
		outbuf.read(rval_ui16, false);
		outbuf.read(rval_ui16);
		msg_size -= sizeof(rval_i16);
		msg_size -= sizeof(rval_i161);
		msg_size -= sizeof(rval_ui16);
		ev_assert(outbuf.readable_size() == msg_size);
		ev_assert(rval_i16 == val_i16 && rval_i161 == val_i161 && rval_ui16 == val_ui16);

		int32  rval_i32;
		int32  rval_i321;
		uint32 rval_ui32;
		outbuf.read(rval_i32, false);
		outbuf.read(rval_i32);
		outbuf.read(rval_i321, false);
		outbuf.read(rval_i321);
		outbuf.read(rval_ui32, false);
		outbuf.read(rval_ui32);
		msg_size -= sizeof(rval_i32);
		msg_size -= sizeof(rval_i321);
		msg_size -= sizeof(rval_ui32);
		ev_assert(outbuf.readable_size() == msg_size);
		ev_assert(rval_i32 == val_i32 && rval_i321 == val_i321 && rval_ui32 == val_ui32);

		int64  rval_i64;
		int64  rval_i641;
		uint64 rval_ui64;
		outbuf.read(rval_i64, false);
		outbuf.read(rval_i64);
		outbuf.read(rval_i641, false);
		outbuf.read(rval_i641);
		outbuf.read(rval_ui64, false);
		outbuf.read(rval_ui64);
		msg_size -= sizeof(rval_i64);
		msg_size -= sizeof(rval_i641);
		msg_size -= sizeof(rval_ui64);
		ev_assert(outbuf.readable_size() == msg_size);
		ev_assert(rval_i64 == val_i64 && rval_i641 == val_i641 && rval_ui64 == val_ui64);

		std::string rval_str;

		outbuf.read(rval_str, false);
		ev_assert(rval_str == val_str);

		rval_str.clear();

		outbuf.read(rval_str);

		msg_size -= sizeof(uint32);
		msg_size -= static_cast<uint32>(rval_str.size());
		ev_assert(outbuf.readable_size() == msg_size && rval_str == val_str);

		uint32 rarr_size = 0;
		outbuf.read(rarr_size, false);
		outbuf.read(rarr_size);
		msg_size -= sizeof(rarr_size);
		ev_assert(outbuf.readable_size() == msg_size && 256 == rarr_size);

		uint32 ridx = 0x7FFFFFFF;
		for (uint32 idx = 0; idx != rarr_size; ++idx)
		{
			outbuf.read(ridx, false);
			outbuf.read(ridx);
			msg_size -= sizeof(ridx);
			ev_assert(outbuf.readable_size() == msg_size && idx == ridx);
		}

		ev_assert(0 == outbuf.readable_size());
	}
}

void test_file(void)
{
	const char* filename = "./test_file.txt";
	ev_assert(!ev_access_exists(filename));

	{
		file_ptr file = ev_file::fopen(filename, file_utils::FM_WRITE_ONLY);
		ev_assert(file && ev_access_exists(filename) && "open file failed!");

		uint32 write_size = file->fwrite("0123456789", 10);
		ev_assert(10 == write_size && "write file failed!");

		file->fflush();

		write_size = file->fwrite_unlocked("0123456789", 10);
		ev_assert(10 == write_size && "write file failed!");

		file->fflush_unlocked();

		write_size = file->fwrite_unlocked("0123456789", 10);
		ev_assert(10 == write_size && "write file failed!");

		file.reset();
		ev_assert(!file && 0 == file.use_count());

		ev_assert(30 == file_utils::ev_fsize(filename, false));

		file = ev_file::fopen(filename, file_utils::FM_APPEND_ONLY);
		ev_assert(file && ev_access_exists(filename) && "open file failed!");
		ev_assert(30 == file_utils::ev_fsize(filename, false));
		ev_assert(30 == file->fsize());

		write_size = file->fwrite_unlocked("0123456789", 10);
		ev_assert(10 == write_size && "write file failed!");
		ev_assert(40 == file->fsize());

		file.reset();
		ev_assert(!file && 0 == file.use_count());

		file = ev_file::fopen(filename, file_utils::FM_READ_ONLY);
		ev_assert(file && ev_access_exists(filename) && "open file failed!");
		ev_assert(40 == file_utils::ev_fsize(filename, false));
		ev_assert(40 == file->fsize());

		char temp_buf[10];

		memset(temp_buf, 0, sizeof(temp_buf));
		ev_assert(10 == file->fread(temp_buf, 10));
		ev_assert(0 == strncmp("0123456789", temp_buf, 10));

		memset(temp_buf, 0, sizeof(temp_buf));
		ev_assert(10 == file->fread_unlocked(temp_buf, 10));
		ev_assert(0 == strncmp("0123456789", temp_buf, 10));

		memset(temp_buf, 0, sizeof(temp_buf));
		ev_assert(9 == file->fread_unlocked(temp_buf, 9));
		ev_assert(0 == strncmp("012345678", temp_buf, 9));

		memset(temp_buf, 0, sizeof(temp_buf));
		ev_assert(9 == file->fread_unlocked(temp_buf, 9));
		ev_assert(0 == strncmp("901234567", temp_buf, 9));

		memset(temp_buf, 0, sizeof(temp_buf));
		ev_assert(2 == file->fread_unlocked(temp_buf, 2));
		ev_assert(0 == strncmp("89", temp_buf, 2));

		memset(temp_buf, 0, sizeof(temp_buf));
		ev_assert(0 == file->fread_unlocked(temp_buf, 10));
	}

	{
		std::string result1        = ev_md5_file(filename);
		std::string result2        = ev_md5_file(filename);
		int32       compare_result = ev_strcasecmp(result1.c_str(), result2.c_str());

		ev_assert(!result1.empty() && !result2.empty());
		ev_assert(0 == compare_result);

		if (!result1.empty() && !result2.empty() && 0 == compare_result)
		{
			fprintf(stdout, "test md5 file success!\n");
		}
		else
		{
			fprintf(stdout, "test md5 file failed!\n");
		}
	}

	{
		std::string result1        = ev_sha1_file(filename);
		std::string result2        = ev_sha1_file(filename);
		int32       compare_result = ev_strcasecmp(result1.c_str(), result2.c_str());

		ev_assert(!result1.empty() && !result2.empty());
		ev_assert(0 == compare_result);

		if (!result1.empty() && !result2.empty() && 0 == compare_result)
		{
			fprintf(stdout, "test sha1 success!\n");
		}
		else
		{
			fprintf(stdout, "test sha1 failed!\n");
		}
	}

	ev_assert(40 == file_utils::ev_fsize(filename, false));
	ev_assert(file_utils::ev_fdelete(filename));
	ev_assert(!ev_access_exists(filename));
}

void test_socketpair(void)
{
	using namespace socket_utils;

	socket_t    fds[2]   = {-1};
	char        buf[64]  = { 0};
	int32       ret      = 0;
	const char* send_msg = "Hello, world!";
	uint32      send_len = static_cast<uint32>(strlen(send_msg));

	if (!ev_create_socketpair(fds))
	{
		ev_assert(false && "ev_create_socketpair() failed!");
		return;
	}

	fprintf(stdout, "ev_create_socketpair success! fds[0] = " EV_SOCK ", fds[1] = " EV_SOCK "\n", fds[0], fds[1]);

	ret = ev_send(fds[0], send_msg, send_len);
	if (-1 == ret)
	{
		fprintf(stdout, "socketpair write failed! errno = %d\n", ev_socket_errno());
		goto cleanup_handle;
	}

	ret = ev_recv(fds[1], buf, 64);
	if (0 == ret || -1 == ret)
	{
		fprintf(stdout, "socketpair read failed! errno = %d\n", ev_socket_errno());
		goto cleanup_handle;
	}

	if (ret > 0)
	{
		fprintf(stdout, "test_socketpair success! read msg = %s\n", buf);
	}
	else
	{
		fprintf(stdout, "test_socketpair failed!\n");
		ev_assert(false && "test_socketpair failed!");
	}

cleanup_handle:
	ev_assert(ev_close_socket(fds[0]));
	ev_assert(ev_close_socket(fds[1]));
}

} // namespace test_misc