#include "join/join.h"
#include<mpi.h>

Geometry* Join:: convertEnvToGeom(Envelope *env) 
{
    	CoordinateArraySequence* coords = new geos::geom::CoordinateArraySequence();
    	
    	coords->add(Coordinate(env->getMinX(), env->getMinY() ));
    	coords->add(Coordinate(env->getMaxX(), env->getMinY() ));
 	    coords->add(Coordinate(env->getMaxX(), env->getMaxY()));
		coords->add(Coordinate(env->getMinX(), env->getMaxY()));
    	
    	coords->add(Coordinate(env->getMinX(), env->getMinY() ));
    	    	
		// Create exterior ring
        LinearRing* ring = factory.createLinearRing(coords);
		
        //tut::ensure("simple ext", exterior->isSimple() );
		
	   // Geometry* poly = factory.createGeometry(ring);    	    	
    	//Polygon* poly = factory.createPolygon(ring, NULL);    	    	
    	return ring;
}

// Geom to vector of envs map
void Join :: joinGeomEnv(map<Geometry*, vector<void *> >*GeomToEnvsMap)
{
   int counter = 0;
   int inner = 0;
   //list<pair<Geometry*, Geometry*> >* joinPairs = new list<pair<Geometry*, Geometry*> >();  
 
   map<Geometry*, vector<void *> >::iterator itOuter;
       
   for (itOuter = GeomToEnvsMap->begin(); itOuter != GeomToEnvsMap->end(); ++itOuter) {
           // std::cout << it->first << " => " << it->second->size() << '\n';
 	   inner = 0;
       Geometry *poly1 = itOuter->first;
    
       vector<void *> layer2 = itOuter->second;
       
       for(vector<void *>::iterator it = layer2.begin() ; it != layer2.end(); ++it) {
             void *poly2Ptr = *it;
             
             Envelope* env2 = (Envelope*)poly2Ptr;
             
             if(env2 == NULL)
               return;
               
             try {
                
                Geometry* geom2 = convertEnvToGeom(env2);
                if(geom2!= nullptr && !geom2->isEmpty() && poly1->intersects(geom2)) { 
                   counter++;
                   //joinPairs->push_back(pair<Geometry*, Geometry*>(poly1, poly2));
                 
                   //cout<<poly1->getEnvelopeInternal()->toString()<<endl<<endl<<poly2->getEnvelopeInternal()->toString()<<endl<<endl;
                   //cerr<<" "<<poly2->getGeometryType()<<" "<<poly2->getNumPoints()<<endl;
                  }
              } catch(exception ex) {
                //cerr<< " Intersects Error ";
              } 
          }
   }
   cout<<" Geom-Env-Join Cnt "<<counter<<endl;
   
   int total;
   MPI_Allreduce(&counter, &total, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
   cout<<" total "<<total<<endl;
}

long Join :: join(map<Envelope*, vector<void *> >* intersectionMap)
{
   //list<pair<Envelope*, Envelope*> >* joinPairs = new list<pair<Envelope*, Envelope*> >();
   long joinPairs = 0;
   
   map<Envelope*, vector<void *> >::iterator itOuter;
       
   for (itOuter = intersectionMap->begin(); itOuter != intersectionMap->end(); ++itOuter) {
        Envelope *env1 = itOuter->first;
        vector<void *> layer2 = itOuter->second;

        joinPairs += layer2.size();
        
        // for(vector<void *>::iterator it = layer2.begin() ; it != layer2.end(); ++it) {
//              void *poly2Ptr = *it;
//              Envelope* env2 = (Envelope*)poly2Ptr;
//              
//              if(env1->intersects(env2))
//              {
//                joinPairs++;
//              }
//                //joinPairs->push_back(pair<Envelope*, Envelope*>(env1, env2));
//         }
   }
   return joinPairs;
}

list<pair<Geometry*, Geometry*> >* Join :: join(map<Geometry*, vector<void *> >* intersectionMap)
{
   int counter = 0;
   int inner = 0;
   list<pair<Geometry*, Geometry*> >* joinPairs = new list<pair<Geometry*, Geometry*> >();
 
   prep::PreparedGeometryFactory pgf;        
 
   map<Geometry*, vector<void *> >::iterator itOuter;
       
   for (itOuter = intersectionMap->begin(); itOuter != intersectionMap->end(); ++itOuter) {
           // std::cout << it->first << " => " << it->second->size() << '\n';
 	   inner = 0;
       Geometry *poly1 = itOuter->first;
     
       const prep::PreparedGeometry* pgpoly1 = pgf.create(poly1);

       if(pgpoly1 == nullptr)
         continue;

       vector<void *> layer2 = itOuter->second;

       for(vector<void *>::iterator it = layer2.begin() ; it != layer2.end(); ++it) {
             void *poly2Ptr = *it;
             
             Geometry* poly2 = (Geometry*)poly2Ptr;
             //cerr<<poly2->getGeometryType()<<": "<<poly2->getNumPoints()<<endl;
             //cerr<<poly2->toString()<<endl<<endl<<endl;
             try {
                if(poly2!= nullptr && !poly2->isEmpty() && pgpoly1->intersects(poly2)) { 
                   
                   joinPairs->push_back(pair<Geometry*, Geometry*>(poly1, poly2));
                 
                   //cout<<poly1->getEnvelopeInternal()->toString()<<endl<<endl<<poly2->getEnvelopeInternal()->toString()<<endl<<endl;
                   //cerr<<" "<<poly2->getGeometryType()<<" "<<poly2->getNumPoints()<<endl;
                  }
              }
              // catch (const IllegalArgumentException& ill) {
// 				
// 			  }
              catch(exception ex) {
                //cerr<< " Intersects Error ";
              } 
          }
    
          pgf.destroy(pgpoly1);
   }
   return joinPairs;
}