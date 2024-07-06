//
// Created by SIDDHANT on 24-04-2023.
//

#include "CmdStatement.hpp"
#include "ParseHelper.hpp"
#include "Index.hpp"
#include <cstring>
#include <memory>

namespace ECE141 {

  class CreateDBStatement : public DbCmdStatement {

  public:
    CreateDBStatement() : DbCmdStatement(Keywords::create_kw) {}

    StatusResult parse(Tokenizer &aTokenizer) override {
      return parseDbNameCmd(aTokenizer, dbName);
    }

    StatusResult execute(std::optional<TableView> &aTableView, int &aNumAffectedRows, std::string &aStatement) override {
      if (!validDbName(dbName)) return Errors::invalidDatabaseName;
      StatusResult theResult = Database::createDb(dbName);
      if (!theResult) return theResult;
      aNumAffectedRows = 1;
      aStatement = Config::getOkQueryMessage(aNumAffectedRows);
      return Errors::noError;
    }

  protected:
    std::string dbName;
  };

  class DropDBStatement : public DbCmdStatement {
  public:
    DropDBStatement() : DbCmdStatement(Keywords::drop_kw) {}

    StatusResult parse(Tokenizer &aTokenizer) override {
      return parseDbNameCmd(aTokenizer, dbName);
    }

    StatusResult execute(std::optional<TableView> &aTableView, int &aNumAffectedRows, std::string &aStatement) override {
      StatusResult theResult = Database::dropDb(dbName);
      if(!theResult) return theResult;
      aNumAffectedRows = 0;
      aStatement = Config::getDbChangedMessage();
      return theResult;
    }

  protected:
    std::string dbName;
  };

  class UseDBStatement : public DbCmdStatement {
  public:
    UseDBStatement() : DbCmdStatement(Keywords::use_kw) {}

    StatusResult parse(Tokenizer &aTokenizer) override {
      if (aTokenizer.remaining() < 2
          || aTokenizer.peek().keyword != Keywords::unknown_kw)
        return Errors::invalidCommand;
      aTokenizer.next();
      dbName = aTokenizer.current().data;
      aTokenizer.next(2);
      return Errors::noError;
    }

    StatusResult execute(std::optional<TableView> &aTableView, int &aNumAffectedRows, std::string &aStatement) override {
      StatusResult theResult = Database::useDb(dbName);
      if (!theResult) return theResult;
      aStatement = "Database changed";
      aNumAffectedRows = 0;
      return Errors::noError;
    }

  protected:
    std::string dbName;
  };

  class DumpDBStatement : public DbCmdStatement {
  public:
    DumpDBStatement() : DbCmdStatement(Keywords::dump_kw) {}

    StatusResult parse(Tokenizer &aTokenizer) override {
      return parseDbNameCmd(aTokenizer, dbName);
    }

    StatusResult execute(std::optional<TableView> &aTableView, int &aNumAffectedRows, std::string &aStatement) override {
      int theRows;
      StatusResult theResult = Database::dumpDb(dbName, theRows);
      if (!theResult) return theResult;
      aStatement = Config::getRowsInSetMessage(theRows);
      return Errors::noError;
    }

  protected:
    std::string dbName;
  };

  class ShowDBStatement : public DbCmdStatement {
  public:
    ShowDBStatement() : DbCmdStatement(Keywords::show_kw) {}

    StatusResult parse(Tokenizer &aTokenizer) override {
      if (aTokenizer.remaining() < 2 || aTokenizer.peek().keyword != Keywords::databases_kw)
        return Errors::invalidCommand;
      aTokenizer.next(2);
      return Errors::noError;
    }

    StatusResult execute(std::optional<TableView> &aTableView, int &aNumAffectedRows, std::string &aStatement) override {
      std::vector<std::string> headerList{Config::getShowDatabasesTitle()}; // using statements?
      std::vector<std::string> theDBs = Database::getDbs();
      Grid entries = createDbListTable(theDBs);
      aNumAffectedRows = (int)theDBs.size();

      std::vector<int> sizes{50};
      TableView theTableView = TableView(headerList, entries, sizes);
      std::optional<TableView> theOptionalTable(theTableView);
      aTableView = theOptionalTable;
      aStatement = Config::getOkQueryMessage(aNumAffectedRows);

      return Errors::noError;
    }
  };

  class DeleteStatement : public RecordCmdStatement {
  public:
    DeleteStatement() : RecordCmdStatement(Keywords::delete_kw), deleteAll(false) {}
    DeleteStatement(bool aDeleteAll, std::string aTableName): RecordCmdStatement(Keywords::delete_kw, aTableName), deleteAll(aDeleteAll) {}


    StatusResult parse(Tokenizer &aTokenizer) override {
      if (aTokenizer.remaining() < 7) return Errors::invalidCommand;
      if (aTokenizer.current().keyword != Keywords::delete_kw
          || aTokenizer.peek().keyword != Keywords::from_kw
          || aTokenizer.peek(2).type != TokenType::identifier
          || aTokenizer.peek(3).keyword != Keywords::where_kw) {
        aTokenizer.restart();
        return Errors::syntaxError;
      }

      tableName = aTokenizer.peek(2).data;

      aTokenizer.next(4);
      StatusResult theResult = parseExpression(aTokenizer, expression);
      if (!theResult) {
        aTokenizer.restart();
        return theResult;
      }

      aTokenizer.skipIf(semicolon);
      return Errors::noError;
    }


