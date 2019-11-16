#include "parser/WKTParser.h"

bool WKTParser :: checkForEmptyCollection(Geometry *iGeom)
{
    bool isEmpty = false;

    if(iGeom == NULL || iGeom->isEmpty())
      return false;
      
    for(int i=0; i<iGeom->getNumGeometries(); i++) {
       const Geometry *g = iGeom->getGeometryN(i);
      
       if(g->isEmpty())
          isEmpty = true;
    }
    
    return isEmpty;
}

list<Geometry*>* WKTParser:: readOSM(char* filename) {

    ifstream in_stream;
    string line;
    
    list<Geometry*> *layer;
    
    in_stream.open(filename);
  
    if (in_stream.is_open()) {
   
      layer = new list<Geometry*>();
    
      geos::io::WKTReader wktreader;
      Geometry *geom = NULL;

      for( string line; getline( in_stream, line ); )
      {
        geom = wktreader.read(line);
  
        if(!checkForEmptyCollection(geom))
          layer->push_back(geom);
      }
	
	  in_stream.close();
   }	
	return layer;
}

pair<int, string*> WKTParser :: extract(const string &str) 
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
    //cout<<tokens[0]<<"  ";
    int wayId;
    
    /*if(tokens.empty() == false && tokens[0]!=NULL)
      wayId = std::stoi(*tokens[0]);
    else
    */
      wayId = 101;
    
    delete[] cstr;
    
    string empty = "ERROR";

    if(tokens.empty() == false && tokens.size()<3)
       return pair<int, string*>(-1, &empty);
    else 
       return pair<int, string*>(wayId, tokens[1]);
}

/*
geos::io::WKTReader wktreader;
  Geometry *geom = NULL;
  
  try
  {
    geom = wktreader.read(*p.second);
*/

list<Geometry*>* WKTParser :: parseGeoms(const FileSplits &split) 
{
  list<Geometry*> *geoms = new list<Geometry*>();
  
  list<string>::const_iterator i;

  list<string> *contents = split.getContents();
  
  geos::io::WKTReader wktreader;
  Geometry *geom = NULL;
  
  for(i = contents->begin(); i!= contents->end(); i++) {

    if(false == ((string)*i).empty()) {
       
       try
  	   {
          Geometry *geom = wktreader.read(*i);
          
          if(geom != NULL)
             geoms->push_back(geom);
       }
       catch(exception &e)
  	   {
    		//cout<< e.what() <<endl;
  	   }     
    } // end if
  } // end for
  return geoms;
}

list<Geometry*>* WKTParser :: parse(const FileSplits &split) 
{
  list<Geometry*> *geoms = new list<Geometry*>();
  
  list<string>::const_iterator i;

  list<string> *contents = split.getContents();
  
  for(i = contents->begin(); i!= contents->end(); i++) {

    if(false == ((string)*i).empty()) {
        pair<int, string*> p = extract(*i);
     
        if(p.first != -1 && p.second != NULL && !p.second->empty()) {
          Geometry *geom = parseString(p);
    
          if(geom != NULL)
             geoms->push_back(geom);
        }
    } // end if
  } // end for
  return geoms;
}

Geometry* WKTParser :: parseString(const std::pair<int, string*> &p) 
{
  geos::io::WKTReader wktreader;
  Geometry *geom = NULL;
  
  try
  {
    geom = wktreader.read(*p.second);
    
    ExtraInfo *extraInfo = new ExtraInfo();
    extraInfo->vertexStr = *p.second;
    //int *id = new int;
    //int arg = p.first;
    //id = (int *)&p.first;
    geom->setUserData(extraInfo);
  }
  catch(exception &e)
  {
    cout<< e.what() <<endl;
    //cout<<p.second<<endl;
    //cout<<geom->toString()<<endl;
  }
  
   if(checkForEmptyCollection(geom))
      return NULL;
   else
      return geom;
}

list<Geometry*>* WKTParser :: parseGeometryFromRecvBuffer( pair< bbox*, int>* l1BoxPair, 
															pair<char *,int>* l1RecvBuf) 
{      
   list<Geometry*> *l1Geoms =  parseGeometryFromLayer(l1BoxPair, l1RecvBuf);
   
   return l1Geoms;
}

