#include "spatialPartition/uniformGrid.h"
#include "iomanip"

void UniformGrid :: initShapesInCell() 
{
   shapesInCell = new map<int, Cell*>();
   								  
   for(int counter = 0; counter < numCells; counter++) {
     Cell *cell = new Cell();
     shapesInCell->insert(std::pair<int, Cell*>(counter, cell)); 
   }
   //cout<<"Grid initialized"<<endl;
}

// display unordered_map<int, Envelope*> *gridCells;
void UniformGrid :: printGridCoordinates()
{
  for(auto it = gridCells->cbegin(); it != gridCells->cend(); ++it)
  {
    Envelope *r = it->second;
    std::cout << it->first << ": " << r->getMinX()<< ", " << r->getMinY() << " to " << r->getMaxX()<< ", " << r->getMaxY() << "\n";
    // (xmin, ymin -> xmax, ymax)
    //std::cout << it->first << ": " << r.boundary[0]<< ", " << r.boundary[1] << " to " << r.boundary[2]<< ", " << r.boundary[3] << "\n";
  }
  
  //cout<<"Print Grid : Number of cells in gridCells "<<gridCells->size()<<endl;
}


/*
 input is shapesInCell
 output is cell id : Pair(base, overlay)
 Each process has an array of length = (2*numCells) which is populated from shapesInCell
*/
int* UniformGrid :: numShapesPerCell() 
{
    int *inputBuffer = new int[2*numCells];
    
    //for(auto it = shapesInCell->cbegin(); it != shapesInCell->cend(); ++it)
    for(int cellId = 0; cellId < numCells; cellId++)
    {
       int index = 2*cellId;
       
       Cell* cell = shapesInCell->at(cellId);
       
       list<Geometry*> *layerAGeoms = cell->getLayerAGeom();
       inputBuffer[index] = layerAGeoms->size();
       
       list<Geometry*> *layerBGeoms = cell->getLayerBGeom();
       inputBuffer[index+1] = layerBGeoms->size();
    }
    
    int *outputBuffer = new int[2*numCells];
    
	MPI_Reduce(inputBuffer, outputBuffer, (2*numCells), MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
    
    return outputBuffer;
}

/*
  Only root 0 will call this method.
  Returns a map with
  key: cell id
  value: (numLayer1Shapes, numLayer2Shapes)
*/
map<int, pair<int, int> > UniformGrid :: globalGridStatistics(int *gridShapeCounts)
{
    map<int, pair<int, int> > cellToShapeCounts;
    int sumLayer1 = 0;
    int sumLayer2 = 0;
    
    for(int cellId = 0; cellId < numCells; cellId++) {
       int index = 2*cellId;
       int layer1Count = gridShapeCounts[index];
       int layer2Count = gridShapeCounts[index+1];
	   
	   sumLayer1 += layer1Count;
	   sumLayer2 += layer2Count;
	
	   // if(layer1Count>0)
//     	   cout<<layer1Count<<", ";
	
	   pair<int,int> shapesPerCell(layer1Count, layer2Count);
	   cellToShapeCounts.insert(make_pair(cellId, shapesPerCell));
	}
	
	//avg = new pair<int,int>(sumLayer1/numCells, sumLayer2/numCells);
	cout<<"Total Sum of shapes distributed across all the cells in a grid from layer 1 and 2 "<<sumLayer1<<", "<<sumLayer2<<endl;
    cout<<"Average shapes per cell layer1 "<< (sumLayer1/numCells)<<" "<< (sumLayer2/numCells)<<endl;
	return  cellToShapeCounts;
}

/*
  Only root 0 will call this method.
*/

void UniformGrid :: printGlobalGridStatistics(map<int, pair<int, int> > &cellMap)
{
  cout<<"cell id count layer1, layer2"<<endl;
  
  for(auto it = cellMap.cbegin(); it != cellMap.cend(); ++it) {
     pair<int,int> counts = it->second;
     
     if(counts.first > 0)
       cout<<"["<<it->first<<"]: "<<counts.first<<setw(4);
       //cout<<"["<<it->first<<"]: "<<counts.first<<" "<<counts.second<<setw(10);
  }
  //cout<<"Average shapes per cell layer1 "<< avg->first<<" "<<avg->second<<endl;
}
	
list<string> UniformGrid :: localGridStatistics() 
{
  //cout<<"Number of cells in shapesInCell map "<<shapesInCell->size()<<endl;
  list<string> logs;
  logs.push_back("[cell id] : layerA #geoms, layerB #geoms");
   // display map<int, Cell*> *shapesInCell;
  for(auto it = shapesInCell->cbegin(); it != shapesInCell->cend(); ++it)
  {
    Cell* cell = it->second;
    list<Geometry*> *layerAGeoms = cell->getLayerAGeom();
    
    //list<Geometry*> *layerBGeoms = cell->getLayerBGeom();
    
    //std::cout << "("<<it->first << "): (" <<layerAGeoms->size()<<", "<<layerBGeoms->size()<<") "<<endl;
    //std::cout << "("<<it->first << "): " <<layerAGeoms->size()<<" ";
    
    //string log("[" + to_string(it->first) + "] : " + to_string(layerAGeoms->size()) + ", " +  to_string(layerBGeoms->size()) );
    //logs.push_back(log);
  }
  cout<<endl;
  return logs;
}

int UniformGrid :: partition()
{
  partitionHelper(numCells);
  return 0;
}

void UniformGrid :: addToMBR(int index, double lowX, double lowY, double upX,   double upY)
 {
  //Rect cell = {{lowX, lowY, upX, upY}};
  Envelope *cell = new Envelope(lowX, upX, lowY, upY);
  gridCells->insert(make_pair(index, cell));

 }

// int UniformGrid :: populateGridCells(vector<Envelope*>* envs, bool isBaseLayer)
// {
//    int numShapes =    tree.populateWithShapes(envs, shapesInCell, isBaseLayer);
//    //int numOverlayShapes = rtree->populateWithShapes(shapes, shapesInCell, isBaseLayer);
// 
//   return numShapes;
// }
 
int UniformGrid :: populateGridCells(list<Geometry*>* shapes, bool isBaseLayer)
{
   int numShapes =    tree.populateWithShapes(shapes, shapesInCell, isBaseLayer);
   //int numOverlayShapes = rtree->populateWithShapes(shapes, shapesInCell, isBaseLayer);

  return numShapes;
}

int UniformGrid :: partitionHelper( int numberOfPartitions)
{
  double MBR0, MBR1, MBR2, MBR3;
//  universe.getMinX(), universe.getMinY(), universe.getMaxX() , universe.getMaxY()
  MBR0 = universe->getMinX();
  MBR1 = universe->getMinY();
  MBR2 = universe->getMaxX();
  MBR3 = universe->getMaxY();
  
  switch(numberOfPartitions)
  {
   case 2:
   partitionInto2cells(0,MBR0, MBR1, MBR2, MBR3);
   break;
   
   case 4:
   partitionInto4Cells(0,MBR0, MBR1, MBR2, MBR3);
   break;
   
   case 8:
   partitionInto8cells(0,MBR0, MBR1, MBR2, MBR3);
   break;
   
   case 16:
   partitionInto16Cells(0,MBR0, MBR1, MBR2, MBR3);
   break;
   
   case 32:
   partitionInto32Cells(0,MBR0, MBR1, MBR2, MBR3);
   break;
   
   case 64:
   partitionInto64Cells(0,MBR0, MBR1, MBR2, MBR3);
   break;
   
   case 128:
   partitionInto128Cells(0,MBR0, MBR1, MBR2, MBR3);
   break;
   
   case 256:
   partitionInto256Cells(0,MBR0, MBR1, MBR2, MBR3);
   break;
   
   case 512:
   partitionInto512Cells(0,MBR0, MBR1, MBR2, MBR3);
   break;
   
   case 1024:
   partitionInto1024Cells(0,MBR0, MBR1, MBR2, MBR3);
   break;
   
   case 2048:
   partitionInto2048Cells(0,MBR0, MBR1, MBR2, MBR3);
   break;
   
   case 4096:
   partitionInto4096Cells(0,MBR0, MBR1, MBR2, MBR3);
   break;
  
   case 8192:
   partitionInto8000Cells(0,MBR0, MBR1, MBR2, MBR3);
   break; 
  
   case 16384:
   partitionInto16kCells(0,MBR0, MBR1, MBR2, MBR3);
   break; 
   
   case 32768:
   partitionInto32kCells(0,MBR0, MBR1, MBR2, MBR3);
   break; 
   
   case 65536:
   partitionInto64kCells(0,MBR0, MBR1, MBR2, MBR3);
   break; 

   case 131072:
   partitionInto128kCells(0,MBR0, MBR1, MBR2, MBR3);
   break; 
   
   case 262144:
   partitionInto256kCells(0,MBR0, MBR1, MBR2, MBR3);
   break; 
   
   default:
   {
    printf("NOT SUPPORTED spatial partitioning argument \n");
    exit(1);
   }
  }
  return 0;
}

void UniformGrid :: partitionInto2cells(int index,double lowX, double lowY, 
double upX, double upY)
{
 double deltaX = upX - lowX;
 //calculate x-centre
 double midX = lowX + deltaX/2;
 
 addToMBR(index+0,lowX, lowY, midX, upY);
 addToMBR(index+1,midX, lowY, upX, upY);
}

void UniformGrid :: partitionInto4Cells(int index, double lowX, double lowY, double upX, double upY)
{
 double deltaX = upX - lowX;
 double deltaY = upY - lowY;
 //calculate centre
 double midX = lowX + deltaX/2;
 double midY = lowY + deltaY/2;

 addToMBR(index+0,lowX, lowY, midX, midY);
 addToMBR(index+1,midX, lowY, upX, midY);
 addToMBR(index+2,lowX, midY, midX, upY);
 addToMBR(index+3,midX, midY, upX, upY);
}

void UniformGrid :: partitionInto8cells(int index, double lowX, double lowY, double upX, double upY)
{
 double deltaX = upX - lowX;
 double deltaY = upY - lowY;
 //calculate centre
 double midX = lowX + deltaX/2;
 double midY = lowY + deltaY/2;

 partitionInto2cells(index+0,lowX, lowY, midX, midY);
 partitionInto2cells(index+2,midX, lowY, upX, midY);
 partitionInto2cells(index+4,lowX, midY, midX, upY);
 partitionInto2cells(index+6,midX, midY, upX, upY);
}

void UniformGrid :: partitionInto16Cells(int index,double lowX, double lowY, 
double upX, double upY)
{
 double deltaX = upX - lowX;
 double deltaY = upY - lowY;
 //calculate centre
 double midX = lowX + deltaX/2;
 double midY = lowY + deltaY/2;

 partitionInto4Cells(index+0,lowX, lowY, midX, midY);
 partitionInto4Cells(index+4,midX, lowY, upX, midY);
 partitionInto4Cells(index+8,lowX, midY, midX, upY);
 partitionInto4Cells(index+12,midX, midY, upX, upY);
}

void UniformGrid :: partitionInto32Cells(int index, double lowX, double lowY, double upX, double upY)
{
 double deltaX = upX - lowX;
 double deltaY = upY - lowY;
 //calculate centre
 double midX = lowX + deltaX/2;
 double midY = lowY + deltaY/2;

 partitionInto8cells(index+0,lowX, lowY, midX, midY);
 partitionInto8cells(index+8,midX, lowY, upX, midY);
 partitionInto8cells(index+16,lowX, midY, midX, upY);
 partitionInto8cells(index+24,midX, midY, upX, upY);
}

void UniformGrid :: partitionInto64Cells(int index,double lowX, double lowY, double upX, 
double upY)
{
 double deltaX = upX - lowX;
 double deltaY = upY - lowY;
 //calculate centre
 double midX = lowX + deltaX/2;
 double midY = lowY + deltaY/2;

 partitionInto16Cells(index+0,lowX, lowY, midX, midY);
 partitionInto16Cells(index+16,midX, lowY, upX, midY);
 partitionInto16Cells(index+32,lowX, midY, midX, upY);
 partitionInto16Cells(index+48,midX, midY, upX, upY);
}

void UniformGrid :: partitionInto128Cells(int index,double lowX, double lowY, double upX, 
double upY)
{
 double deltaX = upX - lowX;
 //calculate x-centre
 double midX = lowX + deltaX/2;
 
 partitionInto64Cells(index+0,lowX, lowY, midX, upY);
 partitionInto64Cells(index+64,midX, lowY, upX, upY);
}

void UniformGrid :: partitionInto256Cells(int index,double lowX, double lowY, double upX, double upY)
{
 double deltaX = upX - lowX;
 double deltaY = upY - lowY;
 //calculate centre
 double midX = lowX + deltaX/2;
 double midY = lowY + deltaY/2;

 partitionInto64Cells(index+0,lowX, lowY, midX, midY);
 partitionInto64Cells(index+64,midX, lowY, upX, midY);
 partitionInto64Cells(index+128,lowX, midY, midX, upY);
 partitionInto64Cells(index+192,midX, midY, upX, upY);
}


void UniformGrid :: partitionInto512Cells(int index,double lowX, double lowY, double upX, 
double upY)
{
  double deltaX = upX - lowX;
  double deltaY = upY - lowY;
  //calculate centre
  double midX = lowX + deltaX/2;
  double midY = lowY + deltaY/2;
  
  
 partitionInto128Cells(index+0,lowX, lowY, midX, midY);
 partitionInto128Cells(index+128,midX, lowY, upX, midY);
 partitionInto128Cells(index+256,lowX, midY, midX, upY);
 partitionInto128Cells(index+384,midX, midY, upX, upY);
}

void UniformGrid :: partitionInto1024Cells(int index,double lowX, double lowY, double upX, 
double upY)
{
  double deltaX = upX - lowX;
  double deltaY = upY - lowY;
  //calculate centre
  double midX = lowX + deltaX/2;
  double midY = lowY + deltaY/2;
  
 partitionInto256Cells(index+0,lowX, lowY, midX, midY);
 partitionInto256Cells(index+256,midX, lowY, upX, midY);
 partitionInto256Cells(index+512,lowX, midY, midX, upY);
 partitionInto256Cells(index+768,midX, midY, upX, upY);
}

void UniformGrid :: partitionInto2048Cells(int index,double lowX, double lowY, double upX, 
double upY)
{
  double deltaX = upX - lowX;
  double deltaY = upY - lowY;
  //calculate centre
  double midX = lowX + deltaX/2;
  double midY = lowY + deltaY/2;
  
 partitionInto512Cells(index+0,lowX, lowY, midX, midY);
 partitionInto512Cells(index+512,midX, lowY, upX, midY);
 partitionInto512Cells(index+1024,lowX, midY, midX, upY);
 partitionInto512Cells(index+1536,midX, midY, upX, upY);
}

void UniformGrid :: partitionInto4096Cells(int index,double lowX, double lowY, double upX,
double upY)
{
  double deltaX = upX - lowX;
  double deltaY = upY - lowY;
  //calculate centre
  double midX = lowX + deltaX/2;
  double midY = lowY + deltaY/2;

 partitionInto1024Cells(index+0,lowX, lowY, midX, midY);
 partitionInto1024Cells(index+1024,midX, lowY, upX, midY);
 partitionInto1024Cells(index+2048,lowX, midY, midX, upY);
 partitionInto1024Cells(index+3072,midX, midY, upX, upY);
}

void UniformGrid :: partitionInto8000Cells(int index,double lowX, double lowY, double upX,
double upY)
{
  double deltaX = upX - lowX;
  double deltaY = upY - lowY;
  //calculate centre
  double midX = lowX + deltaX/2;
  double midY = lowY + deltaY/2;

 partitionInto2048Cells(index+0,lowX, lowY, midX, midY);
 partitionInto2048Cells(index+2048,midX, lowY, upX, midY);
 partitionInto2048Cells(index+4096,lowX, midY, midX, upY);
 partitionInto2048Cells(index+6144,midX, midY, upX, upY);
}

void UniformGrid :: partitionInto16kCells(int index,double lowX, double lowY, double upX,
double upY)
{
  double deltaX = upX - lowX;
  double deltaY = upY - lowY;
  //calculate centre
  double midX = lowX + deltaX/2;
  double midY = lowY + deltaY/2;

 partitionInto4096Cells(index+0,lowX, lowY, midX, midY);
 partitionInto4096Cells(index+4096,midX, lowY, upX, midY);
 partitionInto4096Cells(index+8192,lowX, lowY, midX, midY);
 partitionInto4096Cells(index+12288,midX, lowY, upX, midY);
}

void UniformGrid :: partitionInto32kCells(int index,double lowX, double lowY, double upX,
double upY)
{
  double deltaX = upX - lowX;
  double deltaY = upY - lowY;
  //calculate centre
  double midX = lowX + deltaX/2;
  double midY = lowY + deltaY/2;
  partitionInto8000Cells(index+0,lowX, lowY, midX, midY);
  partitionInto8000Cells(index+8192,midX, lowY, upX, midY);
  partitionInto8000Cells(index+16384,lowX, lowY, midX, midY);
  partitionInto8000Cells(index+24576,midX, lowY, upX, midY);
}

void UniformGrid :: partitionInto64kCells(int index,double lowX, double lowY, double upX,
double upY)
{
  double deltaX = upX - lowX;
  double deltaY = upY - lowY;
  //calculate centre
  double midX = lowX + deltaX/2;
  double midY = lowY + deltaY/2;
  partitionInto16kCells(index+0,lowX, lowY, midX, midY);
  partitionInto16kCells(index+16384,midX, lowY, upX, midY);
  partitionInto16kCells(index+32768,lowX, lowY, midX, midY);
  partitionInto16kCells(index+49152,midX, lowY, upX, midY);
}

void UniformGrid :: partitionInto128kCells(int index,double lowX, double lowY, double upX,
double upY)
{
  double deltaX = upX - lowX;
  double deltaY = upY - lowY;
  //calculate centre
  double midX = lowX + deltaX/2;
  double midY = lowY + deltaY/2;
  partitionInto32kCells(index+0,lowX, lowY, midX, midY);
  partitionInto32kCells(index+32768,midX, lowY, upX, midY);
  partitionInto32kCells(index+65536,lowX, lowY, midX, midY);
  partitionInto32kCells(index+98304,midX, lowY, upX, midY);
}

void UniformGrid :: partitionInto256kCells(int index,double lowX, double lowY, double upX,
double upY)
{
  double deltaX = upX - lowX;
  double deltaY = upY - lowY;
  //calculate centre
  double midX = lowX + deltaX/2;
  double midY = lowY + deltaY/2;
  partitionInto64kCells(index+0,lowX, lowY, midX, midY);
  partitionInto64kCells(index+65536,midX, lowY, upX, midY);
  partitionInto64kCells(index+131072,lowX, lowY, midX, midY);
  partitionInto64kCells(index+196608,midX, lowY, upX, midY);
}