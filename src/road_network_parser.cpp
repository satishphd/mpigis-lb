#include "parser/Parser.h"
#include "parser/road_network_parser.h"

Geometry* RoadNetworkParser :: parseString(const pair<int, string*> &p)
{
  return NULL;
}

Geometry* RoadNetworkParser :: extract(const string &str) 
{ 
	char *cstr = new char[str.length()+1];
	  
    std::strcpy (cstr, str.c_str());
    vector<char *> tokens;
    // cstr now contains a c-string copy of str

    char *p = std::strtok (cstr,",");
    
    while (p!=0)
    {
      //std::cout << p << '\n';
      //string token(p);
      tokens.push_back(p);
      
      p = std::strtok(NULL,",");
    }
        //delete[] cstr;
        //cout<<"Tokens # "<<tokens.size()<<endl;
   
    if(tokens.size()>8) { 
       try {
    		 geos::geom::GeometryFactory factory1;
    	 	 CoordinateArraySequence* coords = new geos::geom::CoordinateArraySequence();
        
         	//   printf("[2] %s \n",tokens.at(2));

       	 	double start_longitude = atof(tokens.at(2));
    	 	double start_latitude = atof(tokens.at(3));
    	 	coords->add(Coordinate(start_longitude, start_latitude));
    
    	 	double end_longitude = atof(tokens.at(5));
    	 	double end_latitude = atof(tokens.at(6));
    	 	coords->add(Coordinate(end_longitude, end_latitude));

    	 	LineString *line = factory1.createLineString(coords);
            
            RoadNode *node = new RoadNode(atol(tokens[0]), atol(tokens[1]), atol(tokens[4]), atol(tokens[7]), string(tokens[8]));
            
            line->setUserData(node);	
			
			return line;
       }
       catch(exception &e)
       {
         cout<< e.what() <<endl;
         //cout<<p.second<<endl;
         //cout<<geom->toString()<<endl;
         return NULL;
       }
    }

  return NULL;
}

//2100,338642,-2.3634904,51.3845649,14718707,-2.3634254,51.3843983,210,{"highway"="primary"}{"ref"="A4"}{"name"="Gay Street"}
//2101,14718707,-2.3634254,51.3843983,471761118,-2.3632013,51.3838178,210,{"highway"="primary"}{"ref"="A4"}{"name"="Gay Street"}

// edgeId, start_node_id, longitude, latitude, end_node_id, longitude, latitude, wayId, tags
list<Geometry*>* RoadNetworkParser :: parse(const FileSplits &split) 
{
  list<Geometry*> *geoms = new list<Geometry*>();
  
  list<string>::const_iterator i;

  list<string> *contents = split.getContents();
  
 //  #ifdef DBUGROAD    
//   int counter  = 0;
//   #endif
  
  for(i = contents->begin(); i!= contents->end(); i++) {
      Geometry *geom = extract(*i);

      #ifdef DBUGROAD    
      counter++;
      
      cout<<counter<<" ";
      #endif
      
      if(geom != NULL)
        geoms->push_back(geom);
  }
  
  return geoms;
}
