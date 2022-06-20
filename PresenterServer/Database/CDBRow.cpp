/*
 *
 * Copyright 2021
 * Maximilian Lange
 *
 */

#include <string>
#include <iostream>
#include "./CDBRow.h"

DatabaseRow::DatabaseRow() {
}

void DatabaseRow::createField(int i, std::string s) {
  struct field f;
  if (i != 0) {
    if (!s.empty()) {
      std::cerr << "Falsche Eingabe! <dbfield/constructor>" << std::endl;
    }
    f.i = i;
  }
  if (!s.empty()) {
    f.s = s;
  }

  _fields.push_back(f);
}