    StatusResult execute(std::optional<TableView> &aTableView, int &aNumAffectedRows, std::string &aStatement, std::string &aDbName) override {
      // TODO and Think about it: Implement Filters and Limit while fetching the rows from storage
      // TODO: get rows for the table from block IO.

        std::vector<Row> theRows;
            OpenFile tag;
           
          
     //       Storage theStorage(aDbName,tag);
        Storage* theStorage = Storage::GetInstance(aDbName, tag);
        
            Schema theSchema = getSchema(aDbName);

            uint32_t theHash = Helpers::hashString(theSchema.getName().c_str());

           
            

          //  int theNext = theSchema.getNext();
            for(size_t i=0;i<theStorage->getBlockCount();i++){
                Block theBlock;
                theStorage->readBlock((int)i, theBlock);
                if(theBlock.header.hash == theHash && theBlock.header.type == (char)BlockType::data_block){
                    
                    Row theRow;
                    std::stringstream theStream;
                    theStream << theBlock.payload;
                    theRow.decode(theStream);

                  uint16_t theIndexIndex = getIndexIndex(aDbName);
                  Block theIndexBlock;
                  if (theIndexIndex != 0) {
                    theStorage->readBlock(int(theIndexIndex), theIndexBlock);
                  }
                  Index<uint16_t, uint16_t> theIndex(tableName, theIndexBlock);
                  std::stringstream theIndexStream;
                  theIndexStream << theIndexBlock.payload;
                  theIndex.decode(theStorage, theIndexStream);

                  std::optional<std::string> thePrimaryAttributeName = theSchema.getPrimaryName();
                    if(deleteAll || expression.operator()(theRow)){
                        theStorage->markBlockAsFree((int)i);
                        if (Config::useIndex() && theIndexIndex != 0) {
                          if (thePrimaryAttributeName.has_value()) {
                            theIndex.remove(std::stoi(std::get<std::string>(theRow.getData().at(thePrimaryAttributeName.value()))));
                          }
                        }
                        // CHANGE INDEX HERE
                        aNumAffectedRows += 1;
                    }

                  if (Config::useIndex() && theIndexIndex != 0) {
                    theIndex.writeToStorage(theStorage, tableName, theHash);
                  }
                }
            }
        
//        aNumAffectedRows = static_cast<int>(theRowCollection.size());
        aStatement = Config::getOkQueryMessage(aNumAffectedRows);
    
      return Errors::noError;
    }

  protected:
    Expression expression;
    bool deleteAll;
  };


  class CreateTableStatement : public SqlCmdStatement {

  public:
    CreateTableStatement() : SqlCmdStatement(Keywords::create_kw) {}

    StatusResult parseCmdInitial(Tokenizer& aTokenizer) {
      if (aTokenizer.remaining() < 5
          || aTokenizer.peek().keyword != Keywords::table_kw
          || aTokenizer.peek(2).keyword != Keywords::unknown_kw)
        return Errors::invalidCommand;
      std::string theTableName = aTokenizer.peek(2).data;
      if (!validTableName(theTableName)) return Errors::invalidTableName;
      tableName = theTableName;
      aTokenizer.next(3);
      return Errors::noError;
    }

    DataTypes parseDataType(Token& token) {
      if(attributeDataTypes.count(token.keyword)) {
        return attributeDataTypes.at(token.keyword);
      }
      return DataTypes::no_type;
    }

    StatusResult parseAttribute(Tokenizer& aTokenizer, AttributeList& aAttributes) {
      if(aTokenizer.remaining() < 2) return Errors::syntaxError;
      std::string theAttributeName = aTokenizer.current().data;

      aTokenizer.next();
      if (aTokenizer.current().type != TokenType::keyword) return Errors::syntaxError;
      DataTypes theAttributeType = parseDataType(aTokenizer.current());
      if (theAttributeType == DataTypes::no_type) return Errors::unknownType;

      Attribute theAttribute = Attribute(theAttributeName , theAttributeType);

      aTokenizer.next();
      while (aTokenizer.more()) {
        if (aTokenizer.current().type == TokenType::punctuation) {
          if (aTokenizer.current().data ==")" || aTokenizer.current().data == ",") {
            aTokenizer.next();
            break;
          }
        }

        StatusResult theParseTokenResult = parseOptionalAttributeTokens(aTokenizer , theAttribute);
        if (!theParseTokenResult) {
          aTokenizer.restart();
          return theParseTokenResult;
        }
      }
      aAttributes.push_back(theAttribute);
      return Errors::noError;
    }

    StatusResult parseAttributes(Tokenizer& aTokenizer, AttributeList& anAttributeList) {
      if (aTokenizer.current().type != TokenType::punctuation || aTokenizer.current().data != "(")
        return Errors::invalidCommand;
      aTokenizer.next();
      while (aTokenizer.more()) {
        if(aTokenizer.current().data == ")") break;
        StatusResult theResult = parseAttribute(aTokenizer, anAttributeList);
        if(!theResult){
          aTokenizer.restart();
          return theResult;
        }
        aTokenizer.skipIf(semicolon);
      }
      aTokenizer.next();
      aTokenizer.skipIf(semicolon);
      return Errors::noError;
    }

    StatusResult parseOptionalAttributeTokens(Tokenizer& aTokenizer , Attribute& anAttribute) {
      Token& theCurrentToken = aTokenizer.current();

      if (theCurrentToken.data == "(" && anAttribute.getType() == DataTypes::varchar_type)
        return parseSize(aTokenizer,anAttribute);

      if (anAttribute.getType() == DataTypes::int_type && theCurrentToken.keyword == Keywords::auto_increment_kw) {
        return parseAutoIncrease(aTokenizer, anAttribute);
      } else if (theCurrentToken.keyword == Keywords::not_kw){
        return parseNotNull(aTokenizer , anAttribute);
      } else if (theCurrentToken.keyword == Keywords::primary_kw) {
        return parsePrimaryKey(aTokenizer, anAttribute);
      } else if (theCurrentToken.keyword == Keywords::auto_increment_kw) {
        return parseAutoIncrease(aTokenizer, anAttribute);
      } else if (theCurrentToken.keyword == Keywords::default_kw) {
        return parseDefaultValue(aTokenizer, anAttribute);
      }

      return Errors::syntaxError;
    }

