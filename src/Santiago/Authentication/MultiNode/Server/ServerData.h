#ifndef SANTIAGO_AUTHENTICATION_MULTINODE_SERVER_H
#define SANTIAGO_AUTHENTICATION_MULTINODE_SERVER_H

#include "Controller.h"

namespace Santiago{ namespace Authentication{ namespace MultiNode{ namespace Server
{
    struct ServerData
    {
        typedef Thread::ThreadSpecificVar<SantiagoDBTables::MariaDBConnection> ThreadSpecificDbConnection;
        ThreadSpecificDbConnection&         _databaseConnection;       
        std::pair<ControllerPtr,StrandPtr>  _authenticatorStrandPair[26];
    };

}}}}
#endif
