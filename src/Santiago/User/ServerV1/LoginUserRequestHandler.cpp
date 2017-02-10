#include "LoginUserRequestHandler.h"

namespace Santiago{ namespace User { namespace Server
{

    LoginUserRequestHandler::LoginUserRequestHandler(ServerData& serverData_,
                                                     SantiagoDBConnection& databaseConnection_,
                                                     const SendMessageCallbackFn& sendMessageCallbackFn_,
                                                     const OnCompletedCallbackFn& onCompletedCallbackFn_,
                                                     const ServerMessage& initiatingMessage_)
        :RequestHandlerBase(serverData_, databaseConnection_, sendMessageCallbackFn_, onCompletedCallbackFn_, initiatingMessage_)
    {}
    
    void LoginUserRequestHandler::start()
    {
        std::error_code error1, error2;
        boost::optional<SantiagoDBTables::UsersRec> usersRec = _databaseConnection.get().getUsersRecForUserName(_initiatingMessage._connectionMessage->_parameters[0], error1);
        if(error1)
        {
            usersRec = _databaseConnection.get().getUsersRecForEmailAddress(_initiatingMessage._connectionMessage->_parameters[0], error1);
        }
      
        if(!error1 && generateSHA256(_initiatingMessage._connectionMessage->_parameters[1]) == usersRec->_password)                                      
        {
            SantiagoDBTables::SessionsRec sessionsRec;
            sessionsRec._userName = usersRec->_userName;
            sessionsRec._cookieString = generateUniqueCookie();
            sessionsRec._loginTime = boost::posix_time::second_clock::local_time();
            _databaseConnection.get().addSessionsRec(sessionsRec, error2);       
            if(!error2)
            {
                CookieData cookieData;
                cookieData._userName = sessionsRec._userName;
                cookieData._connectionIds.push_back(_initiatingMessage._connectionId);
                _serverData._cookieCookieDataMap[sessionsRec._cookieString] = cookieData;
                if(_serverData._userIdUserIdDataMap.find(sessionsRec._userName) != _serverData._userIdUserIdDataMap.end())
                {
                    _serverData._userIdUserIdDataMap.find(sessionsRec._userName)->second._cookieList.push_back(sessionsRec._cookieString);
                }
                else
                {
                    UserIdData userIdData;
                    userIdData._cookieList.push_back(sessionsRec._cookieString);
                    _serverData._userIdUserIdDataMap[sessionsRec._userName] = userIdData;
                }
                ConnectionMessage connectionMessage(ConnectionMessageType::SUCCEEDED,std::vector<std::string>()); 
                ServerMessage serverMessage(_initiatingMessage._connectionId,
                                            _initiatingMessage._requestId,
                                            ServerMessageType::CONNECTION_MESSAGE_REPLY,
                                            connectionMessage);
                _sendMessageCallbackFn(serverMessage);
                _onCompletedCallbackFn(_initiatingMessage._requestId);
            }
        }
    
        if(error1 || error2)
        {
            ConnectionMessage connectionMessage(ConnectionMessageType::FAILED,std::vector<std::string>()); 
            ServerMessage serverMessage(_initiatingMessage._connectionId,
                                        _initiatingMessage._requestId,
                                        ServerMessageType::CONNECTION_MESSAGE_REPLY,
                                        connectionMessage);
            _sendMessageCallbackFn(serverMessage);
            _onCompletedCallbackFn(_initiatingMessage._requestId);
        }
    }

    std::string LoginUserRequestHandler::generateSHA256(const std::string str)
    {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, str.c_str(), str.size());
        SHA256_Final(hash, &sha256);
        std::stringstream ss;
        for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }
        return ss.str();
    }

    std::string LoginUserRequestHandler::generateUniqueCookie()
    {
        std::string str;
        static const char alphanum[] =
            "0123456789"
            "@#$%^*"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        
        int stringLength = sizeof(alphanum) - 1;
	
	for(unsigned int i = 0; i < 46; ++i)
	{
            str += alphanum[rand() % stringLength];
	}
        return str;
    }
    
    void LoginUserRequestHandler::handleReplyMessage(const ServerMessage& serverMessage)
    {
        BOOST_ASSERT(false);
    }
    
}}}
