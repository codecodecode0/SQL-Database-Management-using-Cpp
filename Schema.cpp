//
//  Schema.cpp
//  PA3
//
//  Created by rick gessner on 3/2/23.
//

#include "Schema.hpp"

namespace ECE141 {

  //STUDENT: Implement this class...

  Schema::Schema(const std::string aName,AttributeList anAttributeList) : name(aName){

    attributes = anAttributeList;
  }

  Schema::Schema(const Schema &aCopy) {
    name=aCopy.name;
    attributes = aCopy.attributes;
  }

  Schema::~Schema() {
    //std::cout << "~Schema()\n";
  }


//*******************//
  StatusResult    Schema::addAttribute(Attribute anAttribute){
    attributes.push_back(anAttribute);
    return Errors::noError;
  }
  StatusResult    Schema::addAttributes(AttributeList anAttributeList){
    for(auto& i: anAttributeList){addAttribute(i);}
    return Errors::noError;
  }
//*******************//

  std::optional<std::string> Schema::getPrimaryName() {
    for (Attribute& theAttribute : attributes) {
      if (theAttribute.getAutoIncrement()) {
        return std::make_optional<std::string>(theAttribute.getName());
      }
    }
    return std::nullopt;
  }

  StatusResult  Schema::encode(std::ostream &anOutput) const{

    anOutput <<"N:" << name << "\n";
    anOutput <<"a:" << attributes.size() << "\n";
    anOutput << "[\n";
    for(auto& i: attributes){
      i.encode(anOutput);
    }
    anOutput << "]\n";

    return Errors::noError;
  }
  StatusResult  Schema::decode(std::istream &anInput){
     
      attributes.clear();
      std::string temp;
      std::getline(anInput,temp);
      name = temp.substr(temp.find(":")+1);
      std::getline(anInput,temp);
      int attributeNum = std::stoi(temp.substr(temp.find(":")+1));
      Attribute theAttribute;
      for(int i=0;i<attributeNum;i++){
          theAttribute.decode(anInput);
          attributes.push_back(theAttribute);
      }
    
      
    return Errors::noError;
  }
  bool          Schema::initHeader(Block &aBlock) const{


    return false;
  }


}
