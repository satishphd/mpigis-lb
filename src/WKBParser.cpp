#include "parser/WKBParser.h"

Geometry* WKBParser :: parseString(const std::pair<int, string*> &p) 
{
  geos::io::WKBReader wkbreader;
  Geometry *geom = NULL;
  
  try
  {
    geom = wkbreader.readHEX(*p.second);
    
    //ExtraInfo *extraInfo = new ExtraInfo();
    //geom->setUserData(extraInfo);
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

// geom	gid	x1	y1	x2	y2	statefp	countyfp
pair<int, string*> WKBParser :: extract(const string &str) 
{ 
	char *cstr = new char [str.length()+1];
	  
    std::strcpy (cstr, str.c_str());
    vector<string*> tokens;
    // cstr now contains a c-string copy of str

    char *p = std::strtok (cstr,",");
    
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

list<Geometry*>* WKBParser :: parse(const FileSplits &split, int pid, int P) 
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

bool WKBParser :: checkForEmptyCollection(Geometry *iGeom)
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