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
        std::error_code error;          
       
        UsersRec userRecord;
        userRecord._userName = "junais";
        userRecord._emailAddress = "junais@gmail.com";
        userRecord._password = "pakistan";

        connection.addUsersRec(userRecord, error);
        if(!error)
        {
            std::cout << "\nUser Added.\n";   
        }
        else
        {
            std::cout << "\nUsername already used.\n";
        }

        userRecord._userName = "vinay";
        userRecord._emailAddress = "vinay@hotmail.com";
        userRecord._password = "india";

        connection.addUsersRec(userRecord, error);
        if(!error)
        {
            std::cout << "\nUser Added.\n";   
        }
        else
        {
            std::cout << "\nUsername already used.\n";
        }

        userRecord._userName = "junais";
       
        connection.addUsersRec(userRecord, error);
        if(!error)
        {
            std::cout << "\nUser Added.\n";   
        }
        else
        {
            std::cout << "\nUsername already used.\n";
        }

        connection.deleteUsersRec("vinay", error);
        if(!error)
        {
            std::cout << "\nUser Deleted.\n";   
        }
        else
        {
            std::cout << "\nInvalid Username.\n";
        }
    
        connection.deleteUsersRec("bineesh", error);
        if(!error)
        {
            std::cout << "\nUser  Deleted.\n";   
        }
        else
        {
            std::cout << "\nInvalid Username.\n";
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

        userRecord._userName = "junais";
        userRecord._emailAddress = "junais@gmail.com";
        userRecord._password = "india";

        connection.updateUsersRec(userRecord, error);
        if(!error)
        {
            std::cout<< "\nUser Record updated.\n";
        }
        else
        {
            std::cout << "\nInvalid Username or Password.\n";
        }

        userRecord._userName = "vinay";
        userRecord._password = "pakistan";

        connection.updateUsersRec(userRecord, error);
        if(!error)
        {
            std::cout<< "\nUser Record updated.\n";
        }
        else
        {
            std::cout << "\nInvalid Username or Password.\n";
        }

        userRecord._userName = "junais";
        userRecord._password = "pakistan";
      
        boost::optional<UsersRec> userRec;

        userRec = connection.getUsersRecForUserName("junais", error);
        if(!error && (userRec->_id > 0))
        { 
            std::cout << "\n" << userRec->_id << "\t"
                      << userRec->_userName << "\t"
                      << userRec->_emailAddress << "\t"
                      << userRec->_password << "\n";
        }
        else
        {
            std::cout << "\nInvalid Username.\n";
        }

        userRec = connection.getUsersRecForEmailAddress("junais@gmail.com", error);
        if(!error && (userRec->_id > 0))
        { 
            std::cout << "\n" << userRec->_id << "\t"
                      << userRec->_userName << "\t"
                      << userRec->_emailAddress << "\t"
                      << userRec->_password << "\n";
        }
        else
        {
            std::cout << "\nInvalid Email Id.\n";
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

        std::vector<SessionsRec> rec;
        rec = connection.getActiveSessions(error);
        for(auto it = rec.begin(); it != rec.end(); ++it)
        {
            std::cout << "\n" << it->_id << "\t" << it->_userName << std::endl;
        }
    }
    catch(std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    
    return 0;
}
    
