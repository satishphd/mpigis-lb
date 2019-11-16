#include <iostream>
#include <fstream>
#include <vector>
#include <stdlib.h>
#include <sys/time.h>
#include "join/join.h"
#include "index/index.h"
#include "parser/WKTParser.h"
#include "filePartition/SeqFilePartitioner.h"
#include "parser/road_network_parser.h"

//#define DBUGROAD 1

using namespace std;

void freeGeomsMemory(list<Geometry*> *geoms);

double my_difftime ()
{
    struct timeval tp;
    struct timezone tzp;

    gettimeofday(&tp,&tzp);
    return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}

/* Road Network Sequential Split IO */
int main(int argc, char *argv[]) 
{
    double starttime, endtime, difference;
    
    starttime = my_difftime();    
    
    SeqFilePartitioner partitioner;
    
    if(argc < 4)
    {
      cout<<"./a.out #blocks filename k"<<endl;
      return 0;
    }
  
    int numBlocks = atoi(argv[1]);
    char *filename = argv[2];
    int k = atoi(argv[3]);
    
    //partitioner.seq_partition(numBlocks, filename);   
    
    partitioner.reserveSampling(numBlocks, filename, k);
    /*Parser *parser = new RoadNetworkParser();
    
    list<FileSplits*>::const_iterator itr;
    
    int blockCounter = 0;
    
    for(itr = splits->begin(); itr != splits->end(); itr++) {
         FileSplits* split = *itr;

         blockCounter++;

        if(blockCounter >= 75) {
         cout<<"Number of lines in a split "<<split->numLines()<<endl; 
         list<Geometry*> *geoms = parser->parse(*split, 0, 0);
         cout<<"# Geoms "<<geoms->size()<<" in block # "<<blockCounter<<endl;      
        }

        split->clear();
        
        delete split;    
 
        if(geoms != NULL && geoms->size()>0)
          freeGeomsMemory(geoms);
    }*/
    
    endtime = my_difftime();
	 
    difference = endtime - starttime;

    cout<<"Time "<<difference<<endl;
    return 0;
}


/* Sequential Split IO */
// int main1(int argc, char *argv[]) 
// {
//     double starttime, endtime, difference;
//     
//     starttime = my_difftime();    
//     
//     SeqFilePartitioner partitioner;
//     
//     if(argc < 3)
//     {
//       cout<<"./a.out #blocks filename"<<endl;
//       return 0;
//     }
//   
//     int numBlocks = atoi(argv[1]);
//     char *filename = argv[2];
//   
//     list<FileSplits*>* splits = partitioner.seq_partition(numBlocks, filename);   
//     
//     WKTParser *parser = new WKTParser();
//     
//     list<FileSplits*>::const_iterator itr;
// 
//     for(itr = splits->begin(); itr != splits->end(); itr++) {
//         FileSplits* split = *itr;
//         cout<<"Number of lines in a split "<<split->numLines()<<endl; 
//      
//         list<Geometry*> *geoms = parser->parse(*split, 0, 0);
//         cout<<"# Geoms "<<geoms->size()<<endl;
// 
//         split->clear();
//         delete split;    
//         freeGeomsMemory(geoms);
//     }
//     
//     endtime = my_difftime();
// 	 
//     difference = endtime - starttime;
// 
//     cout<<"Time "<<difference<<endl;
//     return 0;
// }


// int main1 (int argc, char *argv[]) 
// {
//     double starttime, endtime, difference;
//     
//     starttime = my_difftime();    
//     
//     WKTParser *parserWKT = new WKTParser();
//     // Read only wkt file
//     list<Geometry*> *layer1Geoms = parserWKT->readOSM(argv[1]);
//     cout<<"Number of polygons in File "<< layer1Geoms->size()<<endl;
// 
//     Index rtree;
// 	rtree.createIndex(layer1Geoms);
// 	cout<<"Index created"<<endl;
//     
//     // read spatialhadoop file
//     FileSplits *layer2 = new FileSplits();
//     
//     ifstream in_stream;
//     string line;
//     in_stream.open(argv[2]);
// 
//     for( string line; getline( in_stream, line ); )
//     {
//       layer2->addGeom(line);
//     }
//   
//     cout<<"Number of records in File "<< layer2->numLines()<<endl;
//     
//     Parser *parser = new WKTParser();
//     
//     list<Geometry*> *layer2Geoms = parser->parse(*layer2,1,2);
//     cout<<"Number of polygons in File "<< layer2Geoms->size()<<endl;
// 	
//     /*
//     FileSplits *layer2 = new FileSplits();
//     
//     ifstream in_stream2;
// 
//     in_stream2.open(argv[2]);
// 
//     for( string line; getline( in_stream2, line ); )
//     {
//       layer2->addGeom(line);
//     }
//   
//     cout<<"Number of records in File "<< layer2->numLines()<<endl;    
//    */
//    
//     map<Geometry*, vector<void *> >* intersectionMap = rtree.query(layer2Geoms);
//    
//     Join spatialJoin;
// 
//     list<pair<Geometry*, Geometry*> > *pairs = spatialJoin.join(intersectionMap);
//    
//     endtime = my_difftime();
// 	 
//     difference = endtime - starttime;
// 
//     cout<<"Time "<<difference<<endl;
//    
//     //Geometry *geom1 = layer1Geoms->front(); 
//     //cout<<geom1->toString()<<endl;
//     
//         
//     /*
//     int gcounter = 0;
//     
//     for (list<Geometry*> :: iterator it = layer1Geoms->begin(); it != layer1Geoms->end(); ++it) {
//         Geometry *geom = *it;
//         //cout<<geom->toString()<<endl; 
//         int pCount = geom->getNumGeometries();
//         //cout<< "Number of polygons "<<pCount<<endl;
//         
//         for(int polyIndex = 0; polyIndex < pCount; polyIndex++) {
//           const Geometry *lgeom = geom->getGeometryN(polyIndex);
//           CoordinateSequence* seq = lgeom->getCoordinates();
//           
//           int cords = seq->size();
//           //cout<< "Size of seq "<<cords;
//           
//           cout<<gcounter<<" "; gcounter++;
//           
// 		  for(int i=0; i<cords; i++) {
//   
//   			 //Coordinate &cord = seq[i];
//   			 //cout<<cord.x <<" "<<cord.y;
//   			 cout<<" "<<seq->getX(i)<<" "<<seq->getY(i);
//           }
//           cout<<endl;
//         }
//     }
//     */
//     in_stream.close();
//     //in_stream2.close();
//     
//     return 0;
//  }