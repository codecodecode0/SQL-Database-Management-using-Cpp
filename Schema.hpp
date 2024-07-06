//
//  Schema.hpp
//  PA3
//
//  Created by rick gessner on 3/18/23.
//  Copyright © 2023 rick gessner. All rights reserved.
//

#ifndef Schema_hpp
#define Schema_hpp

#include <memory>
#include <string>

#include "Attribute.hpp"
#include "Errors.hpp"


namespace ECE141 {

  class Schema : public Storable{
  public:
    Schema(const std::string aName,AttributeList anAttributeList);
    Schema(const Schema &aCopy);

    ~Schema();

    const std::string&    getName() const {return name;}
    const AttributeList   getAttributes() const {return attributes;}
//    const Attribute* getAttribute(std::string aFieldName) const {
//      for (Attribute theAttr : attributes) {
//        if (theAttr.getName() == aFieldName) {
//          return &theAttr;
//        }
//      }
//      return nullptr;
//    }

      const size_t                   getIndex() const{return index;}
      const int                   getNext() const{return next;}
      
      void                        setIndex(int aValue){index = aValue;}
      void                        setNext(int aValue){next = aValue;}
    StatusResult          addAttribute(Attribute anAttribute);
    StatusResult          addAttributes(AttributeList anAttributeList);
    std::optional<std::string>    getPrimaryName();


    virtual StatusResult  encode(std::ostream &anOutput) const override;
    virtual StatusResult  decode(std::istream &anInput) override;
    virtual bool          initHeader(Block &aBlock) const override;


  protected:

    AttributeList   attributes;
    std::string     name;

      size_t index;
      int next;
    //how will you manage creation of primary keys?

  };

}
#endif /* Schema_hpp */
