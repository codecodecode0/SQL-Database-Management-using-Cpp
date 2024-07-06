//
//  CmdProcessor.hpp
//  Database5
//
//  Created by rick gessner on 3/30/23.
//  Copyright © 2018-2023 rick gessner. All rights reserved.
//

#ifndef AppController_hpp
#define AppController_hpp

#include <stdio.h>
#include "Config.hpp"
#include "Errors.hpp"
#include "View.hpp"
#include "CmdConfigs.hpp"
#include "CmdProcessor.hpp"
#include "SqlProcessor.hpp"
#include "RecordProcessor.hpp"

namespace ECE141 {

  class AppController{
    void shutdown() {
      running = false;
    }

  public:
    
    AppController();
    virtual ~AppController();

      //app api...    
    virtual StatusResult  handleInput(std::istream &anInput,
                                      ViewListener aViewer);
    virtual CmdProcessor* recognize(Tokenizer &aTokenizer);
            bool          isRunning() const {return running;}

            OptString     getError(StatusResult &aResult) const;

    bool running;
    AppProcessor* appProcessor;
    DbProcessor* dbProcessor;
    SqlProcessor* sqlProcessor;
    RecordProcessor* recordProcessor;
  };
  
}

#endif /* AppController_hpp */