pair<list<Geometry*>*, list<Geometry*>* >* WKTParser :: parseGeometryFromRecvBuffer( pair< pair< bbox*, int>* ,
									 pair< bbox*, int>* >* bBoxPair, pair<pair<char *,int>*, pair<char *,int>* >*recvBufPair) 
{
   /* Layer 1 */    
   pair< bbox*, int> *l1BoxPair = bBoxPair->first;
   
   pair<char *,int>* l1RecvBuf = recvBufPair->first;
      
   list<Geometry*> *l1Geoms =  parseGeometryFromLayer(l1BoxPair, l1RecvBuf);
   
   /* Layer 2 */
   pair< bbox*, int> *l2BoxPair = bBoxPair->second;
   
   pair<char *,int>* l2RecvBuf = recvBufPair->second;

   list<Geometry*> *l2Geoms =  parseGeometryFromLayer(l2BoxPair, l2RecvBuf);

   return new pair<list<Geometry*>*, list<Geometry*>* >(l1Geoms, l2Geoms);
}

map<int, list<Geometry*>* >*  WKTParser :: parseGeomByProcess( 
					CollectiveAttributes *mbrAttr, pair< bbox*, int> *l1BoxPair,
					pair<char *,long>* l1RecvBuf, AllObjectsCollectiveAttr* geomAttr, 
					Config *args, int PACKING_LEN)
{
    map<int, list<Geometry*>* >* cellToGeomMap = new map<int, list<Geometry*>* >();
	
	char *l1RecvBuffer = l1RecvBuf->first;		
	
	bbox *l1Boxes = l1BoxPair->first;
	int *boxDispls = mbrAttr->rdispls;
	int *boxCounts = mbrAttr->recvCountArr;
	
	geos::io::WKTReader wktreader;
    Geometry *geom = NULL;
	
	//long bufOffset = 0;
	int *geomDispls = geomAttr->rdispls;
	//geomAttr->recvCountArr;
	
	for(int pid = 0; pid < args->numProcesses; pid++) { 
	    
	    int box_offset = boxDispls[pid];
	    int boxCount = boxCounts[pid];
	    
	    int geom_offset = geomDispls[pid];
	    long geomRecvBuf_offset = ((long)geom_offset) * PACKING_LEN;
	    
	    for(int boxCounter = box_offset; boxCounter < (box_offset + boxCount); boxCounter++) {
	    
	    	int geomStrLen = l1Boxes[boxCounter].vertStrLen;
        	int cellId = l1Boxes[boxCounter].cellId;
        	
        	char *wktHolder = (char*)malloc(sizeof(char) * geomStrLen);
      	    memcpy(wktHolder, l1RecvBuffer + geomRecvBuf_offset, geomStrLen);
      	    
      	    geomRecvBuf_offset = geomRecvBuf_offset + geomStrLen;
      	    
      	    try
            {
      	       geom = wktreader.read(string(wktHolder));
               free( wktHolder);
      	       	  
      	       if(cellToGeomMap->find(cellId) != cellToGeomMap->end()) {
      	       
      	     	  list<Geometry*>* geoms = cellToGeomMap->at(cellId);
      	       	  geoms->push_back(geom);
		       }
		       else {
		    
		          list<Geometry*>* geoms = new list<Geometry*>();
		          geoms->push_back(geom);
		       	  cellToGeomMap->insert(pair<int, list<Geometry*>* >(cellId, geoms));
		       }
      	    
      	       //cout<<geom->getNumGeometries() <<"_"<<geom->getNumPoints()<<", ";
      	    }
   			catch(exception &e)
   			{
      			cout<< e.what() <<endl;
      			continue;
      			//cout<<p.second<<endl;
      			//cout<<geom->toString()<<endl;
   			}
   	     } // end inner for   
     } // end outer for
     return cellToGeomMap;
}

