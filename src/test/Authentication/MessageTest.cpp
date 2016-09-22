#include "Message.h"
#include <iostream>

int main()
{
    std::stringstream str;
    int temp, tp, size, nop;
    std::string tempStr;
    std::cout << "Enter number of parameters:";
    std::cin >> nop;
    size = 8 + (nop * 4); 
    std::vector<int> parameterSize;
    std::vector<std::string> parameter;
    for(int i = 0 ; i < nop ; ++i)
    {
        std::cin >> tempStr;
        size += tempStr.size();
        parameterSize.push_back(tempStr.size());
        parameter.push_back(tempStr);
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
    str.write(reinterpret_cast<char*>(&tp), sizeof(int));
    str.write(reinterpret_cast<char*>(&nop), sizeof(int));
    auto it2 = parameter.begin();
    for(auto it1 = parameterSize.begin(); it1 != parameterSize.end(); ++it1, ++it2)
    {
        int paraSize = *it1;
        str.write(reinterpret_cast<char*>(&paraSize), sizeof(int));
        str << *it2;
    }
    std::cout << "\n" << str.str(); 
    Santiago::Authentication::ConnectionMessage message(str.str().c_str(), size);
    std::cout << "\n" << enumInt.find(message._type)->second << "\n";
    for(auto it = message._parameters.begin(); it != message._parameters.end(); ++it)
    {
        temp = (*it).size();
        std::cout << temp << "\t" << *it << "\n";
    }

    std::stringbuf str1;
    std::ostream os(&str1);
    message.writeToStream(os);
    std::cout << "\n" << str1.str(); // printing str.str() and str1.str() gives same result.
    return 0;
}
