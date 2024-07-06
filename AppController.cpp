//
//  CommandProcessor.cpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/23.
//  Copyright © 2018-2023 rick gessner. All rights reserved.
//

#include <iostream>
#include <memory>
#include <vector>
#include "AppController.hpp"
#include "Tokenizer.hpp"

namespace ECE141 {
  

 
  AppController::AppController() :
    running{true},
    appProcessor(new AppProcessor()),
    dbProcessor(new DbProcessor()),
    sqlProcessor(new SqlProcessor()),
    recordProcessor(new RecordProcessor()) {}

  AppController::~AppController() {}

  CmdProcessor* AppController::recognize(Tokenizer &aTokenizer) {
    if (appProcessor->extractStatement(aTokenizer)) {
      return appProcessor;
    } else if (dbProcessor->extractStatement(aTokenizer)) {
      return dbProcessor;
    } else if (sqlProcessor->extractStatement(aTokenizer)) {
      return sqlProcessor;
    } else if (recordProcessor->extractStatement(aTokenizer)) {
      return recordProcessor;
    }
    return nullptr;
  }
  
  // USE: -----------------------------------------------------
  
  //build a tokenizer, tokenize input, ask processors to handle...
  StatusResult AppController::handleInput(std::istream &anInput,
                                        ViewListener aViewer){
    Tokenizer theTokenizer(anInput);
    StatusResult theResult=theTokenizer.tokenize();

    while (theResult && theTokenizer.more() && isRunning()) {
      if(auto *theCmdProcessor= recognize(theTokenizer)) {
        theResult = theCmdProcessor->execute(aViewer);
        if(theResult == Errors::userQuit) {
          shutdown();
          theResult = Errors::noError;
        }

        if(theResult) theTokenizer.skipIf(semicolon);
      } else {
        theResult=Errors::unknownCommand;
      }

      if(!theResult) {
        std::string theError = std::string(getError(theResult).value());
        // TODO: Redesign error messages
        theError = "Error 101: Identifier expected at line 1";
        AppView theErrorView(theError);
        aViewer(theErrorView);
      }
//      theTokenizer.next();
    }

    return theResult;
  }

  OptString AppController::getError(StatusResult &aResult) const {

    static std::map<ECE141::Errors, std::string_view> theMessages = {
      {Errors::illegalIdentifier, "Illegal identifier"},
      {Errors::unknownIdentifier, "Unknown identifier"},
      {Errors::databaseExists, "Database exists"},
      {Errors::tableExists, "Table Exists"},
      {Errors::syntaxError, "Syntax Error"},
      {Errors::unknownCommand, "Unknown command"},
      {Errors::unknownDatabase,"Unknown database"},
      {Errors::unknownTable,   "Unknown table"},
      {Errors::unknownError,   "Unknown error"},
      {Errors::invalidDatabaseName, "Invalid database name"},
      {Errors::invalidCommand, "Invalid command or its arguments"}
    };

    std::string_view theMessage="Unknown Error";
    if(theMessages.count(aResult.error)) {
      theMessage=theMessages[aResult.error];
    }
    return theMessage;
  }




}
