//
// Created by SIDDHANT on 23-04-2023.
//

#include "SqlProcessor.hpp"

#include <iostream>
#include <vector>
#include "Attribute.hpp"

namespace ECE141 {
  StatusResult SqlProcessor::execute(ViewListener aViewer) {
    auto theTimer=Config::getTimer();
    auto theStart=theTimer.now();
    if(!cmdStatement) {
      return Errors::unknownError;
    }

    std::optional<TableView> theTableView;
    int theNumRowsAffected = 0;
    std::string theStatement = "";

    StatusResult theResult = cmdStatement->execute(theTableView, theNumRowsAffected, theStatement);
    if (!theResult) return theResult;

    if (!theStatement.length()) {
      theStatement = Config::getOkQueryMessage(theNumRowsAffected);
    }

    double theTimeElapsed = theTimer.elapsed(theStart);
    DbView dbView = DbView(theTimeElapsed, theTableView, theStatement);
    aViewer(dbView);
    return theResult;
  };

  StatusResult SqlProcessor::extractStatement(Tokenizer &aTokenizer) {
    Keywords theKeyword = aTokenizer.current().keyword;
    if(theKeyword == Keywords::create_kw) {
      cmdStatement = new CreateTableStatement();
    } else if (theKeyword == Keywords::show_kw && aTokenizer.peek().keyword == Keywords::tables_kw) {
      cmdStatement = new ShowTablesStatement();
    } else if (theKeyword == Keywords::show_kw && aTokenizer.peek().keyword == Keywords::indexes_kw){
      cmdStatement = new ShowIndexesStatement();
    } else if (theKeyword == Keywords::drop_kw) {
      cmdStatement = new DropTableStatement();
    } else if (theKeyword == Keywords::describe_kw) {
      cmdStatement = new DescribeTableStatement();
    } else {
      return Errors::unknownDbCommand;
    }
    return cmdStatement->parse(aTokenizer);
  };

}
