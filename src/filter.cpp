#include "filter/filter.h"


 int Filter :: test(list<Tile*>* xTiles, Geometry* y)
 {

   list<Tile*>* yTiles = getTiles(y);
   int count = 0;
     
   for (list<Tile*>::iterator xIt = xTiles->begin(); xIt != xTiles->end(); ++xIt)
   {
        Tile *tx = *xIt;
		for (list<Tile*>::iterator yIt = yTiles->begin(); yIt != yTiles->end(); ++yIt) 
		{
            Tile *ty = *yIt;
            if( tx->tileEnv.intersects(ty->tileEnv) )	
            {
               count++;
            }	
		}  
   }
   
   return count;
 }
 
 
 list<Tile*>* Filter :: getTiles(Geometry *g)
 {
     list<Tile*>* tileList = new list<Tile*>();
     
     int vCount = g->getNumPoints();
     
     if(vCount < NUM_TILES)
     {
        Tile *t = createTile(g, 0, (vCount-1));
        tileList->push_back(t);
        return tileList;
     }
     
     int chunkSize = vCount/NUM_TILES;
     
     for(int i = 0; i < NUM_TILES; i++)
     {
       int start = i * chunkSize;
       int end = (i+1) * chunkSize;
       
       if(i == (NUM_TILES-1))
         end = vCount - 1;
         
       Tile *t = createTile(g, start, end);
       tileList->push_back(t);
     }
     
     return tileList;
 }
 
 Tile* Filter :: createTile(Geometry* x, int first, int last)
 {
      CoordinateSequence *seq =	x->getCoordinates(); 
      
      Coordinate minPt = seq->getAt(first);

 	  double minX = minPt.x;
 	  double minY = minPt.y; 
 	  double maxX = minX;
 	  double maxY = minY;
      
      for( int index = first+1; index <= last; index++)
 	  {
 	    
 	    Coordinate pt = seq->getAt(index);
 	    
 	    if(pt.x < minX)
 		   minX = pt.x;
 		   
 		if(pt.y < minY)
 		   minY = pt.y;
 	   
 	    if(pt.x > maxX)
 		   maxX = pt.x;
 	   
 	    if(pt.y > maxY)
 		   maxY = pt.y;
 	  }
      /* Envelope (double x1, double x2, double y1, double y2)
          x1 	the first x-value
		  x2 	the second x-value
		  y1 	the first y-value
		  y2 	the second y-value
      */
      
 	  Envelope r(minX, maxX, minY, maxY);

 	  Tile *t = new Tile(first, last);
 	  t->setRect(r);
 	  
 	  return t;
   }
   
     /*
 	  Point2D minPt = ptArr[first];
 	  double minX = minPt.getX();
 	  double minY = minPt.getY(); 
 	  double maxX = minX;
 	  double maxY = minY;
 	  
 	   for( int index = first; index <= last; index++)
 	   {
 	    Point2D pt = ptArr[index];
 	    if(pt.getX() < minX)
 		   minX = pt.getX();
 	   
 	   if(pt.getY() < minY)
 		   minY = pt.getY();
 	   
 	   if(pt.getX() > maxX)
 		   maxX = pt.getX();
 	   
 	   if(pt.getY() > maxY)
 		   maxY = pt.getY();
 	 }
 	  Rectangle2D r = new Rectangle2D.Double();
 	  r.setRect(minX, minY, (maxX - minX), (maxY - minY));
 	  Tile t = new Tile(first, last);
 	  t.setRect(r);
 	  
 	  return t; */
