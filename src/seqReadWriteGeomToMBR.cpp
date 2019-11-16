#include<iostream>
#include<fstream>
#include<sstream>
#include<stdio.h>
#include<string>
#include<map>

#include<list>
#include <stdlib.h>
#include <sys/time.h>
#include <geos/io/WKTReader.h>

#include<cstring>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/Geometry.h>

#include "geom_util/fileRead.h"
// #include <geos/geom/Coordinate.h>
// #include <geos/geom/Point.h>

using namespace std;
using namespace geos::geom;

//  string tag;
//     long nodeId;
//     double longi, lat;
typedef struct MBR
{
   double minx;
   double miny;
   double maxx;
   double maxy;
}MBR;

//int readMBRFile(string path, list<Envelope*> *outEnvs);
//map<int, list<Envelope*>* >* readMBRFiles(string path, int numFiles);
int writeMBRFiles(string input_folder, string output_folder, int numFiles);

Geometry* parseString(string &p) 
{
  geos::io::WKTReader wktreader;
  Geometry *geom = NULL;
  
  try
  {
    geom = wktreader.read(p);

  }
  catch(exception &e)
  {
    cout<< e.what() <<endl;
  }
  return geom;
}

string extract(string &str) 
{ 
	char *cstr = new char [str.length()+1];
	  
    std::strcpy (cstr, str.c_str());
    vector<string*> tokens;
    // cstr now contains a c-string copy of str

    char *p = std::strtok (cstr,"\t");
    
    while (p!=0)
    {
      //std::cout << p << '\n';
      string *token = new string(p);
      tokens.push_back(token);
      
      p = std::strtok(NULL,"\t");
    }
    
    delete[] cstr;
    
    string empty = "ERROR";

    if(tokens.empty() == false && tokens.size()<3)
       return empty;
    else 
       return *tokens[1];
}

int writeMBRFiles(string input_folder, string output_folder, int numFiles)
{
  
  for(int i = 0; i<numFiles; i++)
  {
     string input_path = input_folder + to_string(i);
     
     string output_path = output_folder + to_string(i);
     
     //cout<<input_path<<" "<<output_path<<endl;
     
     FILE *ofp = fopen(output_path.c_str(), "w");
   
  	 ifstream myfile (input_path);
   
	 Geometry *geom;
    
	 if (myfile.is_open())
  	 {
    	std::string line;
	
		while (std::getline(myfile, line))
		{
			 geom = parseString(line);  

			 if(geom != NULL) {
				 const Envelope *env = geom->getEnvelopeInternal();  
				 MBR m;
				 m.minx = env->getMinX(); 
				 m.miny = env->getMinY();  		 
				 m.maxx = env->getMaxX(); 
				 m.maxy	= env->getMaxY();  
			     
			     fprintf(ofp, "%f %f %f %f\n", m.minx, m.miny, m.maxx, m.maxy);
     			 //fwrite(&m, sizeof(MBR), 1, ofp);
     			 //printf("a");
     		  }
		} 
   	  }
    
     fclose(ofp);
     myfile.close();
   } // end for
}

// compile: mpicxx -std=c++11 -o writeMBR seqReadWriteGeomToMBR.cpp -L/usr/local/lib -lgeos -lm
int main(int argc, char **argv)
{ 
  // if(argc<3) 
//   {
//     cout<<"Enter file1 and file2"<<endl;
//     return 1;
//   }
  
  // /home/satish/mpigis/indexed_data/cemetery/64Parts
  //char* input_folder = argv[1];
  
  //string input_folder = "/home/satish/mpigis/indexed_data/cemetery/64Parts/";
  string input_folder = "/home/satish/mpigis/indexed_data/sports/8192Parts/";
  //string input_folder = "/home/satish/mpigis/indexed_data/lakes/4096Parts/";
  //  /home/satish/mpigis/indexed_data/cemetery/index/64Parts
  //FILE *ofp = fopen(argv[2],"w");
  
  string mbr_folder = "/home/satish/mpigis/indexed_data/sports/index/8192Parts/";
  //string mbr_folder = "/home/satish/mpigis/indexed_data/lakes/index/4096Parts/";
  
  int mbrFiles = 8192;
  writeMBRFiles(input_folder, mbr_folder, mbrFiles); 
//  FileReader mbrReader;
  
  //map<int, list<Envelope*>* >* mbrsByCell = mbrReader.readMBRFiles(mbr_folder, 64);
  
  return 0;
}


