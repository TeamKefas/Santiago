#include <iostream>
#include "Server.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <csignal>



void onSigintHandler(int signum_)
{
    throw std::runtime_error("Received sigint signal..exiting");
}


int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        std::cout << "Format is ./CreateUserWalkthrough <config.json>" << std::endl;
        return -1;
    }

    try
    {
        if(signal(SIGINT, onSigintHandler) == SIG_ERR)
        {
            throw std::runtime_error("Unable to register for sigint signal...exiting");
        }

        //   SimpleAppServer::Server server(Santiago::LocalEndpoint<boost::asio::ip::tcp> (7000));
        boost::property_tree::ptree config;
        boost::property_tree::read_json(argv[1],config);
        Test::AppServer::Server server(config);
        server.start();
        int i;
        std::cin>>i;
        server.stop();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
