#include "net/ev_socket.h"
#include "base/ev_byte_order.h"
#include "event2/util.h"
#include "util-internal.h"

#if defined(__EV_WINDOWS__)
    #include <mstcpip.h>
#endif // defined(__EV_WINDOWS__)

#include <cstring>
#include <cassert>

namespace evpp    {
namespace details {

/**
 * @brief 绑定端口
 * @param sock    socket
 * @param addr    socket地址信息
 * @param addrlen socket地址信息的长度
 * @return 成功返回true，失败返回false
 */
static inline bool ev_bind_internal(socket_t sock, const sockaddr* addr, socklen_t addrlen)
{
	assert(sock != -1 && addr != NULL && addrlen > 0 && "Libevpp fatal error! Invalid parameters!");

	int32 ret = bind(sock, addr, addrlen);
	return (-1 == ret ? false : true);
}

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
static inline int32 ev_connect_internal(socket_t sock, const sockaddr* addr, socklen_t addrlen)
{
	assert(sock != -1 && addr != NULL && addrlen > 0 && "Libevpp fatal error! Invalid parameters!");

	int32 ret = evutil_socket_connect_(reinterpret_cast<evutil_socket_t*>(&sock), addr, addrlen);
	return (2 == ret ? -1 : ret);
}

/**
 * @brief 接受客户端连接
 * @param sock 监听socket
 * @param addr 客户端的socket地址信息，一般为sockaddr_in、sockaddr_in6或者通用结构sockaddr_storage(可选，可以为NULL)
 * @return 成功返回与客户端建立好连接的socket(>= 0)，失败返回值对应含义如下所示：
 *          -1 : 其它错误，调用ev_socket_errno()获取错误码
 *          -2 : 非阻塞监听socket，当前已无新连接
 *         无其它情况返回值
 */
static inline socket_t ev_accept_internal(socket_t sock, sockaddr* addr, socklen_t* addrlen)
{
	assert(sock != -1 && "Libevpp fatal error! Invalid parameters!");

	socket_t client = accept(sock, addr, addrlen);
	if (client >= 0)
	{
		return client;
	}

	int32 error_code = socket_utils::ev_socket_errno();
	if (EVUTIL_ERR_ACCEPT_RETRIABLE(error_code))
	{
		return -2;
	}

	return -1;
}

/**
 * @brief 获取一个已绑定或者已连接的socket的本地地址
 * @param sock    socket
 * @param addr    socket地址信息
 * @param addrlen socket地址信息的长度
 * @return 成功返回true，失败返回false
 */
static inline bool ev_getsockname_internal(socket_t sock, sockaddr* addr, socklen_t* addrlen)
{
	assert(sock != -1 && addr != NULL && addrlen != NULL && *addrlen > 0 && "Libevpp fatal error! Invalid parameters!");

	int32 ret = getsockname(sock, addr, addrlen);
	return (-1 == ret ? false : true);
}

/**
 * @brief 获取一个已连接的socket的对端地址
 * @param sock    socket
 * @param addr    socket地址信息
 * @param addrlen socket地址信息的长度
 * @return 成功返回true，失败返回false
 */
static inline bool ev_getpeername_internal(socket_t sock, sockaddr* addr, socklen_t* addrlen)
{
	assert(sock != -1 && addr != NULL && addrlen != NULL && *addrlen > 0 && "Libevpp fatal error! Invalid parameters!");

	int32 ret = getpeername(sock, addr, addrlen);
	return (-1 == ret ? false : true);
}

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
 * @param addr    保存转换结果的socket地址信息
 * @param addrlen socket地址信息的长度
 * @return 成功返回true，失败返回false
 */
static inline bool ev_parse_from_ip_and_port_internal(const char* ip_port, sockaddr* addr, int32* addrlen)
{
	assert(ip_port != NULL && addr != NULL && addrlen != NULL && *addrlen > 0 && "Libevpp fatal error! Invalid parameters!");

	int32 ret = evutil_parse_sockaddr_port(ip_port, addr, addrlen);
	return (-1 == ret ? false : true);
}

}

//////////////////////////////////////////////////////////////////////////
// socket_utils
//////////////////////////////////////////////////////////////////////////
namespace socket_utils {

bool ev_socket_init(void)
{
#if defined(__EV_WINDOWS__)
	WSADATA  wsaData;
	WSADATA* lpwsaData = &wsaData;

	// 请求2.2版本的WinSock库
	WORD wVersionRequested = MAKEWORD(2, 2);
	int nResult            = WSAStartup(wVersionRequested, lpwsaData);
	if (nResult != 0)
	{
		return false;
	}

	if (LOBYTE(lpwsaData->wVersion) != 2 || HIBYTE(lpwsaData->wVersion) != 2)
	{
		WSACleanup();
		return false;
	}

	return true;
#else
	return true;
#endif // defined(__EV_WINDOWS__)
}

bool ev_socket_destroy(void)
{
#if defined(__EV_WINDOWS__)
	return (0 == WSACleanup());
#else
	return true;
#endif // defined(__EV_WINDOWS__)
}

bool ev_set_send_buffer_size(socket_t sock, int32 buffer_size)
{
	assert(sock != -1 && buffer_size >= 0 && "Libevpp fatal error! Invalid parameters!");

	int32 ret = setsockopt(sock, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&buffer_size), sizeof(buffer_size));
	return (-1 == ret ? false : true);
}

bool ev_get_send_buffer_size(socket_t sock, int32& buffer_size)
{
	assert(sock != -1 && "Libevpp fatal error! Invalid parameters!");

	buffer_size = -1;

	socklen_t len = sizeof(buffer_size);
	int32     ret = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&buffer_size), &len);
	return (-1 == ret ? false : true);
}

