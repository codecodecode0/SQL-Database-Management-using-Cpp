//
// Created by SIDDHANT on 15-04-2023.
//


#include <iostream>
#include <vector>
#include "View.hpp"

namespace ECE141 {
  static const std::string CellCorner = "+";
  static const std::string TableColumnSep = "|";
  static const std::string TableRowSep = "-";

  static std::string buildCell(std::string aData, int aLength) {
    std::string theStr = " ";
    theStr += aData;
    for(size_t i = aData.size(); (int)i < aLength - 1; i++) {
      theStr += " ";
    }
    return theStr;
  }

  static std::string buildRowSep(std::vector<int> aSizes) {
    std::string theStr;
    theStr += CellCorner;
    for (int theSize : aSizes) {
      for (int j = 0; j < theSize; j++) {
        theStr += TableRowSep;
      }
      theStr += CellCorner;
    }
    return theStr;
  }

  static std::string buildSingleRow(std::vector<std::string> aSingleRow, std::vector<int> aSizes) {
    std::string theStr;
    theStr += TableColumnSep;
    for (size_t i = 0 ; i < aSizes.size() ; i++) {
      theStr += buildCell(aSingleRow[i], aSizes[i]);
      theStr += TableColumnSep;
    }
    return theStr;
  }

  static std::vector<std::string> buildRow(std::vector<std::vector<std::string>> aRow, std::vector<int> aSizes) {
    std::vector<std::string> theRowStr;

    int theNumCols = 0;
    for (const std::vector<std::string>& theCell : aRow) {
      theNumCols = std::max(theNumCols, (int) theCell.size());
    }

    for (int j = 0; j < theNumCols; j++) {
      std::vector<std::string> theSingleRow;
      theSingleRow.reserve(aRow.size());
      for (std::vector<std::string> theEntry : aRow) {
        theSingleRow.push_back(theEntry[j]);
      }
      theRowStr.push_back(buildSingleRow(theSingleRow, aSizes));
    }
    return theRowStr;
  }


  bool TableView::show(std::ostream &aStream) {
    std::string theRowSep = buildRowSep(sizes);
    aStream << theRowSep << std::endl;
    aStream << buildSingleRow(headerList, sizes) << std::endl;
    aStream << theRowSep << std::endl;
    for (const std::vector<std::vector<std::string>> &theRowEntries : entries) {
      for (const std::string& theSingleRow : buildRow(theRowEntries, sizes)) {
        aStream << theSingleRow << std::endl;
      }
      aStream << theRowSep << std::endl;
    }
    return true;
  }

  bool DbView::show(std::ostream &aStream) {
    if (tableView.has_value()) {
      tableView.value().show(aStream);
    }
    aStream << statement + " (" + std::to_string(timeElapsed) + " sec)" <<std::endl;
    return true;
  }

  bool AppView::show(std::ostream &aStream) {
    aStream << statement << std::endl;
    return true;
  }
}

// "Query OK, " + std::to_string(rowsAffected) + ((rowsAffected == 1) ? " row" : " rows" + " affected";

