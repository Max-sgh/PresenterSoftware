/*
 * 
 * Copyright 2021
 * Maximilian Lange
 * 
 */
#ifndef CDBROW_H_
#define CDBROW_H_

#include <vector>
#include <string>

struct field {
  int i;
  std::string s;
};

class DatabaseRow{
 public:
  DatabaseRow();
  std::vector<struct field> _fields;
  void createField(int i, std::string s);
};

#endif  // CDBROW_H_
