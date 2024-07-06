//
//  ParseHelper.hpp
//  RGAssignment4
//
//  Created by rick gessner on 4/18/21.
//

#ifndef ParseHelper_hpp
#define ParseHelper_hpp

#include <stdio.h>
#include "keywords.hpp"
#include "BasicTypes.hpp"
#include "Tokenizer.hpp"
#include "Attribute.hpp"
#include "Filters.hpp"

namespace ECE141 {

  //-------------------------------------------------

  struct TableField {
    TableField(const std::string &aFieldName, const std::string &aTable)
        : fieldName(aFieldName), table(aTable) {}

    std::string  fieldName;
    std::string table;
  };

  struct TableName {
    TableName(const std::string &anAlias, const std::string &aTable)
        : alias(anAlias), table(aTable) {}

    std::string  alias;
    std::string table;
  };

  class Entity;
  struct Expression;

  struct ParseHelper {

    ParseHelper(Tokenizer &aTokenizer) : tokenizer(aTokenizer) {}

    StatusResult parseTableName(TableName &aTableName);
    StatusResult parseTableField(Tokenizer &aTokenizer, TableField &aField);
    StatusResult parseAttributeOptions(Attribute &anAttribute);

    StatusResult parseAttribute(Attribute &anAttribute);

    StatusResult parseIdentifierList(StringList &aList);

    StatusResult parseAssignments(Expressions &aList, Schema&);

    StatusResult parseValueList(StringList &aList);

    StatusResult parseOperator(Operators &anOp);
    StatusResult parseOperand(Schema&, Operand&);
    StatusResult parseExpression(Schema&, Expression&);

    Tokenizer &tokenizer;
  };

}

#endif /* ParseHelper_hpp */

