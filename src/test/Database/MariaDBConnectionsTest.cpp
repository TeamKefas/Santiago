#include "MariaDBConnections.h"

using namespace Santiago::Database;

int main()
{
    MariaDBConnections connection;
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
    if(connection.checkUserProfileRecord("junais", "pakistan"))
    {
        std::cout << "\nMatch Found.\n";   
    }
    else
    {
        std::cout << "\nNo match.\n";
    }
    if(connection.checkUserProfileRecord("junais", "india"))
    {
        std::cout << "\nMatch Found.\n";   
    }
    else
    {
        std::cout << "\nNo match.\n";
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
    if(connection.updateUserPassword("juais", "pakistan","india"))
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
    
    std::vector<UserProfile> records1 = connection.getUserProfileRecord();
    for(auto it = records1.begin(); it != records1.end(); ++it)
    {
        std::cout << "\n" << (*it)._id << "\t" << (*it)._userName << "\t" << (*it)._password << "\n";
    }
    
    std::vector<Session> records2 = connection.getSessionRecord();
    for(auto it = records2.begin(); it != records2.end(); ++it)
    {
        std::cout << "\n" << (*it)._id << "\t" << (*it)._cookieId <<
            "\t" << (*it)._userName << "\t" << (*it)._loginTime << "\t" << (*it)._logoutTime << "\n";
    }
    
    std::vector<Permission> records3 = connection.getPermissionRecord();
    for(auto it = records3.begin(); it != records3.end(); ++it)
    {
        std::cout << "\n" << (*it)._id << "\t" << (*it)._resId <<
            "\t" << (*it)._userName << "\t" <<
            connection._permission.find((*it)._userPermission)->second << "\n";
    }
    return 0;
}
    
