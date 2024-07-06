//
//  DBQuery.hpp
//  PA5
//
//  Created by rick gessner on 4/7/23.
//

#ifndef DBQuery_h
#define DBQuery_h

#include "Schema.hpp"
#include "Filters.hpp"

namespace ECE141 {

  // State held from a fancy select statement

  struct Property {
    Property(std::string aName, uint32_t aTableId=0) : name(aName), tableId(aTableId), desc(true) {}
    std::string     name;
    uint32_t        tableId;
    bool            desc;
  };

  using PropertyList = std::vector<Property>;

  //--------------------------
  
  class DBQuery {
  public:

    DBQuery(Schema *aSchema=nullptr, bool allFields=true) 
      : fromTable(aSchema), all(allFields) {}
    
    DBQuery(const DBQuery &aQuery) : fromTable(aQuery.fromTable) {}
                 
    //from (table) -- *, or a comma separated list of fields
    //where specific options...
    //offset
    //limit
    //group, etc.
    //maybe joins?  

    bool Matches(const ) const {
      return true;
    }
        
  protected:
    Schema*  fromTable;
    Filters  filters;
    bool     all; //if user used SELECT * FROM...

    //e.g. what if the user says, "SELECT name, age FROM students WHERE..."

  };

}

#endif /* DBQuery_h */


