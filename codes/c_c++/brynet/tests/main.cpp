#include "spdlog.h"
#include "brynet.h"

using namespace brynet;

int main(void)
{
    auto service = net::IOThreadTcpService::Create();
    service->startWorkerThread(1);

    auto httpEnterCallback = [](const net::http::HTTPParser& httpParser, const net::http::HttpSession::Ptr& session)
    {
        std::ignore = httpParser;

        net::http::HttpResponse response;
        //std::cout << "method:" << http_method_str(static_cast<http_method>(httpParser.method())) << std::endl;
        response.setBody(std::string("<html>hello world </html>"));
        if (httpParser.isKeepAlive())
        {
            response.addHeadValue("Connection", "Keep-Alive");
            session->send(response.getResult());
        }
        else
        {
            response.addHeadValue("Connection", "Close");
            session->send(response.getResult(), [session]() {
                session->postShutdown();
            });
        }
    };

    auto wsEnterCallback = [](const net::http::HttpSession::Ptr& httpSession,
                              net::http::WebSocketFormat::WebSocketFrameType opcode,
                              const std::string& payload)
    {
        std::cout << "frame enter of type:" << int(opcode) << std::endl;
        std::cout << "payload is:" << payload << std::endl;
        // echo frame
        httpSession->send(net::http::WebSocketFormat::wsFrameBuild(payload));
    };

    net::wrapper::HttpListenerBuilder listenBuilder;
    listenBuilder
            .WithService(service)
            .AddSocketProcess([](net::TcpSocket& socket) {
                socket.setNodelay();
            })
            .WithMaxRecvBufferSize(1024)
            .WithAddr(false, "0.0.0.0", 8080)
            .WithReusePort()
            .WithEnterCallback([httpEnterCallback, wsEnterCallback](const net::http::HttpSession::Ptr& httpSession, net::http::HttpSessionHandlers& handlers) {
                std::ignore = httpSession;

                handlers.setHttpEndCallback(httpEnterCallback);
                handlers.setWSCallback(wsEnterCallback);
            })
            .asyncRun();

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if (brynet::base::app_kbhit())
        {
            break;
        }
    }

    return 0;
}