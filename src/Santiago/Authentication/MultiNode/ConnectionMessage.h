#ifndef SANTIAGO_AUTHENTICATION_MULTINODE_CONNECTIONMESSAGE_H
#define SANTIAGO_AUTHENTICATION_MULTINODE_CONNECTIONMESSAGE_H

/**
 * @file ConnectionMessage.h
 *
 * @section DESCRIPTION
 *
 * Contains the structs for the data that is passed between Client and Server
 */

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <string>

#include "../../Utils/STLog.h"
#include "../../Common.h"
#include "../../ErrorCategory.h"

namespace Santiago{ namespace Authentication { namespace MultiNode
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
        CR_VERIFY_COOKIE_AND_GET_USER_INFO,
        CR_LOGOUT_USER_FOR_COOKIE,
        CR_LOGOUT_USER_FOR_ALL_COOKIES,
        CR_CHANGE_USER_PASSWORD,
	CR_GET_USER_FOR_EMAIL_ADDRESS_AND_RECOVERY_STRING,
	CR_CHANGE_USER_PASSWORD_FOR_EMAIL_ADDRESS_AND_RECOVERY_STRING,
        CR_CHANGE_USER_EMAIL_ADDRESS,
	CR_CREATE_AND_RETURN_RECOVERY_STRING,
        CR_DELETE_USER,

        SR_PING,
        SR_LOGOUT_USER_FOR_COOKIE,
        SR_LOGOUT_USER_FOR_ALL_COOKIES
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

        unsigned  _initiatingConnectionId;
        unsigned  _requestNo;
    };
    
    bool operator <(const RequestId& lhs_, const RequestId& rhs_)
    {
        return std::tie(lhs_._initiatingConnectionId, lhs_._requestNo) < std::tie(rhs_._initiatingConnectionId, rhs_._requestNo);
    }
    
    /***********************************************************
     * ConnectionMessage
     ***********************************************************/
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
        ConnectionMessage(const RequestId& requestId_,
                          ConnectionMessageType type_,
                          const std::vector<std::string>& parameters_);

        /**
         * ///Message\\
         * @param outStream_ -
         */
        std::ostream& writeToStream(std::ostream& outStream_) const;
        /**
         * ///Message\\
         */
        unsigned getSize() const;

        RequestId                 _requestId;
        ConnectionMessageType     _type;
        std::vector<std::string>  _parameters;
    };

    inline std::ostream& operator<<(std::ostream& ostream_, const ConnectionMessage& connectionMessage_)
    { return connectionMessage_.writeToStream(ostream_);}

}}} //closing namespace Santiago::Authentication::MultiNode

#endif
