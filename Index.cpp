//
// Created by SIDDHANT on 02-06-2023.
//

#include <sstream>
#include <cstring>
#include "Index.hpp"

namespace ECE141 {

  template<>
  StatusResult  Index<uint16_t , uint16_t>::encode(std::ostream &anOutput) const {
    // data = d
    // size = s

//    anOutput << "{\n";
//    anOutput << "s:" << data.size() << "\n";
//    anOutput << "[\n";
//    for(auto& i: data){
//      // key = k
//      // value = v
//      anOutput << std::to_string(i.first) << "\n";
//      anOutput << std::to_string(i.second) << "\n";
//    }
//    anOutput << "]\n";
//    anOutput << "}\n";

    return Errors::noError;
  }

  template<>
  StatusResult  Index<uint16_t, uint16_t>::encodeAll(std::vector<std::string> &aOutputs) {
    uint16_t theLimit = (kBlockSize-16)/10;

    std::map<uint16_t, uint16_t> theRemainingData{data};

    auto itBegin = data.begin();
    while (!theRemainingData.empty()) {
      std::map<uint16_t, uint16_t> theData;
      if (theLimit < theRemainingData.size()) {
        auto it = itBegin;
        std::advance(it, theLimit);
        theData.insert(itBegin, it);
        theRemainingData.clear();
        theRemainingData.insert(it, data.end());
        itBegin = it;
      } else {
        theRemainingData.clear();
        theData.insert(itBegin, data.end());
      }

      std::stringstream theOp;
      theOp << "{\n";
      theOp << "s:" << theData.size() << "\n";
      theOp << "[\n";
      for(auto& i: theData){
        // key = k
        // value = v
        theOp << std::to_string(i.first) << "\n";
        theOp << std::to_string(i.second) << "\n";
      }
      theOp << "]\n";
      theOp << "}\n";

      aOutputs.push_back(theOp.str());
    }

    return Errors::noError;
  }

  template<>
  StatusResult  Index<uint16_t, uint16_t>::decode(std::istream &anInput) {
    return Errors::noError;
  }

  template<>
  StatusResult  Index<uint16_t, uint16_t>::decode(Storage* aStorage, std::istream &anInput) {
    std::string temp;
    std::getline(anInput,temp);
    std::getline(anInput,temp);
    int dataSize = std::stoi(temp.substr(temp.find(':')+1));

    std::getline(anInput,temp);
    if (temp.front() == '[') {
      for(int i=0;i<dataSize;i++){
        std::getline(anInput,temp);
        uint16_t theKey = std::stoi(temp);
        std::getline(anInput,temp);
        uint16_t theValue = std::stoi(temp);
        data[theKey] = theValue;
      }
    }

    allBlocks.push_back(blockNumber);

    uint16_t theNext = next;
    if (theNext != 0) {
//      OpenFile tag;
//      BlockIO theStorage(dbName, tag);
      Block theBlock;

      aStorage->readBlock((int)theNext, theBlock);

      Index theTempIndex(dbName, theBlock);
      std::stringstream theStream;
      theStream << theBlock.payload;
      theTempIndex.decode(aStorage,theStream);

      std::map<uint16_t, uint16_t> theOtherData = theTempIndex.getData();
      std::vector<uint16_t> theBlockNumbers = theTempIndex.allBlocks;
      allBlocks.insert(allBlocks.end(), theBlockNumbers.begin(), theBlockNumbers.end());

      data.insert(theOtherData.begin(), theOtherData.end());
    }

    return Errors::noError;
  }

  template<>
  StatusResult  Index<uint16_t, uint16_t>::storeNewIndexBlock(Storage* aStorage, Block &theNewBlock, const std::string& aTableName, uint32_t aHash) {
    std::stringstream stream;
    theNewBlock.header.hash = aHash;

    theNewBlock.header.type = (char)BlockType::index_block;
    std::memcpy(theNewBlock.header.name, aTableName.c_str(),std::min(aTableName.length(), sizeof(theNewBlock.header.name)));

    stream << "{\n";
    stream << "s:" << std::to_string(0) << "\n";
    stream << "[\n";
    stream << "]\n";
    stream << "}\n";

    theNewBlock.copyToPayload(stream.str());
    aStorage->addBlock(theNewBlock);

    return Errors::noError;
  }

