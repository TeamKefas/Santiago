set (DatabaseTest_SRCS DatabaseTest.cpp)
add_executable(DatabaseTest ${DatabaseTest_SRCS})
target_link_libraries(DatabaseTest Santiago  ${Boost_LIBRARIES})
