#include "test_socket.h"

#include "net/ev_poll.h"
#include "net/ev_socket.h"
#include "base/ev_assert.h"
#include "base/ev_thread.h"
#include "base/ev_byte_order.h"
#include "base/ev_async_logging.h"

using namespace evpp;

namespace test_socket {

// 服务器监听socket
socket_t g_listen_sock      = -1;
// 连接数
int32    g_connection_count = 0;

static void on_event_handler(ev_poll& poll_backend, socket_t sock, uint32 events)
{
	if (g_listen_sock == sock && (events & ev_poll::ET_READ))
	{
		// 有新连接到达
		sockaddr_in client_addr;
		socket_t    client_fd;

		while (true)
		{
			client_fd = socket_utils::ev_accept(g_listen_sock, &client_addr);
			if (client_fd >= 0)
			{
				// 设置通讯socket为非阻塞模式
				if (!socket_utils::ev_set_socket_nonblocking(client_fd))
				{
					ev_log_writef(ev_trace, "ev_set_socket_nonblocking() failed! sockfd = " EV_SOCK, client_fd);
					socket_utils::ev_close_socket(client_fd);
					continue;
				}

				// 打开通讯socket的TCP_NODELAY选项
				if (!socket_utils::ev_set_tcp_nodelay(client_fd, true))
				{
					ev_log_writef(ev_trace, "ev_set_tcp_nodelay() failed! sockfd = " EV_SOCK, client_fd);
					socket_utils::ev_close_socket(client_fd);
					continue;
				}

				// 设置socket的linger选项
				if (!socket_utils::ev_set_socket_linger(client_fd, true, 0))
				{
					ev_log_writef(ev_trace, "ev_set_socket_linger() failed! sockfd = " EV_SOCK, client_fd);
					socket_utils::ev_close_socket(client_fd);
					continue;
				}

				std::string ip;
				uint16      port;
				if (!socket_utils::ev_parse_from_sockaddr(&client_addr, ip, port))
				{
					ev_log_writef(ev_trace, "ev_parse_from_sockaddr() failed! sockfd = " EV_SOCK, client_fd);
					socket_utils::ev_close_socket(client_fd);
					continue;
				}

				int32 default_send_buf_len = 0;
				if (!socket_utils::ev_get_send_buffer_size(client_fd, default_send_buf_len))
				{
					ev_log_writef(ev_trace, "ev_get_send_buffer_size() failed! sockfd = " EV_SOCK, client_fd);
					socket_utils::ev_close_socket(client_fd);
					continue;
				}

				int32 default_recv_buf_len = 0;
				if (!socket_utils::ev_get_recv_buffer_size(client_fd, default_recv_buf_len))
				{
					ev_log_writef(ev_trace, "ev_get_recv_buffer_size() failed! sockfd = " EV_SOCK, client_fd);
					socket_utils::ev_close_socket(client_fd);
					continue;
				}

				bool tcp_nodelay_falg = false;
				if (!socket_utils::ev_get_tcp_nodelay(client_fd, tcp_nodelay_falg))
				{
					ev_log_writef(ev_trace, "ev_get_tcp_nodelay() failed! sockfd = " EV_SOCK, client_fd);
					socket_utils::ev_close_socket(client_fd);
					continue;
				}

				bool   linger_onoff = false;
				uint16 linger_time = 0;
				if (!socket_utils::ev_get_socket_linger(client_fd, linger_onoff, linger_time))
				{
					ev_log_writef(ev_trace, "ev_get_socket_linger() failed! sockfd = " EV_SOCK, client_fd);
					socket_utils::ev_close_socket(client_fd);
					continue;
				}

				// 加入后端，监听可读
				if (!poll_backend.add_event(client_fd, ev_poll::ET_READ))
				{
					ev_log_writef(ev_trace, "add_event() failed! sockfd = " EV_SOCK, client_fd);
					socket_utils::ev_close_socket(client_fd);
					continue;
				}

				ev_log_writef(ev_trace, "Accept a new client(%s:%hu), sendBuf = %d, recvBuf = %d, "
				                 "nodelayFlag = %d, lingerOnOff = %d, lingerTime = %hu, connectionCount = %d",
					ip.c_str(), port, default_send_buf_len, default_recv_buf_len, tcp_nodelay_falg ? 1 : 0, 
					linger_onoff ? 1 : 0, linger_time, ++g_connection_count);
			}
			else if (-2 == client_fd)
			{
				// 没有新客户端连接，直接跳出循环
				break;
			}
			else
			{
				// accept出错，直接跳出循环
				int32       error_code = socket_utils::ev_socket_errno();
				const char* error_msg  = socket_utils::ev_socket_errno_to_string(error_code);

				ev_log_writef(ev_error, "Accept new client failed, errno = %d, error msg = %s", error_code, error_msg);

				break;
			}
		}

		return;
	}

	const uint32 BUFFER_SIZE             = 1024;
	char         msg_buffer[BUFFER_SIZE] = {0};
	int32        recv_len                = 0;
	int32        send_len                = 0;
	bool         need_close              = false;

	// recv
	if (!need_close)
	{
		recv_len = socket_utils::ev_recv(sock, msg_buffer, BUFFER_SIZE);
		if (0 == recv_len || -1 == recv_len)
		{
			need_close = true;
		}
	}

	// send
	if (!need_close && recv_len > 0)
	{
		send_len = socket_utils::ev_send(sock, msg_buffer, recv_len);
		if (-1 == send_len)
		{
			need_close = true;
		}
	}

	if (need_close)
	{
		std::string ip;
		uint16      port;
		sockaddr_in peer_addr;

		socket_utils::ev_getpeername(sock, &peer_addr);
		socket_utils::ev_parse_from_sockaddr(&peer_addr, ip, port);

		ev_log_writef(ev_trace, "A client(%s:%hu) is disconnected from server, connectionCount = %d", ip.c_str(), port, --g_connection_count);

		// 清空监听的事件
		poll_backend.del_event(sock, ev_poll::ET_READ | ev_poll::ET_WRITE);

		// 关闭socket
		socket_utils::ev_close_socket(sock);
	}
}

void test_tcp_echo_server(void)
{
	int32                       poll_ret     = 0;
	int32                       poll_timeout = 3000;
	int32                       error_code   = 0;
	const char*                 error_msg    = NULL;
	sockaddr_in                 server_addr;
	ev_poll                     poll_backend;
	ev_poll::active_event_array event_array;

	// 创建服务器监听socket
	g_listen_sock = socket_utils::ev_create_socket(AF_INET, SOCK_STREAM);
	if (-1 == g_listen_sock)
	{
		error_code = socket_utils::ev_socket_errno();
		error_msg  = socket_utils::ev_socket_errno_to_string(error_code);

		ev_log_writef(ev_error, "Create listen socket failed, errno = %d, error msg = %s", error_code, error_msg);

		goto error_handle;
	}

	// 设置监听socket为非阻塞模式
	if (!socket_utils::ev_set_socket_nonblocking(g_listen_sock))
	{
		error_code = socket_utils::ev_socket_errno();
		error_msg  = socket_utils::ev_socket_errno_to_string(error_code);

		ev_log_writef(ev_error, "Set listen socket nonblocking failed, errno = %d, error msg = %s", error_code, error_msg);

		goto error_handle;
	}

	// 设置监听socket在关闭时，地址可以被其它程序立刻使用
	if (!socket_utils::ev_set_socket_reuseable(g_listen_sock))
	{
		error_code = socket_utils::ev_socket_errno();
		error_msg  = socket_utils::ev_socket_errno_to_string(error_code);

		ev_log_writef(ev_error, "Set listen socket reuse address failed, errno = %d, error msg = %s", error_code, error_msg);

		goto error_handle;
	}

	// 设置接收的IP地址和监听端口
	if (!socket_utils::ev_parse_from_ip_and_port("0.0.0.0:8088", &server_addr))
	{
		ev_log_write(ev_error, "Parse ip and port failed!");
		goto error_handle;
	}

	// 绑定socket
	if (!socket_utils::ev_bind(g_listen_sock, &server_addr))
	{
		error_code = socket_utils::ev_socket_errno();
		error_msg  = socket_utils::ev_socket_errno_to_string(error_code);

		ev_log_writef(ev_error, "Bind socket failed, errno = %d, error msg = %s", error_code, error_msg);

		goto error_handle;
	}

	// 监听socket
	if (!socket_utils::ev_listen(g_listen_sock))
	{
		error_code = socket_utils::ev_socket_errno();
		error_msg  = socket_utils::ev_socket_errno_to_string(error_code);

		ev_log_writef(ev_error, "Listen socket failed, errno = %d, error msg = %s", error_code, error_msg);

		goto error_handle;
	}

	// 将监听socket加入后端，检测客户端连接
	if (!poll_backend.add_event(g_listen_sock, ev_poll::ET_READ))
	{
		ev_log_write(ev_error, "Add listen socket to poll backend failed!");
		goto error_handle;
	}

	ev_log_write(ev_trace, "--- !!!TCP echo server has been started!!! ---");

	while (true)
	{
		// 设置3秒超时
		poll_ret = poll_backend.dispatch(poll_timeout);
		if (poll_ret >= 1)
		{
			// 处理活跃事件
			event_array = poll_backend.get_active_event_array();
			for (size_t idx = 0; idx != event_array.size(); ++idx)
			{
				const ev_poll::active_event& ev = event_array[idx];
				on_event_handler(poll_backend, ev.first, ev.second);
			}

			continue;
		}
		else if (0 == poll_ret)
		{
			// 超时
			//ev_log_writef(ev_trace, "Poll timeout, timeout = %d(ms). [thread_id:%u, thread_name:%s]",
			//	poll_timeout, this_thread::thread_id(), this_thread::thread_name());
			continue;
		}
		else
		{
			error_code = socket_utils::ev_socket_errno();
			error_msg  = socket_utils::ev_socket_errno_to_string(error_code);

			ev_log_writef(ev_error, "Poll failed! error_code = %d, error_msg = %s [thread_id:%u, thread_name:%s]",
				error_code, error_msg, this_thread::thread_id(), this_thread::thread_name());

			break;
		}
	}

	ev_log_write(ev_trace, "--- !!!TCP echo server has been stopped!!! ---");

error_handle:
	if (g_listen_sock != -1)
	{
		socket_utils::ev_close_socket(g_listen_sock);
	}
}

} // namespace test_socket