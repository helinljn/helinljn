#include "test_libevent.h"

#include "event2/event.h"
#include "event2/buffer.h"
#include "event2/bufferevent.h"
#include "event2/util.h"
#include "event2/thread.h"
#include "event2/listener.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <csignal>

namespace test_libevent {

static int connection_count = 0;

//////////////////////////////////////////////////////////////////////////
// declares
//////////////////////////////////////////////////////////////////////////
// 功能：有新连接的回调
void accept_callback(evconnlistener* evlistener, evutil_socket_t sock, sockaddr* sock_addr, int sock_addr_len, void* arg);

// 功能：socket有数据可读的回调
void socket_read_callback(bufferevent* bev, void* arg);

// 功能：socket上有事件发生的回调
void socket_event_callback(bufferevent* bev, short events, void* arg);

// 功能：信号回调
void signal_callback(evutil_socket_t sockfd, short events, void* arg);

// 功能：超时回调
void timeout_callback(evutil_socket_t sockfd, short events, void* arg);

//////////////////////////////////////////////////////////////////////////
// entry function
//////////////////////////////////////////////////////////////////////////
void test_libevent_interface(void)
{
	event_config*   ev_config         = NULL;
	event_base*     ev_base           = NULL;
	evconnlistener* ev_serverlistener = NULL;
	event*          ev_sig            = NULL;
	event*          ev_timeout        = NULL;

	/* Generate n bytes of secure pseudorandom data */
	{
		ev_uint32_t random_num = 0;
		int         test_times = 10, idx;

		for (idx = 0; idx != test_times; ++idx)
		{
			evutil_secure_rng_get_bytes(&random_num, sizeof(random_num));
			printf("random_num = %u\n", random_num);
		}
	}

	/* evbuffer_* test */
	{
		evbuffer* buf = evbuffer_new();
		if (!buf)
		{
			printf("error:evbuffer_new!\n");
			goto exit_handle;
		}

		const char*  content_str = "01234567890123456789012345678901234567890123456789";
		const size_t content_len = strlen(content_str);
		const size_t add_times   = 20;

		int add_result;
		for (size_t idx = 0; idx != add_times; ++idx)
		{
			add_result = evbuffer_add(buf, content_str, content_len);
			if (-1 == add_result)
			{
				printf("error:evbuffer_add!\n");
				goto exit_handle;
			}
		}

		size_t buflen = evbuffer_get_length(buf);
		if (buflen != content_len * add_times)
		{
			printf("error:evbuffer_get_length!\n");
			goto exit_handle;
		}

		char content_buf[128];
		int read_result;
		for (size_t idx = 0; idx != add_times; ++idx)
		{
			read_result = (int)evbuffer_copyout(buf, content_buf, 50);
			if (-1 == read_result || read_result != 50)
			{
				printf("error:evbuffer_copyout!\n");
				goto exit_handle;
			}

			if (-1 == evbuffer_drain(buf, 50))
			{
				printf("error:evbuffer_drain!\n");
				goto exit_handle;
			}
		}

		evbuffer_free(buf);
	}

	/* Initalize the event library */
	{
		ev_config = event_config_new();
		if (NULL == ev_config)
		{
			printf("error:event_config_new!\n");
			goto exit_handle;
		}

		ev_base = event_base_new_with_config(ev_config);
		if (NULL == ev_base)
		{
			printf("error:event_base_new_with_config!\n");
			goto exit_handle;
		}
	}

	/* Initalize listener */
	{
		const char* ip_and_port     = "0.0.0.0:8088";
		int         server_addr_len = sizeof(sockaddr_in);
		sockaddr_in server_addr;

		if (evutil_parse_sockaddr_port(ip_and_port, (sockaddr*)&server_addr, &server_addr_len))
		{
			printf("error:evutil_parse_sockaddr_port!\n");
			goto exit_handle;
		}

		ev_serverlistener = evconnlistener_new_bind(
			ev_base,
			accept_callback,
			NULL,
			LEV_OPT_CLOSE_ON_FREE | LEV_OPT_CLOSE_ON_EXEC | LEV_OPT_REUSEABLE | LEV_OPT_THREADSAFE,
			256,
			(sockaddr*)&server_addr,
			server_addr_len
		);

		if (NULL == ev_serverlistener)
		{
			printf("error:evconnlistener_new_bind!\n");
			goto exit_handle;
		}
	}

	/* Register signal SIGINT handler event */
	{
		// 为了将ev做为参数传输过去
		ev_sig = evsignal_new(NULL, -1, NULL, NULL);
		if (NULL == ev_sig)
		{
			printf("error:evsignal_new!\n");
			goto exit_handle;
		}

		if (evsignal_assign(ev_sig, ev_base, SIGINT, signal_callback, ev_sig))
		{
			printf("error:evsignal_assign!\n");
			goto exit_handle;
		}

		if (evsignal_add(ev_sig, NULL))
		{
			printf("error:evsignal_add!\n");
			goto exit_handle;
		}
	}

	/* Register a timeout event */
	{
		timeval tv;
		evutil_timerclear(&tv);
		tv.tv_sec = 5;

		// one-time timeout event
		//ev_timeout = evtimer_new(NULL, NULL, NULL);
		//if (NULL == ev_timeout)
		//{
		//	printf("error:event_new!\n");
		//	goto exit_handle;
		//}
		//if (evtimer_assign(ev_timeout, ev_base, timeout_callback, ev_timeout))
		//{
		//	printf("error:event_assign!\n");
		//	goto exit_handle;
		//}
		//if (evtimer_add(ev_timeout, &tv))
		//{
		//	printf("error:event_add!\n");
		//	goto exit_handle;
		//}

		// persist timeout event
		ev_timeout = event_new(NULL, -1, 0, NULL, NULL);
		if (NULL == ev_timeout)
		{
			printf("error:event_new!\n");
			goto exit_handle;
		}
		if (event_assign(ev_timeout, ev_base, -1, EV_PERSIST, timeout_callback, ev_timeout))
		{
			printf("error:event_assign!\n");
			goto exit_handle;
		}
		if (event_add(ev_timeout, &tv))
		{
			printf("error:event_add!\n");
			goto exit_handle;
		}
	}

	/* Get all supported methods */
	{
		const char** all_methods = event_get_supported_methods();
		while(all_methods && *all_methods)
		{
			printf("%s\t", *all_methods++);
		}
		printf("\n");

		printf("current method:%s\n", event_base_get_method(ev_base));
	}

	printf("Server started...\n");

	/* Run the event loop */
	event_base_dispatch(ev_base);

exit_handle:
	if (ev_config)
		event_config_free(ev_config);

	if (ev_serverlistener)
		evconnlistener_free(ev_serverlistener);

	if (ev_sig)
		event_free(ev_sig);

	if (ev_timeout)
		event_free(ev_timeout);

	if (ev_base)
		event_base_free(ev_base);

	printf("Server stopped...\n");
}

//////////////////////////////////////////////////////////////////////////
// implementations
//////////////////////////////////////////////////////////////////////////
void accept_callback(evconnlistener* evlistener, evutil_socket_t sock, sockaddr* sock_addr, int sock_addr_len, void* arg)
{
	event_base*   ev_base     = evconnlistener_get_base(evlistener);
	sockaddr_in*  client_addr = (sockaddr_in*)sock_addr;
	char          ip_buffer[32];

	// 获取客户端的IP地址
	evutil_inet_ntop(AF_INET, &client_addr->sin_addr, ip_buffer, sizeof(ip_buffer));

	// 设置了BEV_OPT_CLOSE_ON_FREE标志后，调用bufferevent_free()会自动关闭socket
	bufferevent* client_bufevent = bufferevent_socket_new(ev_base, sock, BEV_OPT_CLOSE_ON_FREE);
	if (NULL == client_bufevent)
	{
		printf("error:bufferevent_socket_new!\n");
		evutil_closesocket(sock);
		return;
	}

	bufferevent_setcb(client_bufevent, socket_read_callback, NULL, socket_event_callback, NULL);

	if (bufferevent_enable(client_bufevent, EV_READ))
	{
		printf("error:bufferevent_enable!\n");
		bufferevent_free(client_bufevent);
		return;
	}

	printf("client connect:(%d:%s), current connection count = %d\n", sock, ip_buffer, ++connection_count);
}

void socket_read_callback(bufferevent* bev, void* arg)
{
	const size_t BUFFER_SIZE = 1024;
	char         recvbuf[BUFFER_SIZE];

	size_t recvlen = bufferevent_read(bev, recvbuf, BUFFER_SIZE - 1);
	recvbuf[recvlen] = '\0';

	bufferevent_write(bev, recvbuf,  recvlen);
}

void socket_event_callback(bufferevent* bev, short events, void* arg)
{
	evutil_socket_t sock      = bufferevent_getfd(bev);
	const char*     eventtype = NULL;
	const char*     reason    = NULL;

	if (events & BEV_EVENT_READING)
		eventtype = "BEV_EVENT_READING";
	else if (events & BEV_EVENT_WRITING)
		eventtype = "BEV_EVENT_WRITING";
	else
		eventtype = "UNKNOW";

	if (events & BEV_EVENT_EOF)
		reason = "BEV_EVENT_EOF";
	else if (events & BEV_EVENT_ERROR)
		reason = "BEV_EVENT_ERROR";
	else if (events & BEV_EVENT_TIMEOUT)
		reason = "BEV_EVENT_TIMEOUT";
	else
		reason = "UNKNOW";

	printf("client disconnect:(%d:%s:%s), current connection count = %d\n", sock, eventtype, reason, --connection_count);

	bufferevent_free(bev);
}

void signal_callback(evutil_socket_t sockfd, short events, void* arg)
{
	static int  catched_sigint_signal = 0;
	event*      ev_sig                = (event*)arg;

	// 不是SIGINT信号，或者该信号已经被处理过，则忽略
	if (sockfd != SIGINT || catched_sigint_signal)
	{
		return;
	}

	printf("Got signal %d\n", event_get_signal(ev_sig));
	printf("%s\n", "//////////////////////////////////////////////////");
	printf("%s\n", "/////////////////    WARNING    //////////////////");
	printf("%s\n", "//////////////////////////////////////////////////");
	printf("%s\n", "// You have been triggered stop operation. If   //");
	printf("%s\n", "// there is no IO within 10 seconds, the server //");
	printf("%s\n", "// will be automatically stopped with safety.   //");
	printf("%s\n", "//////////////////////////////////////////////////");
	
	event_base* ev_base = event_get_base(ev_sig);
	if (NULL == ev_base)
	{
		printf("error:event_get_base!\n");
		return;
	}
	
	// 10秒后关闭服务器
	timeval tv_timeout;
	evutil_timerclear(&tv_timeout);
	tv_timeout.tv_sec = 10;

	if (event_base_loopexit(ev_base, &tv_timeout))
	{
		printf("error:event_base_loopexit!\n");
		return;
	}
	
	catched_sigint_signal = 1;
}

void timeout_callback(evutil_socket_t sockfd, short events, void* arg)
{
	event* ev_timeout = (event*)arg;
	short  ev_events  = 0;

	ev_events = event_get_events(ev_timeout);
	if (EV_PERSIST & ev_events)
	{
		printf("persist timeout, ev_events = %d\n", ev_events);
	}
	else
	{
		printf("one-time timeout, ev_events = %d\n", ev_events);
	}
}

} // namespace test_libevent