//
// Created by SIDDHANT on 24-04-2023.
//

#ifndef SP23DATABASE_CMDSTATEMENT_HPP
#define SP23DATABASE_CMDSTATEMENT_HPP

#include "keywords.hpp"
#include "Tokenizer.hpp"
#include "CmdConfigs.hpp"
#include "FolderReader.hpp"
#include "BlockIO.hpp"
#include "Attribute.hpp"
#include "Schema.hpp"
#include "Helpers.hpp"
#include "Row.hpp"
#include <unordered_set>
#include "Database.hpp"
#include "Filters.hpp"
#include "ParseHelper.hpp"

using Grid = std::vector<std::vector<std::vector<std::string>>>;

namespace ECE141 {

  class CmdStatement {
  public:
    CmdStatement(Keywords aKey=Keywords::unknown_kw) {
      key = aKey;
    };

                     ~CmdStatement() {};

    StatusResult  parse(Tokenizer &aTokenizer) {
      return StatusResult();
    };
       StatusResult  parseDbName(Tokenizer &aTokenizer, std::string &aDbName) {
      return StatusResult();
    };
     StatusResult  parseDbNameCmd(Tokenizer &aTokenizer, std::string &aDbName) {
      return StatusResult();
    };

    Grid createIndexesList(std::vector<std::string> &aTables, std::vector<std::string> &aAttrs) const {
      Grid entries(
          1,std::vector<std::vector<std::string>>{aTables, aAttrs});
      return entries;
    }
//       StatusResult  execute(ViewListener aViewer) {
//      return StatusResult();
//    };

     bool isValidStatement() { return true; }
  protected:
    Keywords key;
  };

  class AppCmdStatement : public CmdStatement {
  public:
    AppCmdStatement(Keywords aKey) : CmdStatement(aKey) {}
    ~AppCmdStatement() {};

    StatusResult parse(Tokenizer &aTokenizer) {
      if(!appCommands.count(key)) {
        return Errors::invalidCommand;
      }
      aTokenizer.next();
      return {};
    }

    bool isValidStatement() {
      return appCommands.count(key);
    };

    StatusResult execute(ViewListener aViewer) {
      StatusResult theResult;
      if(isValidStatement()) {
        aViewer(appCommands.at(key));
        if(key == Keywords::quit_kw) {
          theResult = Errors::userQuit;
        }
        return theResult;
      }
      return Errors::unknownAppCommand;
    }
  };

  class DbCmdStatement : public CmdStatement {
  public:
    DbCmdStatement(Keywords aKey) : CmdStatement(aKey) {}
    ~DbCmdStatement() = default;

    virtual StatusResult parse(Tokenizer &aTokenizer){
      return {};
    };

    virtual StatusResult execute(std::optional<TableView> &aTableView, int &aNumAffectedRows, std::string &aStatement) {
      return {};
    };

    std::unordered_set<char> validDbNameSpecialChar{'#', '@', '$', '_'};
    bool validDbName(std::string aName) const {
      char startingChar = aName[0];
      if (!((startingChar >= 'a' && startingChar <= 'z') || (startingChar >= 'A' && startingChar <= 'Z'))) return false;

      for (char theChar : aName.substr(1, aName.size() - 1)) {
        if (theChar >= 'a' && theChar <= 'z') continue;
        if (theChar >= 'A' && theChar <= 'Z') continue;
        if (theChar >= '0' && theChar <= '9') continue;
        if (validDbNameSpecialChar.count(theChar)) continue;
        return false;
      }
      return true;
    }

    Grid createDbListTable(std::vector<std::string> &aDbs) const {
      std::vector<std::vector<std::vector<std::string>>> entries(
          1,std::vector<std::vector<std::string>>(
              1, aDbs));
      return entries;
    }

