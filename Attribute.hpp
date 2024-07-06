//
//  Attribute.hpp
//  PA3
//
//  Created by rick gessner on 4/18/22.
//  Copyright © 2023 rick gessner. All rights reserved.
//

#ifndef Attribute_hpp
#define Attribute_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include <optional>
#include "keywords.hpp"
#include "BasicTypes.hpp"
#include "Storage.hpp"

namespace ECE141 {

  class Attribute : public Storable {
  protected:
    std::string   name;
    DataTypes     type;
    bool          notNullable;
    bool          primaryKey;
    bool          autoIncrement;
    bool          hasDefault;
    std::string   defaultValue;
    uint32_t      size; // only if varchar
    //what other properties do we need?

  public:

    Attribute(DataTypes aType=DataTypes::no_type);
    Attribute(std::string aName, DataTypes aType, uint32_t aSize=0,bool aNullFlag=false, bool aKeyFlag=false, bool anAutoIncrement=false, bool aHasDefault=false, std::string aDefaultValue=""); // add to constructor
    Attribute(const Attribute &aCopy);
    ~Attribute();

    const std::string&  getName() const {return name;}

    bool                getNullState() const { return notNullable;}
    bool                getKeyState() const {return primaryKey;}
    bool                getAutoIncrement() const {return autoIncrement;}
    uint32_t            getSize() const {return size;}
    DataTypes           getType() const {return type;}
    bool                getHasDefault() const {return hasDefault;}
    std::string         getDefaultValue() const {return defaultValue;}
    char                getTypeAsChar() const;

    void                setNullState(bool aState)  { notNullable = aState;}
    void                setKeyState(bool aState)  { primaryKey = aState;}
    void                setAutoIncrement(bool aFlag)  { autoIncrement = aFlag;}
    void                setSize(uint16_t aSize)  { size = aSize;}
    void                setType(DataTypes aType)  { type = aType;}
    void                setDefaultValue(std::string aDefaultValue) {
      if(defaultValue.length() > 0) {
        hasDefault = true;
      } else {
        hasDefault = false;
      }
      defaultValue = aDefaultValue;
    }

    //need more getters and setters right?

    bool                isValid(); //is this  valid?

    //storable interface

    virtual StatusResult  encode(std::ostream &anOutput) const override;
    virtual StatusResult  decode(std::istream &anInput) override;
    virtual bool          initHeader(Block &aBlock) const override;
      
      
      void setDecode(std::string temp);


  };

  using AttributeOpt = std::optional<Attribute>;
  using AttributeList = std::vector<Attribute>;

}


#endif /* Attribute_hpp */