map<int, list<Geometry*>* >*  WKTParser :: parseGeomFromLayerAllObj( pair< bbox*, int> *l1BoxPair,
																		 pair<char *,long>* l1RecvBuf)
{
   map<int, list<Geometry*>* >* cellToGeomMap = new map<int, list<Geometry*>* >();
   
   bbox *l1Boxes = l1BoxPair->first;
   int numBoxesL1 = l1BoxPair->second;
   
   char *l1RecvBuffer = l1RecvBuf->first;
   long numBufLenL1 = l1RecvBuf->second;
	
   geos::io::WKTReader wktreader;
   Geometry *geom = NULL;
   
   
   //void *memcpy(void *dest, const void *src, size_t n);
   long bufOffset = 0;
      
   for(int i = 0; i< numBoxesL1; i++) {
         
            int geomStrLen = l1Boxes[i].vertStrLen;
            int cellId = l1Boxes[i].cellId;
            
      	    char *wktHolder = (char*)malloc(sizeof(char) * geomStrLen);
      	    memcpy(wktHolder, l1RecvBuffer + bufOffset, geomStrLen);
      	   
        	bufOffset = bufOffset + geomStrLen;
      	    
      	    try
            {
      	       geom = wktreader.read(string(wktHolder));
               free( wktHolder);
      	       	  
      	       if(cellToGeomMap->find(cellId) != cellToGeomMap->end()) {
      	       
      	     	  list<Geometry*>* geoms = cellToGeomMap->at(cellId);
      	       	  geoms->push_back(geom);
		       }
		       else {
		    
		          list<Geometry*>* geoms = new list<Geometry*>();
		          geoms->push_back(geom);
		       	  cellToGeomMap->insert(pair<int, list<Geometry*>* >(cellId, geoms));
		       }
      	    
      	       //cout<<geom->getNumGeometries() <<"_"<<geom->getNumPoints()<<", ";
      	    }
   			catch(exception &e)
   			{
      			cout<< e.what() <<endl;
      			continue;
      			//cout<<p.second<<endl;
      			//cout<<geom->toString()<<endl;
   			 }
   	 }
     return cellToGeomMap;
}

/*
map<int, list<Geometry*>* >* WKTParser :: parseGeomFromRecvAllObj( pair< bbox*, int>* l1BoxPair,
  														pair<char *,long>* l1RecvBuf) 
{   
   map<int, list<Geometry*>* > *l1Geoms =  parseGeomFromLayerAllObj(l1BoxPair, l1RecvBuf);
   
   return l1Geoms;
}
*/

map<int, list<Geometry*>* >* WKTParser :: parseGeomFromRecvGrpByCellId( pair< bbox*, int>* l1BoxPair,
  														pair<char *,int>* l1RecvBuf) 
{   
   map<int, list<Geometry*>* > *l1Geoms =  parseGeomFromLayerGrpByCell(l1BoxPair, l1RecvBuf);
   
   return l1Geoms;
}

pair<map<int, list<Geometry*>* > *, map<int, list<Geometry*>* > * >* WKTParser :: 
								parseGeomFromRecvGrpByCellId( pair< pair< bbox*, int>* ,
									 pair< bbox*, int>* >* bBoxPair, pair<pair<char *,int>*, pair<char *,int>* >*recvBufPair) 
{
   /* Layer 1 */    
   pair< bbox*, int> *l1BoxPair = bBoxPair->first;
   
   pair<char *,int>* l1RecvBuf = recvBufPair->first;
      
   map<int, list<Geometry*>* > *l1Geoms =  parseGeomFromLayerGrpByCell(l1BoxPair, l1RecvBuf);
   
   /* Layer 2 */
   pair< bbox*, int> *l2BoxPair = bBoxPair->second;
   
   pair<char *,int>* l2RecvBuf = recvBufPair->second;

   map<int, list<Geometry*>* > *l2Geoms =  parseGeomFromLayerGrpByCell(l2BoxPair, l2RecvBuf);

   return new pair<map<int, list<Geometry*>* > *, map<int, list<Geometry*>* > * >(l1Geoms, l2Geoms);
}


// pair<map<int, list<Geometry*>* > *, map<int, list<Geometry*>* > * >* WKTParser :: parseGeomFromRecvGrpByCellId( pair< pair< bbox*, int>* ,
// 									 pair< bbox*, int>* >* bBoxPair, pair<pair<char *,int>*, pair<char *,int>* >*recvBufPair) 
// {
//    /* Layer 1 */    
//    pair< bbox*, int> *l1BoxPair = bBoxPair->first;
//    
//    pair<char *,int>* l1RecvBuf = recvBufPair->first;
//       
//    map<int, list<Geometry*>* > *l1Geoms =  parseGeomFromLayerGrpByCell(l1BoxPair, l1RecvBuf);
//    
//    /* Layer 2 */
//    pair< bbox*, int> *l2BoxPair = bBoxPair->second;
//    
//    pair<char *,int>* l2RecvBuf = recvBufPair->second;
// 
//    map<int, list<Geometry*>* > *l2Geoms =  parseGeomFromLayerGrpByCell(l2BoxPair, l2RecvBuf);
// 
//    return new pair<map<int, list<Geometry*>* > *, map<int, list<Geometry*>* > * >(l1Geoms, l2Geoms);
// }

