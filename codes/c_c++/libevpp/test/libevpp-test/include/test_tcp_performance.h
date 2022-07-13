//////////////////////////////////////////////////////////////////////////
// TCP性能测试
//////////////////////////////////////////////////////////////////////////
#ifndef __TEST_TCP_PERFORMANCE_H__
#define __TEST_TCP_PERFORMANCE_H__

#include "base/ev_types.h"
#include "base/ev_noncopyable.h"

#include <string>
#include <vector>

using namespace std;
using namespace evpp;

namespace test_tcp_performance {

/**
 * @brief 获取屏幕输入的字符串
 * @param
 * @return 屏幕输入的字符串
 */
string get_input_string(void);

/**
 * @brief 拆分屏幕输入字符串，并验证有效性
 * @param input_string 待拆分的字符串
 * @param split_string 存放最终的拆分结果
 * @return 成功返回true，失败返回false
 */
bool split_input_string(const string& input_string, vector<string>& split_string);

/**
 * @brief 获取性能测试的参数信息
 * @param ip_port          测试服务器的IP地址和端口，格式为：IPv4Address:port
 * @param connection_count 连接数
 * @param send_bytes       每条连接发送的字节数
 * @return 成功返回true，失败返回false
 */
bool get_performance_parameters(string& ip_port, int32& connection_count, int32& send_bytes);

/**
 * @brief TCP性能测试入口函数
 * @param
 * @return
 */
void test_performance_main(void);

} // namespace test_tcp_performance

#endif // __TEST_TCP_PERFORMANCE_H__