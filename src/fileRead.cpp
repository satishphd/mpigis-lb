#include "geom_util/fileRead.h"

// returns a map of <cellid, MBRs list> for all files
map<int, list<Envelope*>* >* FileReader :: readMBRFiles(string path, int numFiles)
{
  map<int, list<Envelope*>* > *cellMBRs = new map<int, list<Envelope*>* >();
  for(int i = 0; i<numFiles; i++)
  {
   string input_mbr_file = path + to_string(i);
   list<Envelope*> *envs = new list<Envelope*>();
   
   cout<<"File: "<<input_mbr_file<<"# ->"<<readMBRFile(input_mbr_file, envs)<<endl;
   
   if(envs->size() > 0)
      cellMBRs->insert(pair<int, list<Envelope*>* >(i, envs));
  }
  
  return cellMBRs;
}

int FileReader :: readMBRFile(string path, list<Envelope*> *envs)
{
  int recordsRead = 0;
  
  ifstream myfile(path);
  double x1, y1, x2, y2;
  if (myfile.is_open())
  {
    	std::string line;
	
		while (std::getline(myfile, line))
		{
           stringstream env(line); 
           if (env >> x1 >> y1 >> x2 >> y2)
           {
             // Envelope (double x1, double x2, double y1, double y2)
             Envelope *env = new Envelope(x1, x2, y1, y2);
             envs->push_back(env);
             
             recordsRead++;
           }
        }
        myfile.close();
  }

  return recordsRead;
}

int FileReader :: readMBRFile(string path, vector<Envelope*> *envs)
{
  int recordsRead = 0;
  
  ifstream myfile(path);
  double x1, y1, x2, y2;
  if (myfile.is_open())
  {
    	std::string line;
	
		while (std::getline(myfile, line))
		{
           stringstream env(line); 
           if (env >> x1 >> y1 >> x2 >> y2)
           {
             // Envelope (double x1, double x2, double y1, double y2)
             Envelope *env = new Envelope(x1, x2, y1, y2);
             envs->push_back(env);
             
             recordsRead++;
           }
        }
        myfile.close();
  }

  return recordsRead;
}

list<Geometry*>* FileReader :: readWKTFile(char *filename, Config *args)
{
	FileSplits *layer = new FileSplits();
    
    ifstream in_stream;
    string line;
    
    in_stream.open(filename); 
    if(in_stream.is_open()) {
	    for( string line; getline( in_stream, line ); )
    	{
      		layer->addGeom(line);
    	}
	    if(layer->numLines() == 0)
	      return NULL;
		//cout<<"P"<<args->rank<<" Number of lines in layer2 "<<layer2->numLines()<<endl;
	
		Parser *parser = new WKTParser();
    
    	list<Geometry*> *geoms = parser->parseGeoms(*layer);
    	in_stream.close();
    	return geoms;
    }
    else
	    return NULL;
    
}

list<Geometry*>* FileReader :: readSampleFile(char *filename, Config *args)
{
	FileSplits *layer = new FileSplits();
    
    ifstream in_stream;
    string line;
    
    in_stream.open(filename); 
    if(in_stream.is_open()) {
	    for( string line; getline( in_stream, line ); )
    	{
      		layer->addGeom(line);
    	}
	
		//cout<<"P"<<args->rank<<" Number of lines in layer2 "<<layer2->numLines()<<endl;
	
		Parser *parser = new WKTParser();
    
    	list<Geometry*> *geoms = parser->parse(*layer);
    	
    	return geoms;
    }
    else
	    return NULL;
    
}

//char str[80] = {"/Users/Satish/Desktop/MPI_GridCells.txt"};
vector<Envelope*>* FileReader :: readGridCellFile(char *str)
{
  FILE * fp;

  fp = fopen (str,"r");
  
  if (fp == NULL)
  {
    printf("Cannot open the file\n");
    exit(1);
  }
  else
  {
   // printf("Opened the file \n");
  }
  
  double x1, x2, y1, y2;
  int count = 0;
  
  vector<Envelope*>* cells = new vector<Envelope*>();
    
  //printf("%d \n",fscanf(fp, "%f,%f,%f,%f", &x1, &x2, &y1, &y2));
  while(fscanf(fp, "%lf,%lf,%lf,%lf", &x1, &x2, &y1, &y2) == 4) {
     if(ferror(fp)) {
        printf("Read error \n");
        fclose(fp);
        exit(1);;  /* read error */
     }
     if(feof(fp)) {
        printf("Co-ordinates not found \n");
        fclose(fp);
        exit(1); /* co-ords not found */
     }
     fscanf(fp, "%*[^\n]");
     Envelope *env = new Envelope(x1, x2, y1, y2); 
     	
     //	cout<<env->toString()<<endl;
     
     cells->push_back(env);
     count++;
  }
  
  fclose(fp);
  //printf ("I have read: %d lines \n", count);
  
  return cells;

}