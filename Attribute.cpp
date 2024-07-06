//
//  Attribute.hpp
//  PA3
//
//  Created by rick gessner on 4/18/22.
//  Copyright © 2023 rick gessner. All rights reserved.
//

#include "Attribute.hpp"

namespace ECE141 {

  Attribute::Attribute(DataTypes aType) {}

  Attribute::Attribute(std::string aName, DataTypes aType, uint32_t aSize,bool aNullFlag, bool aKeyFlag, bool anAutoIncrement, bool aHasDefault, std::string aDefaultValue)  {
    name=aName;
    type = aType;
    size = aSize;
    notNullable = aNullFlag;
    primaryKey = aKeyFlag;
    autoIncrement = anAutoIncrement;
    hasDefault = aHasDefault;
    defaultValue = aDefaultValue;
    //other fields?
  }

  Attribute::Attribute(const Attribute &aCopy)  {
    name=aCopy.name;
    type = aCopy.type;
    size = aCopy.size;
    notNullable = aCopy.notNullable;
    primaryKey = aCopy.primaryKey;
    autoIncrement = aCopy.autoIncrement;
    hasDefault = aCopy.hasDefault;
    defaultValue = aCopy.defaultValue;

    //other fields?
  }

  Attribute::~Attribute()  {
  }

  bool Attribute::isValid() {
    // what makess an attrivute valid or not

    //
    return true;
  }

  StatusResult  Attribute::encode(std::ostream &anOutput) const {

    // name = i
    // aType = t
    // size = s
    // null = n   - 0/1
    // key = p  - 0/1
    // default = d;
    // auto increment = a;

    anOutput << "{\n";

    anOutput << "i:" << name << "\n";
    anOutput << "t:" << static_cast<char>(type) << "\n";
    anOutput << "s:" << size << "\n";
    anOutput << "n:" << (notNullable ? "1" : "0") << "\n";
    anOutput << "p:" << (primaryKey ? "1" : "0") << "\n";
    anOutput << "d:" << defaultValue << "\n";
    anOutput << "a:" << (autoIncrement ? "1" : "0") << "\n";

    anOutput << "}\n";

    return Errors::noError;
  }

  StatusResult  Attribute::decode(std::istream &anInput){

    std::string temp;
      while(std::getline(anInput,temp)){
        //  if(temp !="{"){return Errors::readError;}
          if(temp =="}"){break;}
          setDecode(temp);
      }

    return Errors::noError;
  }

  bool          Attribute::initHeader(Block &aBlock) const {
    return false;
  }


void Attribute::setDecode(std::string temp){
    if(temp.front() =='i'){
        name = temp.substr(temp.find(":")+1);
    }
    if(temp.front() =='t'){
        std::string theTypeString = (temp.substr(temp.find(":")+1));
        char theTypeChar = theTypeString[0];
        if(theTypeChar == 'N'){type = DataTypes::no_type;}
        if(theTypeChar == 'B'){type = DataTypes::bool_type;}
        if(theTypeChar == 'D'){type = DataTypes::datetime_type;}
        if(theTypeChar == 'F'){type = DataTypes::float_type;}
        if(theTypeChar == 'I'){type = DataTypes::int_type;}
        if(theTypeChar == 'V'){type = DataTypes::varchar_type;}
    }
    if(temp.front() =='s'){
        size = std::stoi(temp.substr(temp.find(":")+1));
    }
    if(temp.front() =='n'){
        std::string theNullType = temp.substr(temp.find(":")+1);
        if(theNullType =="1"){notNullable = true;}
        if(theNullType =="0"){notNullable = false;}
    }
    if(temp.front() =='p'){
        std::string thePType = temp.substr(temp.find(":")+1);
        if(thePType =="1"){primaryKey = true;}
        if(thePType =="0"){primaryKey = false;}
    }
    if(temp.front() == 'd'){
        std::string defaultTemp = temp.substr(temp.find(":")+1);
        if(defaultTemp == ""){
            defaultValue = "";
            hasDefault = false;
        }else{
            defaultValue = defaultTemp;
            hasDefault = true;
        }
    }
    if(temp.front() == 'a'){
        std::string theIncrement = temp.substr(temp.find(":")+1);
        if(theIncrement =="1"){autoIncrement = true;}
        if(theIncrement =="0"){autoIncrement = false;}
    }
   
}


}
