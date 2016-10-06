#include "MariaDBConnection.h"

using namespace Santiago::SantiagoDBTables;

int main()
{
    MariaDBConnection connection;
    UserPermission permission = READ_WRITE;
       
    UserProfilesRec userProfileRecord;
    userProfileRecord._userName = "junais";
    userProfileRecord._password = "pakistan";
    
    if(!connection.addUserProfilesRec(userProfileRecord))
    {
        std::cout << "\nUser Profile Added.\n";   
    }
    else
    {
        std::cout << "\nUsername already used.\n";
    }

    userProfileRecord._userName = "vinay";
    userProfileRecord._password = "india";
    
    if(!connection.addUserProfilesRec(userProfileRecord))
    {
        std::cout << "\nUser Profile Added.\n";   
    }
    else
    {
        std::cout << "\nUsername already used.\n";
    }

    userProfileRecord._userName = "junais";
    userProfileRecord._password = "india";
    
    if(!connection.addUserProfilesRec(userProfileRecord))
    {
        std::cout << "\nUser Profile Added.\n";   
    }
    else
    {
        std::cout << "\nUsername already used.\n";
    }
    
    if(!connection.deleteUserProfilesRec("vinay"))
    {
        std::cout << "\nUser Profile Deleted.\n";   
    }
    else
    {
        std::cout << "\nInvalid User Id.\n";
    }

    if(!connection.deleteUserProfilesRec("bineesh"))
    {
        std::cout << "\nUser Profile Deleted.\n";   
    }
    else
    {
        std::cout << "\nInvalid User Id.\n";
    }

    SessionsRec sessionRecord;
    sessionRecord._userName = "junais";
    sessionRecord._cookieId = "kefas";
    ptime loginTime(from_iso_string("20160612T120000"));
    sessionRecord._loginTime = loginTime;

    if(!connection.addSessionsRec(sessionRecord))
    {
         std::cout << "\nSession Added.\n"; 
    }
    else
    {
         std::cout << "\nSession Add Error.\n"; 
    }

    ptime logoutTime(from_iso_string("20160612T130015"));
    sessionRecord._logoutTime = logoutTime;
    
    if(!connection.updateSessionsRec(sessionRecord))
    {
         std::cout << "\nUpdated Session Record.\n"; 
    }
    else
    {
         std::cout << "\nSession Updation Error.\n"; 
    }

    PermissionsRec permissionRecord;
    permissionRecord._resId = 34;
    permissionRecord._userName = "junais";
    permissionRecord._userPermission = permission;
    
    if(!connection.addPermissionsRec(permissionRecord))
    {
        std::cout << "\nPermission Added.\n"; 
    }
    else
    {
        std::cout << "\nPermission Add Error.\n"; 
    }
    
    userProfileRecord._userName = "junais";
    userProfileRecord._password = "pakistan";
    
    if(!connection.updateUserProfilesRec(userProfileRecord,"india"))
    {
        std::cout<< "\nPassword updated.\n";
    }
    else
    {
        std::cout << "\nInvalid User Id or Password.\n";
    }

    userProfileRecord._userName = "vinay";
    userProfileRecord._password = "pakistan";
    
    if(!connection.updateUserProfilesRec(userProfileRecord,"india"))
    {
        std::cout<< "\nPassword updated.\n";
    }
    else
    {
        std::cout << "\nInvalid User Id or Password.\n";
    }

    userProfileRecord._userName = "junais";
    userProfileRecord._password = "pakistan";
    
    if(!connection.updateUserProfilesRec(userProfileRecord,"india"))
    {
        std::cout<< "\nPassword updated.\n";
    }
    else
    {
        std::cout << "\nInvalid User Id or Password.\n";
    }
    
    boost::optional<UserProfilesRec> userProfileRec = UserProfilesRec();
    
    if(!connection.getUserProfilesRec("junais", userProfileRec))
    {
        std::cout << "\n" << userProfileRec->_id << "\t"
                  << userProfileRec->_userName << "\t"
                  << userProfileRec->_password << "\n";
    }
    else
    {
        std::cout << "\nInvalid User Id.\n";
    }
        
    if(!connection.getUserProfilesRec("vinay", userProfileRec))
    {
        std::cout << "\n" << userProfileRec->_id << "\t"
                  << userProfileRec->_userName << "\t"
                  << userProfileRec->_password << "\n";
    }
    else
    {
        std::cout << "\nInvalid User Id.\n";
    }

    boost::optional<SessionsRec> sessionRec = SessionsRec();
    
    if(!connection.getSessionsRec("junais", sessionRec))
    {
        std::cout << "\n" << sessionRec->_id << "\t"
                  << sessionRec->_userName << "\t"
                  << sessionRec->_cookieId << "\t"
                  << sessionRec->_loginTime << "\t"
                  << sessionRec->_logoutTime << "\n";
    }
    else
    {
        std::cout << "\nInvalid User Id.\n";
    }

    if(!connection.getSessionsRec("vinay", sessionRec))
    {
        std::cout << "\n" << sessionRec->_id << "\t"
                  << sessionRec->_userName << "\t"
                  << sessionRec->_cookieId << "\t"
                  << sessionRec->_loginTime << "\t"
                  << sessionRec->_logoutTime << "\n";
    }
    else
    {
        std::cout << "\nInvalid User Id.\n";
    }

    boost::optional<PermissionsRec> permissionRec = PermissionsRec();
        
    if(!connection.getPermissionsRec("junais", permissionRec))
    {
        std::cout << "\n" << permissionRec->_id << "\t"
                  << permissionRec->_resId << "\t"
                  << permissionRec->_userName << "\t"
                  << userPermissionString.find(permissionRec->_userPermission)->second << "\n";
    }
    else
    {
        std::cout << "\nInvalid User Id.\n";
    }

    if(!connection.getPermissionsRec("vinay", permissionRec))
    {
        std::cout << "\n" << permissionRec->_id << "\t"
                  << permissionRec->_resId << "\t"
                  << permissionRec->_userName << "\t"
                  << userPermissionString.find(permissionRec->_userPermission)->second << "\n";
    }
    else
    {
        std::cout << "\nInvalid User Id.\n";
    }
    
    return 0;
}
    
