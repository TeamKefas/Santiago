#include <iostream>

#include "AuthenticatorTesterBase.h"
#include "Santiago/Authentication/AuthenticatorBaseV1.h"

namespace Test{ namespace Authentication
    {
        void AuthenticatorTesterBase::printResult(const std::string& description, bool value)
        {
            std::cout<<description<<" : "<<(value?"[FAILED]":"[SUCCEEDED]")<<std::endl;
        }

        void AuthenticatorTesterBase::runTests(boost::asio::yield_context yieldContext_)
        {
            //first create 3 users santiagoTestUser1, santiagoTestUser2, santiagoTestUser3
            //call spawn on run1UserTests, run2UserTests
        
            SantiagoTestUser user1,user2,user3;

            user1.userName = "user1";
            user1.password = "pass1";
            user1.emailAddress = "user1@email";

            user2.userName = "user2";
            user2.password = "pass2";
            user2.emailAddress = "user2@email";
        
            user3.userName = "user3";
            user3.password = "pass3";
            user3.emailAddress = "user3@email";


            boost::asio::spawn(_ioService, std::bind(&AuthenticatorTesterBase::run1UserTests, this, yieldContext_, user1));
            boost::asio::spawn(_ioService, std::bind(&AuthenticatorTesterBase::run1UserTests, this, yieldContext_, user2));
            boost::asio::spawn(_ioService, std::bind(&AuthenticatorTesterBase::run1UserTests, this, yieldContext_, user3));
            boost::asio::spawn(_ioService, std::bind(&AuthenticatorTesterBase::run2UserTests, this, yieldContext_, user2, user3));
            boost::asio::spawn(_ioService, std::bind(&AuthenticatorTesterBase::run2UserTests, this, yieldContext_, user1, user3));
            boost::asio::spawn(_ioService, std::bind(&AuthenticatorTesterBase::run2UserTests, this, yieldContext_, user1, user2));

        }
    
