#include <iostream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <csignal>

#include "Server.h"




void onSigintHandler(int signum_)
{
    throw std::runtime_error("Received sigint signal..exiting");
}


int main(int argc, char *argv[])
{
    try
    {
        if(signal(SIGINT, onSigintHandler) == SIG_ERR)
        {
            throw std::runtime_error("Unable to register for sigint signal...exiting");
        }

        Test::AppServer::Server server(Santiago::LocalEndpoint<boost::asio::ip::tcp> (7000));
        server.start();

        while(1)
        {
            sleep(1000);
        }
        server.stop();
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
