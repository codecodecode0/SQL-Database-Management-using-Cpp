//
// Created by SIDDHANT on 23-04-2023.
//

#ifndef SP23DATABASE_SQLPROCESSOR_HPP
#define SP23DATABASE_SQLPROCESSOR_HPP

#include <vector>
#include "CmdProcessor.hpp"
#include "Errors.hpp"
#include "Attribute.hpp"
#include "CmdStatement.cpp"

namespace ECE141 {
  class SqlProcessor : public CmdProcessor {
  public:
    SqlProcessor() = default;

    ~SqlProcessor() override = default;

    StatusResult execute(ViewListener aViewer) override;
    StatusResult extractStatement(Tokenizer &aTokenizer) override;

  protected:
    SqlCmdStatement* cmdStatement;
  };
}


#endif //SP23DATABASE_SQLPROCESSOR_HPP