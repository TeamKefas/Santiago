#ifndef SANTIAGO_USER_SERVER_SERVER_MESSAGE_H
#define SANTIAGO_USER_SERVER_SERVER_MESSAGE_H

/**
 * @file ServerMessage.h
 *
 * @section DESCRIPTION
 *
 * Contains the RequestId ServerMessageType ServerMessage class  
 */

#include <boost/optional.hpp>
#include "ConnectionMessage.h"


namespace Santiago{ namespace User { namespace Server
{
    struct RequestId 
    {
        /**
         * The constructor
         * @param _initiatingConnectionId - 
         * @param _requestNo -
         */
        RequestId(unsigned _initiatingConnectionId,unsigned _requestNo);
        bool operator<(const RequestId& rhs_) const;
        
        unsigned  _initiatingConnectionId;
        unsigned  _requestNo;
    };

    enum class ServerMessageType
    {
        CONNECTION_MESSAGE_NEW, //new request 
        CONNECTION_MESSAGE_REPLY, //reply to a CR or SR
        CONNECTION_DISCONNECT,
//        DB_MESSAGE    -for when we implement Redis
    };

    struct ServerMessage
    {
        /**
         * The constructor
         * @param connectionId_ - 
         * @param requestId_ -
         * @param type_ -
         * @param connectionMessage_ -
         */
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

}}} //closing namespace Santiago::User::Server

#endif
