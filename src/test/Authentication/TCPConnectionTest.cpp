#include "TCPConnection.h"
#include <memory>
#include <iostream>
using boost::asio::ip::tcp;
using namespace Santiago::Authentication;

void handleDisconnect();
void handleClientMessage();
std::function<void()> onDisconnectCallbackFn =
    std::bind(&handleDisconnect);
std::function<void(const ConnectionMessage)> onMessageCallbackFn =
    std::bind(&handleClientMessage);
void start();
void handleAccept(const TCPConnection::MySocketPtr socketPtr_,
                  const boost::system::error_code& error_);
std::shared_ptr<tcp::acceptor> _acceptorPtr;

int main()
{

    boost::asio::io_service io_service;
    _acceptorPtr.reset(new tcp::acceptor(io_service, tcp::endpoint(tcp::v4(),4024)));
    start();
    io_service.run();
}


void start()
{
    TCPConnection::MySocketPtr socketPtr(new TCPConnection::MySocket(_acceptorPtr->get_io_service()));
    _acceptorPtr->async_accept(*socketPtr,
                               boost::bind(&handleAccept, socketPtr,
                                           boost::asio::placeholders::error));
}

void handleAccept(const TCPConnection::MySocketPtr socketPtr_,
                          const boost::system::error_code& error_)
{
    if (!error_)
    {
        BOOST_ASSERT(socketPtr_);
        
        TCPConnection::Ptr newConnection(new TCPConnection(socketPtr_,
                                                           onDisconnectCallbackFn,
                                                           onMessageCallbackFn));
        newConnection->start(); 
    }
    start();
}
            
void handleDisconnect()
{
    std::cout<<"handleDisconnect"<<std::endl;
}

void handleClientMessage()
{
    std::cout<<"handleClientMessage"<<std::endl;
}