        void AuthenticatorTesterBase::run1UserTests(boost::asio::yield_context yieldContext_,
                                                    const SantiagoTestUser& santiagoTestUser1)
        {
            std::error_code error;
            _authenticatorBasePtr->createUser(santiagoTestUser1.userName,
                                              santiagoTestUser1.emailAddress,
                                              santiagoTestUser1.password,
                                              yieldContext_,
                                              error);
            printResult("Create user", (bool)error);

            _authenticatorBasePtr->createUser(santiagoTestUser1.userName,
                                              santiagoTestUser1.emailAddress,
                                              santiagoTestUser1.password,
                                              yieldContext_,
                                              error);
            printResult("Failing create user", (bool)error);
                     
            boost::optional<std::pair<Santiago::Authentication::UserInfo, std::string> > userInfo1 = _authenticatorBasePtr->loginUser(santiagoTestUser1.userName,
                                                                                                                                      1,
                                                                                                                                      santiagoTestUser1.password,
                                                                                                                                      yieldContext_,
                                                                                                                                      error);
            printResult("Login with username", (bool)error);

            boost::optional<std::pair<Santiago::Authentication::UserInfo, std::string> > userInfo2 = _authenticatorBasePtr->loginUser(santiagoTestUser1.userName,
                                                                                                                                      0,
                                                                                                                                      santiagoTestUser1.password,
                                                                                                                                      yieldContext_,
                                                                                                                                      error);
            printResult("Failing login with username", (bool)error);

            boost::optional<std::pair<Santiago::Authentication::UserInfo, std::string> > userInfo3 = _authenticatorBasePtr->loginUser("testuser",
                                                                                                                                      1,
                                                                                                                                      santiagoTestUser1.password,
                                                                                                                                      yieldContext_,
                                                                                                                                      error);
            printResult("Failing login with username", (bool)error);
        
            boost::optional<std::pair<Santiago::Authentication::UserInfo, std::string> > userInfo4 = _authenticatorBasePtr->loginUser(santiagoTestUser1.emailAddress,
                                                                                                                                      0,
                                                                                                                                      santiagoTestUser1.password,
                                                                                                                                      yieldContext_,
                                                                                                                                      error);
            printResult("Login with e-mail", (bool)error);

            boost::optional<std::pair<Santiago::Authentication::UserInfo, std::string> > userInfo5 = _authenticatorBasePtr->loginUser(santiagoTestUser1.emailAddress,
                                                                                                                                      1,
                                                                                                                                      santiagoTestUser1.password,
                                                                                                                                      yieldContext_,
                                                                                                                                      error);
            printResult("Failing login with e-mail", (bool)error);

            boost::optional<std::pair<Santiago::Authentication::UserInfo, std::string> > userInfo6 = _authenticatorBasePtr->loginUser("test@email",
                                                                                                                                      0,
                                                                                                                                      santiagoTestUser1.password,
                                                                                                                                      yieldContext_,
                                                                                                                                      error);
            printResult("Failing login with e-mail", (bool)error);

             const std::string cookieString = userInfo1->second;
             _authenticatorBasePtr->changeUserPassword(cookieString,
                                                      santiagoTestUser1.password,
                                                      "new",
                                                      yieldContext_,
                                                      error);
            printResult("Change password", (bool)error);

            _authenticatorBasePtr->changeUserEmailAddress(cookieString,
                                                          "test@gmail",
                                                          santiagoTestUser1.password,
                                                          yieldContext_,
                                                          error);      
            printResult("Failing change e-mail address", (bool)error);

            _authenticatorBasePtr->changeUserEmailAddress(cookieString,
                                                          "test@gmail",
                                                          "new",
                                                          yieldContext_,
                                                          error);      
            printResult("Change e-mail address", (bool)error);
        
            _authenticatorBasePtr->logoutUserForCookie(cookieString,
                                                       yieldContext_,
                                                       error);
            printResult("Logout for cookie", (bool)error);

            _authenticatorBasePtr->logoutUserForCookie("cookiestring",
                                                       yieldContext_,
                                                       error);
            printResult("Failing logout for cookie", (bool)error);

            _authenticatorBasePtr->logoutUserForAllCookies(santiagoTestUser1.userName,
                                                           yieldContext_,
                                                           error);
            printResult("Logout for all cookies", (bool)error);

            _authenticatorBasePtr->logoutUserForAllCookies("username",
                                                           yieldContext_,
                                                           error);
            printResult("Failing logout for all cookies", (bool)error);
        
            _authenticatorBasePtr->changeUserPassword(cookieString,
                                                      santiagoTestUser1.password,
                                                      "new",
                                                      yieldContext_,
                                                      error);
            printResult("Failing change password", (bool)error);
        
            _authenticatorBasePtr->changeUserEmailAddress(cookieString,
                                                          "test@hotmail.com",
                                                          santiagoTestUser1.password,
                                                          yieldContext_,
                                                          error);      
            printResult("Failing change e-mail address", (bool)error);

            boost::optional<std::string> recoveryString1 = _authenticatorBasePtr->createAndReturnRecoveryString(santiagoTestUser1.emailAddress,
                                                                                                               yieldContext_,
                                                                                                               error);
            printResult("Failing create recovery string", (bool)error);

            boost::optional<std::string> recoveryString2 = _authenticatorBasePtr->createAndReturnRecoveryString("test@gmail",
                                                                                                               yieldContext_,
                                                                                                               error);
            printResult("Create recovery string", (bool)error);

            boost::optional<std::string> userName1 = _authenticatorBasePtr->getUserForEmailAddressAndRecoveryString("totest@gmail",
                                                                                                                   *recoveryString2,
                                                                                                                   yieldContext_,
                                                                                                                   error);
            printResult("Failing get user for e-mail address and recovery string", (bool)error);

            boost::optional<std::string> userName2 = _authenticatorBasePtr->getUserForEmailAddressAndRecoveryString("test@gmail",
                                                                                                                   *recoveryString2,
                                                                                                                   yieldContext_,
                                                                                                                   error);
            printResult("Get user for e-mail address and recovery string", (bool)error);
        
            /*_authenticatorBasePtr->changeUserPasswordForEmailAddressAndRecoveryString("vnvijayaraj@gmail.com",
                                                                                      *recoveryString,
                                                                                      "newpass",
                                                                                      yieldContext_,
                                                                                      error);
            printResult("Change password for e-mail address and recovery string", (bool)error);
            */
            _authenticatorBasePtr->deleteUser(cookieString,
                                              yieldContext_,
                                              error);
            printResult("Failing delete user", (bool)error);

            boost::optional<std::pair<Santiago::Authentication::UserInfo, std::string> > userInfo7 = _authenticatorBasePtr->loginUser(santiagoTestUser1.userName,
                                                                                                                                      1,
                                                                                                                                      "new",
                                                                                                                                      yieldContext_,
                                                                                                                                      error);
            printResult("Login with username", (bool)error);

            std::string newCookieString = userInfo7->second;
            _authenticatorBasePtr->deleteUser(newCookieString,
                                              yieldContext_,
                                              error);
            printResult("Delete user", (bool)error);
        }
    
