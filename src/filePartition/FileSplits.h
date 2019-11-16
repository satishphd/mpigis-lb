#ifndef __FILESPLIT_H_INCLUDED__
#define __FILESPLIT_H_INCLUDED__


#include <list>
#include <cstring>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

class FileSplits 
{
	int splitSize; // in MB
	list<string> *contents;
			
	public:
	FileSplits()
	{
	   contents = new list<string>();
	}
	
	~FileSplits() 
	{
	  contents->clear();
	  delete contents;
	}
	
	void setContents(list<string> *contents)
	{
	   this->contents = contents;
	}
	
	void addGeom(string geom); 
	
	void clear()
	{
	  contents->clear();
	}
	
	list<string>* getContents() const;
	int numLines();
	// writes the contents to the vector
	void write(char *buffer);
	void writeV2(char *buffer);
	
	void printK(int k);
};

#endif 
