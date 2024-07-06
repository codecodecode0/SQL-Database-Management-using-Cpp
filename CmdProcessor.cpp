//
// Created by SIDDHANT on 15-04-2023.
//

#include <iostream>
#include <vector>
#include "CmdProcessor.hpp"
#include "CmdStatement.cpp"

namespace ECE141 {
  StatusResult AppProcessor::execute(ViewListener aViewer) {
    StatusResult theResult = cmdStatement->execute(aViewer);
    return theResult;
  }

  StatusResult AppProcessor::extractStatement(Tokenizer &aTokenizer) {
    cmdStatement = new AppCmdStatement(aTokenizer.current().keyword);
    if(cmdStatement->isValidStatement()) {
      return cmdStatement->parse(aTokenizer);
    }
    return Errors::unknownAppCommand;
  }

  StatusResult DbProcessor::extractStatement(Tokenizer &aTokenizer) {
    Keywords theKeyword = aTokenizer.current().keyword;
      if(theKeyword == Keywords::create_kw) {
        cmdStatement = new CreateDBStatement();
      } else if (theKeyword == Keywords::show_kw) {
        cmdStatement = new ShowDBStatement();
      } else if (theKeyword == Keywords::use_kw) {
        cmdStatement = new UseDBStatement();
      } else if (theKeyword == Keywords::drop_kw) {
        cmdStatement = new DropDBStatement();
      } else if (theKeyword == Keywords::dump_kw) {
        cmdStatement = new DumpDBStatement();
      } else {
        return Errors::unknownDbCommand;
      }
      return cmdStatement->parse(aTokenizer);
  }

  StatusResult DbProcessor::execute(ViewListener aViewer) {
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
      
//    if (!theStatement.length()) {
//      if(theTableView != std::nullopt) {
//        theStatement = Config::getRowsInSetMessage(theNumRowsAffected);
//      } else {
//        theStatement = Config::getOkQueryMessage(theNumRowsAffected);
//      }
//    }


    double theTimeElapsed = theTimer.elapsed(theStart);
    DbView dbView = DbView(theTimeElapsed, theTableView, theStatement);
    aViewer(dbView);
    return theResult;
  }

  StatusResult CmdProcessor::execute(ViewListener aViewer) {
    return {};
  }

  StatusResult CmdProcessor::extractStatement(Tokenizer &aTokenizer) {
    return StatusResult();
  }

}