bool ev_set_recv_buffer_size(socket_t sock, int32 buffer_size)
{
	assert(sock != -1 && buffer_size >= 0 && "Libevpp fatal error! Invalid parameters!");

	int32 ret = setsockopt(sock, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&buffer_size), sizeof(buffer_size));
	return (-1 == ret ? false : true);
}

bool ev_get_recv_buffer_size(socket_t sock, int32& buffer_size)
{
	assert(sock != -1 && "Libevpp fatal error! Invalid parameters!");

	buffer_size = -1;

	socklen_t len = sizeof(buffer_size);
	int32     ret = getsockopt(sock, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&buffer_size), &len);
	return (-1 == ret ? false : true);
}

bool ev_set_tcp_nodelay(socket_t sock, bool flag)
{
	assert(sock != -1 && "Libevpp fatal error! Invalid parameters!");

	int32 opt = (flag ? 1 : 0);
	int32 ret = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&opt), sizeof(opt));
	return (-1 == ret ? false : true);
}

bool ev_get_tcp_nodelay(socket_t sock, bool& flag)
{
	assert(sock != -1 && "Libevpp fatal error! Invalid parameters!");

	int32     opt = -1;
	socklen_t len = sizeof(opt);
	int32     ret = getsockopt(sock, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&opt), &len);
	if (0 == ret)
	{
		flag = (1 == opt ? true : false);
	}

	return (-1 == ret ? false : true);
}

bool ev_set_socket_linger(socket_t sock, bool on_off, uint16 linger_time)
{
	assert(sock != -1 && "Libevpp fatal error! Invalid parameters!");

	linger ll;
	ll.l_onoff  = (on_off ? 1 : 0);
	ll.l_linger = linger_time;

	int32 ret = setsockopt(sock, SOL_SOCKET, SO_LINGER, reinterpret_cast<char*>(&ll), sizeof(ll));
	return (-1 == ret ? false : true);
}

bool ev_get_socket_linger(socket_t sock, bool& on_off, uint16& linger_time)
{
	assert(sock != -1 && "Libevpp fatal error! Invalid parameters!");

	linger    ll  = {0, 0};
	socklen_t len = sizeof(ll);
	int32     ret = getsockopt(sock, SOL_SOCKET, SO_LINGER, reinterpret_cast<char*>(&ll), &len);
	if (0 == ret)
	{
		on_off      = (0 == ll.l_onoff ? false : true);
		linger_time = static_cast<uint16>(ll.l_linger);
	}

	return (-1 == ret ? false : true);
}

