//
//  Config.hpp
//
//  Created by rick gessner on 3/30/23.
//  Copyright © 2018-2023 rick gessner. All rights reserved.
//
//

#ifndef Config_h
#define Config_h
#include <sstream>
#include <filesystem>
#include "Timer.hpp"
//#include "Log.hpp"

namespace ECE141 {

  enum class CacheType : int {block=0, rows, views};

  struct Config {
    static size_t cacheSize[3];
    static bool   indexing;

    static const char* getDBExtension() {return ".db";}

    static const std::string getStoragePath() {
        
      #if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
        
        //STUDENT: If you're on windows, return a path to folder on your machine...
        return std::filesystem::temp_directory_path().string();
      
      #elif __APPLE__ || defined __linux__ || defined __unix__

        return std::string("/tmp");
              
      #endif
    }
    
    static std::string getDBPath(const std::string &aDBName) {
      std::ostringstream theStream;
      theStream << Config::getStoragePath() << "/" << aDBName << ".db";
      return theStream.str();
    }
      
    static Timer& getTimer() {
      static Timer theTimer;
      return theTimer;
    }

//    static Logger& Log() {
//      return Logger::instance();
//    }
    
    static std::string getVersion() {return "1.0";}
    static std::string getMembers() {return "Siddhant and Declan";}
    static std::string getExitMessage() {return "DB::141 is shutting down";}
      static std::string getHelp() {
          std::string theHelpStr;
          std::vector<std::string> theHelpMessages = getHelpMessages();
          for (std::string theStr : theHelpMessages) {
              theHelpStr += theStr + "\n";
          }
          return theHelpStr;
      }
      
      static std::vector<std::string> getHelpMessages() {
          return std::vector<std::string>{
              "create database {name}: make a new database in storage folder",
              "drop database {name}: delete a known database from storage folder",
              "show databases: list databases in storage folder",
              "use database {name}: load a known database for use",
              "about: show members",
              "help: show list of commands",
              "quit: stop app",
              "version: show app version",
              "dump database: shows database memory storage",
              "create table: creates a table in database",
              "drop table: deletes table and all contents",
              "describe table: list table attributes",
              "select: selects rows from table",
              "insert: inserts rows into table",
              "update: updates rows in table",
              "delete: delete rows form table",
              "show tables: shows tables in current database",
              
          };
      }

    static std::string getShowDatabasesTitle(){return "Database";}
    static std::string getShowTablesTitle(std::string aDbName){return "Tables_in_" + aDbName;}
    static std::string getShowIndex(){return "Index";}
    static std::vector<std::string> getDescribeTablesTitle(){
      return std::vector<std::string>{"Field", "Type", "Null", "Key", "Default", "Extra"};
    }

//    static std::string getOkQueryMessage(int aRowsAffected){return "Query OK, " + std::to_string(aRowsAffected) + (aRowsAffected == 1 ? " row" : " rows") + " affected";}
    static std::string getOkQueryMessage(int aRowsAffected){return "Query OK, " + std::to_string(aRowsAffected) + " rows" + " affected";}
    //    static std::string getRowsInSetMessage(int aRowsAffected){return "Query OK, " + std::to_string(aRowsAffected) + (aRowsAffected == 1 ? " row" : " rows") + " in set";}
    static std::string getRowsInSetMessage(int aRowsAffected){return "Query OK, " + std::to_string(aRowsAffected) + " rows" + " in set";}
    static std::string getDbChangedMessage(){return "Query OK, Database changed";}

    //cachetype: block, row, view...
    static size_t getCacheSize(CacheType aType) {
      return cacheSize[(int)aType];
    }

    static void setCacheSize(CacheType aType, size_t aSize) {
      cacheSize[(int)aType]=aSize;
    }

    //cachetype: block, row, view...
    static bool useCache(CacheType aType) {
      return cacheSize[(int)aType]>0;
    }

    static bool useIndex() {return indexing;}
    static bool setUseIndex(bool aState) {
      indexing=aState;
      return indexing;
    }
      
      static const std::string getConnectionString() {
          return "localhost";
      }
      
      
  };



}

#endif /* Config_h */
