#include "ConnectionMessage.h"

namespace Santiago{ namespace Authentication
{
    ConnectionMessage::ConnectionMessage(const char* content_, unsigned size_)
    {
        unsigned curPos = 0;
        //parse the type
        _type = *reinterpret_cast<const ConnectionMessageType*>(content_ + curPos);
        curPos += sizeof(ConnectionMessageType);
        //parse the noOfParameters
        unsigned noOfParameters = *reinterpret_cast<const unsigned*>(content_ + curPos);        
        curPos += sizeof(unsigned);
        //parse the parameters
        while(curPos < size_)
        {
            //parse the parameter size
            unsigned parameterSize = *reinterpret_cast<const unsigned*>(content_ + curPos);
            curPos += sizeof(unsigned);
            //check for size overflow
            if(curPos + parameterSize > size_)
            {
                throw std::runtime_error("Invalid message format: Parameter size causing size overflow.");
            }

            _parameters.push_back(std::string(content_ + curPos, parameterSize));
            curPos += parameterSize;
            //TODO: check for parameter size inconsistency
            /*if()
            {
                throw std::runtime_error("Invalid message format: Parameter size does not match.");
                }*/
        }
        //check for no of parameters inconsistency.
        if(_parameters.size() != noOfParameters)
        {
            throw std::runtime_error("Invalid message format: Number of parameters does not match.");
        }
    }

    ConnectionMessage::ConnectionMessage(ConnectionMessageType type_, const std::vector<std::string>& parameters_)
        :_type(type_)
        ,_parameters(parameters_)
    {}
      
    std::ostream& ConnectionMessage::writeToStream(std::ostream& outStream_) const
    {
        outStream_.write(reinterpret_cast<const char*>(&_type), sizeof(_type));
        unsigned noOfParameters = _parameters.size();
        outStream_.write(reinterpret_cast<const char*>(&noOfParameters), sizeof(noOfParameters));

        for(auto it = _parameters.begin(); it != _parameters.end(); ++it)
        {
            unsigned parameterSize = it->size();
            outStream_.write(reinterpret_cast<const char*>(&parameterSize), sizeof(parameterSize));
            outStream_ << *it; 
        }

        return outStream_;
    }

    unsigned ConnectionMessage::getSize() const
    {
        unsigned size = sizeof(ConnectionMessageType) + sizeof(unsigned);
        for(unsigned i=0;i<_parameters.size();i++)
        {
            size += sizeof(unsigned) + _parameters[i].size();
        }
        return size;
    }
    
}}//closing Santiago::Authentication 
