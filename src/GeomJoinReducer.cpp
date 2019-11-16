#include "mapreduce/geomreducer.h"

int GeomJoinReducer :: reduce(int cellId, list<Geometry*>* col1, list<Geometry*>* col2)
{
    //cout<<col1->size()<<","<<col2->size()<<endl;
    
    Index tree;
   
    if(col1 != nullptr && !col1->empty())
	 tree.createRTree(col1);
 
    map<Geometry*, vector<void *> >* intersectionMap = nullptr;	
	
    if(col2 != nullptr && !col2->empty())
      intersectionMap = tree.query(col2);
	
	Join spatialJoin;

    list<pair<Geometry*, Geometry*> > *pairs = nullptr;
    
    if(intersectionMap != nullptr)
        pairs = spatialJoin.join(intersectionMap);
     
    if(pairs!= nullptr && !pairs->empty()) 
      return pairs->size();
    else 
      return 0;
      //cout<< "  "<<pairs->size() <<endl;
 return 0;
}