#include "Client.h"
#include <iostream>
#include <boost/asio.hpp>
#include <memory>


int main()
{
    unsigned short port;
    std::cout<<"enter port";
    std::cin>>port;
    std::cout<<"connecting to port "<<port<<std::endl;
    boost::asio::ip::tcp::endpoint endPoint(boost::asio::ip::tcp::v4(),port);
    //port is the server listening port
    boost::asio::io_service ioService;
//    boost::asio::ip::tcp::socket socket(ioService); //need to make this a socketPtr
    
    try
    {
        std::shared_ptr<boost::asio::ip::tcp::socket> socketPtr(new boost::asio::ip::tcp::socket(ioService));
        socketPtr->connect(endPoint);

//        std::ostringstream stream;
//        stream<<"abcd";
                
//        boost::asio::write(*socketPtr,boost::asio::buffer(stream.str()));
    
        Santiago::User::Client::Client client(socketPtr);
        client.start();
//        ioService.run();
        while(1)
        {
            char flag;
            std::cout<<"exit ? Press Y to exit."<<std::endl;
            std::cin>>flag;
            if(flag == 'Y' || flag == 'y')
            {
                break;
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cout<<"Caught exception: "<<e.what()<<std::endl;
    }

    
}
