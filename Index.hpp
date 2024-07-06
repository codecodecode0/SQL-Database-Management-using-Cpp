//
// Created by SIDDHANT on 02-06-2023.
//

#ifndef SP23DATABASE_INDEX_HPP
#define SP23DATABASE_INDEX_HPP

#include <cstdint>
#include <utility>
#include <queue>
#include "Storage.hpp"

namespace ECE141 {
  template <typename KeyType, typename ValueType>
  class Index: public Storable {

    uint16_t blockNumber;
      std::string dbName;
      uint16_t next;
    std::vector<uint16_t> allBlocks;
    std::map<KeyType, ValueType> data;

//    uint16_t getBlockNumber() { return blockNumber; }
//    uint16_t getNext() { return next; }
//    void setNext(uint16_t aNext) { next = aNext; }
//    std::queue<uint16_t> getAllBlockNumbers() { return allBlocks; };

    StatusResult encodeAll(std::vector<std::string> &aOutput);
  public:
    Index(std::string aDbName, const Block& aBlock):
      blockNumber(aBlock.header.index), dbName(std::move(aDbName)), next(aBlock.header.next) {}

    std::map<KeyType, ValueType> getData() { return data; }
    bool add(KeyType aKey, ValueType aValue) {
      if (exists(aKey)) return false;
      data[aKey] = aValue;
      return true;
    }
    bool remove(KeyType aKey) {
      data.erase(aKey);
      return true;
    }
    bool exists(KeyType aKey) { return data.count(aKey) != 0; }
    StatusResult deleteIndex();

    StatusResult encode(std::ostream &anOutput) const;
    StatusResult  decode(Storage* aStorage, std::istream &anInput);
    StatusResult  decode(std::istream &anInput);

    bool  initHeader(Block &aBlock) const {
      return false;
    }

    StatusResult writeToStorage(Storage* aStorage, const std::string& aTableName, uint32_t aHash);
    StatusResult deleteIndex(Storage* aStorage);
    static StatusResult storeNewIndexBlock(Storage* aStorage, Block &theNewBlock, const std::string& aTableName, uint32_t aHash);
  };

} // ECE141

#endif //SP23DATABASE_INDEX_HPP
