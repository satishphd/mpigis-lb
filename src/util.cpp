#include "geom_util/util.h"
#include "geos/operation/union/CascadedPolygonUnion.h"

int GeometryUtility :: estimateLoad(map<Geometry*, vector<void *> >*mbrQueryResultMap)
{
   long candidates = 0;
   map<Geometry*, vector<void *> >::iterator it;
       
   for (it = mbrQueryResultMap->begin(); it != mbrQueryResultMap->end(); ++it) {
           
       vector<void *> geoms = it->second;
       candidates += geoms.size();
   }
   long totalCandidates = 0;
   MPI_Allreduce(&candidates, &totalCandidates, 1, MPI_LONG, MPI_SUM, MPI_COMM_WORLD);

   int normalizedValue = (candidates * 100)/totalCandidates;
   cout<<"load estimator "<<normalizedValue<<endl;
   return normalizedValue;
}

Geometry* GeometryUtility :: unionGeoms(list<Geometry*> *geoms) 
{
  list<Geometry *>::const_iterator i;
  vector<Polygon *> *polyVec = new vector<Polygon *>();
  
  for(i = geoms->begin(); i!= geoms->end(); i++) {
    Geometry *geom = *i;
    //cout<<geom->getGeometryType();
    if(geom->getGeometryTypeId() == GEOS_POLYGON) 
    {
      //Polygon *poly = (Polygon *)geom;
      Polygon *poly = dynamic_cast<Polygon*>(geom);
      if(poly != NULL)
         polyVec->push_back(poly);
    }
  }
  
  printf("#polygon = %d \n",polyVec->size());
  fflush(stdout);
  
  using geos::operation::geounion::CascadedPolygonUnion;
 
  Geometry *unionGeom = NULL;
  // Polygon *poly = polyVec->front();
//   if(poly == NULL)
//     cout<<"Yes";
//   else
//   {
//    printf(" Point # %d ", poly->getNumPoints());
//    fflush(stdout);
//   } 
   try
   {
      unionGeom = CascadedPolygonUnion :: Union(polyVec);
   }
   catch(exception &ex)
   {
      //cout<<ex.what()<<endl;
   }
   
   return unionGeom;
 /*
   list<Geometry *>::const_iterator i;
   Geometry *firstGeom = geoms->front();
  
   for(i = next(geoms->begin()); i!= geoms->end(); i++) {
       Geometry *nextGeom = *i;
       try
       {
        firstGeom = firstGeom->Union(nextGeom);
       }
       catch(exception &ex)
       {
         cout<<ex.what()<<endl;
       }
       
   }
  cout<<firstGeom->toString();
  return firstGeom;
  */
}

Envelope GeometryUtility :: getMBR(list<Geometry*> *geoms) 
{
  list<Geometry *>::const_iterator i;
  
  double maxX, maxY;
  double minX, minY;
  
  //X axis : -170 to 193
  minX = 9999.99;
  maxX = -9999.99;
  //  Y axis : -64 to 85
  minY = 1000.00;
  maxY = -1000.99;
  
  for(i = geoms->begin(); i!= geoms->end(); i++) {
    
    Geometry *geom = *i;
    const Envelope *envptr = geom->getEnvelopeInternal();

    if(minX > envptr->getMinX())	
      minX = envptr->getMinX();
      
    if(maxX < envptr->getMaxX())
      maxX = envptr->getMaxX();  
      
    if(minY > envptr->getMinY())
      minY = envptr->getMinY();
      
    if(maxY < envptr->getMaxY())
      maxY = envptr->getMaxY();  
  }
  
  return Envelope(minX, maxX, minY, maxY);
}