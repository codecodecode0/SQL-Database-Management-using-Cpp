//
//  FolderReader.hpp
//  Database5
//
//  Created by rick gessner on 4/4/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef FolderReader_h
#define FolderReader_h

#include <string>
#include <filesystem>
#include <fstream>

namespace fs =  std::filesystem;

namespace ECE141 {
  
  using FileVisitor = std::function<bool(const std::string&)>;

  class FolderReader {
    static bool checkExtension(const std::string &aPath, const std::string &anExt) {
      if (!anExt.empty()) {
        int idx = (int)(aPath.rfind('.'));
        if (idx != -1) {
          return aPath.substr(idx + 1) == ((int)anExt.find('.') != -1 ? anExt.substr(1) : anExt);
        }
      }
      return false;
    }

  public:
            FolderReader(const char *aPath) : path(aPath) {}
    virtual ~FolderReader() {}
    
    virtual bool exists(const std::string &aFilename) {
      std::ifstream theStream(aFilename);
      return !theStream ? false : true;
    }
    
    virtual void each(const std::string &anExt,
                      const FileVisitor &aVisitor) const {
      for (auto& thePath : fs::directory_iterator(path)) {
          std::string thePathStr = thePath.path().string();
       
        if (checkExtension(thePathStr, anExt)) {
          aVisitor(thePathStr);
        }
      }
    };
      
      std::string parseDbNameFromPath(std::string aPath) {
          std::string theStr = aPath;
          size_t thePos = theStr.find('/');
//          size_t thePos = theStr.find('\\');
          while(thePos != std::string::npos) {
              theStr = theStr.substr(thePos+1, theStr.length() - thePos - 1);
              thePos = theStr.find('/');
//              thePos = theStr.find('\\');
          }

          thePos = theStr.find('.');
          if(thePos != std::string::npos) {
              theStr = theStr.substr(0, thePos);
          }
          return theStr;
      }
    
    std::string path;
  };
  
}

#endif /* FolderReader_h */
