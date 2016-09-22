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
#include <boost/function.hpp>

#include "Santiago/Server/RequestHandler.h"
#include "Santiago/Server/Server.h"

class TimerService
{
    typedef boost::shared_ptr<Santiago::Server::RequestHandler<boost::asio::ip::tcp> > RequestHandlerPtr;
    typedef boost::shared_ptr<boost::asio::deadline_timer> TimerPtr;

public:
    TimerService(boost::asio::io_service& ioService_):
        _ioService(ioService_)
    {}

    void asyncWait(uint seconds_,boost::function<void()> callbackFn_)
    {
        TimerPtr timer(new boost::asio::deadline_timer(_ioService,boost::posix_time::seconds(seconds_)));
        timer->async_wait(boost::bind(&TimerService::timerCallback,this,timer,callbackFn_));
    }

    ~TimerService()
    {}
private:

    void timerCallback(TimerPtr timer_,boost::function<void()> callbackFn_)
    {
        callbackFn_();
    }

    boost::asio::io_service &_ioService;
};

class RequestHandlerA:public Santiago::Server::RequestHandler<boost::asio::ip::tcp>
{
public:
    RequestHandlerA(TimerService &timerService_):
        Santiago::Server::RequestHandler<boost::asio::ip::tcp>(),
        _timerService(timerService_)                                                  
    {}
    
    void handleRequest(RequestPtr request_)
    {
        if(request_->getFCGIParams()["DOCUMENT_URI"] == "/async-service-cancel.fcgi")
        {
            request_->cancel();
            return;
        }

        request_->out()<<"Content-Type: text/plain\r\n\r\n"<<"Timee Echo Handler A\n";
        std::map<std::string,std::string> &fcgiParams = request_->getFCGIParams(); 
    
        for(std::map<std::string,std::string>::iterator iter = fcgiParams.begin();iter != fcgiParams.end();iter++)
        {
            request_->out()<<iter->first<<" \t"<<iter->second<<"\n";
        }

        request_->setAppStatus(0);

        boost::function<void()> callbackFn = boost::bind(&RequestHandlerA::timerCallback,this);
        _timerService.asyncWait(5,boost::bind(&RequestHandlerA::postInStrand,this,callbackFn));
        _request = request_;
    }

    void timerCallback()
    {
        _request->commit();
        _request.reset();
    }

    ~RequestHandlerA()
    {}

    TimerService       &_timerService;
    RequestPtr          _request;
};

class MyServer:public Santiago::Server::Server<boost::asio::ip::tcp>
{
public:
    typedef boost::shared_ptr<Santiago::Server::RequestHandler<boost::asio::ip::tcp> > RequestHandlerPtr; 

    MyServer(Santiago::LocalEndpoint<boost::asio::ip::tcp> listenEndpoint_):
        Santiago::Server::Server<boost::asio::ip::tcp>(listenEndpoint_),
        _timerService(getIOService())
    {}

    ~MyServer()
    {}

    RequestHandlerPtr route(std::map<std::string,std::string>& requestParams_)
    {
            return RequestHandlerPtr(new RequestHandlerA(_timerService));
    }
private:
    TimerService        _timerService;
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

      MyServer server(Santiago::LocalEndpoint<boost::asio::ip::tcp> (7002));

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