  template<>
  StatusResult Index<uint16_t, uint16_t>::writeToStorage(Storage* aStorage, const std::string& aTableName, uint32_t aHash) {
    std::map<uint16_t, uint16_t> theData;
    std::map<uint16_t, uint16_t> theRemainingData;

    std::vector<std::string> theOps;
    StatusResult theEncode = encodeAll(theOps);
    if (!theEncode) return theEncode;

    while (theOps.size() > allBlocks.size()) {
      // Create index block. Make it as next of the previous block and add to allBlocks
      Block theNewBlock;

      StatusResult theRes = storeNewIndexBlock(aStorage, theNewBlock, aTableName, aHash);
      if (!theRes) return theRes;

      Block theBlock;
      aStorage->readBlock((int)allBlocks.back(), theBlock);
      theBlock.header.next = theNewBlock.header.index;
      aStorage->writeBlock((int)allBlocks.back(), theBlock);
      allBlocks.push_back(theNewBlock.header.index);
    }

    while (theOps.size() < allBlocks.size()) {
      // Mark the last block in the array as free. And remove from the array allBlocks
      aStorage->markBlockAsFree((int)allBlocks.back());
      allBlocks.pop_back();
    }

    if (theOps.empty()) {
      std::stringstream theOp;
      uint16_t theBlockNumber = allBlocks[0];
      // Get block at blockNumber theBlockNumber
      Block theBlock;
      aStorage->readBlock((int)theBlockNumber, theBlock);
      theBlock.copyToPayload(theOp.str());
      aStorage->writeBlock((int)theBlockNumber, theBlock);
      return Errors::noError;
    }

    for (size_t i = 0; i < theOps.size(); i++) {
      auto theStr = theOps.at(i);

      std::stringstream theOp;
      theOp << theStr;

      uint16_t theBlockNumber = allBlocks[i];
      // Get block at blockNumber theBlockNumber
      Block theBlock;
      aStorage->readBlock((int)theBlockNumber, theBlock);
      theBlock.copyToPayload(theOp.str());
      aStorage->writeBlock((int)theBlockNumber, theBlock);
    }

    return Errors::noError;
  }

  template<>
  StatusResult Index<uint16_t , uint16_t>::deleteIndex(Storage* aStorage) {
    for (auto i : allBlocks) {
   //   aStorage->markBlockAsFree((int)allBlocks.back()); // check here
      aStorage->markBlockAsFree((int)i);
        
    }
    return Errors::noError;
  }

  template<>
  StatusResult  Index<uint32_t , uint16_t>::encode(std::ostream &anOutput) const {
    // data = d
    // size = s

//    anOutput << "{\n";
//    anOutput << "s:" << data.size() << "\n";
//    anOutput << "[\n";
//    for(auto& i: data){
//      // key = k
//      // value = v
//      anOutput << std::to_string(i.first) << "\n";
//      anOutput << std::to_string(i.second) << "\n";
//    }
//    anOutput << "]\n";
//    anOutput << "}\n";

    return Errors::noError;
  }

  template<>
  StatusResult  Index<uint32_t, uint16_t>::encodeAll(std::vector<std::string> &aOutputs) {
    uint16_t theLimit = (kBlockSize-16)/8;

    std::map<uint32_t, uint16_t> theRemainingData{data};

    auto itBegin = data.begin();
    while (!theRemainingData.empty()) {
      std::map<uint32_t, uint16_t> theData;
      if (theLimit < theRemainingData.size()) {
        auto it = itBegin;
        std::advance(it, theLimit);
        theData.insert(itBegin, it);
        theRemainingData.clear();
        theRemainingData.insert(it, data.end());
        itBegin = it;
      } else {
        theRemainingData.clear();
        theData.insert(itBegin, data.end());
      }

      std::stringstream theOp;
      theOp << "{\n";
      theOp << "s:" << theData.size() << "\n";
      theOp << "[\n";
      for(auto& i: theData){
        // key = k
        // value = v
        theOp << std::to_string(i.first) << "\n";
        theOp << std::to_string(i.second) << "\n";
      }
      theOp << "]\n";
      theOp << "}\n";

      aOutputs.push_back(theOp.str());
    }

    return Errors::noError;
  }