    StatusResult parseDbNameCmd(Tokenizer &aTokenizer, std::string &aDbName) const {
      if (aTokenizer.remaining() < 3
          || aTokenizer.current().type != TokenType::keyword
          || aTokenizer.peek().type != TokenType::keyword
          || aTokenizer.peek(2).type != TokenType::identifier
          || aTokenizer.peek().keyword != Keywords::database_kw
          || aTokenizer.peek(2).keyword != Keywords::unknown_kw)
        return Errors::invalidCommand;
      std::string theDbName = aTokenizer.peek(2).data;
      if (!validDbName(theDbName)) return Errors::invalidDatabaseName;
      aDbName = theDbName;
      aTokenizer.next(3);
      return Errors::noError;
    }

    bool databaseExists(std::string aDbName) const {
      std::string thePath = Config::getDBPath(aDbName).c_str();
      FolderReader theReader(aDbName.c_str());
      return theReader.exists(thePath);
    }

    std::vector<std::string> getDbs() const {
      std::vector<std::string> theDbs;
      FolderReader theReader{Config::getStoragePath().c_str()};
      theReader.each(Config::getDBExtension(),
                     [&](const std::string &aName) {
                       theDbs.push_back(theReader.parseDbNameFromPath(aName));
                       return true;
                     });
      return theDbs;
    }
  };

  class SqlCmdStatement : public CmdStatement {
  public:
    SqlCmdStatement(Keywords aKey) : CmdStatement(aKey) {}
    ~SqlCmdStatement() = default;

    virtual StatusResult parse(Tokenizer &aTokenizer){
      return {};
    };

    virtual StatusResult execute(std::optional<TableView> &aTableView, int &aNumAffectedRows, std::string &aStatement) {
      return {};
    };

    Schema getSchema(std::string &aDbName, std::string &aTableName){
      //   TODO: pull and display.
      AttributeList temp;
      Schema theTable("",temp);
      OpenFile tag;
      BlockIO theIO(aDbName,tag);
      Block theBlock;
      for(size_t i=0;i<theIO.getBlockCount();i++){
        theIO.readBlock((int)i, theBlock);
        if(theBlock.header.name==aTableName && theBlock.header.type == (char)BlockType::schema_block){
          std::stringstream theStream;
          theBlock.write(theStream);
          //   theBlock.copyToPayload(theStream.str());
          //    std::cout << theStream.str();
          theTable.decode(theStream);
          //  theBlock.header.type = (char)BlockType::free_block;
          //      theIO.writeBlock(i, theBlock);

          break;
        }

      }
      theTable.setNext(theBlock.header.next);
      theTable.setIndex(theBlock.header.index);

      return theTable;
    }

    std::unordered_set<char> validTableNameSpecialChar{'#', '@', '$', '_'};
    bool validTableName(std::string aName) const {
      char startingChar = aName[0];
      if (!((startingChar >= 'a' && startingChar <= 'z') || (startingChar >= 'A' && startingChar <= 'Z') || (startingChar == '_'))) return false;

      for (char theChar : aName.substr(1, aName.size() - 1)) {
        if (theChar >= 'a' && theChar <= 'z') continue;
        if (theChar >= 'A' && theChar <= 'Z') continue;
        if (theChar >= '0' && theChar <= '9') continue;
        if (validTableNameSpecialChar.count(theChar)) continue;
        return false;
      }
      return true;
    }

    Grid createTableList(std::vector<std::string> &aTables) const {
      Grid entries(
          1,std::vector<std::vector<std::string>>(
              1, aTables));
      return entries;
    }



    std::vector<std::string> getFirstColumn(AttributeList& anAttributeList) const {
      std::vector<std::string> theTempColumn;
      for (const Attribute& theAttribute : anAttributeList) {
        theTempColumn.push_back(theAttribute.getName());
      }
      return theTempColumn;
    }

    std::vector<std::string> getSecondColumn(AttributeList& anAttributeList) const {
      std::vector<std::string> theTempColumn;
      for (const Attribute& theAttribute : anAttributeList) {
        std::string theTemp;
        if(theAttribute.getType() == DataTypes::int_type) {
          theTemp = "integer";
        } else if(theAttribute.getType() == DataTypes::varchar_type) {
          theTemp = "varchar(" + std::to_string(theAttribute.getSize()) + ")";
        } else if(theAttribute.getType() == DataTypes::bool_type) {
          theTemp = "boolean";
        } else if(theAttribute.getType() == DataTypes::datetime_type) {
          theTemp = "date";
        } else if(theAttribute.getType() == DataTypes::float_type) {
          theTemp = "float";
        }

        theTempColumn.push_back(theTemp);
      }
      return theTempColumn;
    }

