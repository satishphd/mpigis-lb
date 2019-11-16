#include "mapreduce/envreducer.h"

  
  int EnvelopeReducer :: reduce(int cellId, vector<GeomInfo*>* col1, vector<GeomInfo*>* col2)
  {
    Index tree;
   
    if(col1 != nullptr && !col1->empty())
	 tree.createRTree(col1);

	map<Envelope*, vector<void *> >* intersectionMap = nullptr;	
	
	if(col2 != nullptr && !col2->empty())
      intersectionMap = tree.query(col2);
	
	Join spatialJoin;

    list<pair<Envelope*, Envelope*> > *pairs = nullptr;
    
    if(intersectionMap != nullptr)
        pairs = spatialJoin.join(intersectionMap);
     
    if(pairs!= nullptr && !pairs->empty()) 
      return pairs->size();
    else 
      return 0;
      //cout<< "  "<<pairs->size() <<endl;
  }
