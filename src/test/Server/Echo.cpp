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
#include <string>

#include <boost/thread.hpp>

#include "Santiago/Server/RequestHandler.h"
#include "Santiago/Server/Server.h"

class RequestHandlerA:public Santiago::Server::RequestHandler<boost::asio::ip::tcp>
{
public:
    RequestHandlerA():Santiago::Server::RequestHandler<boost::asio::ip::tcp>()
    {}
    
    void handleRequest(RequestPtr request_)
    {
        request_->out()<<"Content-Type: text/plain\r\n\r\n"<<"Handler A\n";
        std::map<std::string,std::string> &fcgiParams = request_->getFCGIParams(); 
    
        for(std::map<std::string,std::string>::iterator iter = fcgiParams.begin();iter != fcgiParams.end();iter++)
        {
            request_->out()<<iter->first<<" \t"<<iter->second<<"\n";
        }

        request_->setAppStatus(0);
        request_->commit();        
    }

    ~RequestHandlerA()
    {}
};

class RequestHandlerB:public Santiago::Server::RequestHandler<boost::asio::ip::tcp>
{
public:
    RequestHandlerB():Santiago::Server::RequestHandler<boost::asio::ip::tcp>()
    {}
    
    void handleRequest(RequestPtr request_)
    {
        request_->out()<<"Content-Type: text/plain\r\n\r\n"<<"Handler B\n";
        std::map<std::string,std::string> &fcgiParams = request_->getFCGIParams(); 
    
        for(std::map<std::string,std::string>::iterator iter = fcgiParams.begin();iter != fcgiParams.end();iter++)
        {
            request_->out()<<iter->first<<" \t"<<iter->second<<"\n";
        }

        request_->setAppStatus(0);
        request_->commit();        
    }

    ~RequestHandlerB()
    {}
};

class MyServer:public Santiago::Server::Server<boost::asio::ip::tcp>
{
public:
    typedef boost::shared_ptr<Santiago::Server::RequestHandler<boost::asio::ip::tcp> > RequestHandlerPtr; 

    MyServer(Santiago::LocalEndpoint<boost::asio::ip::tcp> listenEndpoint_):
        Santiago::Server::Server<boost::asio::ip::tcp>(listenEndpoint_)
    {}

    ~MyServer()
    {}

    RequestHandlerPtr route(std::map<std::string,std::string>& requestParams_)
    {
            std::map<std::string,std::string>::iterator iter = requestParams_.find("DOCUMENT_URI");

            BOOST_ASSERT(iter != requestParams_.end());
            std::string uri = iter->second;

        if(uri.compare("/server-tcp-echoA.fcgi") == 0)
        {
            return RequestHandlerPtr(new RequestHandlerA());
        }
        else
        {
            return RequestHandlerPtr(new RequestHandlerB());
        }
    }
};

void onSigintHandler(int signum_)
{
    throw std::runtime_error("Received sigint signal..exiting");
}


int main()
{
  try
  {
      if(signal(SIGINT, onSigintHandler) == SIG_ERR)
      {
          throw std::runtime_error("Unable to register for sigint signal...exiting");
      }


      MyServer server(Santiago::LocalEndpoint<boost::asio::ip::tcp> (7001));

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