    std::vector<std::string> getThirdColumn(AttributeList& anAttributeList) const {
      std::vector<std::string> theTempColumn;
      for (const Attribute& theAttribute : anAttributeList) {
        std::string theTemp = theAttribute.getNullState() ? "NO" : "YES";
        theTempColumn.push_back(theTemp);
      }
      return theTempColumn;
    }

    std::vector<std::string> getFourthColumn(AttributeList& anAttributeList) const {
      std::vector<std::string> theTempColumn;
      for (const Attribute& theAttribute : anAttributeList) {
        std::string theTemp = theAttribute.getKeyState() ? "YES" : "";
        theTempColumn.push_back(theTemp);
      }
      return theTempColumn;
    }

    std::vector<std::string> getFifthColumn(AttributeList& anAttributeList) const {
      std::vector<std::string> theTempColumn;
      for (const Attribute& theAttribute : anAttributeList) {
        if (theAttribute.getHasDefault()) {
          theTempColumn.push_back(theAttribute.getDefaultValue());
        } else {
          theTempColumn.emplace_back("NULL");
        }
      }
      return theTempColumn;
    }

    std::vector<std::string> getSixthColumn(AttributeList& anAttributeList) const {
      std::vector<std::string> theTempColumn;
      for (const Attribute& theAttribute : anAttributeList) {
        std::string theTemp;
        if (theAttribute.getAutoIncrement()) {
          theTemp = "auto_increment ";   // TODO: make it better than just hardcoding
        }
        if (theAttribute.getKeyState()) {
          theTemp = " primary key";
        }
        theTempColumn.push_back(theTemp);
      }
      return theTempColumn;
    }

    Grid createTableMetadataView(Schema& aSchema) const {
      std::vector<std::vector<std::vector<std::string>>> entries(6);
//      "Field", "Type", "Null", "Key", "Default", "Extra"
      AttributeList theList = aSchema.getAttributes();

      entries[0].push_back(getFirstColumn(theList));
      entries[1].push_back(getSecondColumn(theList));
      entries[2].push_back(getThirdColumn(theList));
      entries[3].push_back(getFourthColumn(theList));
      entries[4].push_back(getFifthColumn(theList));
      entries[5].push_back(getSixthColumn(theList));

      return entries;
    }

    StatusResult parseTableNameCmd(Tokenizer &aTokenizer, std::string &aTableName) const {
      if (aTokenizer.remaining() < 3
          || aTokenizer.current().type != TokenType::keyword
          || aTokenizer.peek().type != TokenType::keyword
          || aTokenizer.peek(2).type != TokenType::identifier
          || aTokenizer.peek().keyword != Keywords::table_kw
          || aTokenizer.peek(2).keyword != Keywords::unknown_kw)
        return Errors::invalidCommand;
      std::string theTableName = aTokenizer.peek(2).data;
      if (!validTableName(theTableName)) return Errors::invalidTableName;
      aTableName = theTableName;
      aTokenizer.next(3);
      return Errors::noError;
    }

    bool tableExists(std::string aDbName, std::string aTableName) const {
      std::vector<std::string> theTables = getTables(aDbName);
      for (std::string theTable : theTables) {
        if (theTable == aTableName) return true;
      }
      return false;
    }

