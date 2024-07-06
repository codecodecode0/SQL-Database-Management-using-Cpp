//
//  Database.cpp
//  PA2
//
//  Created by rick gessner on 2/27/23.
//

#include <string>
#include <iostream>
#include <iomanip>
#include <map>
#include "Database.hpp"
#include "Config.hpp"
#include "FolderReader.hpp"
#include "Schema.hpp"

namespace ECE141 {

  Database* Database::activeDatabase = nullptr;

  Database* Database::getDbInstance() {
    return activeDatabase;
  }
  
  Database::Database(const std::string dbName, AccessMode aMode)
    : name(dbName), accessMode(aMode) {
    std::string thePath = Config::getDBPath(name);
    //create for read/write
  }

  Database::Database(const std::string dbName)
      : name(dbName) {
    std::string thePath = Config::getDBPath(name);
    //create for read/write
  }


  Database::~Database() {
    if(changed) {
      //stuff to save?
    }
  }

  // USE: Dump command for debug purposes...
  StatusResult Database::dump(std::ostream &anOutput) {    
    return StatusResult{Errors::noError};
  }

  StatusResult Database::useDb(const std::string aDbName) {
    std::vector<std::string> theDbs = getDbs();
    if(!databaseExists(aDbName)) return Errors::unknownDatabase;
    activeDatabase = new Database(aDbName);
    return Errors::noError;
  }

  StatusResult Database::dropDb(const std::string dbName) {
    StatusResult theResult;
    if(!databaseExists(dbName)){theResult = Errors::unknownDatabase;}
    if(theResult){
      std::remove(Config::getDBPath(dbName).c_str());
      if(getDbInstance() != nullptr) {
        if(getDbInstance()->getDbName() == dbName){activeDatabase = nullptr;} // cant use dropped database
      }
    }
    return theResult;
  }

  StatusResult Database::createDb(const std::string aDbName) {
    if(databaseExists(aDbName)) return Errors::databaseExists;
    CreateFile tag;
    BlockIO theIO{aDbName,tag};
    Block theBlock;
    std::memcpy(theBlock.header.name, aDbName.c_str(),std::min(aDbName.length(), sizeof(theBlock.header.name))); // must decide for our meta data for extraction
      
//      if(Config::useIndex()){
//          Index<uint32_t,uint16_t> theIndexIndex;
//
//      }
      
    theIO.writeBlock(0, theBlock);
    return Errors::noError;
  }

  StatusResult Database::dumpDb(const std::string aDbName, int &aRows) {
    OpenFile tag;
    BlockIO theIO{aDbName,tag};
    int theBlockAmmount = theIO.getBlockCount();

    int dataBlocks{0};
    int schemaBlocks{0};

    for(size_t i=0;(int)i<theBlockAmmount;i++){
      Block theBlock;
      theIO.readBlock((int)i, theBlock);
      if(theBlock.header.type == (char)BlockType::schema_block){schemaBlocks++;}
      if(theBlock.header.type == (char)BlockType::data_block){dataBlocks++;}
    }
    aRows = theBlockAmmount;
    return Errors::noError;
  }

  std::vector<std::string> Database::getDbs() {
    std::vector<std::string> theDbs;
    FolderReader theReader{Config::getStoragePath().c_str()};
    theReader.each(Config::getDBExtension(),
                   [&](const std::string &aName) {
                     theDbs.push_back(theReader.parseDbNameFromPath(aName));
                     return true;
                   });
    return theDbs;
  }

  bool Database::databaseExists(std::string aDbName) {
    std::string thePath = Config::getDBPath(aDbName).c_str();
    FolderReader theReader(aDbName.c_str());
    return theReader.exists(thePath);
  }

  uint16_t getIndexIndex(std::string &aDbName, uint32_t aHashedTableName){
    AttributeList temp;
    OpenFile tag;
    BlockIO theIO(aDbName,tag);
    Block theBlock;
    for(size_t i=0;i<theIO.getBlockCount();i++){
      theIO.readBlock((int)i, theBlock);
      if(theBlock.header.hash==aHashedTableName && theBlock.header.type == (char)BlockType::schema_block){
        return theBlock.header.indexIndex;
      }
    }
    return 0;
  }

  StatusResult Database::getIndex(std::string aDbName, uint32_t aHashedTableName, std::map<uint16_t, uint16_t> &aData) {
    OpenFile tag;
    Storage* theStorage = Storage::GetInstance(aDbName, tag);

    if (Config::useIndex()) {
      uint16_t theIdxIdx = getIndexIndex(aDbName, aHashedTableName);
      if (theIdxIdx != 0) {
        Block theIndexBlock;
        theStorage->readBlock((int)theIdxIdx, theIndexBlock);
        Index<uint16_t, uint16_t> theIndex(aDbName, theIndexBlock);
        std::stringstream theIndexStream{theIndexBlock.payload};
        theIndex.decode(theStorage, theIndexStream);
        std::map<uint16_t, uint16_t> theRowBlocks = theIndex.getData();
        aData.insert(theRowBlocks.begin(), theRowBlocks.end());
        return Errors::noError;
      }
    }
    return Errors::noError;
  }

  StatusResult Database::getRows(std::string aDbName, uint32_t aHashedTableName, std::vector<Row> &aRows) {
    OpenFile tag;
    Storage* theStorage = Storage::GetInstance(aDbName, tag);

    if (Config::useIndex()) {
      uint16_t theIdxIdx = getIndexIndex(aDbName, aHashedTableName);
      if (theIdxIdx != 0) {
        Block theIndexBlock;
        theStorage->readBlock((int)theIdxIdx, theIndexBlock);
        Index<uint16_t, uint16_t> theIndex(aDbName, theIndexBlock);
        std::stringstream theIndexStream{theIndexBlock.payload};
        theIndex.decode(theStorage, theIndexStream);
        std::map<uint16_t, uint16_t> theRowBlocks = theIndex.getData();
        for (auto & theRowBlock : theRowBlocks) {
          Block theBlock;
          theStorage->readBlock((int)theRowBlock.second, theBlock);
          if (theBlock.header.type == (char)BlockType::data_block) {
            Row theRow;
            std::stringstream theStream;
            theStream << theBlock.payload;
            theRow.decode(theStream);
            aRows.push_back(theRow);
          }
        }
        return Errors::noError;
      }
    }

    for (size_t i=0;i<theStorage->getBlockCount();i++) {
      Block theBlock;
      theStorage->readBlock((int)i, theBlock);
      if (theBlock.header.hash == aHashedTableName && theBlock.header.type == (char)BlockType::data_block){
        Row theRow;
        std::stringstream theStream;
        theStream << theBlock.payload;
        theRow.decode(theStream);
        aRows.push_back(theRow);
      }
    }
    return Errors::noError;
  }

}