    StatusResult parse(Tokenizer& aTokenizer) {
      StatusResult theResult = parseCmdInitial(aTokenizer);
      if (!theResult) {
        aTokenizer.restart();
        return theResult;
      }
      std::string tmpName = aTokenizer.current().data;
      AttributeList theAttributes;

      theResult = parseAttributes(aTokenizer, theAttributes);
      if (!theResult) {
        aTokenizer.restart();
        return theResult;
      }

      tableSchema = Schema(tableName, theAttributes);

      return Errors::noError;
    }

    StatusResult parsePrimaryKey(Tokenizer &aTokenizer , Attribute& anAttribute) {
      if (aTokenizer.current().type == TokenType::keyword && aTokenizer.peek().keyword == Keywords::key_kw){
        anAttribute.setKeyState(true);
        aTokenizer.next(2);
        return Errors::noError;
      }
      return Errors::syntaxError;
    }

    StatusResult parseSize(Tokenizer& aTokenizer , Attribute& anAttribute) {
      if (aTokenizer.remaining() < 3
           || aTokenizer.current().data != "("
           || aTokenizer.peek(2).data !=")") return Errors::syntaxError;
      anAttribute.setSize(std::stoi(aTokenizer.peek().data));
      aTokenizer.next(3);
      return Errors::noError;
    }

    StatusResult parseNotNull(Tokenizer& aTokenizer  , Attribute& anAttribute) {
      if (aTokenizer.current().type == TokenType::keyword && aTokenizer.peek().keyword == Keywords::null_kw) {
        anAttribute.setNullState(true);
        aTokenizer.next(2);
        return Errors::noError;
      }

      return Errors::syntaxError;
    }

    StatusResult parseAutoIncrease(Tokenizer& aTokenizer , Attribute& anAttribute) {
      anAttribute.setAutoIncrement(true);
      aTokenizer.next();
      return Errors::noError;
    }

    bool validateDefaultValue(Token& aToken, Attribute& anAttribute) {
      if (anAttribute.getType() == DataTypes::float_type || anAttribute.getType() == DataTypes::int_type) {
        if (aToken.type != TokenType::number) return false;
      } else if (anAttribute.getType() == DataTypes::bool_type) {
        if(!(aToken.data == "true" || aToken.data == "false")) return false;
      }
      return true;
    }

    StatusResult parseDefaultValue(Tokenizer& aTokenizer , Attribute& anAttribute) {
      if (aTokenizer.remaining() < 2) return Errors::syntaxError;
      aTokenizer.next();
      if(!validateDefaultValue(aTokenizer.current(), anAttribute)) return Errors::syntaxError;
      anAttribute.setDefaultValue(aTokenizer.current().data);
      aTokenizer.next();
      return Errors::noError;
    }


    StatusResult execute(std::optional<TableView> &aTableView, int &aNumAffectedRows, std::string &aStatement) override {
      if (!validTableName(tableName)) return Errors::invalidDatabaseName;
      Database* theDb = Database::getDbInstance();
      if(theDb == nullptr) {
        return Errors::noDatabaseSpecified;
      }
      std::string theDbName = theDb->getDbName();

      if(tableExists(theDbName, tableName)) return Errors::tableExists;

        OpenFile tag;
      BlockIO theIO{theDbName,tag};
      Block theBlock;
      uint32_t theHash = Helpers::hashString(tableName.c_str());
        // TODO: Create and save Index for the table
        if (Config::useIndex()) {
          Storage *theStorage = Storage::GetInstance(theDbName, tag);
          Block theIndexBlock;
          StatusResult theIdxBlockRes = Index<uint16_t, uint16_t>::storeNewIndexBlock(theStorage, theIndexBlock,tableName, theHash);
          if (!theIdxBlockRes) return theIdxBlockRes;
          theBlock.header.indexIndex = theIndexBlock.header.index;
            if(!theIdxBlockRes){return Errors::cantCreateIndex;}
        }
        

     //   theBlock.header.name = tableName;
        std::memcpy(theBlock.header.name, tableName.c_str(),std::min(tableName.length(), sizeof(theBlock.header.name)));

        // TODO: Block number of index should be stored in the Schema header

        theBlock.header.hash = theHash;
        theBlock.header.type = (char)BlockType::schema_block;
        std::stringstream theStream;
        tableSchema->encode(theStream);
     
        theBlock.copyToPayload(theStream.str());
        
        theBlock.header.next = 0; // not actually zero
        theBlock.header.index = theIO.getBlockCount() + 1;
     // fix this
        theIO.addBlock(theBlock);
        
      aNumAffectedRows = 1;

      aStatement = Config::getOkQueryMessage(aNumAffectedRows);
      return Errors::noError;
    }

  protected:
    std::string tableName;
    std::optional<Schema> tableSchema = std::nullopt;
  };

  class DropTableStatement : public SqlCmdStatement {
  public:
    DropTableStatement() : SqlCmdStatement(Keywords::drop_kw) {}

    StatusResult parse(Tokenizer &aTokenizer) override {
      return parseTableNameCmd(aTokenizer, tableName);
    }

    StatusResult execute(std::optional<TableView> &aTableView, int &aNumAffectedRows, std::string &aStatement) override {
      Database* theDb = Database::getDbInstance();
      if(theDb == nullptr) {
        return Errors::noDatabaseSpecified;
      }
      std::string theDbName = theDb->getDbName();
      StatusResult theResult;
      if(!tableExists(theDbName, tableName)){theResult = Errors::unknownTable;}
      //TODO: implement
      auto* theDeleteStatement = new DeleteStatement(true, tableName);
      StatusResult theDeleteAllRowsResult = theDeleteStatement->execute(aTableView, aNumAffectedRows, aStatement, theDbName);
      if (!theDeleteAllRowsResult) {
        return theDeleteAllRowsResult;
      }
        OpenFile tag;
        BlockIO theIO(theDbName,tag);
        Block theBlock;
        // USE THE INDEX HERE AND DESTROY INDEX AFTER
        for(size_t i=0;i<theIO.getBlockCount();i++){
            theIO.readBlock((int)i, theBlock);
            if (theBlock.header.name==tableName && theBlock.header.type == (char)BlockType::schema_block) {
                theBlock.header.type = (char)BlockType::free_block;
                theIO.writeBlock((int)i, theBlock);
                break;
            }
        }
        
//      if(theResult){
//        std::remove(Config::getDBPath(dbName).c_str());
//      }
//      aNumAffectedRows = 0;
      aStatement = Config::getOkQueryMessage(aNumAffectedRows);
      return theResult;
    }

