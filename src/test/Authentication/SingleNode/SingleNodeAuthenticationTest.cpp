#include <csignal>
#include <functional>
#include <iostream>

#include <boost/asio.hpp>

#include "AuthenticatorTester.h"

void onSigintHandler(int signum_)
{
    throw std::runtime_error("Received sigint signal..exiting");
}

int main(int argc, char *argv[])
{

    if(argc != 2)
    {
        std::cout << "Format is ./AuthenticatorTest <config.json>" << std::endl;
        return -1;
    }

    try
    {
        if(signal(SIGINT, onSigintHandler) == SIG_ERR)
        {
            throw std::runtime_error("Unable to register for sigint signal...exiting");
        }

        //   SimpleAppServer::Server server(Santiago::LocalEndpoint<boost::asio::ip::tcp> (7000));
        boost::asio::io_service ioService;
        boost::property_tree::ptree config;
        boost::property_tree::read_json(argv[1],config);

        Test::Authentication::SingleNode::AuthenticationTester authenticationTester(config);
        boost::asio::spawn(ioService,&Test::Authentication::AuthenticatorTesterBase::runTests,
                           &authenticationTester,
                           std::placeholders::_1);

        std::thread thread1(std::bind(&boost::asio::io_service::run,&ioService));
        std::thread thread2(std::bind(&boost::asio::io_service::run,&ioService));
        std::thread thread3(std::bind(&boost::asio::io_service::run,&ioService));
        ioService.run();

        thread1.join();
        thread2.join();
        thread3.join();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

}
