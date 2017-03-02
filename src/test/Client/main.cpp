#include "Client.h"
#include <iostream>
#include <boost/asio.hpp>
#include <memory>

void GetUserInputInLoop(const std::shared_ptr<boost::asio::io_service>& ioServicePtr_)
{
    std::shared_ptr<boost::asio::io_service> ioServicePtr = ioServicePtr_;
    try
    {
        while(1)
        {
            char flag;
            std::cout<<"[s]end msg or [q]uit?."<<std::endl;
            std::cin>>flag;
            if(flag == 'q')
            {
                std::cout<<"Exiting..."<<std::endl;
                ioServicePtr->stop();
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
    catch(std::exception& e)
    {
        ioServicePtr->stop();
        std::cout<<"Caught exception: "<<e.what()<<std::endl;
        std::cerr<<"Caught exception: "<<e.what()<<std::endl;
    }
}

int main()
{
    unsigned short port;
    std::cout<<"enter port";
    std::cin>>port;
    std::cout<<"connecting to port "<<port<<std::endl;
    boost::asio::ip::tcp::endpoint endPoint(boost::asio::ip::tcp::v4(),port);
    //port is the server listening port
    std::shared_ptr<boost::asio::io_service> ioServicePtr(new boost::asio::io_service());
    
    try
    {
        std::shared_ptr<boost::asio::ip::tcp::socket> socketPtr(new boost::asio::ip::tcp::socket(*ioServicePtr));
        socketPtr->connect(endPoint);
    
        Santiago::User::Client::Client client(socketPtr);
        client.startReadCycle();
        std::thread thread(std::bind(GetUserInputInLoop,ioServicePtr_));
        ioServicePtr->run();
        thread.join();
    }
    catch(const std::exception& e)
    {
        std::cout<<"Caught exception: "<<e.what()<<std::endl;
        std::cerr<<"Caught exception: "<<e.what()<<std::endl;
    }    
}
