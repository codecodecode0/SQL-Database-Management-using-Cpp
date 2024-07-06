////
////  BlockIO.cpp
////  PA2
////
////  Created by rick gessner on 2/27/23.
////
//




//  BlockIO.cpp
//  PA2
//
//  Created by rick gessner on 2/27/23.
//

#include <cstring>
#include "BlockIO.hpp"
#include "Config.hpp"

namespace ECE141 {

  Block::Block(BlockType aType) {}

  Block::Block(const Block &aCopy) {
    *this=aCopy;
  }

  Block& Block::operator=(const Block &aCopy) {
    std::memcpy(payload, aCopy.payload, kPayloadSize);
    header=aCopy.header;
    return *this;
  }

  StatusResult Block::write(std::ostream &aStream) { // not complete
      
    //  aStream.write((char*)&*this, kBlockSize);
      
      aStream.write(payload, kPayloadSize);
      
      
      
    return StatusResult{Errors::noError};
  }

    StatusResult Block::copyToPayload(const std::string& aString){
    
    std::memset(&payload,0,kPayloadSize);

 //   theStream.write((char*)&theBlock.payload, kPayloadSize);
//    const std::string &theStr = theStream.str();
    std::memmove(&payload,(void*)aString.c_str(), aString.size());
    
    return Errors::noError;
}

  //---------------------------------------------------


  struct getMode {
    std::ios_base::openmode operator()(CreateFile &aVal) {
      return (std::ios_base::openmode)aVal;
    }
    std::ios_base::openmode operator()(OpenFile &aVal) {
      return (std::ios_base::openmode)aVal;
    }

  };

void BlockIO::changeState(const std::string &aName, AccessMode aMode){
    std::string thePath = Config::getDBPath(aName);
    
    auto theMode=std::visit(getMode(), aMode);
    stream.clear(); // Clear flag just-in-case...
    stream.open(thePath.c_str(), theMode); //force truncate if...
    stream.close();
    stream.open(thePath.c_str(), theMode);
}


//BlockIO *BlockIO::GetInstance(const std::string &aName, AccessMode aMode)
//{
//
//    if(BlockIO_==nullptr){
//        BlockIO_ = new BlockIO(aName,aMode);
//    }else{
//        BlockIO_->changeState(aName, aMode);
//    }
//    return BlockIO_;
//}



  BlockIO::BlockIO(const std::string &aName, AccessMode aMode) {
    std::string thePath = Config::getDBPath(aName);
    
    auto theMode=std::visit(getMode(), aMode);
    stream.clear(); // Clear flag just-in-case...
    stream.open(thePath.c_str(), theMode); //force truncate if...
    stream.close();
    stream.open(thePath.c_str(), theMode);
  }

  // USE: write data a given block (after seek) ---------------------------------------
  StatusResult BlockIO::writeBlock(uint32_t aBlockNum, Block &aBlock) {
      
      // find block at position aBlockNum and write in with the block I was given aBlock

      StatusResult theResult{Errors::noError};
  //    if(aBlockNum > getBlockCount()){return Errors::writeError;}
      aBlock.header.index = aBlockNum;
      stream.seekg(aBlockNum*kBlockSize,std::ios::beg);
      stream.write((char*)&aBlock, sizeof(aBlock));
      if(cache.contains(aBlockNum) && cache.size() > 0){cache.erase(aBlockNum);} // no longer same value in cache
      // could read here - polciy decision
      stream.clear();
      stream << std::flush;
      return StatusResult{Errors::noError};
      
   //   return StatusResult{Errors::writeError};
  }

  StatusResult BlockIO::addBlock(Block &aBlock){
      uint32_t theBlockIndex = getBlockCount();
      aBlock.header.index = theBlockIndex;
      stream.seekg(0,std::ios::end);
      stream.write((char*)&aBlock, sizeof(aBlock));
    
      stream.clear();

      stream << std::flush;

      return StatusResult{Errors::noError};
  }


  // USE: write data a given block (after seek) ---------------------------------------
  StatusResult BlockIO::readBlock(uint32_t aBlockNumber, Block &aBlock) {
      
      //  find block at a blocknumber position and read it

      StatusResult theResult{Errors::noError};
      if(aBlockNumber > getBlockCount()){return Errors::writeError;}
     
      if(cache.contains(aBlockNumber) && cache.size() < 0){
          aBlock = cache.get(aBlockNumber);
          return theResult;
      }else{
          stream.seekg(aBlockNumber*kBlockSize,std::ios::beg);
          stream.read((char*)&aBlock, sizeof(aBlock));
          if(cache.size()){
              cache.evictIf();
              cache.add(aBlockNumber, aBlock);
          }
          stream.clear();
      }
      return theResult;
  }

  // USE: count blocks in  file ---------------------------------------
  uint32_t BlockIO::getBlockCount()  {
      stream.seekg(0,std::ios::end);
      uint32_t theSize = (uint32_t)stream.tellg();
      double temp = (float)theSize/(float)kBlockSize; // blocksize or payload size?
      uint32_t count = std::ceil(temp);
      
      return count;
     //What should this be?
  }

}

