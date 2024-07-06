//
//  Database.hpp
//  PA2
//
//  Created by rick gessner on 2/27/23.
//

#ifndef Database_hpp
#define Database_hpp

#include <stdio.h>
#include <cstring>
#include <fstream> 
#include "Storage.hpp"
#include "Row.hpp"

#include "Index.hpp"

namespace ECE141 {

  //db should be related to storage somehow...

  class Database {
  public:    
    Database(const std::string dbName, AccessMode);
    Database(const std::string dbName);
    virtual ~Database();

    StatusResult    dump(std::ostream &anOutput); //debug...
    static Database* getDbInstance();
    static StatusResult useDb(const std::string aPath);
    static std::vector<std::string> getDbs();
    static StatusResult dropDb(const std::string aDbName);
    static bool databaseExists(std::string aDbName);
    static StatusResult createDb(const std::string aDbName);
    static StatusResult dumpDb(const std::string aDbName, int &aRows);
    std::string getDbName() { return name; }

    static StatusResult getRows(std::string aDbName, uint32_t aHashedTableName, std::vector<Row> &aRows);
    static StatusResult getIndex(std::string aDbName, uint32_t aHashedTableName, std::map<uint16_t, uint16_t> &aData);

  protected:

    std::string     name;
    bool            changed;  //might be helpful, or ignore if you prefer.
    AccessMode      accessMode;

  private:
    static Database* activeDatabase;
  };

}
#endif /* Database_hpp */
