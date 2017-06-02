set(CreateUserWalkThrough_SRCS  main.cpp 
                                RequestHandlerBase.cpp 
                                CreateUserHandler.cpp 
                                LogoutUserHandler.cpp 
                                LoginUserHandler.cpp 
                                LogoutUserForAllCookieHandler.cpp 
                                GetUserInfoHandler.cpp
                                DeleteUserHandler.cpp
				ChangePasswordHandler.cpp
				PasswordRecoveryInitiateRecoveryHandler.cpp
				PasswordRecoveryReceiveNewPasswordHandler.cpp
				PasswordRecoverySetNewPasswordHandler.cpp
				Asynchronous/AsyncCreateUserHandler.cpp	
	  			Asynchronous/AsyncLoginUserHandler.cpp
				Asynchronous/AsyncLogoutUserHandler.cpp
				Asynchronous/AsyncLogoutUserForAllCookieHandler.cpp
				Asynchronous/AsyncGetUserInfoHandler.cpp
				Asynchronous/AsyncDeleteUserHandler.cpp
				Asynchronous/AsyncPasswordRecoveryInitiateRecoveryHandler.cpp
				Asynchronous/AsyncPasswordRecoveryReceiveNewPasswordHandler.cpp
				Asynchronous/AsyncPasswordRecoverySetNewPasswordHandler.cpp)

if(WIN32)
  set(CreateUserWalkThrough_SRCS 
    ${CreateUserWalkThroug_SRCS}
    Server.h
    RequestHandlerBase.h
    CreateUserHandler.h
    ErrorURIHandler.h
    LoginUserHandler.h
    LogoutUserHandler.h
    LogoutUserForAllCookieHandler.h
    GetUserInfoHandler.h
    DeleteUserHandler.h
    ChangePasswordHandler.h
    PasswordRecoveryInitiateRecoveryHandler.h
    PasswordRecoveryReceiveNewPasswordHandler.h
    PasswordRecoverySetNewPasswordHandler.h
    Asynchronous/AsyncCreateUserHandler.h
    Asynchronous/AsyncLoginUserHandler.h
    Asynchronous/AsyncLogoutUserHandler.h
    Asynchronous/AsyncLogoutUserForAllCookieHandler.h
    Asynchronous/AsyncGetUserInfoHandler.h
    Asynchronous/AsyncDeleteUserHandler.h
    Asynchronous/AsyncPasswordRecoveryInitiateRecoveryHandler.h
    Asynchronous/AsyncPasswordRecoveryReceiveNewPasswordHandler.h
    Asynchronous/AsyncPasswordRecoverySetNewPasswordHandler.h)
endif(WIN32)



add_executable(CreateUserWalkThroughExec ${CreateUserWalkThrough_SRCS}) 
target_link_libraries(CreateUserWalkThroughExec Santiago ${MARIADB_LIBRARIES} ${Boost_LIBRARIES}) 
