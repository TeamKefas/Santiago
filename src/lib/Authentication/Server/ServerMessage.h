#ifndef SANTIAGO_AUTHENTICATION_SERVER_MESSAGE_H
#define SANTIAGO_AUTHENTICATION_SERVER_MESSAGE_H

#include <boost/optional.hpp>
#include "ConnectionMessage.h"


namespace Santiago{ namespace Authentication
{
    struct RequestId 
    {
        RequestId(unsigned _initiatingConnectionId,unsigned _requestNo);
        bool operator<(const RequestId& rhs_) const;
        
        unsigned  _initiatingConnectionId;
        unsigned  _requestNo;
    };

    enum class ServerMessageType
    {
        CONNECTION_MESSAGE_NEW, //new request 
        CONNECTION_MESSAGE_REPLY, //reply to a SR
        CONNECTION_DISCONNECT,
//        DB_MESSAGE    -for when we implement Redis
    };

    struct ServerMessage
    {
        ServerMessage(unsigned connectionId_,
                      const RequestId& requestId_,
                      ServerMessageType type_,
                      const boost::optional<ConnectionMessage>& connectionMessage_);

        unsigned                              _connectionId;
        RequestId                             _requestId;
        ServerMessageType                     _type;
        boost::optional<ConnectionMessage>    _connectionMessage;
//        std::string                    _dbMessage;
    };

}} //closing namespace Santiago::Authentication

#endif
