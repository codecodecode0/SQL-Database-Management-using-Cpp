//
// Created by SIDDHANT on 15-04-2023.
//

#ifndef SP23DATABASE_CMDPROCESSOR_HPP
#define SP23DATABASE_CMDPROCESSOR_HPP

#include "Errors.hpp"
#include "View.hpp"
#include "Tokenizer.hpp"
#include "CmdConfigs.hpp"
#include "FolderReader.hpp"

#include "BlockIO.hpp"
#include "CmdStatement.hpp"

namespace ECE141 {
  class CmdProcessor {
  public:
    virtual         ~CmdProcessor() = default;

    virtual StatusResult execute(ViewListener aViewer);
    virtual StatusResult extractStatement(Tokenizer &aTokenizer);
  };

  class AppProcessor: public CmdProcessor {
  public:
    AppProcessor() = default;
    ~AppProcessor() override = default;

    StatusResult execute(ViewListener aViewer);
    StatusResult extractStatement(Tokenizer &aTokenizer);

    AppCmdStatement* cmdStatement;
  };

  class DbProcessor: public CmdProcessor {
  public:
    DbProcessor() = default;
    ~DbProcessor() override = default;

    // Design questions:
    // 1. Can we make Tokenizer a variable of the class Cmd Processor?
    // 2. Can we make separate classes for creating, updating, etc.
    //    databases/tables that will create the stuff on the disk and
    //    this class will handle view and parsing?

    StatusResult execute(ViewListener aViewer) override;
    StatusResult extractStatement(Tokenizer &aTokenizer);

  protected:
    DbCmdStatement* cmdStatement;
  };
}

#endif //SP23DATABASE_CMDPROCESSOR_HPP
