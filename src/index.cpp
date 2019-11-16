#include "index/index.h"

map<int, list<Geometry*>* >* Index :: partitionGeomsAmongGridCells(int aNumCells, list<Geometry*> *shapes)
{
   map<int, list<Geometry*>* > *cellToGeomListMap = new map<int, list<Geometry*>* >();

   //initialize cellToGeomsMap
   for(int cellId = 0; cellId < aNumCells; cellId++) {
       list<Geometry*>* geomList = new list<Geometry*>();
       cellToGeomListMap->insert(std::pair<int, list<Geometry*>*>(cellId, geomList)); 
   }
     
   for (list<Geometry*>::iterator it = shapes->begin() ; it != shapes->end(); ++it) {
      
      Geometry* geom = *it;
     
      std::vector<void *> results;
      
      index->query(geom->getEnvelopeInternal(), results);
      
      if(results.empty() == false) {
         
         for(auto cellId : results) {
             int intCellId = *((int*)cellId);
        
             list<Geometry*>* geomList = cellToGeomListMap->at(intCellId);
			 geomList->push_back(geom);
         } //for
       } //if     
	} // outer for
	
	return cellToGeomListMap;
}

int Index :: populateWithShapes(list<Geometry*> *shapes, map<int, Cell*> *shapesInCell,
                                bool isBaseLayer)
{
   //cout<<"Index populateWithShapes: |shapes| "<<shapes->size()<<"Shapes in Cell Map "<<shapesInCell->size()<<endl;
   //int count = 0;
   
   for (list<Geometry*>::iterator it = shapes->begin() ; it != shapes->end(); ++it) {
      
      Geometry* geom = *it;
      //int *id = (int *)poly1->getUserData();
       
      std::vector<void *> results;
      
      //cout<<"In query"<<endl;
      
      index->query(geom->getEnvelopeInternal(), results);
      
      if(results.empty() == false) {
         //count += results.size();
         for(auto cellId : results) {
             int intCellId = *((int*)cellId);
               // printf(" intCellId %d ",intCellId); 
//                fflush(stdout);
             Cell *cell = shapesInCell->at(intCellId);

             if(isBaseLayer)
                cell->addToLayerA(geom);
             else 
                cell->addToLayerB(geom); 
            //count++;
          } //for
       } //if     
	} // outer for

   return 0;
}


void Index :: createRTree(list<Geometry*> *polys) 
{
  index = new geos::index::strtree::STRtree();
  createIndex(polys);
}

void Index :: createRTree(vector<Geometry*> *polys) 
{
  index = new geos::index::strtree::STRtree();
  createIndex(polys);
}

void Index :: createRTree(vector<Envelope*> *envs)
{
   index = new geos::index::strtree::STRtree();
  
   for (vector<Envelope*>::iterator it = envs->begin() ; it != envs->end(); ++it) {
        Envelope* p = *it;
      
        index->insert( p, p );
      
      //cout<<env->toString()<<endl; //<<p->toString();
    }
}

/* called by binary file I/O program */
void Index :: createRTree(list<Envelope*> *envs)
{
   index = new geos::index::strtree::STRtree();
  
   for (list<Envelope*>::iterator it = envs->begin() ; it != envs->end(); ++it) {
        Envelope* p = *it;
      
        index->insert( p, p );
      
      //cout<<env->toString()<<endl; //<<p->toString();
    }
}


void Index :: createRTree(vector<GeomInfo*> *envs)
{
   index = new geos::index::strtree::STRtree();
  
   for (vector<GeomInfo*>::iterator it = envs->begin() ; it != envs->end(); ++it) {
        GeomInfo* g = *it;
        Envelope* p = g->env;
        index->insert( p, p );
      
      //cout<<env->toString()<<endl; //<<p->toString();
    }
}


void Index :: createRTree(map<int, Envelope*> *gridCells)
{
  //cout<<"inside createTree"<<endl;
  index = new geos::index::strtree::STRtree();
  
  for(auto it = gridCells->cbegin(); it != gridCells->cend(); ++it)
  {
     index->insert(it->second, (void *)&(it->first));
      //Envelope *env = it->second;
      //cout<<env->toString()<<endl;
  }
}

void Index :: createQuadTree(list<Geometry*> *polys)
{
    index = new  geos::index::quadtree::Quadtree();
    createIndex(polys);
}

