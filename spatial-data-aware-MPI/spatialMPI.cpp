#include "spatialMPI.h"

void SpatialMPI :: initSpatialMPI()
{
    initDataTypes();
    initOperators();
}

void SpatialMPI ::initOperators()
{ 
    MPI_Op_create(mbrUnion, 0, &MPI_UNION);
}

 // for multiple MBRs    
 void SpatialMPI :: mbrUnion(void *envIn, void *envInOut, int *len, MPI_Datatype *type) 
 {  
     double x1, x2, y1, y2;
     
     Envelope *in = (Envelope*)envIn;
     Envelope *inout = (Envelope*)envInOut;
     
     for(int i = 0; i < *len; i++) { 
     
        if(in->getMinX() < inout->getMinX())
            x1 = in->getMinX();
        else
            x1 = inout->getMinX();

		if(in->getMaxX() > inout->getMaxX())
            x2 = in->getMaxX();
        else
            x2 = inout->getMaxX();

 		if(in->getMinY() < inout->getMinY())
            y1 = in->getMinY();
        else
            y1 = inout->getMinY();

		if(in->getMaxY() > inout->getMaxY())
            y2 = in->getMaxY();
        else
            y2 = inout->getMaxY();
        
        *inout = Envelope(x1, x2, y1, y2);
        
        in++;
        inout++;
     }
  }


MPI_Datatype SpatialMPI :: createRectType()
{
    const int nitems=5;
    int          blocklengths[5] = {1,1,1,1,1};
    MPI_Datatype types[5] = {MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_INT};
  
    MPI_Aint     offsets[5];
    
    offsets[0] = offsetof(Rect, x1);
    offsets[1] = offsetof(Rect, x2);
    offsets[2] = offsetof(Rect, y1);
    offsets[3] = offsetof(Rect, y2);
    offsets[4] = offsetof(Rect, id);
    
    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &MPI_RECT);
    MPI_Type_commit(&MPI_RECT);
    
    return MPI_RECT;
}

MPI_Datatype SpatialMPI :: createPointType()
{
    const int nitems = 3;
    int          blocklengths[3] = {1,1,1};
    MPI_Datatype types[3] = {MPI_DOUBLE, MPI_DOUBLE, MPI_INT};
  
    MPI_Aint     offsets[3];
    
    offsets[0] = offsetof(Point2D, x1);
    offsets[1] = offsetof(Point2D, y1);
    offsets[2] = offsetof(Point2D, id);
    
    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &MPI_POINT);
    MPI_Type_commit(&MPI_POINT);
    
    return MPI_POINT;
}

MPI_Datatype SpatialMPI :: createLineType()
{
    const int nitems=5;
    int          blocklengths[5] = {1,1,1,1,1};
    MPI_Datatype types[5] = {MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_INT};
  
    MPI_Aint     offsets[5];
    
    offsets[0] = offsetof(Line, x1);
    offsets[1] = offsetof(Line, x2);
    offsets[2] = offsetof(Line, y1);
    offsets[3] = offsetof(Line, y2);
    offsets[4] = offsetof(Line, id);
    
    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &MPI_LINE);
    MPI_Type_commit(&MPI_LINE);
    
    return MPI_LINE;
}

void SpatialMPI :: initDataTypes()
{
  createRectType();
  createPointType();
  createLineType();
}
