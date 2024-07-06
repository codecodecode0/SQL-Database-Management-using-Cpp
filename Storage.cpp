//
//  Storage.cpp
//  PA2
//
//  Created by rick gessner on 2/27/23.
//


#include <sstream>
#include <cmath>
#include <cstdlib>
#include <optional>
#include <cstring>
#include <iostream>
#include "Storage.hpp"
#include "Config.hpp"

namespace ECE141 {


Storage* Storage::Storage_ = nullptr;

  // USE: ctor ---------------------------------------

  Storage::Storage(const std::string &aName, AccessMode aMode)
    : BlockIO(aName, aMode) {
        
     //   BlockIO::GetInstance(aName, aMode);
    }

  // USE: dtor ---------------------------------------
  Storage::~Storage() {
  }

  bool Storage::each(const BlockVisitor &aVisitor) {
      bool theStatus{true};
      
      size_t theCount = getBlockCount();
      
      Block theBlock;
      for(size_t i=0;i<theCount;i++){
          readBlock((int)i, theBlock);
          theStatus = aVisitor(theBlock,(int)i);
          writeBlock((int)i, theBlock);
      }
      
      
    return theStatus;
  }
uint32_t  Storage::getFreeBlock(){
    size_t theCount = getBlockCount();
    
    Block theBlock;
    for(size_t i=0;i<theCount;i++){
        readBlock((int)i, theBlock);
        if(theBlock.header.type == (char)BlockType::free_block){return (int)i;}
    }
    return (int)(theCount);
    
}
 
StatusResult Storage::markBlockAsFree(uint32_t aPos){
    
    if(aPos > getBlockCount()){return Errors::readError;}
    
    Block theBlock;
    readBlock(aPos, theBlock);
    theBlock.header.type = (char)(BlockType::free_block);
    writeBlock(aPos, theBlock);
    
    return Errors::noError;
    
}
StatusResult save(const Storable &aStorable, int32_t aStartPos=kNewBlock){
    
    std::stringstream theStream;
    aStorable.encode(theStream);
    
    
    return Errors::noError;
}

Storage *Storage::GetInstance(const std::string &aName, AccessMode aMode){
    if(Storage_ == nullptr){
        Storage_ = new Storage(aName,aMode);
    }else{
        Storage_->changeState(aName, aMode);
    }
    return Storage_;
}





}