      std::vector<std::string> getTables(std::string aDbName) const{
          std::vector<std::string> theReturn;
          
          OpenFile tag;
          BlockIO theIO(aDbName,tag);
          Block theBlock;
          for(size_t i=0;i<theIO.getBlockCount();i++){
              theIO.readBlock((int)i, theBlock);
              if(theBlock.header.type == (char)BlockType::schema_block){
               //   theBlock.header.type = (char)BlockType::free_block;
                //  theIO.writeBlock(i, theBlock);
               //   break;
                  theReturn.push_back(theBlock.header.name);
              }
                
          }
          return theReturn;
          
  //      std::vector<std::string> theDbs;
  //      FolderReader theReader{Config::getStoragePath().c_str()};
  //      theReader.each(Config::getDBExtension(),
  //                     [&](const std::string &aName) {
  //                       theDbs.push_back(theReader.parseDbNameFromPath(aName));
  //                       return true;
  //                     });
  //      return theDbs;
      // TODO: implement this
     
      }
        
       
    
  };

  struct Join  {
    Join(const std::string &aTable, Keywords aType, TableField &aLHS, TableField &aRHS)
        : table(aTable), joinType(aType), onLeft(aLHS), onRight(aRHS) {}

    std::string table;
    Keywords    joinType;
    TableField  onLeft;
    TableField  onRight;
    std::vector<std::string> attributes;
  };

  class RecordCmdStatement : public CmdStatement {
  public:
    RecordCmdStatement(Keywords aKey) : CmdStatement(aKey) {}
    RecordCmdStatement(Keywords aKey, std::string aTableName) : CmdStatement(aKey), tableName(aTableName) {}
    ~RecordCmdStatement() = default;

    virtual StatusResult parse(Tokenizer &aTokenizer){
      return {};
    };

    virtual StatusResult execute(std::optional<TableView> &aTableView, int &aNumAffectedRows, std::string &aStatement, std::string &currentDb) {
      return {};
    };

    StatusResult parseExpression(Tokenizer& aTokenizer, Expression& anExpression) {
      if (aTokenizer.remaining() < 3) return Errors::syntaxError;

      Operand theLhs;
      theLhs.ttype = aTokenizer.current().type;
      theLhs.name = aTokenizer.current().data;
      aTokenizer.next();

      if (aTokenizer.current().type != TokenType::operators) return Errors::syntaxError;
      Operators theOp = aTokenizer.current().op;
      aTokenizer.next();

      Operand theRhs;
      theRhs.ttype = aTokenizer.current().type;
      theRhs.name = aTokenizer.current().data;
      aTokenizer.next();

      anExpression = Expression(theLhs , theOp , theRhs);
      return Errors::noError;
    }
      
      Schema getSchema(std::string &aDbName){
   //   TODO: pull and display.
       AttributeList temp;
       Schema theTable("",temp);
       OpenFile tag;
       BlockIO theIO(aDbName,tag);
       Block theBlock;
       for(size_t i=0;i<theIO.getBlockCount();i++){
           theIO.readBlock((int)i, theBlock);
           if(theBlock.header.name==tableName && theBlock.header.type == (char)BlockType::schema_block){
               std::stringstream theStream;
               theBlock.write(theStream);
            //   theBlock.copyToPayload(theStream.str());
           //    std::cout << theStream.str();
               theTable.decode(theStream);
             //  theBlock.header.type = (char)BlockType::free_block;
         //      theIO.writeBlock(i, theBlock);
               
               break;
           }
             
       }
          theTable.setNext(theBlock.header.next);
          theTable.setIndex(theBlock.header.index);
          
          return theTable;
      }

      // TODO
    uint16_t getIndexIndex(std::string &aDbName){
      //   TODO: pull and display.
      AttributeList temp;
      OpenFile tag;
      BlockIO theIO(aDbName,tag);
      Block theBlock;
      for(size_t i=0;i<theIO.getBlockCount();i++){
        theIO.readBlock((int)i, theBlock);
        if(theBlock.header.name==tableName && theBlock.header.type == (char)BlockType::schema_block){
          return theBlock.header.indexIndex;
          break;
        }
      }
      return 0;
    }

    std::string convertValuesToStr(Value& aValue) const {
      if (aValue.index() == 0) {
        bool theValue = std::get<bool>(aValue);
        return theValue ? "true" : "false";
      } else if (aValue.index() == 1) {
        int theValue = std::get<int>(aValue);
        return std::to_string(theValue);
      } else if (aValue.index() == 2) {
        float theValue = std::get<float>(aValue);
        return std::to_string(theValue);
      } else {
        return std::get<std::string>(aValue);
      }
    }

