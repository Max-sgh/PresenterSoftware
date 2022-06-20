/*
 *
 * Copyright 2021
 * Maximilian Lange
 * 
 */
#include <zlib.h>
#include <stdio.h>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include "./CDBFileHandler.h"
#include "./CDB.h"
#include "./CDBRow.h"

CDBFilehandler::CDBFilehandler(std::string filename) {
  _filename = filename;
  _path = "./";
}

CDBFilehandler::CDBFilehandler(std::string filename, std::string path) {
  _filename = filename;
  _path = path;
}

CDB* CDBFilehandler::openDB() {
  std::ifstream infile(_filename);
  std::string line;
  std::string dbname;
  std::vector<char> types;
  std::vector<std::string> names;

  if (!infile.is_open()) {
    std::cerr << "Error opening File " << std::endl;
    return nullptr;
  }

  std::getline(infile, line);
  if (line.compare("#CDB-HEADER") == 0) {
    std::cerr << "False Filetype" << std::endl;
    return nullptr;
  }

  if (line.rfind(":dbname", 0) == 0) {
    int pos = line.find(':', 2);
    dbname = line.substr(pos + 1);
  }

  CDB* db = new CDB(_filename, dbname);

  while (true) {
    std::getline(infile, line);
    if (infile.eof()) { break; }

    if (line.rfind(":types", 0) == 0) {
      line = line.substr(line.find(':', 2));
      for (int i = 0; i < static_cast<int>(line.length() - 1); i++) {
        int pos = line.find(':');
        line.erase(pos, 1);
      }
      for (int i = 0; i < static_cast<int>(line.length()); i++) {
        types.push_back(line[i]);
      }
      db->setTypes(types);
    }

    if (line.rfind(":names", 0) == 0) {
      line = line.substr(line.find(':', 2));
      names = parseLine(line);
      db->setNames(names);
    }

    if (line.rfind(":row", 0) == 0) {
      line = line.substr(line.find(':', 2));
      DatabaseRow* row = new DatabaseRow();
      std::vector<std::string> v = parseLine(line);
      for (int i = 0; i < static_cast<int>(v.size()); i++) {
        if (types[i] == 'i') {
          row->createField(atoi(v[i].c_str()), "");
        } else {
          row->createField(0, v[i]);
        }
      }
      db->addRow(row);
    }
  }
  return db;
}

std::vector<std::string> CDBFilehandler::parseLine(std::string line) {
  std::vector<std::string> v;
  int num = 0;
  for (int i = 0; i < static_cast<int>(line.length() -1); i++) {
    if (line[i] == ':') {
      num++;
    }
  }
  for (int i = 0; i < num; i++) {
    int pos = line.find(':', 2);
    std::string s = line.substr(0, pos);
    s.erase(0, 1);
    v.push_back(s);
    line.erase(0, pos);
  }
  return v;
}

bool CDBFilehandler::saveDB(CDB* db) {
  if (!checkFile()) {
    std::cout << "cannot write" << std::endl;
    return false;
  }

  std::string line = "#CDB-Header\n";
  line += ":dbname:" + db->getdbName() + "\n";

  // Types
  line += ":types";
  for (int i = 0; i < db->getTypesVectorSize(); i++) {
    line += ":";
    line += db->getType(i);
  }
  line += "\n";

  // Names
  line += ":names";
  for (int i = 0; i < db->getTypesVectorSize(); i++) {
    line += ":";
    line += db->getName(i);
  }
  line += "\n";

  // Rows
  for (int i = 0; i < db->getRowsVectorSize(); i++) {
    DatabaseRow row = db->getRowByNumber(i);
    line += ":row";
    for (int j = 0; j < static_cast<int>(row._fields.size()); j++) {
      line += ":";
      struct field f = row._fields[j];
      if (db->getType(j) == 's') {
        line += f.s;
      } else {
        line += std::to_string(f.i);
      }
    }
    line += "\n";
  }

  writeLineInFile(line);

  return true;
}

void CDBFilehandler::writeLineInFile(std::string line) {
  // gzFile output = gzopen(_filename.c_str(), "wb");
  std::ofstream outstream(_filename, std::ios::out);

  // gzwrite(output, line.c_str(), sizeof(line.c_str()));
  outstream << line;

  // gzclose(output);
  outstream.close();
  return;
}

bool CDBFilehandler::checkFile() {
  std::ofstream outstream(_filename, std::ios::out);
  if (!outstream.is_open()) {
    outstream.close();
    return false;
  } else {
    outstream.close();
    return true;
  }
}