  template<>
  StatusResult  Index<uint32_t, uint16_t>::decode(std::istream &anInput) {
    std::string temp;
    std::getline(anInput,temp);
    std::getline(anInput,temp);
    int dataSize = std::stoi(temp.substr(temp.find(':')+1));

    std::getline(anInput,temp);
    if (temp.front() == '[') {
      for(int i=0;i<dataSize;i++){
        std::getline(anInput,temp);
        uint16_t theKey = std::stoi(temp);
        std::getline(anInput,temp);
        uint16_t theValue = std::stoi(temp);
        data[theKey] = theValue;
      }
    }

    allBlocks.push_back(blockNumber);

    uint16_t theNext = next;
    if (theNext != 0) {
      OpenFile tag;
      Block theBlock;
      Storage theIO(dbName, tag);
      theIO.readBlock((int)theNext, theBlock);

      Index theTempIndex(dbName, theBlock);
      std::stringstream theStream;
      theStream << theBlock.payload;
      theTempIndex.decode(theStream);

      std::map<uint32_t, uint16_t> theOtherData = theTempIndex.getData();
      std::vector<uint16_t> theBlockNumbers = theTempIndex.allBlocks;
      allBlocks.insert(allBlocks.begin(), theBlockNumbers.begin(), theBlockNumbers.end());

      data.insert(theOtherData.begin(), theOtherData.end());
    }

    return Errors::noError;
  }

  template<>
  StatusResult  Index<uint32_t, uint16_t>::storeNewIndexBlock(Storage* aStorage, Block &theNewBlock, const std::string& aTableName, uint32_t aHash) {
    std::stringstream stream;
    theNewBlock.header.hash = aHash;

    theNewBlock.header.type = (char)BlockType::index_block;
    std::memcpy(theNewBlock.header.name, aTableName.c_str(),std::min(aTableName.length(), sizeof(theNewBlock.header.name)));

    stream << "{\n";
    stream << "s:" << std::to_string(0) << "\n";
    stream << "[\n";
    stream << "]\n";
    stream << "}\n";

    theNewBlock.copyToPayload(stream.str());
    aStorage->addBlock(theNewBlock);

    return Errors::noError;
  }

  template<>
  StatusResult Index<uint32_t, uint16_t>::writeToStorage(Storage* aStorage, const std::string& aTableName, uint32_t aHash) {
    std::map<uint32_t, uint16_t> theData;
    std::map<uint32_t, uint16_t> theRemainingData;

    std::vector<std::string> theOps;
    StatusResult theEncode = encodeAll(theOps);
    if (!theEncode) return theEncode;

    while (theOps.size() > allBlocks.size()) {
      // Create index block. Make it as next of the previous block and add to allBlocks
      Block theNewBlock;

      StatusResult theRes = storeNewIndexBlock(aStorage, theNewBlock, aTableName, aHash);
      if (!theRes) return theRes;

      Block theBlock;
      aStorage->readBlock((int)allBlocks.back(), theBlock);
      theBlock.header.next = theNewBlock.header.index;
      aStorage->writeBlock((int)allBlocks.back(), theBlock);
      allBlocks.push_back(theNewBlock.header.index);
    }

    while (theOps.size() < allBlocks.size()) {
      // Mark the last block in the array as free. And remove from the array allBlocks
      aStorage->markBlockAsFree((int)allBlocks.back());
      allBlocks.pop_back();
    }

    if (theOps.empty()) {
      std::stringstream theOp;
      uint16_t theBlockNumber = allBlocks[0];
      // Get block at blockNumber theBlockNumber
      Block theBlock;
      aStorage->readBlock((int)theBlockNumber, theBlock);
      theBlock.copyToPayload(theOp.str());
      aStorage->writeBlock((int)theBlockNumber, theBlock);
      return Errors::noError;
    }

    for (size_t i = 0; i < theOps.size(); i++) {
      auto theStr = theOps.at(i);
      std::stringstream theOp{theStr};


      uint16_t theBlockNumber = allBlocks[i];
      // Get block at blockNumber theBlockNumber
      Block theBlock;
      aStorage->readBlock((int)theBlockNumber, theBlock);
      theBlock.copyToPayload(theOp.str());
      aStorage->writeBlock((int)theBlockNumber, theBlock);
    }

    return Errors::noError;
  }

  template<>
  StatusResult Index<uint32_t , uint16_t>::deleteIndex(Storage* aStorage) {
    for (auto i : allBlocks) {

//      aStorage->markBlockAsFree((int)allBlocks.back()); // okay so does this break things though
        aStorage->markBlockAsFree((int)i);

    }
    return Errors::noError;
  }


} // ECE141
