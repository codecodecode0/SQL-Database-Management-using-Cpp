//
// Created by SIDDHANT on 08-04-2023.
//

#ifndef SP23DATABASE_CMDCONFIGS_HPP
#define SP23DATABASE_CMDCONFIGS_HPP

#include <unordered_set>
#include "View.hpp"
#include "keywords.hpp"
#include "Config.hpp"

namespace ECE141 {
  static std::unordered_map<Keywords, AppView> appCommands = {
    {Keywords::version_kw, AppView("Version: " + Config::getVersion())},
    {Keywords::about_kw, AppView("Authors: " + Config::getMembers())},
    {Keywords::quit_kw, AppView(Config::getExitMessage())},
      {Keywords::help_kw, AppView(Config::getHelp())}
  };

//  static std::unordered_set<Keywords> dbCommands = {
//      Keywords::create_kw, Keywords::show_kw, Keywords::use_kw, Keywords::drop_kw, Keywords::dump_kw
//  };

//  static std::unordered_set<Keywords> sqlCommands = {
//      Keywords::create_kw, Keywords::show_kw, Keywords::use_kw, Keywords::drop_kw, Keywords::dump_kw
//  };

  static std::unordered_map<Keywords, DataTypes> attributeDataTypes = {
      {Keywords::integer_kw , DataTypes::int_type} ,
      {Keywords::float_kw   , DataTypes::float_type} ,
      {Keywords::boolean_kw , DataTypes::bool_type},
      {Keywords::varchar_kw , DataTypes::varchar_type},
      {Keywords::datetime_kw, DataTypes::datetime_type},
      {Keywords::time_stamp_kw, DataTypes::datetime_type}
  };
}

#endif //SP23DATABASE_CMDCONFIGS_HPP
