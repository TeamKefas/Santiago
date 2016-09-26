#ifndef SANTIAGO_AUTHENTICATION_MESSAGE_H
#define SANTIAGO_AUTHENTICATION_MESSAGE_H

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <string>
namespace Santiago{ namespace Authentication
{

    /***********************************************************
     * MessageType
     ***********************************************************/

    enum class ConnectionMessageType
    {
        /* 2 types of messages: Requests and Replies
         * Requests - CR - Client Request, SR - Server Request
         * Replies - Succeeded, Failed
         */
        SUCCEEDED,
        FAILED,

        CR_CREATE_USER, 
        CR_LOGIN_USER,
        CR_VERIFY_USER_FOR_COOKIE,
        CR_LOGOUT_USER_FOR_COOKIE,
        CR_LOGOUT_USER_FOR_ALL_COOKIES,
        CR_CHANGE_USER_PASSWORD,
        CR_DELETE_USER,
        CR_REMOVED_COOKIE_FROM_APPSERVER,

        CR_ADD_RESOURCE,
        CR_ADD_USER_PERMISSION_FOR_RESOURCE,
        CR_MODIFY_USER_PERMISSION_FOR_RESOURCE,
        CR_DELETE_USER_PERMISSION_FOR_RESOURCE,
        CR_VERIFY_USER_PERMISSION_FOR_RESOURCE,
        CR_REMOVED_USER_PERMISSION_FROM_APPSERVER,

        SR_LOGOUT_USER_FOR_COOKIE,
        SR_LOGOUT_USER_FOR_ALL_COOKIES,
    };

    /***********************************************************
     * RequestId
     ***********************************************************/
    struct RequestId 
    {
        /*
         * TODO: Implement the < operator. first check initiatingConnectionId 
         * and if equal then check requestNo.
         */
        RequestId(unsigned _initiatingConnectionId,unsigned _requestNo);

        bool operator<(const RequestId& rhs_);

        unsigned  _initiatingConnectionId;
        unsigned  _requestNo;
    };

    /***********************************************************
     * ConnectionMessage
     ***********************************************************/

    struct ConnectionMessage
    {
        ConnectionMessage(const char* content_,unsigned size_);         

        ConnectionMessage(ConnectionMessageType type_,const std::vector<std::string>& parameters_);

        std::ostream& writeToStream(std::ostream& outStream_) const;
        unsigned getSize() const;

        ConnectionMessageType     _type;
        std::vector<std::string>  _parameters;
    };

    inline std::ostream& operator<<(std::ostream& ostream_, const ConnectionMessage& connectionMessage_)
    { return connectionMessage_.writeToStream(ostream_);}

    /***********************************************************
     * ServerMessage
     ***********************************************************/
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
                      const Optional<ConnectionMessage>& connectionMessage_);

        unsigned                       _connectionId;
        RequestId                      _requestId;
        ServerMessageType              _type;
        Optional<ConnectionMessage>    _connectionMessage;
//        std::string                    _dbMessage;
    };

}} //closing namespace Santiago::Authentication

#endif