//int writePointNodeData_InBinary()
int main1(int argc, char **argv)
{
    //char* input_filename = "/home/satish/data/allnodes_small/allnodesSmall.txt";
    char* input_filename = "/home/satish/data/lakes/lakes";
    
    //char* input_filename = "/home/satish/data/lakes/lakes";
    
    FILE *ofp = fopen("/home/satish/data/binary_format/lakesMBR.bin","wb");
    
    ifstream myfile (input_filename);

    Geometry *geom;
    
  if (myfile.is_open())
  {
    std::string line;
	
	while (std::getline(myfile, line))
	{
	   string shape = extract(line);
	   if(shape != "ERROR")
       {
			 geom = parseString(shape);  

			 if(geom != NULL) {
				 const Envelope *env = geom->getEnvelopeInternal();  
				 MBR m;
				 m.minx = env->getMinX(); 
				 m.miny = env->getMinY();  		 
				 m.maxx = env->getMaxX(); 
				 m.maxy	= env->getMaxY();  
			 
     			 fwrite(&m, sizeof(MBR), 1, ofp);
     			 printf("a");
     		  }
		} 
       }
   }
    
   fclose(ofp);
   myfile.close();
   
   //readBinaryFile();
   
   return 0;
}

int readPointNodeData()
{
    //FILE *fp = fopen("/home/satish/data/allnodes_small/allnodesSmall.txt","r"); 
    
     double starttime, endtime, difference;
    
    //starttime = my_difftime();    
    
    //char* input_filename = "/home/satish/data/allnodes_small/allnodesSmall.txt";
    char* input_filename = "/home/satish/data/allnodes/all_nodes";
    //FILE *ofp = fopen("allNodesPointsSmall.bin","w");
    
    string tag;
    long nodeId;
    double longi, lat;
    
    ifstream myfile (input_filename);
    
    list<Geometry*> geoms;
    
    long geomsCnt = 0;
    
  if (myfile.is_open())
  {
    std::string line;
	geos::geom::GeometryFactory factory;
	
	while (std::getline(myfile, line))
	{
      {	 
    	stringstream pointString(line);
	
		if (pointString >> nodeId >> longi >> lat >> tag)
		{
			Coordinate cord(longi, lat);
  			
 	    	Point *pt = factory.createPoint(cord);
 	    	geomsCnt++;
 	    	//geoms.push_back(pt);
 	    	delete pt;
 	    	//cout<<nodeId<<" ";
  			//fwrite(coords, sizeof(float), 4, ofp); 
		} 
       }
       // fwrite(coords, sizeof(float), 4, ofp); 
       // printf("%f, %f, %f, %f\n", x1, y1, x2, y2);  
    }
   } 
   // fclose(fp);
   // fclose(ofp);
    //cout<<geoms.size()<<endl;
    
    cout<<geomsCnt<<endl;
    
    //endtime = my_difftime();
	 
    //difference = endtime - starttime;

   // cout<<"Time "<<difference<<endl;
    
	return 0;
}

// int readBinaryFile()
// {
// 	FILE *fp = fopen("allNodesPointsSmall.bin","rb");
// 	long count = 1000000;
//     
//     pNode p;
//     size_t result;
// 	for(long i=0; i<count; i++) {
// 	    result = fread(&p, sizeof(p), 1, fp);
// 	    //printf("%s \t", p.tag);
// 	    printf("%ld \t", p.nodeId);
// 	    if (result != 1)
// 	    {
// 	      fputs ("Reading error",stderr);
// 	    }
// 	}
// 	
// 	fclose(fp);
// }

// double my_difftime ()
// {
//     struct timeval tp;
//     struct timezone tzp;
// 
//     gettimeofday(&tp,&tzp);
//     return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
// }

// FILE *bfp = fopen("road_networkMBRsOnly.bin", "r");
//     
//     for(int i = 0; i < 100; i++) {
//     	int numRead = fread(coords, sizeof(float), 4, bfp); 
//     	//cout<<numRead<<", ";
//     	printf("%f, %f, %f, %f\n", coords[0], coords[1], coords[2], coords[3]);
//     }
//     cout<<endl;
//      
//     fclose(bfp);