map<Envelope*, vector<void *> >* Index :: query(vector<GeomInfo*> *layer2) 
{   
    map<Envelope*, vector<void *> >* overlapMap = new map<Envelope*, vector<void *> >();
    int count = 0;
    for (vector<GeomInfo*>::iterator layer2it = layer2->begin() ; layer2it != layer2->end(); ++layer2it) {
      
      GeomInfo* info = *layer2it;
      Envelope* poly1 = info->env;
      //int *id = (int *)poly1->getUserData();
       
      std::vector<void *> results;
      
      //cout<<"In query"<<endl;
      
      index->query(poly1, results);
      
      if(results.empty() == false) {
         count += results.size();
         overlapMap->insert(pair<Envelope*, vector<void *> >(poly1, results));
      }
    }
  // cout<<"In Index query, "<<count<<endl;
   return overlapMap;
}

map<Envelope*, vector<void *> >* Index :: query(list<Envelope*> *layer2) 
{   
    map<Envelope*, vector<void *> >* overlapMap = new map<Envelope*, vector<void *> >();
    int count = 0;
    for (list<Envelope*>::iterator layer2it = layer2->begin() ; layer2it != layer2->end(); ++layer2it) {
      
      Envelope* poly1 = *layer2it;
      //int *id = (int *)poly1->getUserData();
       
      std::vector<void *> results;
      
      //cout<<"In query"<<endl;
      
      index->query(poly1, results);
      
      if(results.empty() == false) {
         count += results.size();
         overlapMap->insert(pair<Envelope*, vector<void *> >(poly1, results));
      }
    }
  // cout<<"In Index query, "<<count<<endl;
   return overlapMap;
}

map<Envelope*, vector<void *> >* Index :: query(vector<Envelope*> *layer2) 
{   
    map<Envelope*, vector<void *> >* overlapMap = new map<Envelope*, vector<void *> >();
    int count = 0;
    for (vector<Envelope*>::iterator layer2it = layer2->begin() ; layer2it != layer2->end(); ++layer2it) {
      
      Envelope* poly1 = *layer2it;
      //int *id = (int *)poly1->getUserData();
       
      std::vector<void *> results;
      
      //cout<<"In query"<<endl;
      
      index->query(poly1, results);
      
      if(results.empty() == false) {
         count += results.size();
         overlapMap->insert(pair<Envelope*, vector<void *> >(poly1, results));
      }
    }
  // cout<<"In Index query, "<<count<<endl;
   return overlapMap;
}

map<Geometry*, vector<void *> >* Index :: query(list<Geometry*> *layer2) 
{   
    map<Geometry*, vector<void *> >* overlapMap = new map<Geometry*, vector<void *> >();
    int count = 0;
    for (list<Geometry*>::iterator layer2it = layer2->begin() ; layer2it != layer2->end(); ++layer2it) {
      
      Geometry* poly1 = *layer2it;
      //int *id = (int *)poly1->getUserData();
       
      std::vector<void *> results;
      
      //cout<<"In query"<<endl;
      
      index->query(poly1->getEnvelopeInternal(), results);
      
      if(results.empty() == false) {
         count += results.size();
         overlapMap->insert(pair<Geometry*, vector<void *> >(poly1, results));
      }
    }
  // cout<<"In Index query, "<<count<<endl;
   return overlapMap;
}

/* called by binary MPI file I/O program*/
map<Geometry*, vector<void *> >* Index :: query(vector<Geometry*> *layer2) 
{   
    map<Geometry*, vector<void *> >* overlapMap = new map<Geometry*, vector<void *> >();
    int count = 0;
    for (vector<Geometry*>::iterator layer2it = layer2->begin() ; layer2it != layer2->end(); ++layer2it) {
      
      Geometry* poly1 = *layer2it;
      //int *id = (int *)poly1->getUserData();
       
      std::vector<void *> results;
      
      //cout<<"In query"<<endl;
      
      index->query(poly1->getEnvelopeInternal(), results);
      
      if(results.empty() == false) {
         count += results.size();
         overlapMap->insert(pair<Geometry*, vector<void *> >(poly1, results));
      }
    }
   cout<<"Binary file I/O Index query, "<<count<<endl;
   return overlapMap;
}

void Index :: createIndex(vector<Geometry*> *polys)
{
   for (vector<Geometry*>::iterator it = polys->begin() ; it != polys->end(); ++it) {
        Geometry* p = *it;
      
        index->insert( p->getEnvelopeInternal(), p );
      
      //cout<<env->toString()<<endl; //<<p->toString();
    }
}

void Index :: createIndex(list<Geometry*> *polys)
{
   for (list<Geometry*>::iterator it = polys->begin() ; it != polys->end(); ++it) {
        Geometry* p = *it;
      
        index->insert( p->getEnvelopeInternal(), p );
      
      //cout<<env->toString()<<endl; //<<p->toString();
    }
}
