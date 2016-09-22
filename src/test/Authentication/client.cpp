#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <unistd.h>
#include "Message.h"
#include <sstream>
#include <vector>
#include <string>
using namespace Santiago::Authentication;
using boost::asio::ip::tcp;

int main()
{
  try
  {
   
    boost::asio::io_service io_service;
    tcp::resolver resolver(io_service);
    tcp::resolver::query query("localhost","4024");
    tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    tcp::socket socket(io_service);
    boost::asio::connect(socket, endpoint_iterator);
    boost::array<char, 100000> buf;

    std::stringstream _inputBuffer;
    int temp, tp, size, nop;
    std::string tempStr;
    std::cout << "Enter number of parameters:";
    std::cin >> nop;
    size = 12 + (nop * 4); 
    std::vector<int> parameterSize;
    std::vector<std::string> parameter;
    for(int i = 0 ; i < nop ; ++i)
    {
        std::cin >> tempStr;
        parameterSize.push_back(tempStr.size());
        parameter.push_back(tempStr);
    }
    for(auto it = parameterSize.begin(); it != parameterSize.end(); ++it)
    {
        size += *it;
    }

    MessageType type = MessageType::CREATE_USER;
    std::map<MessageType, int> enumInt;
    enumInt = {{MessageType::SUCCEEDED, 1},
               {MessageType::FAILED, 0},
               {MessageType::CREATE_USER, 65},
               {MessageType::LOGIN_USER, 66},
               {MessageType::VERIFY_USER_FOR_COOKIE, 67},
               {MessageType::LOGOUT_USER_FOR_COOKIE, 68},
               {MessageType::LOGOUT_USER_FOR_ALL_COOKIES, 69},
               {MessageType::CHANGE_USER_PASSWORD, 70}};
    tp = enumInt.find(type)->second;
    _inputBuffer.write(reinterpret_cast<char*>(&size), sizeof(int));
    _inputBuffer.write(reinterpret_cast<char*>(&tp), sizeof(int));
    _inputBuffer.write(reinterpret_cast<char*>(&nop), sizeof(int));
    auto it2 = parameter.begin();
    for(auto it1 = parameterSize.begin(); it1 != parameterSize.end(); ++it1, ++it2)
    {
        int paraSize = *it1;
        _inputBuffer.write(reinterpret_cast<char*>(&paraSize), sizeof(int));
        _inputBuffer << *it2;
    }

    std::string msg = _inputBuffer.str();
    
    boost::system::error_code error;
    boost::asio::write(socket, boost::asio::buffer(msg),
                       boost::asio::transfer_all(),error);
    if(!error)
        std::cout<<"send";
  }
  catch (std::exception& e)
  {
      std::cerr << e.what() << std::endl;
  }
  return 0;
}
