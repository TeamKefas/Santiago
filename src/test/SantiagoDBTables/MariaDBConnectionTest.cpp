#include "MariaDBConnection.h"

using namespace Santiago::SantiagoDBTables;

int main()
{
    MariaDBConnection connection;
    UserPermission x = UserPermission::READ_WRITE;
    if(connection.addUserProfileRecord("junais", "pakistan"))
    {
        std::cout << "\nAdded.\n";   
    }
    else
    {
        std::cout << "\nUsername already used.\n";
    }
    if(connection.addUserProfileRecord("junais", "india"))
    {
        std::cout << "\nAdded.\n";   
    }
    else
    {
        std::cout << "\nUsername already used.\n";
    }
        
    connection.addSessionRecord("junais", "kefas", from_iso_string("20160612T120000"));
    connection.updateSessionRecord("junais", from_iso_string("20160612T130015"));
    connection.addPermissionRecord(34, "junais", x);
    
    if(connection.updateUserPassword("junais", "pakistan","india"))
    {
        std::cout<< "\nPassword updated.\n";
    }
    else
    {
        std::cout << "\nInvalid User Id or Password.\n";
    }
    if(connection.updateUserPassword("vinay", "pakistan","india"))
    {
        std::cout<< "\nPassword updated.\n";
    }
    else
    {
        std::cout << "\nInvalid User Id or Password.\n";
    }
    if(connection.updateUserPassword("junais", "pakistan","india"))
    {
        std::cout<< "\nPassword updated.\n";
    }
    else
    {
        std::cout << "\nInvalid User Id or Password.\n";
    }

    boost::optional<UserProfile> userProfileRecord = UserProfile();
    
    if(connection.getUserProfileRecord("junais", userProfileRecord))
    {
        std::cout << "\n" << userProfileRecord->_id << "\t"
                  << userProfileRecord->_userName << "\t"
                  << userProfileRecord->_password << "\n";
    }
        
    if(connection.getUserProfileRecord("vinay", userProfileRecord))
    {
        std::cout << "\nInvalid User Id.\n";
    }

    boost::optional<Session> sessionRecord = Session();
    
    if(connection.getSessionRecord("junais", sessionRecord))
    {
        std::cout << "\n" << sessionRecord->_id << "\t"
                  << sessionRecord->_userName << "\t"
                  << sessionRecord->_cookieId << "\t"
                  << sessionRecord->_loginTime << "\t"
                  << sessionRecord->_logoutTime << "\n";
    }

    if(connection.getSessionRecord("vinay", sessionRecord))
    {
        std::cout << "\nInvalid User Id.\n";
    }    

    boost::optional<Permission> permissionRecord = Permission();
        
    if(connection.getPermissionRecord("junais", permissionRecord))
    {
        std::cout << "\n" << permissionRecord->_id << "\t"
                  << permissionRecord->_resId << "\t"
                  << permissionRecord->_userName << "\t"
                  << connection._permission.find(permissionRecord->_userPermission)->second << "\n";
    }

    if(connection.getPermissionRecord("vinay", permissionRecord))
    {
        std::cout << "\nInvalid User Id.\n";
    }

    return 0;
}
    