  protected:
    std::string tableName;
  };

  class DescribeTableStatement : public SqlCmdStatement {
  public:
    DescribeTableStatement() : SqlCmdStatement(Keywords::describe_kw) {}

    StatusResult parse(Tokenizer &aTokenizer) override {
      if (aTokenizer.remaining() < 2
          || aTokenizer.peek().keyword != Keywords::unknown_kw)
        return Errors::invalidCommand;
      aTokenizer.next();
      tableName = aTokenizer.current().data;
      aTokenizer.next(2);
      return Errors::noError;
    }

    StatusResult execute(std::optional<TableView> &aTableView, int &aNumAffectedRows, std::string &aStatement) override {
      Database* theDb = Database::getDbInstance();
      if(theDb == nullptr) {
        return Errors::noDatabaseSpecified;
      }
      std::string theDbName = theDb->getDbName();
        
        AttributeList temp;
        Schema theTable("",temp);
        OpenFile tag;
        BlockIO theIO(theDbName,tag);
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

      std::vector<std::string> headerList = Config::getDescribeTablesTitle(); // using statements?
      Grid entries = createTableMetadataView(theTable);
      aNumAffectedRows = (int)theTable.getAttributes().size();
      aStatement = Config::getRowsInSetMessage(aNumAffectedRows);
      std::vector<int> theSizes(std::vector<int>(6, 30)); // TODO: Write an algorithm for sizes
      TableView theTableView = TableView(headerList, entries, theSizes);
      std::optional<TableView> theOptionalTable(theTableView);
//      aTableView = theOptionalTable;
      return Errors::noError;
    }

  protected:
    std::string tableName;
  };

  class ShowTablesStatement : public SqlCmdStatement {
  public:
    ShowTablesStatement() : SqlCmdStatement(Keywords::show_kw) {}

    StatusResult parse(Tokenizer &aTokenizer) override {
      if (aTokenizer.remaining() < 2 || aTokenizer.peek().keyword != Keywords::tables_kw)
        return Errors::invalidCommand;
      aTokenizer.next(2);
      return Errors::noError;
    }

    StatusResult execute(std::optional<TableView> &aTableView, int &aNumAffectedRows, std::string &aStatement) override {
      Database* theDb = Database::getDbInstance();
      if(theDb == nullptr) {
        return Errors::noDatabaseSpecified;
      }
      std::string theDbName = theDb->getDbName();
      std::vector<std::string> headerList{Config::getShowTablesTitle(theDbName)}; // using statements?
      std::vector<std::string> theTables = getTables(theDbName);
      Grid entries = createTableList(theTables);
      aNumAffectedRows = (int)theTables.size();
      aStatement = Config::getRowsInSetMessage((int)theTables.size());
      std::vector<int> sizes{50};
      TableView theTableView = TableView(headerList, entries, sizes);
      std::optional<TableView> theOptionalTable(theTableView);
      aTableView = theOptionalTable;

      return Errors::noError;
    }
  };

  class ShowIndexesStatement : public SqlCmdStatement {
  public:
    ShowIndexesStatement() : SqlCmdStatement(Keywords::indexes_kw) {}

    StatusResult parse(Tokenizer &aTokenizer) override {
      if (aTokenizer.remaining() < 2 || aTokenizer.peek().keyword != Keywords::indexes_kw)
        return Errors::invalidCommand;
      aTokenizer.next(2);
      return Errors::noError;
    }

    StatusResult execute(std::optional<TableView> &aTableView, int &aNumAffectedRows, std::string &aStatement) override {
      Database* theDb = Database::getDbInstance();
      if(theDb == nullptr) {
        return Errors::noDatabaseSpecified;
      }
      std::string theDbName = theDb->getDbName();
      std::vector<std::string> headerList{Config::getShowTablesTitle(theDbName), Config::getShowIndex()}; // using statements?
      std::vector<std::string> theTables = getTables(theDbName);
      std::vector<std::string> thePrimAttrs;
      for (auto theTable : theTables) {
        Schema theSchema = getSchema(theDbName, theTable);
        std::optional<std::string> thePrimaryAttributeName = theSchema.getPrimaryName();
        if (thePrimaryAttributeName.has_value()) {
          thePrimAttrs.push_back(thePrimaryAttributeName.value());
        } else {
          thePrimAttrs.emplace_back("");
        }
      }
      Grid entries = createIndexesList(theTables, thePrimAttrs);
      aNumAffectedRows = (int)theTables.size();
      aStatement = Config::getRowsInSetMessage((int)theTables.size());
      std::vector<int> sizes{40, 40};
      TableView theTableView = TableView(headerList, entries, sizes);
      std::optional<TableView> theOptionalTable(theTableView);
      aTableView = theOptionalTable;

      return Errors::noError;
    }
  };

  class ShowIndexStatement : public RecordCmdStatement {
  public:
    ShowIndexStatement() : RecordCmdStatement(Keywords::index_kw) {}

    StatusResult parse(Tokenizer &aTokenizer) override {
      if (aTokenizer.remaining() < 5 || aTokenizer.peek().keyword != Keywords::index_kw)
        return Errors::invalidCommand;
      aTokenizer.next(4);
      // TODO: Check parsing
      tableName = aTokenizer.current().data;
      aTokenizer.next();
      aTokenizer.skipIf(semicolon);
      return Errors::noError;
    }

