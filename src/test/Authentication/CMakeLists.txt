#define the variable echo-external_SRCS
set (TestAuthentication_SRCS MessageTest.cpp)

add_executable(TestAuthentication ${TestAuthentication_SRCS}) 
target_link_libraries(TestAuthentication ${Boost_LIBRARIES})