map<int, list<Geometry*>* >*  WKTParser :: parseGeomFromLayerGrpByCell( pair< bbox*, int> *l1BoxPair,
																		 pair<char *,int>* l1RecvBuf)
{
   map<int, list<Geometry*>* >* cellToGeomMap = new map<int, list<Geometry*>* >();
   
   bbox *l1Boxes = l1BoxPair->first;
   int numBoxesL1 = l1BoxPair->second;
   
   char *l1RecvBuffer = l1RecvBuf->first;
   int numBufLenL1 = l1RecvBuf->second;
	
   geos::io::WKTReader wktreader;
   Geometry *geom = NULL;
   
  
   //void *memcpy(void *dest, const void *src, size_t n);
   int bufOffset = 0;
      
   for(int i = 0; i< numBoxesL1; i++) {
         try
         {
            int geomStrLen = l1Boxes[i].vertStrLen;
            int cellId = l1Boxes[i].cellId;
            
      	    char *wktHolder = (char*)malloc(sizeof(char) * geomStrLen);
      	    memcpy(wktHolder, l1RecvBuffer + bufOffset, geomStrLen);
      	  
      	    geom = wktreader.read(string(wktHolder));
            free( wktHolder);
      	    
      	    if(cellToGeomMap->find(cellId) != cellToGeomMap->end()) {
      	       
      	       list<Geometry*>* geoms = cellToGeomMap->at(cellId);
      	       geoms->push_back(geom);
		    }
		    else {
		    
		       list<Geometry*>* geoms = new list<Geometry*>();
		       geoms->push_back(geom);
		       cellToGeomMap->insert(pair<int, list<Geometry*>* >(cellId, geoms));
		    }
      	    
      	    //cout<<geom->getNumGeometries() <<"_"<<geom->getNumPoints()<<", ";
      	  
      	    bufOffset = bufOffset + geomStrLen;
   		}
   		catch(exception &e)
   		{
      		cout<< e.what() <<endl;
      		//cout<<p.second<<endl;
      		//cout<<geom->toString()<<endl;
   		}   	  
    }
 
   return cellToGeomMap;
}


list<Geometry*>*  WKTParser :: parseGeometryFromLayer( pair< bbox*, int> *l1BoxPair, pair<char *,int>* l1RecvBuf)
{
   bbox *l1Boxes = l1BoxPair->first;
   int numBoxesL1 = l1BoxPair->second;
   
   char *l1RecvBuffer = l1RecvBuf->first;
   int numBufLenL1 = l1RecvBuf->second;
	
   geos::io::WKTReader wktreader;
   Geometry *geom = NULL;
   
   list<Geometry*>* geoms = new list<Geometry*>();
  
   //void *memcpy(void *dest, const void *src, size_t n);
   int bufOffset = 0;
      
   for(int i = 0; i< numBoxesL1; i++) {
         try
         {
            int geomStrLen = l1Boxes[i].vertStrLen;
      	    char *wktHolder = (char*)malloc(sizeof(char) * geomStrLen);
      	    memcpy(wktHolder, l1RecvBuffer + bufOffset, geomStrLen);
      	  
      	    geom = wktreader.read(string(wktHolder));

      	    geoms->push_back(geom);

      	    //cout<<geom->getNumGeometries() <<"_"<<geom->getNumPoints()<<", ";
      	    free( wktHolder);
      	  
      	    bufOffset = bufOffset + geomStrLen;
   		}
   		catch(exception &e)
   		{
      		cout<< e.what() <<endl;
      		//cout<<p.second<<endl;
      		//cout<<geom->toString()<<endl;
   		}   	  
    }
   
   return geoms;
}

/*
pair<int, string> WKTParser :: extract(const string &str)
{
    int start = 0;

    while(start<str.length()) {
	  if(isalpha(str[start]))
		  break;
	  else
	     start++;
    }
    
    string wayIdStr = str.substr(0, start);
    int wayId = atoi(wayIdStr.c_str());
    
    int end = str.length()-1;
    while(end>0) {
	   if(str[end] == ')')
		   break;
	   else
		   end--;
    }

    int actual_length =  end - start + 1;

    string result = str.substr(start, actual_length);

    pair<int, string> p(wayId, result);

    return p;
}
*/