//
//  compare.hpp
//  Created by rick gessner on 4/30/21.
//

#ifndef compare_h
#define compare_h

#include <sstream>
  
  //Equal ------------------------------------------

  template <typename T>
  bool isEqual(const T &arg1, const T &arg2) {
    return arg1==arg2;
  }

  bool isEqual(const std::string &aStr1, const std::string &aStr2) {
    return aStr1==aStr2;
  }

  template <typename T1, typename T2>
  bool isEqual(const T1 &arg1, const T2 &arg2) {
    return static_cast<T1>(arg2)==arg1;
  }

  template <typename T1>
  bool isEqual(const T1 &arg1, const std::string &aStr) {
    std::stringstream temp;
    temp << arg1;
    std::string theStr=temp.str();
    bool theResult=theStr==aStr;
    return theResult;
  }

  template <>
  bool isEqual(const int &arg1, const std::string &aStr) {
    return std::stoi(aStr) == arg1;
  }

  template <>
  bool isEqual(const float &arg1, const std::string &aStr) {
    return std::stof(aStr) == arg1;
  }

  bool isEqual(const std::string &aStr, const bool &arg2) {
    return isEqual(arg2,aStr);
  }

  bool isEqual(const std::string &aStr, const int &arg2) {
    return isEqual(arg2,aStr);
  }

  bool isEqual(const std::string &aStr, const float &arg2) {
    return isEqual(arg2,aStr);
  }

// Less than ------------------------------------------
  template <typename T>
  bool isLess(const T &arg1, const T &arg2) {
    return arg1<arg2;
  }

  bool isLess(const std::string &aStr1, const std::string &aStr2) {
    return aStr1<aStr2;
  }

  template <typename T1, typename T2>
  bool isLess(const T1 &arg1, const T2 &arg2) {
    return static_cast<T1>(arg2)>arg1;
  }

  template <typename T1>
  bool isLess(const T1 &arg1, const std::string &aStr) {
    std::stringstream temp;
    temp << arg1;
    std::string theStr=temp.str();
    bool theResult=theStr<aStr;
    return theResult;
  }

  template <>
  bool isLess(const int &arg1, const std::string &aStr) {
    return std::stoi(aStr) > arg1;
  }

  template <>
  bool isLess(const float &arg1, const std::string &aStr) {
    return std::stof(aStr) > arg1;
  }

  bool isLess(const std::string &aStr, const bool &arg2) {
    return isLess(arg2,aStr);
  }

  bool isLess(const std::string &aStr, const int &arg2) {
    return isLess(arg2,aStr);
  }

  bool isLess(const std::string &aStr, const float &arg2) {
    return isLess(arg2,aStr);
  }


  // Greater than ------------------------------------------
  template <typename T>
  bool isGreater(const T &arg1, const T &arg2) {
    return arg1>arg2;
  }

  bool isGreater(const std::string &aStr1, const std::string &aStr2) {
    return aStr1>aStr2;
  }

  template <typename T1, typename T2>
  bool isGreater(const T1 &arg1, const T2 &arg2) {
    return static_cast<T1>(arg2)<arg1;
  }

  template <typename T1>
  bool isGreater(const T1 &arg1, const std::string &aStr) {
    std::stringstream temp;
    temp << arg1;
    std::string theStr=temp.str();
    bool theResult=theStr>aStr;
    return theResult;
  }

  template <>
  bool isGreater(const int &arg1, const std::string &aStr) {
    return std::stoi(aStr) < arg1;
  }

  template <>
  bool isGreater(const float &arg1, const std::string &aStr) {
    return std::stof(aStr) < arg1;
  }

  bool isGreater(const std::string &aStr, const bool &arg2) {
    return isGreater(arg2,aStr);
  }

  bool isGreater(const std::string &aStr, const int &arg2) {
    return isGreater(arg2,aStr);
  }

  bool isGreater(const std::string &aStr, const float &arg2) {
    return isGreater(arg2,aStr);
  }

  //--------- feel free to add more operators... ------


#endif /* compare_h */
