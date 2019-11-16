#include "filePartition/FileSplits.h"

list<string>* FileSplits :: getContents() const
{
  return contents;
}

void FileSplits :: addGeom(string geom) 
{
  contents->push_back(geom);
}


int FileSplits :: numLines()
{
  if(contents!=nullptr)
  {
    return contents->size();
  }
  else
  {
    return 0;
  }
}

void FileSplits :: printK(int k)
{
 int counter = 0;
 list<string>::const_iterator i;

 for(i = contents->begin(); i!= contents->end() && counter < k; i++) {
    cout<<*i<<endl;
    counter++;
  }
}

void FileSplits :: writeV2(char *data)
{
   std::stringstream ss(data);
   std::string to;
 size_t total = 0;
  if (data != NULL)
  {
    while(std::getline(ss,to,'\n')){
      //cout << to <<endl;
      contents->push_back(to);
      total++;
    }
  }

  cout<<" FileSplits total "<<total<<endl;

}


void FileSplits :: write(char *data){
    // to avoid modifying original string
    // first duplicate the original string and return a char pointer then free the memory
    char *dup = strdup(data);
    
    char *token = std::strtok(dup, "\n");
    while(token != NULL){
        contents->push_back(string(token));
        // the call is treated as a subsequent calls to strtok:
        // the function continues from where it left in previous invocation
        token = std::strtok(NULL, "\n");
    }
    free(dup);
}
