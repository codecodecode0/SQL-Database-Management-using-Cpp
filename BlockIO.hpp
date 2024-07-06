//
//  BlockIO.hpp
//  PA2
//
//  Created by rick gessner on 2/27/23.
//

#ifndef BlockIO_hpp
#define BlockIO_hpp

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <variant>
#include <functional>
#include <cmath>
#include "Errors.hpp"
#include "LRUCache.hpp"

namespace ECE141 {

  enum class BlockType {
    data_block='D',
    free_block='F',
    schema_block='S',
    index_block='I',
    //other types?
    unknown_block='U',
  };


  //a small header that describes the block...
  struct BlockHeader {

    BlockHeader(BlockType aType=BlockType::data_block)
      : type(static_cast<char>(aType)) {
          for(size_t i=0;i<128;i++){ // no garbage in filename for length
                              name[i] = '\0';
                          }
      }

    BlockHeader(const BlockHeader &aCopy) {
      *this=aCopy;
    }

    void empty() {
      type=static_cast<char>(BlockType::free_block);
    }
    
    BlockHeader& operator=(const BlockHeader &aCopy) {
      type=aCopy.type;
      return *this;
    }

      char      type;     //char version of block type
      uint16_t index;
      uint16_t indexIndex;
      uint16_t next;
      uint32_t hash;
      
      char name[128];
    //other properties?
  };

  const size_t kBlockSize = 1024;
  const size_t kPayloadSize = kBlockSize - sizeof(BlockHeader);

  //block .................
  class Block {
  public:
    Block(BlockType aType=BlockType::data_block);
    Block(const Block &aCopy);

    Block& operator=(const Block &aCopy);

    StatusResult write(std::ostream &aStream);
      
      StatusResult copyToPayload(const std::string& aString);

    BlockHeader   header;
    char          payload[kPayloadSize];
  };

  //------------------------------

   struct CreateFile {
    operator std::ios_base::openmode() {
      return std::ios::binary | std::ios::in | std::ios::out | std::ios::trunc;
    }
  }; //tags for db-open modes...
  struct OpenFile {
    operator std::ios_base::openmode() {
      return std::ios::binary | std::ios::in | std::ios::out;
    }
  };

  using AccessMode=std::variant<CreateFile, OpenFile>;

  class BlockIO {
  public:

    BlockIO(const std::string &aName, AccessMode aMode);

    uint32_t              getBlockCount();

    virtual StatusResult  readBlock(uint32_t aBlockNumber, Block &aBlock);
    virtual StatusResult  writeBlock(uint32_t aBlockNumber, Block &aBlock);
      
      StatusResult addBlock(Block &aBlock);

      
  //    static BlockIO* BlockIO_;
      
   //   static BlockIO* GetInstance(const std::string &aName, AccessMode aMode);
      
  protected:
      
      
      void changeState(const std::string &aName, AccessMode aMode);
      
      
      
      
      std::fstream stream;
      
      LRUCache<uint32_t, Block> cache;
      
  };

}


#endif /* BlockIO_hpp */