    StatusResult execute(std::optional<TableView> &aTableView, int &aNumAffectedRows, std::string &aStatement, std::string &aDbName) override {
      std::map<uint16_t, uint16_t> theData;
      uint32_t theHash = Helpers::hashString(tableName.c_str());
      StatusResult theRes = Database::getIndex(aDbName, theHash, theData);
      if (!theRes) return theRes;
      std::vector<std::string> headerList{"Key", "Block#"};
      std::vector<std::string> theKeys, theBlocks;
      for (auto theKV : theData) {
        theKeys.push_back(std::to_string(theKV.first));
        theBlocks.push_back(std::to_string(theKV.second));
      }

      Grid entries = createIndexesList(theKeys, theBlocks);
      aNumAffectedRows = (int)theKeys.size();
      aStatement = Config::getRowsInSetMessage((int)theKeys.size());
      std::vector<int> sizes{40, 40};
      TableView theTableView = TableView(headerList, entries, sizes);
      std::optional<TableView> theOptionalTable(theTableView);
      aTableView = theOptionalTable;

      return Errors::noError;
    }
  };

  class InsertStatement : public RecordCmdStatement {
  public:
    InsertStatement() : RecordCmdStatement(Keywords::insert_kw) {}

    StatusResult parseAttributeList(Tokenizer& aTokenizer) {
      if (aTokenizer.current().data != "(") return Errors::syntaxError;
      aTokenizer.next();

      while (aTokenizer.more()) {
        Token& current = aTokenizer.current();
        if (current.type != TokenType::identifier) return Errors::syntaxError;
        attributeList.push_back(current.data);

        aTokenizer.next();
        current = aTokenizer.current();
        if (aTokenizer.current().data == ")") {
          break;
        } else if (aTokenizer.current().data == ",") {
          aTokenizer.next();
        } else {
          return Errors::syntaxError;
        }
      }

      aTokenizer.next();
      return Errors::noError;
    }

    StatusResult parseSinglesRowValues(Tokenizer& aTokenizer) {
      if (aTokenizer.current().data != "(") return Errors::syntaxError;
      aTokenizer.next();

      std::vector<std::string> theValues;

      while (aTokenizer.more()) {
        Token& current = aTokenizer.current();
        if (current.type == TokenType::keyword || current.type == TokenType::function) return Errors::syntaxError;
        theValues.push_back(current.data);

        aTokenizer.next();
        current = aTokenizer.current();
        if (aTokenizer.current().data == ")") {
          break;
        } else if (aTokenizer.current().data == ",") {
          aTokenizer.next();
        } else {
          return Errors::syntaxError;
        }
      }

      aTokenizer.next();
      values.push_back(theValues);
      return Errors::noError;
    }

    StatusResult parseValues(Tokenizer& aTokenizer) {
      while (aTokenizer.more()) {
        StatusResult theResult = parseSinglesRowValues(aTokenizer);
        if (!theResult) {
          aTokenizer.restart();
          return theResult;
        }
        aTokenizer.next();
//        if (aTokenizer.current().data != ",") return Errors::syntaxError;
//        aTokenizer.next();
        aTokenizer.skipIf(semicolon);
      }
      return Errors::noError;
    }

    StatusResult parse(Tokenizer &aTokenizer) override {
      // INSERT INTO users
      //  ('first_name', 'last_name', 'email')
      //VALUES
      //  ('Stephen','Kim', 'sskim@ucsd.edu'),
      //  ('Emin', 'Kirimlioglu', 'ekirimli@ucsd.edu');
      if (aTokenizer.remaining() < 4 || aTokenizer.peek().keyword != Keywords::into_kw)
        return Errors::invalidCommand;
      aTokenizer.next(2);

      if (aTokenizer.current().type != TokenType::identifier) return Errors::syntaxError;
      tableName = aTokenizer.current().data;
      aTokenizer.next();

      StatusResult theResult = parseAttributeList(aTokenizer);
      if (!theResult) {
        aTokenizer.restart();
        return theResult;
      }

      if(aTokenizer.current().keyword !=Keywords::values_kw) return Errors::syntaxError;
      aTokenizer.next();

      theResult = parseValues(aTokenizer);
      if (!theResult) {
        aTokenizer.restart();
        return theResult;
      }

      return Errors::noError;
    }

    StatusResult validateValue(Attribute& anAttribute, std::string aValue, std::unique_ptr<Row>& aRow) {
      // TODO: Also check for nullable, primary key, etc.

      DataTypes theType = anAttribute.getType();
      if (theType == DataTypes::int_type) {
        try {
          int theData = std::stoi(aValue);
          aRow->set(anAttribute.getName(), theData);
        } catch(...) {
          return Errors::invalidArguments;
        }
      } else if (theType == DataTypes::float_type) {
        try {
          float theData = std::stof(aValue);
          aRow->set(anAttribute.getName(), theData);
        } catch(...) {
          return Errors::invalidArguments;
        }
      } else if (theType == DataTypes::bool_type) {
        if (aValue == "true" || aValue == "TRUE") aRow->set(anAttribute.getName(), true);
        else if (aValue == "false" || aValue == "FALSE") aRow->set(anAttribute.getName(), false);
        else return Errors::invalidArguments;
      } else if (theType == DataTypes::varchar_type) {
        if (aValue.size() > anAttribute.getSize()) return Errors::invalidArguments;
        else aRow->set(anAttribute.getName(), aValue);
      } else if (theType == DataTypes::datetime_type) {
        // TODO: Do we need to parse and check this?
        aRow->set(anAttribute.getName(), aValue);
      } else {
        return Errors::invalidAttribute;
      }
      return Errors::noError;
    }

    StatusResult validateValues(RowCollection& aRowCollection,Schema& aSchema) {
      // TODO: Primary key
      AttributeList theAttributeListOriginal = AttributeList(aSchema.getAttributes());
      AttributeList theAttributes;
      for (std::string theAttr : attributeList) {
        auto thePtr = std::find_if(theAttributeListOriginal.begin(), theAttributeListOriginal.end(),
                                         [&](const Attribute anAttr)
                                         {
          return anAttr.getName() == theAttr;
                                         });
        if (thePtr == aSchema.getAttributes().end()) {
          return Errors::invalidAttribute;
        }
        Attribute theAttri = *thePtr;
        theAttributes.push_back(*thePtr);
      }
      for (std::vector<std::string> theVec : values) {
        std::unique_ptr<Row> theRow{new Row()};
        for (int i = 0; i < (int) theAttributes.size(); i++) {
          StatusResult theResult = validateValue(theAttributes[i], theVec[i], theRow);
          if (!theResult) return theResult;
        }
        aRowCollection.push_back(std::move(theRow));
      }

      return Errors::noError;
    }