    Grid joinRows(std::vector<Row>& aLeftRows, std::vector<Row>& aRightRows, std::vector<TableField> &aColumnOrder, TableField& aLhs, TableField& aRhs, std::string aLhsTable, std::string aRhsTable) const {
      Grid entries;
      for (auto &theLeftRow : aLeftRows) {
        bool theLeftRowDisplayed = false;
        RowKeyValues theLeftRowData = theLeftRow.getData();
        for (auto &theRightRow: aRightRows) {
          RowKeyValues theRightRowData = theRightRow.getData();
          if (theRightRowData.count(aRhs.fieldName) && theLeftRowData.count(aLhs.fieldName)) {
            if (convertValuesToStr(theRightRowData.at(aRhs.fieldName)) ==
                convertValuesToStr(theLeftRowData.at(aLhs.fieldName))) {
              std::vector<std::vector<std::string>> theRowList;
              for (const TableField &theCol: aColumnOrder) {
                std::string theColumn = theCol.fieldName;
                std::string theValue;
                if (theCol.table.length()) {
                  if (theCol.table == aLhsTable && theLeftRowData.count(theColumn)) {
                    theValue = convertValuesToStr(theLeftRowData.at(theColumn));
                  } else if (theCol.table == aRhsTable && theRightRowData.count(theColumn)) {
                    theValue = convertValuesToStr(theRightRowData.at(theColumn));
                  }
                } else {
                  if (theLeftRowData.count(theColumn)) {
                    theValue = convertValuesToStr(theLeftRowData.at(theColumn));
                  } else if (theRightRowData.count(theColumn)) {
                    theValue = convertValuesToStr(theRightRowData.at(theColumn));
                  }
                }
                theLeftRowDisplayed = true;
                theRowList.push_back(std::vector<std::string>{theValue});
              }
              entries.push_back(theRowList);
            }
          }
        }
        if (!theLeftRowDisplayed) {
          std::vector<std::vector<std::string>> theRowList;
          for (const TableField &theCol: aColumnOrder) {
            std::string theColumn = theCol.fieldName;
            std::string theValue;
              if (theLeftRowData.count(theColumn)) {
                theValue = convertValuesToStr(theLeftRowData.at(theColumn));
              } else {
                theValue = "NULL";
              }
            theRowList.push_back(std::vector<std::string>{theValue});
          }
          entries.push_back(theRowList);
        }
      }
      return entries;
    }


    Grid createRecordsList(std::vector<Row> &allRows, std::vector<Row> &allOtherTableRows, std::vector<TableField> &aColumnOrder, std::vector<Join>& joins, std::string aTableName) const {
      Grid entries;
      if (joins.empty()) {
        for (auto &theRow: allRows) {
          std::vector<std::vector<std::string>> theRowList;
          for (const TableField& theCol: aColumnOrder) {
            std::string theColumn = theCol.fieldName;
            std::string theValue;
            RowKeyValues theRowData = theRow.getData();
            if (theRowData.count(theColumn)) {
              theValue = convertValuesToStr(theRowData.at(theColumn));
            }
            theRowList.push_back(std::vector<std::string>{theValue});
          }

          entries.push_back(theRowList);
        }
        return entries;
      }
        StatusResult theJoinResult;
        Join join = joins[0];
//        TableField theLhs("", ""), theRhs("", "");
//        if (join.onLeft.table == )
        std::string theOtherTableName = join.table;
        if (join.joinType == Keywords::left_kw) {
          return joinRows(allRows, allOtherTableRows, aColumnOrder, join.onLeft, join.onRight, aTableName, theOtherTableName);
        } else {
          return joinRows(allOtherTableRows, allRows, aColumnOrder, join.onRight, join.onLeft, theOtherTableName, aTableName);
        }
    }
      
      
  protected:
    std::string tableName;
      
  };

}


#endif //SP23DATABASE_CMDSTATEMENT_HPP
