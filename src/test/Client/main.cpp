#include "Client.h"
#include <iostream>
#include <boost/asio.hpp>
#include <memory>


int main()
{
    unsigned port;
    std::cin>>port;
    boost::asio::ip::tcp::endpoint endPoint(boost::asio::ip::tcp::v4(),port);
    //port is the server listening port
    boost::asio::io_service ioService;
//    boost::asio::ip::tcp::socket socket(ioService); //need to make this a socketPtr
    
    std::shared_ptr<boost::asio::ip::tcp::socket> socketPtr(new boost::asio::ip::tcp::socket(ioService));
    socketPtr->connect(endPoint);
    
    Santiago::User::Client::Client client(socketPtr);
    client.start();    

    
}