    StatusResult execute(std::optional<TableView> &aTableView, int &aNumAffectedRows, std::string &aStatement, std::string &aDbName) override {

        
        // check if the schema is auto icnremented
        
        Schema theSchema  = getSchema(aDbName);
        
        
        
      RowCollection theRowCollection;
      StatusResult theValidateValuesResult = validateValues(theRowCollection, theSchema);
      if (!theValidateValuesResult) return theValidateValuesResult;

      std::optional<std::string> thePrimaryAttributeName = theSchema.getPrimaryName();

      // TODO: insert rows using theRowCollection and create View
        OpenFile tag;
   //     BlockIO theIO(aDbName,tag);
        Storage* theIO = Storage::GetInstance(aDbName, tag);
        int count{0};
        theSchema = getSchema(aDbName);



      // TODO: Get Index block and
      uint16_t theIndexIndex = getIndexIndex(aDbName);
      Block theIndexBlock;
      if (theIndexIndex != 0) {
        theIO->readBlock(int(theIndexIndex), theIndexBlock);
      }
      Index<uint16_t, uint16_t> theIndex(tableName, theIndexBlock);
      std::stringstream theIndexStream;
      theIndexStream << theIndexBlock.payload;
        if (theIndexIndex != 0 && Config::useIndex()) {
            theIndex.decode(theIO, theIndexStream);
        }
      

        for(auto& theRow: theRowCollection){
            count++;

            theSchema = getSchema(aDbName);

            uint32_t theHash = Helpers::hashString(theSchema.getName().c_str());

            
            Block theBlock;
            std::vector<Row> theRows;
            /*
            for(size_t i=0;i<theIO->getBlockCount();i++){
                theIO->readBlock((int)i, theBlock);
                if(theBlock.header.hash==theHash && theBlock.header.type==(char)BlockType::data_block){
                    std::stringstream theStream;
                    theBlock.write(theStream);
                    Row theTempRow;
                    
                    theTempRow.decode(theStream);
                    theRows.push_back(theTempRow);
                 //   theBlock.copyToPayload(theStream.str());
                //    std::cout << theStream.str();
             //       theTable.decode(theStream);
                  //  theBlock.header.type = (char)BlockType::free_block;
              //      theIO.writeBlock(i, theBlock);
                }
                  
            }
             */
            Database::getRows(aDbName, theHash, theRows);
            bool flag=true;
            if(theRows.size() == 0){flag = false;}
            
            
            if (thePrimaryAttributeName.has_value() && flag) {
                std::string thePrimAttrName = thePrimaryAttributeName.value();
                std::sort(theRows.begin(), theRows.end(), [&]( Row& lhs,  Row& rhs){
                    return std::stoi(std::get<std::string>(lhs.getData().at(thePrimAttrName))) < std::stoi(std::get<std::string>(rhs.getData().at(thePrimAttrName)));
                });
                Row lastRow = theRows.back();
                
                            Row* lastRowData = new Row();
//                            std::stringstream theStream(lastRow.payload);
//                          lastRowData->decode(theStream);
                            lastRowData = &lastRow;
                        
                             Value aa = lastRowData->getData().at(thePrimAttrName);
                              if (lastRowData->getData().count(thePrimAttrName)) {
                                int lastRowPrimaryValue = std::stoi(std::get<std::string>(lastRowData->getData().at(thePrimAttrName)));
                              theRow->set(thePrimAttrName, lastRowPrimaryValue+1);
                              }
            }
            if (thePrimaryAttributeName.has_value() && !flag){  theRow->set(thePrimaryAttributeName.value(), 1);}

                            std::stringstream theStream;
                            theRow->encode(theStream);
                            Block theNewBlock;
                            theNewBlock.copyToPayload(theStream.str());
                            theNewBlock.header.index = theIO->getBlockCount();
                          //  theNewBlock.header.next = 0;
            
                            theNewBlock.header.hash = theHash;
            
                            theNewBlock.header.type = (char)BlockType::data_block;
                            std::memcpy(theNewBlock.header.name, theSchema.getName().c_str(),std::min(theSchema.getName().length(), sizeof(theNewBlock.header.name)));
                            theIO->addBlock(theNewBlock);

                            // TODO: Set Index if primary attribute is available
                            if (Config::useIndex() && theIndexIndex != 0) {
                              uint16_t theKey = std::get<int>(theRow->getData().at(thePrimaryAttributeName.value()));
                              theIndex.add(theKey,theNewBlock.header.index);
//          uint32_t theHash = Helpers::hashString(tableName.c_str());
                              theIndex.writeToStorage(theIO, tableName, theHash);
                            }

        }

        // TODO: Write to storage
//      uint32_t theHash = Helpers::hashString(tableName.c_str());
//      theIndex.writeToStorage(theIO, tableName, theHash);

        aNumAffectedRows = static_cast<int>(theRowCollection.size());
        aStatement = Config::getOkQueryMessage(aNumAffectedRows);
        
        
      return Errors::noError;
    }

  protected:
    std::vector<std::string> attributeList;
    std::vector<std::vector<std::string>> values;
  };

  class SelectStatement : public RecordCmdStatement {
  public:
    SelectStatement() : RecordCmdStatement(Keywords::select_kw), showAll(false) {}

