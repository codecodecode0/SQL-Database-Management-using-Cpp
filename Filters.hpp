//
//  Filters.hpp
//  RGAssignment5
//
//  Created by rick gessner on 4/4/21.
//  Copyright © 2021 rick gessner. All rights reserved.
//

#ifndef Filters_h
#define Filters_h

#include <stdio.h>
#include <vector>
#include <memory>
#include <string>
#include "Tokenizer.hpp"
#include "Row.hpp"
#include "Schema.hpp"

namespace ECE141 {

  class Schema;

  struct Operand {
    Operand() {}
    Operand(std::string &aName, TokenType aType, Value &aValue, size_t anId=0)
      : ttype(aType), dtype(DataTypes::varchar_type), name(aName),
        value(aValue), schemaId(anId) {}

    void setAttribute(const Token aToken, const size_t aSchemaId, DataTypes aDatatype) {
      name = aToken.data;
      ttype = aToken.type;
      dtype = aDatatype;
      schemaId = aSchemaId;
    }

    void setVarChar(std::string aString) {
      dtype = DataTypes::varchar_type;
      value = aString;
    }

    void setNumber(const Token& aToken) {
      if (aToken.keyword == Keywords::integer_kw) {
        dtype = DataTypes::int_type;
        value = std::stoi(aToken.data);
      } else {
        ttype = aToken.type;
        dtype = DataTypes::float_type;
        value = std::stof(aToken.data);
      }
    }

    TokenType   ttype; //is it a field, or const (#, string)...
    DataTypes   dtype;
    std::string name;  //attr name
    Value       value;
    size_t      schemaId;
  };

  //---------------------------------------------------

  struct Expression {
    Operand     lhs;  //id
    Operand     rhs;  //usually a constant; maybe a field...
    Operators   op;   //=     //users.id=books.author_id
    Logical     logic; //and, or, not...

    Expression(const Operand &aLHSOperand={},
               const Operators anOp=Operators::unknown_op,
               const Operand &aRHSOperand={})
      : lhs(aLHSOperand), rhs(aRHSOperand),
        op(anOp), logic(Logical::no_op) {}

    Expression(const Expression &anExpr) :
      lhs(anExpr.lhs), rhs(anExpr.rhs),
      op(anExpr.op), logic(anExpr.logic) {}

    bool operator()(Row aRow);
  };

  using Expressions = std::vector<std::unique_ptr<Expression>>;

  //---------------------------------------------------

  class Filters {
  public:

    Filters();
    Filters(const Filters &aFilters);
    ~Filters();

    size_t        getCount() const {return expressions.size();}
    bool          matches(KeyValues &aList) const;
    Filters&      add(Expression *anExpression);
    Expressions&  getExpressions() {return expressions;}

    // TODO: Think a out it: This class should handle parsing of filters
    StatusResult  parse(Tokenizer &aTokenizer, Schema &aSchema);

  protected:
    Expressions   expressions;
  };

}

#endif /* Filters_h */
