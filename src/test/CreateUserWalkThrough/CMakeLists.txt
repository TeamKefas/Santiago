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
				AsyncCreateUserHandler.cpp	
	  			AsyncLoginUserHandler.cpp
				AsyncLogoutUserHandler.cpp
				AsyncLogoutUserForAllCookieHandler.cpp
				AsyncGetUserInfoHandler.cpp
				AsyncDeleteUserHandler.cpp)

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
    AsyncCreateUserHandler.ch
    AsyncLoginUserHandler.h
    AsyncLogoutUserHandler.h
    AsyncLogoutUserForAllCookieHandler.h
    AsyncGetUserInfoHandler.h
    AsyncDeleteUserHandler.h)
endif(WIN32)



add_executable(CreateUserWalkThroughExec ${CreateUserWalkThrough_SRCS}) 
target_link_libraries(CreateUserWalkThroughExec Santiago ${MARIADB_LIBRARIES} ${Boost_LIBRARIES}) 
