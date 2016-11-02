#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "Santiago/SantiagoDBTables/MariaDBConnection.h"

using namespace Santiago::SantiagoDBTables;

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        std::cout << "Format is ./DatabaseTest <config.json>" << std::endl;
        return -1;
    }

    try 
    {
        boost::property_tree::ptree config;
        boost::property_tree::read_json(argv[1],config);
        MariaDBConnection connection(config);
        //UserPermission permission = READ_WRITE;
        std::error_code error;
       
        UserProfilesRec userProfileRecord;
        userProfileRecord._userName = "junais";
        userProfileRecord._emailAddress = "junais@gmail.com";
        userProfileRecord._password = "pakistan";

        connection.addUserProfilesRec(userProfileRecord, error);
        if(!error)
        {
            std::cout << "\nUser Profile Added.\n";   
        }
        else
        {
            std::cout << "\nUsername already used.\n";
        }

        userProfileRecord._userName = "vinay";
        userProfileRecord._emailAddress = "vinay@hotmail.com";
        userProfileRecord._password = "india";

        connection.addUserProfilesRec(userProfileRecord, error);
        if(!error)
        {
            std::cout << "\nUser Profile Added.\n";   
        }
        else
        {
            std::cout << "\nUsername already used.\n";
        }

        userProfileRecord._userName = "junais";
       
        connection.addUserProfilesRec(userProfileRecord, error);
        if(!error)
        {
            std::cout << "\nUser Profile Added.\n";   
        }
        else
        {
            std::cout << "\nUsername already used.\n";
        }

        connection.deleteUserProfilesRec("vinay", error);
        if(!error)
        {
            std::cout << "\nUser Profile Deleted.\n";   
        }
        else
        {
            std::cout << "\nInvalid User Id.\n";
        }
    
        connection.deleteUserProfilesRec("bineesh", error);
        if(!error)
        {
            std::cout << "\nUser Profile Deleted.\n";   
        }
        else
        {
            std::cout << "\nInvalid User Id.\n";
        }

        SessionsRec sessionRecord;
        sessionRecord._userName = "junais";
        sessionRecord._cookieString = "kefas";
        ptime loginTime(from_iso_string("20160612T120000"));
        sessionRecord._loginTime = loginTime;
        sessionRecord._lastActiveTime = loginTime;

        connection.addSessionsRec(sessionRecord, error);
        if(!error)
        {
            std::cout << "\nSession Added.\n"; 
        }
        else
        {
            std::cout << "\nSession Add Error.\n"; 
        }

        ptime logoutTime(from_iso_string("20160612T130015"));
        sessionRecord._logoutTime = logoutTime;
    
        connection.updateSessionsRec(sessionRecord, error);
        if(!error)
        {
            std::cout << "\nUpdated Session Record.\n"; 
        }
        else
        {
            std::cout << "\nSession Updation Error.\n"; 
        }

        sessionRecord._cookieString = "safek";
        connection.updateSessionsRec(sessionRecord, error);
        if(!error)
        {
            std::cout << "\nUpdated Session Record.\n"; 
        }
        else
        {
            std::cout << "\nSession Updation Error.\n"; 
        }

        /*PermissionsRec permissionRecord;
          permissionRecord._resId = 34;
          permissionRecord._userName = "junais";
          permissionRecord._userPermission = permission;
    
          if(!connection.addPermissionsRec(permissionRecord, error))
          {
          std::cout << "\nPermission Added.\n"; 
          }
          else
          {
          std::cout << "\nPermission Add Error.\n"; 
          }*/
    
        userProfileRecord._userName = "junais";
        userProfileRecord._password = "india";

        connection.updateUserProfilesRec(userProfileRecord, error);
        if(!error)
        {
            std::cout<< "\nPassword updated.\n";
        }
        else
        {
            std::cout << "\nInvalid User Id or Password.\n";
        }

        userProfileRecord._userName = "vinay";
        userProfileRecord._password = "pakistan";

        connection.updateUserProfilesRec(userProfileRecord, error);
        if(!error)
        {
            std::cout<< "\nPassword updated.\n";
        }
        else
        {
            std::cout << "\nInvalid User Id or Password.\n";
        }

        /*userProfileRecord._userName = "junais";
          userProfileRecord._password = "pakistan";
    
          if(!connection.updateUserProfilesRec(userProfileRecord,"india", error))
          {
          std::cout<< "\nPassword updated.\n";
          }
          else
          {
          std::cout << "\nInvalid User Id or Password.\n";
          }*/
    
        boost::optional<UserProfilesRec> userProfileRec;

        userProfileRec = connection.getUserProfilesRecForUserName("junais", error);
        if(!error && (userProfileRec->_id > 0))
        { 
            std::cout << "\n" << userProfileRec->_id << "\t"
                      << userProfileRec->_userName << "\t"
                      << userProfileRec->_password << "\n";
        }
        else
        {
            std::cout << "\nInvalid User Id.\n";
        }

        userProfileRec = connection.getUserProfilesRecForUserName("vinay", error);
        if(!error && (userProfileRec->_id > 0)) 
        {
            std::cout << "\n" << userProfileRec->_id << "\t"
                      << userProfileRec->_userName << "\t"
                      << userProfileRec->_password << "\n";
        }
        else
        {
            std::cout << "\nInvalid User Id.\n";
        }

        boost::optional<SessionsRec> sessionRec;
        sessionRec = connection.getSessionsRec("kefas", error);
        if(!error && (sessionRec->_id > 0))
        {
            std::cout << "\n" << sessionRec->_id << "\t"
                      << sessionRec->_userName << "\t"
                      << sessionRec->_cookieString << "\t"
                      << sessionRec->_loginTime << "\t"
                      << sessionRec->_logoutTime << "\t"
                      << sessionRec->_lastActiveTime << "\n";
        }
        else
        {
            std::cout << "\nInvalid Cookie String.\n";
        }

        sessionRec = connection.getSessionsRec("safek", error);
        if(!error && (sessionRec->_id > 0))
        {
            std::cout << "\n" << sessionRec->_id << "\t"
                      << sessionRec->_userName << "\t"
                      << sessionRec->_cookieString << "\t"
                      << sessionRec->_loginTime << "\t"
                      << sessionRec->_logoutTime << "\t"
                      << sessionRec->_lastActiveTime << "\n";
        }
        else
        {
            std::cout << "\nInvalid Cookie String.\n";
        }

        /*boost::optional<PermissionsRec> permissionRec = PermissionsRec();
        
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
          }*/

    }
    catch(std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    
    return 0;
}
    
