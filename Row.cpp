//
//  Row.cpp
//  PA3
//
//  Created by rick gessner on 4/2/22.
//


#include <stdio.h>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <memory>
#include "Row.hpp"

namespace ECE141 {
  
  Row::Row(uint32_t entityId) {}
  Row::Row(const Row &aRow) {*this=aRow;}

 // Row::~Row() {}

  Row& Row::operator=(const Row &aRow) {
      
      data = aRow.data;
      return *this;
      
    
  }
  bool Row::operator==(Row &aCopy) const {return false;}

  //STUDENT: What other methods do you require?
                      
 // Row& Row::set(const std::string &aKey,const Value &aValue) {
  //  return *this;
//  }
    
StatusResult  Row::encode(std::ostream &anOutput) const{

    anOutput << "{\n";
    for(auto& i: data){
        anOutput << i.first << ":";
        if(i.second.index() == 0){anOutput << std::get<bool>(i.second);}
        if(i.second.index() == 1){anOutput << std::get<int>(i.second);}
        if(i.second.index() == 2){anOutput << std::get<float>(i.second);}
        if(i.second.index() == 3){anOutput << std::get<std::string>(i.second);}
        anOutput << "\n}\n";
        
        
        
    }
  return Errors::noError;
}
StatusResult  Row::decode(std::istream &anInput){
    data.clear();
    std::string temp;
    while(std::getline(anInput,temp)){
     //   if(temp =="}"){break;}
        if((temp != "{") && temp != "}"){
            data[grabFirst(temp)] = temp.substr(temp.find(":")+1);
        }
      //  if(temp !="{"){return Errors::readError;}
       
   //     setDecode(temp);
    }
    
  return Errors::noError;
}
bool          Row::initHeader(Block &aBlock) const{


  return false;
}

std::string Row::grabFirst(std::string const& aString)
{
    std::string::size_type pos = aString.find(':');
    if (pos != std::string::npos)
    {
        return aString.substr(0, pos);
    }
    else
    {
        return aString;
    }
}

}
