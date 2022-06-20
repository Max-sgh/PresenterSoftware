/*
 *
 * Copyright 2021
 * Maximilian Lange
 *
 */

#include <vector>
#include <string>
#include <iostream>
#include "./CDB.h"
#include "./CDBRow.h"

CDB::CDB(std::string filename, std::string dbName) {
  _filename = filename;
  _dbName = dbName;
}

bool CDB::deleteRow(int id) {
    int num = -1;
    for (int i = 0; i < (int)_rows.size(); i++) {
        if (_rows[i]._fields[0].i == id) {
            num = i;
            break;
        }
    }
    if (num == -1) {
        return false;
    }
    _rows.erase(_rows.begin() + num);
    return true;
}

std::string CDB::getFilename() { return _filename; }

std::string CDB::getdbName() { return _dbName; }

int CDB::getTypesVectorSize() { return static_cast<int>(_types.size()); }

std::string CDB::getName(int i) { return _names[i]; }

int CDB::getRowsVectorSize() { return static_cast<int>(_rows.size()); }

void CDB::setNames(std::vector<std::string> names) { _names = names; }

void CDB::setTypes(std::vector<char> types) { _types = types; }

int CDB::getDBSize() { return _rows.size(); }


bool CDB::changeFieldInRow(int row, std::string fieldname, int value) {
  // check if row exists
  if (!(static_cast<int>(_rows.size()) >= row)) {
    std::cerr << "Row cannot be found <CDB/changeField>" << std::endl;
    return false;
  }

  // get the rowindex for checking if the type is correct
  int rowindex = getIndexByName(fieldname);
  if (rowindex == -1) {
    std::cerr << "fieldname does not exists! <CDB/changeField>" << std::endl;
    return false;
  }

  // check if the type is correct
  if (_types[rowindex] != 'i') {
    std::cerr << "False Datatype! <CDB/changeField>" << std::endl;
    return false;
  }
  _rows[row]._fields[getIndexByName(fieldname)].i = value;
  return true;
}
bool CDB::changeFieldInRow(int row, std::string fieldname, std::string value) {
  // check if row exists
  if (!(static_cast<int>(_rows.size()) >= row)) {
    std::cerr << "Row cannot be found <CDB/changeField>" << std::endl;
    return false;
  }

  // get the rowindex for checking if the type is correct
  int rowindex = getIndexByName(fieldname);
  if (rowindex == -1) {
    std::cerr << "fieldname does not exists! <CDB/changeField>" << std::endl;
    return false;
  }

  // check if the type is correct
  if (_types[rowindex] != 's') {
    std::cerr << "False Datatype! <CDB/changeField>" << std::endl;
    return false;
  }
  _rows[row]._fields[getIndexByName(fieldname)].s = value;
  return true;
}

void CDB::print() {
  for (int k = 0; k < static_cast<int>(_types.size()); k++) {
    std::cout << _types[k] << "-" << _names[k] << "\t";
  }
  std::cout << std::endl;

  for (int i = 0; i < static_cast<int>(_rows.size()); i++) {
    for (int j = 0; j < static_cast<int>(_types.size()); j++) {
      DatabaseRow currentrow = _rows[i];
      struct field f = currentrow._fields[j];
      if (_types[j] == 'i')
        std::cout << f.i << "\t";
      else if (_types[j] == 's')
        std::cout << f.s << "\t";
     }
    std::cout << std::endl;
  }
}

void CDB::setupDatabase
(std::vector<char> types, std::vector<std::string> names) {
  if (types.size() != names.size()) {
    std::cerr << "Fehlerhafte Eingabe <db/setup/size>" << std::endl;
    exit(1);
  }
  for (int i = 0; i <= static_cast<int>(types.size() - 1); i++) {
    _types.push_back(types[i]);
    _names.push_back(names[i]);
  }
}

void CDB::addRow(DatabaseRow* newRow) {
  if (newRow->_fields.size() == 0 || newRow->_fields.size() != _types.size()) {
    std::cerr << "Fehlerhafte Eingabe <db/addRow/size>" << std::endl;
    std::cerr << "Types: " << _types.size() << "; Names: " << _names.size()
      << "; Row: " << newRow->_fields.size() << std::endl;
    exit(1);
  }
  _rows.push_back(*newRow);
}

DatabaseRow CDB::getRowByNumber(int number) {
  DatabaseRow row = _rows[number];
  return row;
}

std::vector<DatabaseRow> CDB::getRowsByValue(std::string name, int value) {
  std::vector<DatabaseRow> vec;
  int n = -1;
  for (int i = 0; i < static_cast<int>(_names.size()); i++) {
      if (_names[i] == name) {
          n = i;
          break;
      }
  }

  if (n == -1)
      return vec;

  for (int i = 0; i < static_cast<int>(_rows.size()); i++) {
    DatabaseRow row = getRowByNumber(i);
    if (row._fields[getIndexByName(name)].i == value){
      vec.push_back(row);
      continue;
    }
  }

  return vec;
}

std::vector<DatabaseRow> CDB::getRowsByValue(std::string name, std::string value) {
  std::vector<DatabaseRow> vec;
  int n = -1;
  for (int i = 0; i < static_cast<int>(_names.size()); i++) {
      if (_names[i] == name) {
          n = i;
          break;
      }
  }

  if (n == -1)
      return vec;

  for (int i = 0; i < static_cast<int>(_rows.size()); i++) {
    DatabaseRow row = getRowByNumber(i);
    if (row._fields[getIndexByName(name)].s.compare(value) == 0){
      vec.push_back(row);
      continue;
    }
  }
  return vec;
}

int CDB::getRowNumber(std::string field, std::string value) {
    int index = getIndexByName(field);
    for (int i = 0; i < (int)_rows.size(); i++) {
        if (_rows[i]._fields[index].s.compare(value) == 0) {
            return i;
        }
    }
    return -1;
}

int CDB::getRowNumber(std::string field, int value) {
    int index = getIndexByName(field);
    for (int i = 0; i < (int)_rows.size(); i++) {
        if (_rows[i]._fields[index].i == value) {
            return i;
        }
    }
    return -1;
}

struct field CDB::getFieldOfRowByName(int rownumber, std::string name) {
  DatabaseRow row = getRowByNumber(rownumber);
  int index = getIndexByName(name);
  return row._fields[index];
}

int CDB::getIndexByName(std::string name) {
  int index = -1;
  for (int i = 0; i < static_cast<int>(_names.size()); i++) {
    if (name.compare(_names[i]) == 0) {
      index = i;
      break;
    }
  }
  if (index == -1) {
    std::cerr << "Spaltenname konnte nicht gefunden werden!" << std::endl;
    // exit(1);
  }
  return index;
}

char CDB::getType(int i) {
  return _types[i];
}

char CDB::getType(std::string name) {
  return _types[getIndexByName(name)];
}
