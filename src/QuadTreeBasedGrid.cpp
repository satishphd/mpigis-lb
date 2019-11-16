#include "spatialPartition/adaptiveGrid.h"

 int AdaptiveGrid :: partition()
 {
    FileReader fr;
//    char *filename =   "/home/satish/mpigis/mpiio/src//MPI_GridCells.txt"; //4603
//    char *filename = "/home/satish/mpigis/mpiio/src/quadtreeGrids/TwentyEightKCells.txt";  //28732  
    char *filename = "/home/satish/mpigis/mpiio/src/quadtreeGrids/Sixty8KCells.txt";  //68677  
    
    vector<Envelope*>* cellMBRs = fr.readGridCellFile(filename);
    
    int cellID = 0;
    for(Envelope *env : *cellMBRs) {
       gridCells->insert(make_pair(cellID, env));
         //cout<<env->toString()<<endl;
       cellID++;
    }
    //cout<< "Partitioning done "<<cellID<<endl;
    return 0;
 }
 
 void AdaptiveGrid :: initShapesInCell()
 {
 	 shapesInCell = new map<int, Cell*>();
   								  
   	 for(int counter = 0; counter < numCells; counter++) {
       Cell *cell = new Cell();
       shapesInCell->insert(std::pair<int, Cell*>(counter, cell)); 
     }
 }
 
 void  AdaptiveGrid :: printGridCoordinates()
 {
 
 }
 
 int* AdaptiveGrid :: numShapesPerCell()
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
 
 //int populateWithShapes(list<Geometry*>);
 int AdaptiveGrid :: populateGridCells(list<Geometry*>* shapes, bool isBaseLayer)
 {
  	int numShapes =    tree.populateWithShapes(shapes, shapesInCell, isBaseLayer);
    //int numOverlayShapes = rtree->populateWithShapes(shapes, shapesInCell, isBaseLayer);

  	return numShapes;
 }

 map<int, list<Geometry*>* >*  AdaptiveGrid :: gridPartition(list<Geometry*>* shapes)
 {
        return tree.partitionGeomsAmongGridCells(numCells, shapes);
 }

 list<string> AdaptiveGrid :: localGridStatistics()
 {
	   //cout<<"Number of cells in shapesInCell map "<<shapesInCell->size()<<endl;
  list<string> logs;
  int totalGeoms = 0;
  
  cout<<"[cell id] : layerA #geoms, layerB #geoms"<<endl;
  
  //logs.push_back("[cell id] : layerA #geoms, layerB #geoms");
  
   // display map<int, Cell*> *shapesInCell;
  for(auto it = shapesInCell->cbegin(); it != shapesInCell->cend(); ++it)
  {
    Cell* cell = it->second;
    //list<Geometry*> *layerAGeoms = cell->getLayerAGeom();
	
	list<Geometry*> *layerBGeoms = cell->getLayerBGeom();
    
    if(layerBGeoms->size() > 0) {
          
          totalGeoms += layerBGeoms->size();
          
          layerBGeoms = cell->getLayerBGeom();
    
           //std::cout << "["<<it->first << "]: " <<layerAGeoms->size()<<" "<<endl;  //<<", "<<layerBGeoms->size()<<") "<<endl;
           std::cout << "("<<it->first << "): " <<layerBGeoms->size()<<" ";
    
           //string log("[" + to_string(it->first) + "] : " + to_string(layerAGeoms->size()) + ", " +  to_string(layerBGeoms->size()) );
      	   //logs.push_back(log);
     }
  }
  cout<<"Total Geoms in Grid "<<totalGeoms<<endl;
  return logs;

 }
 
 map<int, pair<int, int> > AdaptiveGrid :: globalGridStatistics(int *gridShapeCounts)
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
 
 void AdaptiveGrid :: printGlobalGridStatistics( map<int, pair<int, int> > &cellMap)
 {
 	cout<<"cell id count layer1, layer2"<<endl;
  
 	 for(auto it = cellMap.cbegin(); it != cellMap.cend(); ++it) {
  	   pair<int,int> counts = it->second;
     
    	 if(counts.first > 0)
     		  //cout<<"["<<it->first<<"]: "<<counts.first<<setw(4);
     		  cout<<"["<<it->first<<"]: "<<counts.first<<" "<<counts.second<<"	";
 	 }
 	 //cout<<"Average shapes per cell layer1 "<< avg->first<<" "<<avg->second<<endl;
 }
