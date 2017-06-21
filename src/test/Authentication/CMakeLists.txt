set(TestAuthentication_SRCS  
	AuthenticatorTesterBase.cpp
	SingleNode/AuthenticatorTester.cpp
	SingleNode/SingleNodeAuthenticationTest.cpp)
                               

if(WIN32)
  set(TestAuthentication_SRCS
    ${TestAuthentication_SRCS}
    AuthenticatorTesterBase.h
    SingleNode/AuthenticatorTester.h)
endif(WIN32)



add_executable(TestAuthenticationExec ${TestAuthentication_SRCS}) 
target_link_libraries(TestAuthenticationExec Santiago ${MARIADB_LIBRARIES} ${Boost_LIBRARIES}) 
