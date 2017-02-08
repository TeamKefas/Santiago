#ifndef SANTIAGO_USER_SERVER_CONNECTION_MESSAGE_H
#define SANTIAGO_USER_SERVER_CONNECTION_MESSAGE_H

/**
 * @file ConnectionMessage.h
 *
 * @section DESCRIPTION
 *
 * Contains the ConnectionMessage class and ConnectionMessageType class 
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <string>

namespace Santiago{ namespace User { namespace Server
{
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

    struct ConnectionMessage
    {
        /**
         * The constructor
         * @param content_- 
         * @param size_- ///NEED TO WRITE\\\
         */
        ConnectionMessage(const char* content_,unsigned size_);         
        /**
         * The constructor
         * @param type_- 
         * @param parameters_- ///NEED TO WRITE\\\
         */
        ConnectionMessage(ConnectionMessageType type_,const std::vector<std::string>& parameters_);

        /**
         * ///Message\\
         * @param outStream_ -
         */
        std::ostream& writeToStream(std::ostream& outStream_) const;
        /**
         * ///Message\\
         */
        unsigned getSize() const;

        ConnectionMessageType     _type;
        std::vector<std::string>  _parameters;
    };

    inline std::ostream& operator<<(std::ostream& ostream_, const ConnectionMessage& connectionMessage_)
    { return connectionMessage_.writeToStream(ostream_);}

}}} //closing namespace Santiago::User::Server

#endif