bool ev_set_socket_nonblocking(socket_t sock)
{
	assert(sock != -1 && "Libevpp fatal error! Invalid parameters!");

	int32 ret = evutil_make_socket_nonblocking(sock);
	return (-1 == ret ? false : true);
}

bool ev_set_socket_reuseable(socket_t sock)
{
	assert(sock != -1 && "Libevpp fatal error! Invalid parameters!");

	int32 ret = evutil_make_listen_socket_reuseable(sock);
	return (-1 == ret ? false : true);
}

socket_t ev_create_socket(int32 af, int32 type, int32 protocol)
{
	return static_cast<socket_t>(socket(af, type, protocol));
}

bool ev_create_socketpair(socket_t fds[2], bool is_nonblocking)
{
	int32 ret;

	fds[0] = fds[1] = -1;

#if defined(__EV_WINDOWS__)
	ret = static_cast<int32>(evutil_socketpair(AF_INET, SOCK_STREAM, 0, fds));
#else
	ret = static_cast<int32>(evutil_socketpair(AF_UNIX, SOCK_STREAM, 0, fds));
#endif // defined(__EV_WINDOWS__)
	if (-1 == ret)
	{
		goto error_handle;
	}

	if (is_nonblocking)
	{
		if (!ev_set_socket_nonblocking(fds[0]) || !ev_set_socket_nonblocking(fds[1]))
		{
			goto error_handle;
		}
	}

	return true;

error_handle:
	if (fds[0] != -1)
	{
		ev_close_socket(fds[0]);
	}

	if (fds[1] != -1)
	{
		ev_close_socket(fds[1]);
	}

	fds[0] = fds[1] = -1;

	return false;
}

bool ev_bind(socket_t sock, const sockaddr_in* addr)
{
	return details::ev_bind_internal(sock, ev_sockaddr_cast(addr), sizeof(sockaddr_in));
}

bool ev_bind(socket_t sock, const sockaddr_in6* addr)
{
	return details::ev_bind_internal(sock, ev_sockaddr_cast(addr), sizeof(sockaddr_in6));
}

bool ev_bind(socket_t sock, const sockaddr_storage* addr)
{
	return details::ev_bind_internal(sock, ev_sockaddr_cast(addr), sizeof(sockaddr_storage));
}

bool ev_listen(socket_t sock, int32 backlog)
{
	assert(sock != -1 && backlog >= 0 && "Libevpp fatal error! Invalid parameters!");

	int32 ret = listen(sock, backlog);
	return (-1 == ret ? false : true);
}

int32 ev_connect(socket_t sock, const sockaddr_in* addr)
{
	return details::ev_connect_internal(sock, ev_sockaddr_cast(addr), sizeof(sockaddr_in));
}

int32 ev_connect(socket_t sock, const sockaddr_in6* addr)
{
	return details::ev_connect_internal(sock, ev_sockaddr_cast(addr), sizeof(sockaddr_in6));
}

int32 ev_connect(socket_t sock, const sockaddr_storage* addr)
{
	return details::ev_connect_internal(sock, ev_sockaddr_cast(addr), sizeof(sockaddr_storage));
}

int32 ev_finished_connecting(socket_t sock)
{
	assert(sock != -1 && "Libevpp fatal error! Invalid parameters!");
	return evutil_socket_finished_connecting_(sock);
}

socket_t ev_accept(socket_t sock, sockaddr_in* addr)
{
	socklen_t addrlen = sizeof(sockaddr_in);
	return details::ev_accept_internal(sock, ev_sockaddr_cast(addr), (NULL == addr ? NULL : &addrlen));
}

socket_t ev_accept(socket_t sock, sockaddr_in6* addr)
{
	socklen_t addrlen = sizeof(sockaddr_in6);
	return details::ev_accept_internal(sock, ev_sockaddr_cast(addr), (NULL == addr ? NULL : &addrlen));
}

socket_t ev_accept(socket_t sock, sockaddr_storage* addr)
{
	socklen_t addrlen = sizeof(sockaddr_storage);
	return details::ev_accept_internal(sock, ev_sockaddr_cast(addr), (NULL == addr ? NULL : &addrlen));
}

