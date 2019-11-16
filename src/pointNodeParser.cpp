#include "parser/pointNodeParser.h"

Geometry* PointNodeParser :: parseString(const pair<int, string*> &p)
{
  return NULL;
}

// nodeId, longitude, latitude, tags
list<Geometry*>* PointNodeParser :: parse(const FileSplits &split) 
{
  list<Geometry*> *geoms = new list<Geometry*>();
  
  list<string>::const_iterator i;

  list<string> *contents = split.getContents();
  
 //  #ifdef DBUGROAD    
  int counter  = 0;
//   #endif
  
  for(i = contents->begin(); i!= contents->end(); i++) {
      Geometry *geom = extract(*i);

      //#ifdef DBUGROAD    
      //counter++;
      
      //cout<<counter<<" ";
      //#endif
      
      if(geom != NULL)
        geoms->push_back(geom);
  }
  
  return geoms;
}

/*
Geometry* PointNodeParser :: extract1(const string &str) 
{
   string tag;
   long nodeId;
   double longi, lat;
   string tags;
	// nodeId, longitude, latitude, tags

	//std::ifstream in("allnodesSmall.txt");
	stringstream pointString(str);

	if (pointString >> nodeId >> longi >> lat >> tag)
	{
  		//std::cout << tag<< endl;
  		Coordinate cord(longi, lat);
  		
  		geos::geom::GeometryFactory factory;
 	    Point *pt = factory.createPoint(cord);
  		
  		// pointNode *pNode = new pointNode(tag, nodeId);             
//  	    pt->setUserData(pNode);
 	    
 	    return pt;
	} 
	return NULL;
}
*/

Geometry* PointNodeParser :: extract(const string &str) 
{ 
	char *cstr = new char[str.length()+1];
	  
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
    
    if(tokens.size() >3) {
       try {
          long nodeId = std::stol(*tokens[0]);
          double start_longitude = std::stof(*tokens[1]);
    	  double start_latitude = std::stof(*tokens[2]);
          
          Coordinate cord(start_longitude, start_latitude);
          
          Point *pt = factory.createPoint(cord);
    	  
          //string *tag = tokens.at(3);

          // pointNode *pNode = new pointNode(tag, nodeId);             
//           pt->setUserData(pNode);
          
          return pt;
      } 
      catch(exception &e)
      {
         cout<< e.what() <<endl;
         delete[] cstr;
         //cout<<p.second<<endl;
         //cout<<geom->toString()<<endl;
         return NULL;
      }
    }
    delete[] cstr;
}


// node_id	longitude	latitude	tags
// 13	9.5130058	51.3731042	[uic_ref#713807,shelter#yes,highway#bus_stop,name#Bleichplatz,network#NVV]
// 19	-0.20698	51.9458753	[ref#SG4 90,collection_times#Mo-Fr 16:30; Sa 09:45,box_type#lamp_box,amenity#post_box]
// ...
// 3283265148	-86.8137397	33.2540646	[]
// 3283265149	-86.8138263	33.254124	[]
