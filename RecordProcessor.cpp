//
// Created by SIDDHANT on 30-04-2023.
//

#include "RecordProcessor.hpp"
#include "CmdStatement.cpp"

namespace ECE141 {
  StatusResult RecordProcessor::execute(ViewListener aViewer) {
    auto theTimer=Config::getTimer();
    auto theStart=theTimer.now();
    Database* theDb = Database::getDbInstance();
    if(theDb == nullptr) {
      return Errors::noDatabaseSpecified;
    }
    std::string theDbName = theDb->getDbName();
    if(!cmdStatement) {
      return Errors::unknownError;
    }

    std::optional<TableView> theTableView;
    int theNumRowsAffected = 0;
    std::string theStatement = "";
    StatusResult theResult = cmdStatement->execute(theTableView, theNumRowsAffected, theStatement, theDbName);
    if (!theResult) return theResult;

    if (!theStatement.length()) {
      theStatement = Config::getOkQueryMessage(theNumRowsAffected);
    }

    double theTimeElapsed = theTimer.elapsed(theStart);
    DbView dbView = DbView(theTimeElapsed, theTableView, theStatement);
    aViewer(dbView);
    return theResult;
  };

  StatusResult RecordProcessor::extractStatement(Tokenizer &aTokenizer) {
    Keywords theKeyword = aTokenizer.current().keyword;
    if (theKeyword == Keywords::insert_kw) {
      cmdStatement = new InsertStatement();
    } else if (theKeyword == Keywords::select_kw) {
      cmdStatement = new SelectStatement();
    } else if (theKeyword == Keywords::update_kw) {
      cmdStatement = new UpdateStatement();
    } else if (theKeyword == Keywords::delete_kw) {
      cmdStatement = new DeleteStatement();
    } else if (theKeyword == Keywords::show_kw) {
      cmdStatement = new ShowIndexStatement();
    } else {
      return Errors::unknownRecordCommand;
    }
    return cmdStatement->parse(aTokenizer);
  };


} // ECE141