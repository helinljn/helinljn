//////////////////////////////////////////////////////////////////////////
// socket封装，提供常用的socket接口二次封装形式
//////////////////////////////////////////////////////////////////////////
#ifndef __EV_SOCKET_H__
#define __EV_SOCKET_H__

#include "base/ev_types.h"

#include <cstdlib>
#include <cerrno>
#include <string>

#if defined(__EV_WINDOWS__)
    #include <WinSock2.h>
    #include <WS2tcpip.h>
#elif defined(__EV_LINUX__)
    #include <sys/types.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <netinet/tcp.h>
    #include <netinet/udp.h>
#else
    #error "Error! I don't know what to do..."
#endif // defined(__EV_WINDOWS__)

namespace evpp {

//////////////////////////////////////////////////////////////////////////
// socket_utils
//////////////////////////////////////////////////////////////////////////
namespace socket_utils {

// 使用ev_shutdown()关闭socket时，需要关闭的操作
enum shutdown_what
{
#if defined(__EV_WINDOWS__)
	SW_READ  = SD_RECEIVE,  // 关闭读
	SW_WRITE = SD_SEND,     // 关闭写
	SW_BOTH  = SD_BOTH,     // 关闭读写，等同于调用ev_close_socket()
#else
	SW_READ  = SHUT_RD,     // 关闭读
	SW_WRITE = SHUT_WR,     // 关闭写
	SW_BOTH  = SHUT_RDWR,   // 关闭读写，等同于调用ev_close_socket()
#endif // defined(__EV_WINDOWS__)
};

/**
 * @brief 初始化socket(主要用于Windows，其它平台调用无影响)，必须与ev_socket_destroy函数成对调用
 * @param
 * @return 成功返回true，失败返回false
 */
bool ev_socket_init(void);

/**
 * @brief 销毁socket(主要用于Windows，其它平台调用无影响)，必须与ev_socket_init函数成对调用
 * @param
 * @return 成功返回true，失败返回false
 */
bool ev_socket_destroy(void);

/**
 * @brief 设置发送缓冲区大小
 * @param sock        socket
 * @param buffer_size 缓冲区大小
 * @return 成功返回true，失败返回false
 */
bool ev_set_send_buffer_size(socket_t sock, int32 buffer_size);

/**
 * @brief 获取发送缓冲区大小
 * @param sock        socket
 * @param buffer_size 缓冲区大小
 * @return 成功返回true，失败返回false
 */
bool ev_get_send_buffer_size(socket_t sock, int32& buffer_size);

/**
 * @brief 设置接收缓冲区大小
 * @param sock        socket
 * @param buffer_size 缓冲区大小
 * @return 成功返回true，失败返回false
 */
bool ev_set_recv_buffer_size(socket_t sock, int32 buffer_size);

/**
 * @brief 获取接收缓冲区大小
 * @param sock        socket
 * @param buffer_size 缓冲区大小
 * @return 成功返回true，失败返回false
 */
bool ev_get_recv_buffer_size(socket_t sock, int32& buffer_size);

/**
 * @brief 设置TCP_NODELAY开关
 * @param sock socket
 * @param flag 为true时打开，false时关闭
 * @return 成功返回true，失败返回false
 */
bool ev_set_tcp_nodelay(socket_t sock, bool flag);

/**
 * @brief 获取TCP_NODELAY开关
 * @param sock socket
 * @param flag 为true时打开，false时关闭
 * @return 成功返回true，失败返回false
 */
bool ev_get_tcp_nodelay(socket_t sock, bool& flag);

/**
 * @brief 设置socket的linger选项
 * @param sock        socket
 * @param on_off      true开启，false关闭(此时忽略linger_time参数)
 * @param linger_time 延长时间(单位：秒)
 * @return 成功返回true，失败返回false
 */
bool ev_set_socket_linger(socket_t sock, bool on_off, uint16 linger_time);

/**
 * @brief 获取socket的linger选项
 * @param sock        socket
 * @param on_off      true开启，false关闭
 * @param linger_time 延长时间(单位：秒)
 * @return 成功返回true，失败返回false
 */
bool ev_get_socket_linger(socket_t sock, bool& on_off, uint16& linger_time);

/**
 * @brief 设置socket为非阻塞模式
 * @param sock socket
 * @return 成功返回true，失败返回false
 */
bool ev_set_socket_nonblocking(socket_t sock);

/**
 * @brief 设置监听socket在关闭时，地址可以被其它程序立刻使用
 * @param sock socket
 * @return 成功返回true，失败返回false
 */
bool ev_set_socket_reuseable(socket_t sock);

/**
 * @brief 创建socket
 * @param af       协议族
 * @param type     socket类型
 * @param protocol 指定协议
 * @return 成功返回创建的socket，失败返回-1
 */
socket_t ev_create_socket(int32 af, int32 type, int32 protocol = 0);

/**
 * @brief 创建两个互相连接的套接字，由于操作系统的实现不同，请注意以下区别
 *          Windows下实现类似于：
 *            socketpair(AF_INET, SOCK_STREAM, 0, fds);
 *          Linux下则使用原生接口：
 *            socketpair(AF_UNIX, SOCK_STREAM, 0, fds);
 *        通常fds[0]为写端，fds[1]为读端
 * @param fds            存储套接字的数组
 * @param is_nonblocking 是否直接设置为非阻塞套接字
 * @return 成功返回true，失败返回false
 */
bool ev_create_socketpair(socket_t fds[2], bool is_nonblocking = true);

/**
 * @brief 绑定端口
 * @param sock socket
 * @param addr socket地址信息，一般为sockaddr_in、sockaddr_in6或者通用结构sockaddr_storage
 * @return 成功返回true，失败返回false
 */
bool ev_bind(socket_t sock, const sockaddr_in*      addr);
bool ev_bind(socket_t sock, const sockaddr_in6*     addr);
bool ev_bind(socket_t sock, const sockaddr_storage* addr);

/**
 * @brief 监听端口
 * @param sock    socket
 * @param backlog 请求队列的最大长度
 * @return 成功返回true，失败返回false
 */
bool ev_listen(socket_t sock, int32 backlog = 256);

/**
 * @brief 发起套接字连接
 * @param sock socket
 * @param addr socket地址信息，一般为sockaddr_in、sockaddr_in6或者通用结构sockaddr_storage
 * @return 返回值对应含义如下所示：
 *           1 : 连接成功
 *           0 : 连接中，未完成
 *          -1 : 其它错误，调用ev_socket_errno()获取错误码
 *         无其它情况返回值
 */
int32 ev_connect(socket_t sock, const sockaddr_in*      addr);
int32 ev_connect(socket_t sock, const sockaddr_in6*     addr);
int32 ev_connect(socket_t sock, const sockaddr_storage* addr);

/**
 * @brief 检测一个调用过ev_connect()的socket是否连接成功
 * @param sock socket
 * @return 返回值对应含义如下所示：
 *           1 : 连接成功
 *           0 : 连接中，未完成
 *          -1 : 其它错误，调用ev_socket_errno()获取错误码
 *         无其它情况返回值
 */
int32 ev_finished_connecting(socket_t sock);

/**
 * @brief 接受客户端连接
 * @param sock 监听socket
 * @param addr 客户端的socket地址信息，一般为sockaddr_in、sockaddr_in6或者通用结构sockaddr_storage(可选，可以为NULL)
 * @return 成功返回与客户端建立好连接的socket(>= 0)，失败返回值对应含义如下所示：
 *          -1 : 其它错误，调用ev_socket_errno()获取错误码
 *          -2 : 非阻塞socket，当前已无新连接
 *         无其它情况返回值
 */
socket_t ev_accept(socket_t sock, sockaddr_in*      addr = NULL);
socket_t ev_accept(socket_t sock, sockaddr_in6*     addr = NULL);
socket_t ev_accept(socket_t sock, sockaddr_storage* addr = NULL);

/**
 * @brief 发送数据
 * @param sock    socket
 * @param data    待发送的数据
 * @param datalen 数据长度
 * @param flags   标志位(额外的发送选项)
 * @return 成功返回发送的数据长度(>= 0)，失败返回值对应含义如下所示：
 *           -1 : 其它错误，调用ev_socket_errno()获取错误码
 *           -2 : 非阻塞socket，当前发送缓冲区已满
 *         无其它情况返回值
 */
int32 ev_send(socket_t sock, const void* data, uint32 datalen, int32 flags = 0);

/**
 * @brief 接收数据
 * @param sock   socket
 * @param buf    待接收数据的缓冲区
 * @param buflen 缓冲区长度
 * @param flags  标志位(额外的接收选项)
 * @return 成功返回接收的数据长度(> 0)，失败返回值对应含义如下所示：
 *            0 : 代表对端已经关闭socket
 *           -1 : 其它错误，调用ev_socket_errno()获取错误码
 *           -2 : 非阻塞socket，当前接收缓冲区已空
 *         无其它情况返回值
 */
int32 ev_recv(socket_t sock, void* buf, uint32 buflen, int32 flags = 0);

/**
 * @brief 关闭socket
 * @param sock socket
 * @return 成功返回true，失败返回false
 */
bool ev_close_socket(socket_t sock);

/**
 * @brief 关闭socket
 * @param sock socket
 * @param what 关闭的操作，详见enum shutdown_what
 * @return 成功返回true，失败返回false
 */
bool ev_shutdown(socket_t sock, shutdown_what what);

/**
 * @brief 获取一个已绑定或者已连接的socket的本地地址
 * @param sock socket
 * @param addr socket地址信息，一般为sockaddr_in、sockaddr_in6或者通用结构sockaddr_storage
 * @return 成功返回true，失败返回false
 */
bool ev_getsockname(socket_t sock, sockaddr_in*      addr);
bool ev_getsockname(socket_t sock, sockaddr_in6*     addr);
bool ev_getsockname(socket_t sock, sockaddr_storage* addr);

/**
 * @brief 获取一个已连接的socket的对端地址
 * @param sock socket
 * @param addr socket地址信息，一般为sockaddr_in、sockaddr_in6或者通用结构sockaddr_storage
 * @return 成功返回true，失败返回false
 */
bool ev_getpeername(socket_t sock, sockaddr_in*      addr);
bool ev_getpeername(socket_t sock, sockaddr_in6*     addr);
bool ev_getpeername(socket_t sock, sockaddr_storage* addr);

/**
 * @brief 将网络地址转换为字符串形式的IP地址
 * @param af     地址族，必须为：AF_INET(IPv4)或者AF_INET6(IPv6)之一
 * @param src    sockaddr_in.sin_addr或者sockaddr_in6.sin6_addr之一
 * @param buf    存储字符串形式的IP地址的buf
 * @param buflen buf的长度
 * @return 成功返回true(或者IP地址字符串)，失败返回false(或者空字符串)
 */
bool        ev_inet_ntop(int32 af, const void* src, char* buf, size_t buflen);
std::string ev_inet_ntop(int32 af, const void* src);

/**
 * @brief 将字符串形式的IP地址转换为网络地址
 * @param af  地址族，必须为：AF_INET(ipv4)或者AF_INET6(ipv6)之一
 * @param src 字符串形式的IP地址
 * @param dst sockaddr_in.sin_addr或者sockaddr_in6.sin6_addr之一
 * @return 成功返回true，失败返回false
 */
bool ev_inet_pton(int32 af, const char* src, void* dst);

/**
 * @brief 将字符串形式的IP地址和端口号解析到对应的socket地址信息结构体中，支持IPv4和IPv6
 *        识别的格式如下：
 *          - IPv4Address
 *          - IPv4Address:port
 *          - IPv6Address
 *          - [IPv6Address]
 *          - [IPv6Address]:port
 *        如果没有给出对应的端口号，则默认为0
 * @param ip_port 字符串形式的IP地址和端口号
 * @param addr    保存转换结果的socket地址信息，对应IPv4、IPv6两个版本和通用版本
 * @return 成功返回true，失败返回false
 */
bool ev_parse_from_ip_and_port(const char* ip_port, sockaddr_in*      addr);
bool ev_parse_from_ip_and_port(const char* ip_port, sockaddr_in6*     addr);
bool ev_parse_from_ip_and_port(const char* ip_port, sockaddr_storage* addr);

/**
 * @brief 将socket地址信息解析为对应的字符串IP和端口号
 * @param addr     socket地址信息，一般为sockaddr_in、sockaddr_in6或者通用结构sockaddr_storage
 * @param out_ip   解析的IP
 * @param out_port 解析的端口号
 * @return 成功返回true，失败返回false
 */
bool ev_parse_from_sockaddr(const sockaddr_in*      addr, std::string& out_ip, uint16& out_port);
bool ev_parse_from_sockaddr(const sockaddr_in6*     addr, std::string& out_ip, uint16& out_port);
bool ev_parse_from_sockaddr(const sockaddr_storage* addr, std::string& out_ip, uint16& out_port);

/**
 * @brief 比较两个socket地址
 * @param addr1        socket地址1
 * @param addr2        socket地址2
 * @param include_port 比较时，是否包含端口
 * @return addr1 == addr2返回0
 *         addr1  < addr2返回-1
 *         addr1  > addr2返回1
 */
int32 ev_sockaddr_cmp(const sockaddr* addr1, const sockaddr* addr2, bool include_port = true);

/**
 * @brief 获取socket错误码(Windows平台调用WSAGetLastError()，其它平台直接返回errno)
 * @param
 * @return
 */
int32 ev_socket_errno(void);

/**
 * @brief 将socket错误码转换为字符串形式
 * @param socket_errno socket错误码
 * @return
 */
const char* ev_socket_errno_to_string(int32 socket_errno);

} // namespace socket_utils

//////////////////////////////////////////////////////////////////////////
// sockaddr*相关结构互转辅助函数
//////////////////////////////////////////////////////////////////////////
inline const sockaddr* ev_sockaddr_cast(const sockaddr_in* addr)
{
	return reinterpret_cast<const sockaddr*>(addr);
}

inline const sockaddr* ev_sockaddr_cast(const sockaddr_in6* addr)
{
	return reinterpret_cast<const sockaddr*>(addr);
}

inline const sockaddr* ev_sockaddr_cast(const sockaddr_storage* addr)
{
	return reinterpret_cast<const sockaddr*>(addr);
}

inline sockaddr* ev_sockaddr_cast(sockaddr_in* addr)
{
	return reinterpret_cast<sockaddr*>(addr);
}

inline sockaddr* ev_sockaddr_cast(sockaddr_in6* addr)
{
	return reinterpret_cast<sockaddr*>(addr);
}

inline sockaddr* ev_sockaddr_cast(sockaddr_storage* addr)
{
	return reinterpret_cast<sockaddr*>(addr);
}

inline const sockaddr_in* ev_sockaddr_in_cast(const sockaddr* addr)
{
	return reinterpret_cast<const sockaddr_in*>(addr);
}

inline const sockaddr_in* ev_sockaddr_in_cast(const sockaddr_storage* addr)
{
	return reinterpret_cast<const sockaddr_in*>(addr);
}

inline sockaddr_in* ev_sockaddr_in_cast(sockaddr* addr)
{
	return reinterpret_cast<sockaddr_in*>(addr);
}

inline sockaddr_in* ev_sockaddr_in_cast(sockaddr_storage* addr)
{
	return reinterpret_cast<sockaddr_in*>(addr);
}

inline const sockaddr_in6* ev_sockaddr_in6_cast(const sockaddr* addr)
{
	return reinterpret_cast<const sockaddr_in6*>(addr);
}

inline const sockaddr_in6* ev_sockaddr_in6_cast(const sockaddr_storage* addr)
{
	return reinterpret_cast<const sockaddr_in6*>(addr);
}

inline sockaddr_in6* ev_sockaddr_in6_cast(sockaddr* addr)
{
	return reinterpret_cast<sockaddr_in6*>(addr);
}

inline sockaddr_in6* ev_sockaddr_in6_cast(sockaddr_storage* addr)
{
	return reinterpret_cast<sockaddr_in6*>(addr);
}

inline const sockaddr_storage* ev_sockaddr_storage_cast(const sockaddr* addr)
{
	return reinterpret_cast<const sockaddr_storage*>(addr);
}

inline const sockaddr_storage* ev_sockaddr_storage_cast(const sockaddr_in* addr)
{
	return reinterpret_cast<const sockaddr_storage*>(addr);
}

inline const sockaddr_storage* ev_sockaddr_storage_cast(const sockaddr_in6* addr)
{
	return reinterpret_cast<const sockaddr_storage*>(addr);
}

inline sockaddr_storage* ev_sockaddr_storage_cast(sockaddr* addr)
{
	return reinterpret_cast<sockaddr_storage*>(addr);
}

inline sockaddr_storage* ev_sockaddr_storage_cast(sockaddr_in* addr)
{
	return reinterpret_cast<sockaddr_storage*>(addr);
}

inline sockaddr_storage* ev_sockaddr_storage_cast(sockaddr_in6* addr)
{
	return reinterpret_cast<sockaddr_storage*>(addr);
}

} // namespace evpp

#endif // __EV_SOCKET_H__