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
#ifndef FASTCGI_ACCEPTOR_H
#define FASTCGI_ACCEPTOR_H

/**
 * @file Acceptor.h
 * @author  Wilson Sunny Philip
 *
 * @section DESCRIPTION
 *
 * Contains the Acceptor class for listening and accepting new connections from the
 * webserver.
 */

#include <stack>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#include "../LocalEndpoint.h"
#include "Common.h"
#include "Request.h"
#include "Connection.h"


namespace Santiago{ namespace Fastcgi
{
/**
 * Listens for and opens new connections. The class is templated on the
 * protocol of the underlying socket connection. It takes the boost asio protocol tags
 * as the template arguments(eg Acceptor<boost::asio::ip::tcp>)
 */
    template<typename Protocol>
    class Acceptor
    {
        typedef typename Protocol::socket ProtocolSocket;
        typedef boost::shared_ptr<ProtocolSocket> ProtocolSocketPtr;

        typedef boost::shared_ptr<Connection<Protocol> > ConnectionPtr;
        typedef boost::weak_ptr<Connection<Protocol> > ConnectionWeakPtr;        
        typedef std::map<uint,ConnectionPtr > ConnectionMap;

        typedef boost::function<void(boost::shared_ptr<Request<Protocol> >)> NewRequestCallbackFn;

    public:
        /**
         * The constructor
         * @param ioService_- the ioservice to use
         * @param localEndpoint_- the endpoint to listen for new connection
         * @param newRequestCallbackFn - the function to call on receiving new request
         */
        Acceptor(boost::asio::io_service& ioService_,
                 LocalEndpoint<Protocol> listenEndpoint_,
                 NewRequestCallbackFn newRequestCallbackFn_)
            : _ioService(ioService_),
              _strand(ioService_),
              _acceptor(ioService_, listenEndpoint_.getAsioEndpoint()),
              _newRequestCallbackFn(newRequestCallbackFn_),
              _nextConnectionId(0)
        {
            ProtocolSocketPtr newProtocolSocket(new ProtocolSocket(_ioService));

            //listen for new connection. Make sure the callback is called in the
            //acceptor's strand            
            _acceptor.async_accept(*newProtocolSocket,
                                   _strand.wrap(boost::bind(&Acceptor::handleAccept,
                                                            this,
                                                            newProtocolSocket,
                                                            boost::asio::placeholders::error)));

        }

        /**
         * Returns the acceptor's strand
         */
        boost::asio::strand& getStrand()
        {
            return _strand;
        }

        /**
         * The destrunctor
         */
        ~Acceptor()
        {
            _acceptor.cancel();
            _acceptor.close();
        }

    private:
        /**
         * The callback function called by the boost acceptor when a new connection is
         * opened. This function is always called in the Acceptor's strand.
         * @param newProtocolSocket
         * @param error 
         */
        void handleAccept(ProtocolSocketPtr newProtocolSocket_,
                           const boost::system::error_code &error_)
        {
            if(error_)
            {//TODO: end connections and make a callback fn 
                return; 
            }
            //start new connection
            uint newConnectionId = _nextConnectionId++;
            BOOST_ASSERT(_activeConnections.find(newConnectionId) == _activeConnections.end());
            ConnectionPtr newConnection(new Connection<Protocol>(_ioService,
                                                                 newProtocolSocket_,
                                                                 boost::bind(&Acceptor::handleNewRequest,this,newConnectionId,_1,_2),
                                                                 boost::bind(&Acceptor::handleConnectionClose,this,newConnectionId)));

            _activeConnections[newConnectionId] = newConnection;
            newConnection->start();

            //listen for new connection
            ProtocolSocketPtr newProtocolSocket(new ProtocolSocket(_ioService));

            _acceptor.async_accept(*newProtocolSocket,
                                   _strand.wrap(boost::bind(&Acceptor::handleAccept,
                                                            this,
                                                            newProtocolSocket,
                                                            boost::asio::placeholders::error)));
        }

        /**
         * The callback function called by the connection when it's socket closes(due to
         * error or otherwise). This function is not called in the acceptor's strand.
         * So it should not directly modify the _activeConnections map 
         * @param connectionId-id of connection object in the _activeConnections map
         */
        void handleConnectionClose(uint connectionId_)
        {
            BOOST_ASSERT(_activeConnections.find(connectionId_) != _activeConnections.end());
            //call the close connection in the acceptor's strand
            _strand.post(boost::bind(&Acceptor::closeConnection,this,connectionId_));

        }

        /**
         * The function which does the cleanup of a connection. Should always be called
         * in the acceptor's strand.
         * @param connectionId-id of connection object in the _activeConnections map
         */
        void closeConnection(uint connectionId_)
        {
            BOOST_ASSERT(_activeConnections.find(connectionId_) != _activeConnections.end());
            _activeConnections.erase(connectionId_);                        
        }

        /**
         * The function which is called by a Connection object whenever it has a ready
         * request. This is usually not called in the acceptor's strand 
         * @param connectionId-id of the connection
         * @param requestid- received from the server
         * @param newRequestData - new filled in request data 
         */
        void handleNewRequest(uint connectionId_,uint newRequestId_,boost::weak_ptr<RequestData> newRequestData_)
        {
            if(newRequestData_.lock() == NULL)
            {   //request has already been destroyed by now...this check here is
                //unnecessary..as it will be cheked when it is going to be used
                return;
            }

            BOOST_ASSERT(_activeConnections.find(connectionId_) != _activeConnections.end());
            boost::shared_ptr<Request<Protocol> > newRequest(new Request<Protocol>(_ioService,newRequestId_,newRequestData_,connectionId_,ConnectionWeakPtr(_activeConnections[connectionId_])));
            //post it in the acceptor's strand
            _strand.post(boost::bind(_newRequestCallbackFn,newRequest));
        }

        boost::asio::io_service                    &_ioService;
        boost::asio::strand                         _strand;
        typename Protocol::acceptor                 _acceptor;
        NewRequestCallbackFn                        _newRequestCallbackFn;
        ConnectionMap                               _activeConnections;
        unsigned int                                _nextConnectionId;
    };


}}

#endif
