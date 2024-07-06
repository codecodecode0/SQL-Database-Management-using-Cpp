//
//  View.hpp
//  PA1
//
//  Created by rick gessner on 3/30/23.
//  Copyright © 2018-2023 rick gessner. All rights reserved.
//


#ifndef View_h
#define View_h


#include <iostream>
#include <functional>
#include <optional>

namespace ECE141 {

  //completely generic view, which you will subclass to show information
  class View {
  public:
    virtual         ~View() {}
    virtual bool    show(std::ostream &aStream)=0;

  };

  using ViewListener = std::function<void(View &aView)>;

  class AppView : public View {
    std::string statement;
  public:
    AppView(std::string aStatement): statement(aStatement) {};

    ~AppView() override = default;

    bool show(std::ostream &aStream) override;

  };

  class TableView: public View {
    std::vector<std::string> headerList;
    // 3d vector so that one cell in the table can have multiple rows
    std::vector<std::vector<std::vector<std::string>>> entries;

    std::vector<int> sizes;

  public:
    TableView(std::vector<std::string> aHeader,
              std::vector<std::vector<std::vector<std::string>>> anEntryVector,
              std::vector<int> aSizes):
      headerList(std::move(aHeader)), entries(std::move(anEntryVector)), sizes(std::move(aSizes)) {};

    ~TableView() override = default;

    bool show(std::ostream &aStream) override;

  };

  class DbView: public View {
    double timeElapsed;
    std::optional<TableView> tableView;
      std::string statement;
  public:
    DbView(double aTimeElapsed, std::optional<TableView> aTableView, std::string aStatement):
      timeElapsed(aTimeElapsed), tableView(std::move(aTableView)), statement(aStatement) {};

    ~DbView() override = default;

    bool show(std::ostream &aStream) override;

  };
}

#endif /* View_h */
