//
//  Row.hpp
//  PA3
//
//  Created by rick gessner on 4/2/23.
//

#ifndef Row_hpp
#define Row_hpp

#include <stdio.h>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <memory>
#include "Attribute.hpp"
#include "BasicTypes.hpp"

//feel free to use this, or create your own version...

namespace ECE141 {

  //These really shouldn't be here...
  using Value = std::variant<bool, int, float, std::string>;
  using RowKeyValues = std::unordered_map<std::string, Value>;
  using Key = std::variant<std::string, uint16_t>;
  using KeyValues = std::unordered_map<Key, Value>;

  class Row : public Storable {
  public:

    Row(uint32_t entityId=0);
    Row(const Row &aRow);
    
    Row(const Attribute &anAttribute); //maybe?
    
    ~Row() = default;
    
    Row& operator=(const Row &aRow);
    bool operator==(Row &aCopy) const;
    
      //STUDENT: What other methods do you require?
                          
    void                set(const std::string &aKey,
                            const Value &aValue) {
      data[aKey] = aValue;
    };
        
    RowKeyValues&       getData() {return data;}
    
    //uint32_t            entityId; //hash value of entity?
    //uint32_t            blockNumber;
      
      
      virtual StatusResult  encode(std::ostream &anOutput) const override;
      virtual StatusResult  decode(std::istream &anInput) override;
      virtual bool          initHeader(Block &aBlock) const override;
      
      
      std::string grabFirst(std::string const& aString);

  protected:
    RowKeyValues        data;
      
      
  };

  //-------------------------------------------

  using RowCollection = std::vector<std::unique_ptr<Row> >;


}
#endif /* Row_hpp */

