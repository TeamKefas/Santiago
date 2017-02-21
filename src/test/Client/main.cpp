#include "Client.h"
#include <iostream>
#include <boost/asio.hpp>


int main()
{
    unsigned port;
    boost::asio::io_service io_service;
    Santiago::User::Client::Client client(io_service);
    client.start();    

    
}