        void AuthenticatorTesterBase::run2UserTests(boost::asio::yield_context yieldContext_,
                                                    const SantiagoTestUser& santiagoTestUser1,
                                                    const SantiagoTestUser& santiagoTestUser2)
        {
            std::error_code error;
            _authenticatorBasePtr->createUser(santiagoTestUser1.userName,
                                              santiagoTestUser1.emailAddress,
                                              santiagoTestUser1.password,
                                              yieldContext_,
                                              error);
            printResult("Create user", (bool)error);

            _authenticatorBasePtr->createUser(santiagoTestUser1.userName,
                                              santiagoTestUser1.emailAddress,
                                              santiagoTestUser1.password,
                                              yieldContext_,
                                              error);
            printResult("Failing create user", (bool)error);

            _authenticatorBasePtr->createUser(santiagoTestUser2.userName,
                                              santiagoTestUser2.emailAddress,
                                              santiagoTestUser2.password,
                                              yieldContext_,
                                              error);
            printResult("Create user", (bool)error);

            _authenticatorBasePtr->createUser(santiagoTestUser2.userName,
                                              santiagoTestUser2.emailAddress,
                                              santiagoTestUser2.password,
                                              yieldContext_,
                                              error);
            printResult("Failing create user", (bool)error);
             
            boost::optional<std::pair<Santiago::Authentication::UserInfo, std::string> > userInfo1 = _authenticatorBasePtr->loginUser(santiagoTestUser1.userName,
                                                                                                                                      1,
                                                                                                                                      santiagoTestUser1.password,
                                                                                                                                      yieldContext_,
                                                                                                                                      error);
            printResult("Login with username", (bool)error);

            boost::optional<std::pair<Santiago::Authentication::UserInfo, std::string> > userInfo3 = _authenticatorBasePtr->loginUser(santiagoTestUser1.userName,
                                                                                                                                      0,
                                                                                                                                      santiagoTestUser1.password,
                                                                                                                                      yieldContext_,
                                                                                                                                      error);
            printResult("Failing login with username", (bool)error);

            boost::optional<std::pair<Santiago::Authentication::UserInfo, std::string> > userInfo4 = _authenticatorBasePtr->loginUser("userName",
                                                                                                                                      1,
                                                                                                                                      santiagoTestUser1.password,
                                                                                                                                      yieldContext_,
                                                                                                                                      error);
            printResult("Failing login with username", (bool)error);

            boost::optional<std::pair<Santiago::Authentication::UserInfo, std::string> > userInfo2 = _authenticatorBasePtr->loginUser(santiagoTestUser2.userName,
                                                                                                                                      1,
                                                                                                                                      santiagoTestUser2.password,
                                                                                                                                      yieldContext_,
                                                                                                                                      error);
            printResult("Login with username", (bool)error);

            boost::optional<std::pair<Santiago::Authentication::UserInfo, std::string> > userInfo5 = _authenticatorBasePtr->loginUser(santiagoTestUser2.userName,
                                                                                                                                      0,
                                                                                                                                      santiagoTestUser2.password,
                                                                                                                                      yieldContext_,
                                                                                                                                      error);
            printResult("Failing login with username", (bool)error);

            boost::optional<std::pair<Santiago::Authentication::UserInfo, std::string> > userInfo6 = _authenticatorBasePtr->loginUser("testname",
                                                                                                                                      1,
                                                                                                                                      santiagoTestUser2.password,
                                                                                                                                      yieldContext_,
                                                                                                                                      error);
            printResult("Failing login with username", (bool)error);
        
            boost::optional<std::pair<Santiago::Authentication::UserInfo, std::string> > userInfo7 = _authenticatorBasePtr->loginUser(santiagoTestUser1.emailAddress,
                                                                                                                                      0,
                                                                                                                                      santiagoTestUser1.password,
                                                                                                                                      yieldContext_,
                                                                                                                                      error);
            printResult("Login with e-mail", (bool)error);

            boost::optional<std::pair<Santiago::Authentication::UserInfo, std::string> > userInfo9 = _authenticatorBasePtr->loginUser(santiagoTestUser1.emailAddress,
                                                                                                                                      1,
                                                                                                                                      santiagoTestUser1.password,
                                                                                                                                      yieldContext_,
                                                                                                                                      error);
            printResult("Failing login with e-mail", (bool)error);

            boost::optional<std::pair<Santiago::Authentication::UserInfo, std::string> > userInfo10 = _authenticatorBasePtr->loginUser("email",
                                                                                                                                       0,
                                                                                                                                       santiagoTestUser1.password,
                                                                                                                                       yieldContext_,
                                                                                                                                       error);
            printResult("Failing login with e-mail", (bool)error);
        
            boost::optional<std::pair<Santiago::Authentication::UserInfo, std::string> > userInfo8 = _authenticatorBasePtr->loginUser(santiagoTestUser2.emailAddress,
                                                                                                                                      0,
                                                                                                                                      santiagoTestUser2.password,
                                                                                                                                      yieldContext_,
                                                                                                                                      error);
            printResult("Login with e-mail", (bool)error);

            boost::optional<std::pair<Santiago::Authentication::UserInfo, std::string> > userInfo11 = _authenticatorBasePtr->loginUser(santiagoTestUser2.emailAddress,
                                                                                                                                      1,
                                                                                                                                      santiagoTestUser2.password,
                                                                                                                                      yieldContext_,
                                                                                                                                      error);
            printResult("Failing login with e-mail", (bool)error);

            boost::optional<std::pair<Santiago::Authentication::UserInfo, std::string> > userInfo12 = _authenticatorBasePtr->loginUser("e-mail",
                                                                                                                                      0,
                                                                                                                                      santiagoTestUser2.password,
                                                                                                                                      yieldContext_,
                                                                                                                                      error);
            printResult("Failing login with e-mail", (bool)error);
        
            const std::string cookieString1 = userInfo1->second;
            const std::string cookieString2 = userInfo2->second;
            _authenticatorBasePtr->changeUserPassword(cookieString1,
                                                      santiagoTestUser1.password,
                                                      "new",
                                                      yieldContext_,
                                                      error);
            printResult("Change password", (bool)error);

            _authenticatorBasePtr->changeUserPassword(cookieString2,
                                                      santiagoTestUser2.password,
                                                      "new",
                                                      yieldContext_,
                                                      error);
            printResult("Change password", (bool)error);

            _authenticatorBasePtr->changeUserEmailAddress(cookieString1,
                                                          "new1@email",
                                                          santiagoTestUser1.password,
                                                          yieldContext_,
                                                          error);        
            printResult("Failing change e-mail address", (bool)error);

            _authenticatorBasePtr->changeUserEmailAddress(cookieString2,
                                                          "new2@email",
                                                          santiagoTestUser2.password,
                                                          yieldContext_,
                                                          error);        
            printResult("Failing change e-mail address", (bool)error);

            _authenticatorBasePtr->changeUserEmailAddress(cookieString1,
                                                          "new1@email",
                                                          "new",
                                                          yieldContext_,
                                                          error);        
            printResult("Change e-mail address", (bool)error);

            _authenticatorBasePtr->changeUserEmailAddress(cookieString2,
                                                          "new2@email",
                                                          "new",
                                                          yieldContext_,
                                                          error);        
            printResult("Change e-mail address", (bool)error);
            
            _authenticatorBasePtr->logoutUserForCookie(cookieString1,
                                                       yieldContext_,
                                                       error);
            printResult("Logout for cookie", (bool)error);

            _authenticatorBasePtr->logoutUserForCookie("cookieString",
                                                       yieldContext_,
                                                       error);
            printResult("Failing logout for cookie", (bool)error);

            _authenticatorBasePtr->logoutUserForCookie(cookieString2,
                                                       yieldContext_,
                                                       error);
            printResult("Logout for cookie", (bool)error);

            _authenticatorBasePtr->logoutUserForAllCookies(santiagoTestUser1.userName,
                                                           yieldContext_,
                                                           error);
            printResult("Logout for all cookies", (bool)error);

            _authenticatorBasePtr->logoutUserForAllCookies("userName",
                                                           yieldContext_,
                                                           error);
            printResult("Failing logout for all cookies", (bool)error);

            _authenticatorBasePtr->logoutUserForAllCookies(santiagoTestUser2.userName,
                                                           yieldContext_,
                                                           error);
            printResult("Logout for all cookies", (bool)error);

            _authenticatorBasePtr->changeUserPassword(cookieString1,
                                                      santiagoTestUser1.password,
                                                      "new",
                                                      yieldContext_,
                                                      error);
            printResult("Failing change password", (bool)error);

            _authenticatorBasePtr->changeUserPassword(cookieString2,
                                                      santiagoTestUser2.password,
                                                      "new",
                                                      yieldContext_,
                                                      error);
            printResult("Failing change password", (bool)error);
        
            _authenticatorBasePtr->changeUserEmailAddress(cookieString1,
                                                          "new1@email",
                                                          santiagoTestUser1.password,
                                                          yieldContext_,
                                                          error);        
            printResult("Failing change e-mail address", (bool)error);

            _authenticatorBasePtr->changeUserEmailAddress(cookieString2,
                                                          "new2@email",
                                                          santiagoTestUser2.password,
                                                          yieldContext_,
                                                          error);        
            printResult("Failing change e-mail address", (bool)error);
        
            boost::optional<std::string> recoveryString1 = _authenticatorBasePtr->createAndReturnRecoveryString(santiagoTestUser1.emailAddress,
                                                                                                                yieldContext_,
                                                                                                                error);
            printResult("Failing create recovery string", (bool)error);

            boost::optional<std::string> recoveryString2 = _authenticatorBasePtr->createAndReturnRecoveryString("new1@email",
                                                                                                                yieldContext_,
                                                                                                                error);
            printResult("Create recovery string", (bool)error);
            
            boost::optional<std::string> userName3 = _authenticatorBasePtr->getUserForEmailAddressAndRecoveryString("new@email",
                                                                                                                    *recoveryString2,
                                                                                                                    yieldContext_,
                                                                                                                    error);
            printResult("Failing get user for e-mail address and recovery string", (bool)error);
            
            boost::optional<std::string> userName1 = _authenticatorBasePtr->getUserForEmailAddressAndRecoveryString("new1@email",
                                                                                                                    *recoveryString2,
                                                                                                                    yieldContext_,
                                                                                                                    error);
            printResult("Get user for e-mail address and recovery string", (bool)error);

            boost::optional<std::string> recoveryString3 = _authenticatorBasePtr->createAndReturnRecoveryString(santiagoTestUser2.emailAddress,
                                                                                                                yieldContext_,
                                                                                                                error);
            printResult("Failing create recovery string", (bool)error);

            boost::optional<std::string> recoveryString4 = _authenticatorBasePtr->createAndReturnRecoveryString("new2@email",
                                                                                                                yieldContext_,
                                                                                                                error);
            printResult("Create recovery string", (bool)error);

            boost::optional<std::string> userName4 = _authenticatorBasePtr->getUserForEmailAddressAndRecoveryString("new@email",
                                                                                                                    *recoveryString4,
                                                                                                                    yieldContext_,
                                                                                                                    error);
            printResult("Failing get user for e-mail address and recovery string", (bool)error);

            boost::optional<std::string> userName2 = _authenticatorBasePtr->getUserForEmailAddressAndRecoveryString("new2@email",
                                                                                                                    *recoveryString4,
                                                                                                                    yieldContext_,
                                                                                                                    error);
            printResult("Get user for e-mail address and recovery string", (bool)error);
        
            /*_authenticatorBasePtr->changeUserPasswordForEmailAddressAndRecoveryString("vnvijayaraj@gmail.com",
                                                                                      *recoveryString1,
                                                                                      "newpass",
                                                                                      yieldContext_,
                                                                                      error);
            printResult("Change password for e-mail address and recovery string", (bool)error);

            _authenticatorBasePtr->changeUserPasswordForEmailAddressAndRecoveryString("vnvijayaraj@gmail.com",
                                                                                      *recoveryString2,
                                                                                      "newpass",
                                                                                      yieldContext_,
                                                                                      error);
            printResult("Change password for e-mail address and recovery string", (bool)error);
            */
            _authenticatorBasePtr->deleteUser(cookieString1,
                                              yieldContext_,
                                              error);
            printResult("Failing delete user", (bool)error);

            _authenticatorBasePtr->deleteUser(cookieString2,
                                              yieldContext_,
                                              error);
            printResult("Failing delete user", (bool)error);

            boost::optional<std::pair<Santiago::Authentication::UserInfo, std::string> > userInfo13 = _authenticatorBasePtr->loginUser(santiagoTestUser1.userName,
                                                                                                                                      1,
                                                                                                                                      "new",
                                                                                                                                      yieldContext_,
                                                                                                                                      error);
            printResult("Login with username", (bool)error);

            boost::optional<std::pair<Santiago::Authentication::UserInfo, std::string> > userInfo14 = _authenticatorBasePtr->loginUser(santiagoTestUser2.userName,
                                                                                                                                      1,
                                                                                                                                      "new",
                                                                                                                                      yieldContext_,
                                                                                                                                      error);
            printResult("Login with username", (bool)error);

            std::string newCookieString1 = userInfo13->second;
            std::string newCookieString2 = userInfo14->second;
            _authenticatorBasePtr->deleteUser(newCookieString1,
                                              yieldContext_,
                                              error);
            printResult("Delete user", (bool)error);

            _authenticatorBasePtr->deleteUser(newCookieString2,
                                              yieldContext_,
                                              error);
            printResult("Delete user", (bool)error);
        }
    
    }}
