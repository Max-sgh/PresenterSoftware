/*
 *
 * Copyright 2021
 * Maximilian Lange
 *
 */
#ifndef CDB_H_
#define CDB_H_

#include <string>
#include <vector>
#include "./CDBRow.h"

class CDB {
 public:
  explicit CDB(std::string filename, std::string dbName);
  void setupDatabase(std::vector<char> types, std::vector<std::string> names);
  void addRow(DatabaseRow* newRow);
  void print();

  DatabaseRow getRowByNumber(int number);
  struct field getFieldOfRowByName(int rownumber, std::string name);
  int getIndexByName(std::string name);
  char getType(int i);
  char getType(std::string name);
  std::string getFilename();
  std::string getdbName();
  int getTypesVectorSize();
  int getRowsVectorSize();
  std::string getName(int i);
  void setNames(std::vector<std::string> names);
  void setTypes(std::vector<char> types);
  bool changeFieldInRow(int row, std::string fieldname, int value);
  bool changeFieldInRow(int row, std::string fieldname, std::string value);
  std::vector<DatabaseRow> getRowsByValue(std::string name, std::string value);
  std::vector<DatabaseRow> getRowsByValue(std::string name, int value);
  int getRowNumber(std::string field, std::string value);
  int getRowNumber(std::string field, int value);
  int getNumRows() { return _rows.size(); }
  bool deleteRow(int id);
  DatabaseRow getLastRow() { return _rows.back(); }
  int getDBSize();

 private:
  std::string _filename;
  std::string _dbName;
  std::vector<DatabaseRow> _rows;
  std::vector<char> _types;  // s = string; i = int
  std::vector<std::string> _names;
};
#endif  // CDB_H_
