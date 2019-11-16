#include "overlay/overlay.h"

void Overlay :: closeFile()
{
  if (wktfile.is_open())
    wktfile.close();
}

bool Overlay :: initOutputFile(int pid)
{
  string idStr = to_string(pid);
  string filename = idStr + ".wkt";
  
  wktfile.open(filename);
  
  if (wktfile.is_open()) {
    cerr<<"File opened "<<filename<<endl;
    return true;
  }
  else
  {
    cerr<< "Unable to open file";
    return false;
  }
   // wktfile << "This is a line.\n";
}

void  Overlay :: writeGeoms(list<Geometry*> *lst) {
  //list<Geometry*>:: iterator itr;
  
  for (list<Geometry*>::iterator itr = lst->begin(); itr != lst->end(); ++itr) {
      Geometry *geom = *itr;
      wktfile<<geom->toString()<<endl;
  }
}

void Overlay :: write(Geometry *geom) 
{  
   if(geom != nullptr && !geom->isEmpty())
      wktfile<<geom->toString()<<endl;
}

list<Geometry*>* Overlay :: overlay(map<Geometry*, vector<void *> >* intersectionMap)
{
   list<Geometry*>* overlayPairs = new list<Geometry*>();
  
   prep::PreparedGeometryFactory pgf;        
  
   map<Geometry*, vector<void *> >::iterator itOuter;
   Geometry *output;
      
   for (itOuter = intersectionMap->begin(); itOuter != intersectionMap->end(); ++itOuter) {
           // std::cout << it->first << " => " << it->second->size() << '\n';
 	
       Geometry *poly1 = itOuter->first;

       if(poly1 == nullptr) {
        cerr<<"Polygon 1 is null "<<endl;
        continue;
       }
    
       const prep::PreparedGeometry* pgpoly1 = pgf.create(poly1);
   
       vector<void *> layer2 = itOuter->second;
    
       try {
          for(vector<void *>::iterator it = layer2.begin() ; it != layer2.end(); ++it) {
             void *poly2Ptr = *it;
             Geometry* poly2 = (Geometry*)poly2Ptr;
            
             if(pgpoly1->intersects(poly2)) { 
             
                   output = poly1->intersection(poly2);
                   //write(output);
                   overlayPairs->push_back(output);
                 
                 //cout<<poly1->getEnvelopeInternal()->toString()<<endl<<endl<<poly2->getEnvelopeInternal()->toString()<<endl<<endl;
                 //cerr<<"Try "<<poly1->getNumPoints()<<" "<<poly2->getNumPoints()<<endl;
             }
           }
           
           pgf.destroy(pgpoly1);
           
        } catch(exception ex) {
             //cerr<< " Overlay Error ";
        }
   }
   return overlayPairs;
}