    StatusResult parseProperties(Tokenizer& aTokenizer) {
      aTokenizer.next();
      if (aTokenizer.current().type == TokenType::operators) {
        if (aTokenizer.current().data == "*") showAll = true;
        aTokenizer.next();
      } else if (aTokenizer.current().type == TokenType::identifier) {
        while (aTokenizer.more()) {
          Token& cur = aTokenizer.current();
          if (cur.type != TokenType::identifier) return Errors::syntaxError;
          TableField theTableField("", "");
          Token &theToken=aTokenizer.current(); //identifier name?
          if(TokenType::identifier==theToken.type) {
            theTableField.fieldName=theToken.data;
            aTokenizer.next();
            if(aTokenizer.skipIf(Operators::dot_op)) {
              theToken=aTokenizer.current();
              if(TokenType::identifier==theToken.type) {
                aTokenizer.next(); //yank it...
                theTableField.table=theTableField.fieldName;
                theTableField.fieldName=theToken.data;
//                aTokenizer.next();
              }
            }
            attributeList.push_back(theTableField);
            if (aTokenizer.current().keyword == Keywords::from_kw) break;
            if (aTokenizer.current().data != ",") return Errors::syntaxError;
            aTokenizer.next();
          }
        }
      } else return Errors::syntaxError;

      if (!aTokenizer.more() || aTokenizer.current().keyword != Keywords::from_kw)
        return Errors::syntaxError;
      aTokenizer.next();
      return Errors::noError;
    }

    StatusResult parseOrderBy(Tokenizer& aTokenizer) {
      if (aTokenizer.remaining() < 3 || aTokenizer.current().keyword != Keywords::order_kw || aTokenizer.peek().keyword != Keywords::by_kw)
        return Errors::syntaxError;
      aTokenizer.next(2);
      orderBy = aTokenizer.current().data;
      aTokenizer.next();
      if (aTokenizer.more() && aTokenizer.current().keyword == Keywords::desc_kw) {
        aTokenizer.next();
        orderDesc = true;
      } else {
        orderDesc = false;
      }
      return Errors::noError;
    }

    StatusResult parseLimit(Tokenizer& aTokenizer){
      if (!aTokenizer.more()) return Errors::syntaxError;
      aTokenizer.next();
      if (aTokenizer.current().type != TokenType::number) return Errors::syntaxError;
      limit = stoi(aTokenizer.current().data);
      aTokenizer.next();
      return Errors::noError;
    }

    StatusResult parseWhere(Tokenizer& aTokenizer) {
      aTokenizer.next();

      while (aTokenizer.more() && aTokenizer.current().type != TokenType::keyword) {
        Expression theExpression;
        StatusResult theExpressionParse = parseExpression(aTokenizer, theExpression);
        if (!theExpressionParse) {
          aTokenizer.restart();
          return theExpressionParse;
        }

        filters.add(new Expression(theExpression));
        aTokenizer.skipIf(semicolon);
      }
      return Errors::noError;
    }

    StatusResult parseJoinCondition(Tokenizer& aTokenizer) {
      Keywords theJoinType;
      if (aTokenizer.current().keyword == Keywords::left_kw) theJoinType = Keywords::left_kw;
      else if (aTokenizer.current().keyword == Keywords::right_kw) theJoinType = Keywords::right_kw;
      else return Errors::syntaxError;
      aTokenizer.next();

      if (aTokenizer.current().keyword != Keywords::join_kw) return Errors::syntaxError;
      aTokenizer.next();

      std::string theJoinTable = aTokenizer.current().data;
      aTokenizer.next();

      if (aTokenizer.current().keyword != Keywords::on_kw) return Errors::syntaxError;
      aTokenizer.next();

      std::string tableAttr, otherTableAttr;

      if (aTokenizer.remaining() < 7) return Errors::noError;
      for (int i = 0; i < 2; i++) {
        if (aTokenizer.current().data == tableName) {
          aTokenizer.next(2);
          tableAttr = aTokenizer.current().data;
        } else {
          aTokenizer.next(2);
          otherTableAttr = aTokenizer.current().data;
        }
        aTokenizer.next();
        if (i == 0) {
          if (aTokenizer.current().type != TokenType::operators || aTokenizer.current().op != Operators::equal_op)
            return Errors::syntaxError;
          aTokenizer.next();
        }
      }

      TableField theJoinTableField(otherTableAttr, theJoinTable);
      TableField theTableField(tableAttr, tableName);

      Join theJoin(theJoinTable, theJoinType, theTableField, theJoinTableField);
      joins.push_back(theJoin);
      aTokenizer.skipIf(semicolon);

      return Errors::noError;
    }

    StatusResult parse(Tokenizer &aTokenizer) override {
      if (aTokenizer.remaining() < 4) return Errors::invalidCommand;

      StatusResult theResult = parseProperties(aTokenizer);
      if (!theResult) { aTokenizer.restart(); return theResult; }

      if (!aTokenizer.more() || aTokenizer.current().type != TokenType::identifier) return Errors::syntaxError;
      tableName = aTokenizer.current().data;
      aTokenizer.next();
      aTokenizer.skipIf(semicolon);

      while (aTokenizer.more()) {
        StatusResult theResult;
        if (aTokenizer.current().keyword == Keywords::where_kw) {
          theResult = parseWhere(aTokenizer);
        } else if (aTokenizer.current().keyword == Keywords::order_kw) {
          theResult = parseOrderBy(aTokenizer);
        } else if (aTokenizer.current().keyword == Keywords::limit_kw) {
          theResult = parseLimit(aTokenizer);
        } else {
          theResult = parseJoinCondition(aTokenizer);
        }
        if (!theResult) return theResult;

        aTokenizer.skipIf(semicolon);

      }
      return Errors::noError;
    }

    StatusResult filter(std::vector<Row>& allRows, std::vector<Row>& allResultRows) {
        for (auto &theRow: allRows) {
          Expressions &theExpressions = filters.getExpressions();
          bool theOperation = true;
          for (auto &theExp: theExpressions) {
            if (!theExp->operator()(theRow)) {
              theOperation = false;
              break;r
            }
          }
          if (theOperation) {
            allResultRows.push_back(theRow);
          }
        }
      return Errors::noError;
    }

