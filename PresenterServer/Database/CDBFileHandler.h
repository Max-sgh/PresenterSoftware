/*
 *
 * Copyright 2021
 * Maximilian Lange
 *
 */
#ifndef CDBFILEHANDLER_H_
#define CDBFILEHANDLER_H_

#include <zlib.h>
#include <string>
#include <fstream>
#include <vector>
#include "./CDB.h"
#include "./CDBRow.h"

class CDBFilehandler {
 public:
  explicit CDBFilehandler(std::string filename);
  CDBFilehandler(std::string filename, std::string path);
  bool saveDB(CDB* db);
  CDB* openDB();
 private:
  std::string _filename;
  void writeLineInFile(std::string line);
  bool checkFile();
  std::string _path;
  std::vector<std::string> parseLine(std::string line);
};
#endif  // CDBFILEHANDLER_H_