int32 ev_send(socket_t sock, const void* data, uint32 datalen, int32 flags)
{
	assert(sock != -1 && data != NULL && "Libevpp fatal error! Invalid parameters!");

#if defined(__EV_WINDOWS__)
	int32 sendlen = send(sock, static_cast<const char*>(data), datalen, flags);
#else
	int32 sendlen = static_cast<int32>(send(sock, data, datalen, flags));
#endif // defined(__EV_WINDOWS__)
	if (sendlen >= 0)
	{
		return sendlen;
	}

	int32 error_code = socket_utils::ev_socket_errno();
	if (EVUTIL_ERR_RW_RETRIABLE(error_code))
	{
		return -2;
	}

	return -1;
}

int32 ev_recv(socket_t sock, void* buf, uint32 buflen, int32 flags)
{
	assert(sock != -1 && buf != NULL && buflen > 0 && "Libevpp fatal error! Invalid parameters!");

#if defined(__EV_WINDOWS__)
	int32 recvlen = recv(sock, static_cast<char*>(buf), buflen, flags);
#else
	int32 recvlen = static_cast<int32>(recv(sock, buf, buflen, flags));
#endif // defined(__EV_WINDOWS__)
	if (recvlen >= 0)
	{
		return recvlen;
	}

	int32 error_code = socket_utils::ev_socket_errno();
	if (EVUTIL_ERR_RW_RETRIABLE(error_code))
	{
		return -2;
	}

	return -1;
}

bool ev_close_socket(socket_t sock)
{
	assert(sock != -1 && "Libevpp fatal error! Invalid parameters!");

	int32 ret = evutil_closesocket(sock);
	return (-1 == ret ? false : true);
}

bool ev_shutdown(socket_t sock, shutdown_what what)
{
	assert(sock != -1 && what >= SW_READ && what <= SW_BOTH && "Libevpp fatal error! Invalid parameters!");

	int32 ret = shutdown(sock, static_cast<int32>(what));
	return (-1 == ret ? false : true);
}

bool ev_getsockname(socket_t sock, sockaddr_in* addr)
{
	socklen_t addrlen = sizeof(sockaddr_in);
	return details::ev_getsockname_internal(sock, ev_sockaddr_cast(addr), &addrlen);
}

bool ev_getsockname(socket_t sock, sockaddr_in6* addr)
{
	socklen_t addrlen = sizeof(sockaddr_in6);
	return details::ev_getsockname_internal(sock, ev_sockaddr_cast(addr), &addrlen);
}

bool ev_getsockname(socket_t sock, sockaddr_storage* addr)
{
	socklen_t addrlen = sizeof(sockaddr_storage);
	return details::ev_getsockname_internal(sock, ev_sockaddr_cast(addr), &addrlen);
}

bool ev_getpeername(socket_t sock, sockaddr_in* addr)
{
	socklen_t addrlen = sizeof(sockaddr_in);
	return details::ev_getpeername_internal(sock, ev_sockaddr_cast(addr), &addrlen);
}

bool ev_getpeername(socket_t sock, sockaddr_in6* addr)
{
	socklen_t addrlen = sizeof(sockaddr_in6);
	return details::ev_getpeername_internal(sock, ev_sockaddr_cast(addr), &addrlen);
}

bool ev_getpeername(socket_t sock, sockaddr_storage* addr)
{
	socklen_t addrlen = sizeof(sockaddr_storage);
	return details::ev_getpeername_internal(sock, ev_sockaddr_cast(addr), &addrlen);
}

bool ev_inet_ntop(int32 af, const void* src, char* buf, size_t buflen)
{
	assert((AF_INET == af || AF_INET6 == af) && src != NULL && buf != NULL && buflen > 0 && "Libevpp fatal error! Invalid parameters!");

	const char* ret = evutil_inet_ntop(af, src, buf, buflen);
	return (NULL == ret ? false : true);
}

std::string ev_inet_ntop(int32 af, const void* src)
{
	char buf[128];
	if (!ev_inet_ntop(af, src, buf, sizeof(buf)))
	{
		return std::string();
	}

	return std::string(buf);
}

