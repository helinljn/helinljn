#include "spdlog.h"
#include "brynet.h"

namespace bnet  = brynet::net;
namespace bbase = brynet::base;

int main(void)
{
    auto service = bnet::IOThreadTcpService::Create();
    service->startWorkerThread(1);

    auto httpEnterCallback = [](const bnet::http::HTTPParser& httpParser, const bnet::http::HttpSession::Ptr& session)
    {
        bnet::http::HttpResponse response;
        response.setBody(std::string("Welcome to brynet http server!"));

        if (httpParser.isKeepAlive())
        {
            response.addHeadValue("Connection", "Keep-Alive");
            session->send(response.getResult());
        }
        else
        {
            response.addHeadValue("Connection", "Close");
            session->send(response.getResult(), [session]() {session->postShutdown();});
        }
    };

    bnet::wrapper::HttpListenerBuilder listenBuilder;
    listenBuilder
            .WithService(service)
            .AddSocketProcess(
                [](bnet::TcpSocket& socket)
                {
                    socket.setNodelay();
                }
            )
            .WithMaxRecvBufferSize(1024)
            .WithAddr(false, "0.0.0.0", 8088)
            .WithReusePort()
            .WithEnterCallback(
                [httpEnterCallback](const bnet::http::HttpSession::Ptr& httpSession, bnet::http::HttpSessionHandlers& handlers)
                {
                    std::ignore = httpSession;
                    handlers.setHttpEndCallback(httpEnterCallback);
                }
            )
            .asyncRun();

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (bbase::app_kbhit())
        {
            break;
        }
    }

    return 0;
}