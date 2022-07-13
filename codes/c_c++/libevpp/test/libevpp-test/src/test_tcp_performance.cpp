#include "test_tcp_performance.h"

#include "base/ev_common.h"
#include "base/ev_assert.h"
#include "base/ev_async_logging.h"
#include "net/ev_poll.h"
#include "net/ev_socket.h"

#include <cstdio>

namespace test_tcp_performance {

std::string get_input_string(void)
{
	char  temp_buf[1024] = {0};
	char* ret            = NULL;

	fprintf(stdout,
		"-------------------------------------------------------\n"
		"------ Welcome to use tcp performance test tools ------\n"
		"------ Usage  : IP Port ConnectionCount SendByte ------\n"
		"------ Example: 192.168.111.111 8088 1000 10     ------\n"
		"-------------------------------------------------------\n"
		"$ "
	);

	ret = fgets(temp_buf, sizeof(temp_buf), stdin);
	ev_assert(ret != NULL && "fgets() failed!");

	return string(ret);
}

bool split_input_string(const string& input_string, vector<string>& split_string)
{
	if (input_string.empty())
	{
		return false;
	}

	// 按" \n"拆分字符串，空格和\n，拆分的同时，忽略掉\n字符
	ev_split_string(input_string.c_str(), " \n", split_string);

	// 校对拆分结果
	if (split_string.size() != 4)
	{
		return false;
	}

	// 按照'.'对IP地址进行拆分，并将结果继续放入split_string中
	ev_split_string(split_string[0].c_str(), ".", split_string);

	// 校对拆分结果
	if (split_string.size() != 8)
	{
		return false;
	}

	// 验证IP地址，端口号，连接数，发送字节数
	for (vector<string>::size_type idx = 1; idx != split_string.size(); ++idx)
	{
		const string& temp_str = split_string[idx];

		// 不能为空，并且不能以0开头
		if (temp_str.empty() || '0' == temp_str[0])
		{
			return false;
		}

		// 必须全部都是十进制数字
		for (string::size_type j = 0; j != temp_str.size(); ++j)
		{
			if (!ev_isdigit(temp_str[j]))
			{
				return false;
			}
		}
	}

	// 校验成功，删除后面四个IP地址拆分的字符串
	split_string.pop_back();
	split_string.pop_back();
	split_string.pop_back();
	split_string.pop_back();

	ev_assert(4 == split_string.size());

	return true;
}

bool get_performance_parameters(string& ip_port, int32& connection_count, int32& send_bytes)
{
	ip_port.clear();
	connection_count = 0;
	send_bytes       = 0;

	string         input_string;
	vector<string> split_string;
	split_string.reserve(8);

	// 获取屏幕输入字符串
	input_string = get_input_string();

	// 拆分屏幕输入字符串
	if (!split_input_string(input_string, split_string))
	{
		fprintf(stdout, "Invalid input parameters! Please try again!\n");
		return false;
	}

	ip_port           = split_string[0];
	ip_port          += ":";
	ip_port          += split_string[1];
	connection_count  = ev_strtoint32(split_string[2].c_str());
	send_bytes        = ev_strtoint32(split_string[3].c_str());

	return true;
}

void test_performance_main(void)
{
	string ip_port;
	int32  connection_count;
	int32  send_bytes;

	// 获取性能测试的参数信息
	while (true)
	{
		if (!get_performance_parameters(ip_port, connection_count, send_bytes))
		{
			continue;
		}

		break;
	}

	ev_log_trace("ip_port = %s, con_count = %d, send_bytes = %d", ip_port.c_str(), connection_count, send_bytes);
}

} // namespace test_tcp_performance