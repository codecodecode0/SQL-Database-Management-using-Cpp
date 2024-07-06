//
// Created by SIDDHANT on 30-04-2023.
//

#ifndef SP23DATABASE_RECORDPROCESSOR_H
#define SP23DATABASE_RECORDPROCESSOR_H

#include <vector>
#include "CmdProcessor.hpp"
#include "Errors.hpp"
#include "Attribute.hpp"
#include "CmdStatement.hpp"

namespace ECE141 {
  class RecordProcessor : public CmdProcessor {
  public:
    RecordProcessor() = default;
    ~RecordProcessor() override = default;

    StatusResult execute(ViewListener aViewer) override;
    StatusResult extractStatement(Tokenizer &aTokenizer) override;

  protected:
    RecordCmdStatement* cmdStatement;
  };

} // ECE141

#endif //SP23DATABASE_RECORDPROCESSOR_H
