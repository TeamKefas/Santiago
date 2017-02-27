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
    
    try
    {
        std::shared_ptr<boost::asio::ip::tcp::socket> socketPtr(new boost::asio::ip::tcp::socket(ioService));
        socketPtr->connect(endPoint);

    
        Santiago::User::Client::Client client(socketPtr);
        client.startReadCycle();
        std::thread thread(std::bind(static_cast<std::size_t (boost::asio::io_service::*)()>
                                     (&boost::asio::io_service::run),
                                     &ioService));
        while(1)
        {
            char flag;
            std::cout<<"[s]end msg or [q]uit?."<<std::endl;
            std::cin>>flag;
            if(flag == 'q')
            {
                std::cout<<"Exiting..."<<std::endl;
                ioService.stop();
                thread.join();
                break;
            }
            else if(flag == 's')
            {
                client.inputMsgFromUserAndSendToServer();
            }
            else
            {
                std::cout<<"Please press s or q."<<std::endl;
                continue;
            }
        }
    }
    catch(const std::exception& e)
    {
        std::cout<<"Caught exception: "<<e.what()<<std::endl;
        std::cerr<<"Caught exception: "<<e.what()<<std::endl;
    }

    
}
