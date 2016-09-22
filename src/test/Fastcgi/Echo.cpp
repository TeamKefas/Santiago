/*
    Copyright 2012, 2013 Wilson Sunny Philip [wil.pazhorickal<at>gmail<dot>com]

    This file is part of the Santiago library.

    Santiago is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Santiago is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with Santiago.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <signal.h>

#include <boost/asio.hpp>

#include "Fastcgi/Acceptor.h"

#if defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)

void requestHandlerStreamProtocol(boost::shared_ptr<Santiago::Fastcgi::Request<boost::asio::local::stream_protocol> > request_)
{
    request_->out()<<"Content-Type: text/plain\r\n\r\n"<<"Echo - Unix Socket Version\n";
    std::map<std::string,std::string> &fcgiParams = request_->getFCGIParams(); 
    
    for(std::map<std::string,std::string>::iterator iter = fcgiParams.begin();iter != fcgiParams.end();iter++)
    {
        request_->out()<<iter->first<<" \t"<<iter->second<<"\n";
    }

//    std::cout<<request_->getFCGIParams()[REQUEST_URI];

    request_->setAppStatus(0);
    request_->commit();
}

void requestHandlerTcpProtocol(boost::shared_ptr<Santiago::Fastcgi::Request<boost::asio::ip::tcp> > request_)
{
    request_->out()<<"Content-Type: text/plain\r\n\r\n"<<"Echo - Tcp Socket Version\n";
    std::map<std::string,std::string> &fcgiParams = request_->getFCGIParams(); 
    
    for(std::map<std::string,std::string>::iterator iter = fcgiParams.begin();iter != fcgiParams.end();iter++)
    {
        request_->out()<<iter->first<<" \t"<<iter->second<<"\n";
    }

//    std::cout<<request_->getFCGIParams()[REQUEST_URI];

    request_->setAppStatus(0);
    request_->commit();
}

void onSigintHandler(int signum_)
{
    throw std::runtime_error("Received sigint signal..exiting");
}

int main(int argc, char* argv[])
{

  try
  {
      if(signal(SIGINT, onSigintHandler) == SIG_ERR)
      {
          throw std::runtime_error("Unable to register for sigint signal...exiting");
      }

      boost::asio::io_service io_service;

      std::remove(argv[1]);
      Santiago::Fastcgi::Acceptor<boost::asio::local::stream_protocol> acceptor(io_service,Santiago::LocalEndpoint<boost::asio::local::stream_protocol>("/tmp/fastcgi-unix-echo.socket"),boost::bind(&requestHandlerStreamProtocol,_1));

      Santiago::Fastcgi::Acceptor<boost::asio::ip::tcp> acceptor1(io_service,Santiago::LocalEndpoint<boost::asio::ip::tcp> (7000),boost::bind(&requestHandlerTcpProtocol,_1));

      io_service.run();
  }
  catch (std::exception& e)
  {
      std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}

#else // defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
# error Local sockets not available on this platform.
#endif // defined(BOOST_ASIO_HAS_LOCAL_SOCKETS)