bool ev_inet_pton(int32 af, const char* src, void* dst)
{
	assert((AF_INET == af || AF_INET6 == af) && src != NULL && dst != NULL && "Libevpp fatal error! Invalid parameters!");

	int32 ret = evutil_inet_pton(af, src, dst);
	return (1 == ret ? true : false);
}

bool ev_parse_from_ip_and_port(const char* ip_port, sockaddr_in* addr)
{
	int32 addrlen = sizeof(sockaddr_in);
	return details::ev_parse_from_ip_and_port_internal(ip_port, ev_sockaddr_cast(addr), &addrlen);
}

bool ev_parse_from_ip_and_port(const char* ip_port, sockaddr_in6* addr)
{
	int32 addrlen = sizeof(sockaddr_in6);
	return details::ev_parse_from_ip_and_port_internal(ip_port, ev_sockaddr_cast(addr), &addrlen);
}

bool ev_parse_from_ip_and_port(const char* ip_port, sockaddr_storage* addr)
{
	int32 addrlen = sizeof(sockaddr_storage);
	return details::ev_parse_from_ip_and_port_internal(ip_port, ev_sockaddr_cast(addr), &addrlen);
}

bool ev_parse_from_sockaddr(const sockaddr_in* addr, std::string& out_ip, uint16& out_port)
{
	return ev_parse_from_sockaddr(ev_sockaddr_storage_cast(addr), out_ip, out_port);
}

bool ev_parse_from_sockaddr(const sockaddr_in6* addr, std::string& out_ip, uint16& out_port)
{
	return ev_parse_from_sockaddr(ev_sockaddr_storage_cast(addr), out_ip, out_port);
}

bool ev_parse_from_sockaddr(const sockaddr_storage* addr, std::string& out_ip, uint16& out_port)
{
	const sockaddr* caddr = ev_sockaddr_cast(addr);
	assert(caddr != NULL && (AF_INET == caddr->sa_family || AF_INET6 == caddr->sa_family) && "Libevpp fatal error! Invalid parameters!");

	char ip_buf[128];
	if (AF_INET == caddr->sa_family)
	{
		// IPv4
		const sockaddr_in* addr_in = ev_sockaddr_in_cast(caddr);
		if (!socket_utils::ev_inet_ntop(AF_INET, &addr_in->sin_addr, ip_buf, sizeof(ip_buf)))
		{
			return false;
		}

		out_ip   = ip_buf;
		out_port = ev_byte_order::ntoh16(static_cast<uint16>(addr_in->sin_port));

		return true;
	}
	else if (AF_INET6 == caddr->sa_family)
	{
		// IPv6
		const sockaddr_in6* addr_in6 = ev_sockaddr_in6_cast(caddr);
		if (!socket_utils::ev_inet_ntop(AF_INET6, &addr_in6->sin6_addr, ip_buf, sizeof(ip_buf)))
		{
			return false;
		}

		out_ip   = ip_buf;
		out_port = ev_byte_order::ntoh16(static_cast<uint16>(addr_in6->sin6_port));

		return true;
	}
	else
	{
		return false;
	}
}

int32 ev_sockaddr_cmp(const sockaddr* addr1, const sockaddr* addr2, bool include_port)
{
	assert(addr1 != NULL && addr2 != NULL && "Libevpp fatal error! Invalid parameters!");
	return evutil_sockaddr_cmp(addr1, addr2, include_port);
}

int32 ev_socket_errno(void)
{
	return static_cast<int32>(EVUTIL_SOCKET_ERROR());
}

const char* ev_socket_errno_to_string(int32 socket_errno)
{
#if defined(__EV_WINDOWS__)
	return evutil_socket_error_to_string(socket_errno);
#else
	static ev_thread_local char error_msg_buffer[256];

	memset(error_msg_buffer, 0, sizeof(error_msg_buffer));

	strerror_r(socket_errno, error_msg_buffer, sizeof(error_msg_buffer));

	return error_msg_buffer;
#endif // defined(__EV_WINDOWS__)
}

} // namespace socket_utils
} // namespace evpp