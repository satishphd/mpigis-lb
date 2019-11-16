#ifndef FILEREAD_H
#define FILEREAD_H

#include<iostream>
#include <fstream>
#include <stdlib.h>
#include<stdio.h>
#include<vector>
#include<list>

#include <geos/geom/Geometry.h>

#include "../filePartition/FileSplits.h"
#include "../parser/WKTParser.h"
#include "../filePartition/config.h"

using namespace std;
using namespace geos::geom;

class FileReader
{
   public:
   vector<Envelope*>* readGridCellFile(char *str);
   
   list<Geometry*>* readSampleFile(char *filename, Config *args);
   
   list<Geometry*>* readWKTFile(char *filename, Config *args);
   
   int readMBRFile(string path, list<Envelope*> *outEnvs);
   
   int readMBRFile(string path, vector<Envelope*> *outEnvs);
   
   map<int, list<Envelope*>* >* readMBRFiles(string path, int numFiles);

};

#endif