    StatusResult execute(std::optional<TableView> &aTableView, int &aNumAffectedRows, std::string &aStatement, std::string &aDbName) override {
      // TODO and Think about it: Implement Filters and Limit while fetching the rows from storage
    std::vector<Row> theRows;

    //    OpenFile tag;
       
      
  //      Storage theStorage(aDbName,tag);
    //    Storage* theStorage = Storage::GetInstance(aDbName, tag); UNUSED

        Schema theSchema = getSchema(aDbName);

        if (showAll) {
          for (auto& theAttr : theSchema.getAttributes()) {
            TableField theTab("", tableName);
            theTab.fieldName = theAttr.getName();
            attributeList.push_back(theTab);
          }
        }

      StatusResult theGetRows = Database::getRows(aDbName, Helpers::hashString(theSchema.getName().c_str()), theRows);
      if (!theGetRows) return theGetRows;

      std::vector<Row> theFilteredRows;

      std::vector<Row> theOtherTableRows;
      if (!joins.empty()) {
        StatusResult theGetOtherRows = Database::getRows(aDbName, Helpers::hashString(joins[0].table.c_str()), theOtherTableRows);
        if (!theGetOtherRows) return theGetRows;
      }
          StatusResult theFilterResult = filter(theRows, theFilteredRows);
          if (!theFilterResult) return theFilterResult;

          if (orderBy.length())  {
            std::sort(theFilteredRows.begin(), theFilteredRows.end(), [&](Row aFirstRow,Row aSecondRow) {

              Value theFirstValue = aFirstRow.getData()[orderBy];
              Value theSecondValue = aSecondRow.getData()[orderBy];
              if (orderDesc) {
                return theFirstValue > theSecondValue;
              }
              return theFirstValue < theSecondValue;
            });
          }
          if(limit > 0){
            int theAmountDelete = (int)theFilteredRows.size() - limit;
            if(theAmountDelete > 0){
              for(int i=0;i<theAmountDelete;i++){
                theFilteredRows.pop_back();
              }
            }
          }

        std::vector<std::string> theAttrList;
        for (const TableField& theTableField : attributeList) {
          theAttrList.push_back(theTableField.fieldName);
        }

        // TODO: OrderBy and Limit in case of Joins

      std::vector<std::string> headerList(theAttrList);
      Grid entries = createRecordsList(theFilteredRows, theOtherTableRows, attributeList, joins, tableName);
      aNumAffectedRows = (int)entries.size();
      aStatement = Config::getRowsInSetMessage((int)entries.size());
      std::vector<int> sizes(std::vector<int>((int)attributeList.size() ,150/(int)attributeList.size()));
      TableView theTableView = TableView(headerList, entries, sizes);
      std::optional<TableView> theOptionalTable(theTableView);
      aTableView = theOptionalTable;

      return Errors::noError;
    }

  protected:
    std::vector<TableField> attributeList;
    std::vector<std::vector<std::string>> values;
    Filters filters;
    bool showAll;
    std::string orderBy;
    bool orderDesc;
    int limit;
    std::vector<Join> joins;
  };

  class UpdateStatement : public RecordCmdStatement {
  public:
    UpdateStatement() : RecordCmdStatement(Keywords::update_kw) {}



    StatusResult parse(Tokenizer &aTokenizer) override {
      if (aTokenizer.remaining() < 10) return Errors::invalidCommand;
      if (aTokenizer.current().keyword != Keywords::update_kw
            || aTokenizer.peek().type != TokenType::identifier
            || aTokenizer.peek(2).keyword != Keywords::set_kw
            || aTokenizer.peek(4).data != "="
            || aTokenizer.peek(6).keyword != Keywords::where_kw) {
        aTokenizer.restart();
        return Errors::syntaxError;
      }

      tableName = aTokenizer.peek().data;
      attributeName = aTokenizer.peek(3).data;
      value = aTokenizer.peek(5).data;

      aTokenizer.next(7);
      StatusResult theResult = parseExpression(aTokenizer, expression);
      if (!theResult) {
        aTokenizer.restart();
        return theResult;
      }

      aTokenizer.skipIf(semicolon);
      return Errors::noError;
    }


    StatusResult execute(std::optional<TableView> &aTableView, int &aNumAffectedRows, std::string &aStatement, std::string &aDbName) override {
      // TODO and Think about it: Implement Filters and Limit while fetching the rows from storage
      // TODO: get rows for the table from block IO.

        std::vector<Row> theRows;
            OpenFile tag;
           
          
       //     Storage theStorage(aDbName,tag);
            Storage* theStorage = Storage::GetInstance(aDbName, tag);
        
            Schema theSchema = getSchema(aDbName);

            uint32_t theHash = Helpers::hashString(theSchema.getName().c_str());

           
            

          //  int theNext = theSchema.getNext();
            for(size_t i=0;i<theStorage->getBlockCount();i++){
                Block theBlock;
                theStorage->readBlock((int)i, theBlock);
                if(theBlock.header.hash == theHash && theBlock.header.type == (char)BlockType::data_block){
                    
                    Row theRow;
                    std::stringstream theStream;
                    theStream << theBlock.payload;
                    theRow.decode(theStream);
                    
                    if(expression.operator()(theRow)){
                        theRow.set(attributeName, value);
                        std::stringstream inputStream;
                        theRow.encode(inputStream);
                        theBlock.copyToPayload(inputStream.str());
                        theStorage->writeBlock((int)i, theBlock);
                        aNumAffectedRows += 1;
                    }
                    
                    
                  //  theRows.push_back(theRow);
                }
            }
        
//        aNumAffectedRows = static_cast<int>(theRowCollection.size());
        aStatement = Config::getOkQueryMessage(aNumAffectedRows);
        
      // for (every row) {
        // if (!expression.operator()(theRow)) {
        //    updateRow();
        // }
      // }

      return Errors::noError;
    }

  protected:
    std::string attributeName;
    std::string value;
    Expression expression;
  };



}
