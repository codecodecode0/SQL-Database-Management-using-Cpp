//
//  Filters.cpp
//  Datatabase5
//
//  Created by rick gessner on 3/5/21.
//  Copyright © 2021 rick gessner. All rights reserved.
//

#include <string>
#include <limits>
#include "Filters.hpp"
#include "keywords.hpp"
#include "Helpers.hpp"
#include "Schema.hpp"
#include "Attribute.hpp"
#include "ParseHelper.hpp"
#include "Compare.hpp"

namespace ECE141 {

  using Comparitor = bool (*)(Value &aLHS, Value &aRHS);

  bool equals(Value &aLHS, Value &aRHS) {
    bool theResult=false;

    std::visit([&](auto const &aLeft) {
      std::visit([&](auto const &aRight) {
        theResult=isEqual(aLeft,aRight);
      },aRHS);
    },aLHS);
    return theResult;
  }

  bool greaterThan(Value &aLHS, Value &aRHS) {
    bool theResult=false;

    std::visit([&](auto const &aLeft) {
      std::visit([&](auto const &aRight) {
        theResult=isGreater(aLeft,aRight);
      },aRHS);
    },aLHS);
    return theResult;
  }

  bool lessThan(Value &aLHS, Value &aRHS) {
    bool theResult=false;

    std::visit([&](auto const &aLeft) {
      std::visit([&](auto const &aRight) {
        theResult=isLess(aLeft,aRight);
      },aRHS);
    },aLHS);
    return theResult;
  }

  bool lessThanOrEqual(Value &aLHS, Value &aRHS) {
    return lessThan(aLHS, aRHS) || equals(aLHS, aRHS);
  }

  bool greaterThanOrEqual(Value &aLHS, Value &aRHS) {
    return greaterThan(aLHS, aRHS) || equals(aLHS, aRHS);
  }

  bool notEquals(Value &aLHS, Value &aRHS) {
    return !equals(aLHS, aRHS);
  }

  static std::map<Operators, Comparitor> comparitors {
    {Operators::equal_op, equals},
    {Operators::notequal_op, notEquals},
    {Operators::gt_op, greaterThan},
    {Operators::gte_op, greaterThanOrEqual},
    {Operators::lt_op, lessThan},
    {Operators::lte_op, lessThanOrEqual}
  };

  bool isNumber(std::string aStr) {
    int decimalcount = 0;
    for (auto c : aStr) {
      if (c == '.') {
        decimalcount += 1;
        if (decimalcount > 1) return false;
        continue;
      }
      if (!std::isdigit(c)) return false;
    }
    return true;
  }

  bool isInt(std::string aStr) {
    for (auto c : aStr) {
      if (!std::isdigit(c)) return false;
    }
    return true;
  }

  bool Expression::operator()(Row aRow) {
    RowKeyValues theKeyValues = aRow.getData();
    if (theKeyValues.find(lhs.name) == theKeyValues.end()) return false;
    std::optional<Value> theValue = theKeyValues.at(lhs.name);
    if (!theValue.has_value()) return false;

    if (comparitors.count(op)) {
      std::string theLhs = std::get<std::string>(theValue.value());
      Value theRhs = rhs.name;
      if (isInt(theLhs)) {
        Value theNewLhs = std::stoi(theLhs);
        return comparitors.at(op)(theNewLhs, theRhs);
      }
      if (isNumber(theLhs)) {
        Value theNewLhs = std::stof(theLhs);
        return comparitors.at(op)(theNewLhs, theRhs);
      }
      Value theNewLhs = theLhs;
      return comparitors.at(op)(theNewLhs, theRhs);
    }
    return false;
  }

  //--------------------------------------------------------------

  Filters::Filters()  {}

  Filters::Filters(const Filters &aCopy)  {
  }

  Filters::~Filters() {
    //no need to delete expressions, they're unique_ptrs!
  }

  Filters& Filters::add(Expression *anExpression) {
    expressions.push_back(std::unique_ptr<Expression>(anExpression));
    return *this;
  }

  //compare expressions to row; return true if matches
  bool Filters::matches(KeyValues &aList) const {

    //STUDENT: You'll need to add code here to deal with
    //         logical combinations (AND, OR, NOT):
    //         like:  WHERE zipcode=92127 AND age>20

//    for(auto &theExpr : expressions) {
//      if(!(*theExpr)(aList)) {
//        return false;
//      }
//    }
    return true;
  }


  //where operand is field, number, string...
//  StatusResult parseOperand(Tokenizer &aTokenizer,
//                            Schema &aSchema, Operand &anOperand) {
//    StatusResult theResult{noError};
//    Token &theToken = aTokenizer.current();
//    if(TokenType::identifier==theToken.type) {
//      if(auto *theAttr=aSchema.getAttribute(theToken.data)) {
//        anOperand.ttype=theToken.type;
//        anOperand.name=theToken.data; //hang on to name...
//        anOperand.schemaId=aSchema::hashString(theToken.data);
//        anOperand.dtype=theAttr->getType();
//      }
//      else {
//        anOperand.ttype=TokenType::string;
//        anOperand.dtype=DataTypes::varchar_type;
//        anOperand.value=theToken.data;
//      }
//    }
//    else if(TokenType::number==theToken.type) {
//      anOperand.ttype=TokenType::number;
//      anOperand.dtype=DataTypes::int_type;
//      if (theToken.data.find('.')!=std::string::npos) {
//        anOperand.dtype=DataTypes::float_type;
//        anOperand.value=std::stof(theToken.data);
//      }
//      else anOperand.value=std::stoi(theToken.data);
//    }
//    else theResult.error=Errors::syntaxError;
//    if(theResult) aTokenizer.next();
//    return theResult;
//  }

  //STUDENT: Add validation here...
  bool validateOperands(Operand &aLHS, Operand &aRHS, Schema &aSchema) {
    if(TokenType::identifier==aLHS.ttype) { //most common case...
      //STUDENT: Add code for validation as necessary
      return true;
    }
    else if(TokenType::identifier==aRHS.ttype) {
      //STUDENT: Add code for validation as necessary
      return true;
    }
    return false;
  }

  bool isValidOperand(Token &aToken) {
    //identifier, number, string...
    if(aToken.type==TokenType::identifier) return true;
    return false;
  }

  //STUDENT: This starting point code may need adaptation...
//  StatusResult Filters::parse(Tokenizer &aTokenizer,Schema &aSchema) {
//    StatusResult  theResult;
//    ParseHelper theHelper(aTokenizer);
//    while(theResult && (2<aTokenizer.remaining())) {
//      if(isValidOperand(aTokenizer.current())) {
//        Expression theExpr;
//        if((theResult=theHelper.parseExpression(aSchema,theExpr))) {
//          expressions.push_back(theExpr);
//          //add logic to deal with bool combo logic...
//        }
//      }
//      else break;
//    }
//    return theResult;
//  }

}

