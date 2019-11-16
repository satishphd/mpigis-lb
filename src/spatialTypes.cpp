#include "mpiTypes/mpitype.h"

/*
void SpatialTypes :: createOperatorType() 
{     
  // create contiguous derived data type
  MPI_Type_contiguous(4, MPI_DOUBLE, &mbrtype);
  MPI_Type_commit(&mbrtype);
  

  MPI_Op_create(mbrUnion, 0, &boxUnion);
  //return boxUnion;
}*/
	
MPI_Datatype SpatialTypes :: create_MPI_CHARS(int PACKING_LEN)
{
   // To send more than 2GB of character data
   MPI_Datatype MPI_CHARs_NEW;
   MPI_Type_contiguous(PACKING_LEN, MPI_CHAR, &MPI_CHARs_NEW);
   MPI_Type_commit(&MPI_CHARs_NEW);
   
   return MPI_CHARs_NEW;
}
	
		
 MPI_Datatype SpatialTypes :: createCollectiveInfoType() 
 {
    const int nitems=2;
    int          blocklengths[2] = {1,1};
    MPI_Datatype types[2] = { MPI_INT, MPI_INT};
    MPI_Datatype MPI_COLL_INFO;
    MPI_Aint     offsets[2];
   
    offsets[0] = offsetof(CollectiveInfo, layer1BufLen);
    offsets[1] = offsetof(CollectiveInfo, layer2BufLen);
    
    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &MPI_COLL_INFO);
    MPI_Type_commit(&MPI_COLL_INFO);
    return MPI_COLL_INFO;

 }	
 
 MPI_Datatype SpatialTypes :: createAllObjeCollectiveInfoType() 
 {
    const int nitems=3;
    int          blocklengths[3] = {1,1,1};
    MPI_Datatype types[3] = { MPI_INT, MPI_LONG, MPI_INT};
    MPI_Datatype MPI_ALL_COLL_INFO;
    MPI_Aint     offsets[3];
   
    offsets[0] = offsetof(AllObjCollectiveInfo, derivedLayer1BufLen);
    offsets[1] = offsetof(AllObjCollectiveInfo, realLayer1BufLen);
    offsets[2] = offsetof(AllObjCollectiveInfo, layer2BufLen);
    
    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &MPI_ALL_COLL_INFO);
    MPI_Type_commit(&MPI_ALL_COLL_INFO);
    return MPI_ALL_COLL_INFO;

 }
		
//  long edgeId;
//  long startNodeId;
//  long endNodeId;
//  long wayid;
MPI_Datatype SpatialTypes :: createRoadBoxType()
{
    /* create a type for struct ROAD box */
    const int nitems = 9;
    int          blocklengths[9] = {1,1,1,1,1,1,1,1,1};
    MPI_Datatype types[9] = {MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_LONG, MPI_LONG, MPI_LONG, MPI_LONG,MPI_INT};
    MPI_Datatype MPI_ROAD_BOX;
    MPI_Aint     offsets[9];
    //struct bbox box;
    offsets[0] = offsetof(roadbox, x1);
    offsets[1] = offsetof(roadbox, x2);
    offsets[2] = offsetof(roadbox, y1);
    offsets[3] = offsetof(roadbox, y2);
    offsets[4] = offsetof(roadbox, edgeId);
    offsets[5] = offsetof(roadbox, startNodeId);
    offsets[6] = offsetof(roadbox, endNodeId);
    offsets[7] = offsetof(roadbox, wayId);
    offsets[8] = offsetof(roadbox, cellId);
        
    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &MPI_ROAD_BOX);
    MPI_Type_commit(&MPI_ROAD_BOX);
    return MPI_ROAD_BOX;
}

MPI_Datatype SpatialTypes :: createBoxType()
{
    /* create a type for struct box */
    const int nitems=7;
    int          blocklengths[7] = {1,1,1,1,1,1,1};
    MPI_Datatype types[7] = {MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_INT, MPI_INT, MPI_INT};
    MPI_Datatype MPI_BOX;
    MPI_Aint     offsets[7];
    //struct bbox box;
    offsets[0] = offsetof(bbox, x1);
    offsets[1] = offsetof(bbox, x2);
    offsets[2] = offsetof(bbox, y1);
    offsets[3] = offsetof(bbox, y2);
    offsets[4] = offsetof(bbox, id);
    offsets[5] = offsetof(bbox, cellId);
    offsets[6] = offsetof(bbox, vertStrLen);
    
    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &MPI_BOX);
    MPI_Type_commit(&MPI_BOX);
    return MPI_BOX;
}

Envelope SpatialTypes :: reduceByUnion(Envelope *env) {

   //createOperatorType();
    
  Envelope recv;
   
   //MPI_Reduce(env, &recv, 1, mbrtype, boxUnion, 0, MPI_COMM_WORLD);
   
  MPI_Op boxUnion;
  MPI_Datatype mbrtype;
  
  MPI_Type_contiguous(4, MPI_DOUBLE, &mbrtype);
  MPI_Type_commit(&mbrtype);
  
  MPI_Op_create(mbrUnion, 0, &boxUnion);
  
  //MPI_Reduce(env, &recv, 1, mbrtype, boxUnion, 0, MPI_COMM_WORLD);
  // int MPI_Allreduce(const void *sendbuf, void *recvbuf, int count,
  //                   MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
  
  MPI_Allreduce(env, &recv, 1, mbrtype, boxUnion, MPI_COMM_WORLD);
  
  return recv;
}
 //void SpatialTypes :: createUnionOp(){ }

 void SpatialTypes :: mbrUnion(Envelope *in, Envelope *inout, int *len, MPI_Datatype *type) 
 {  
     
        double x1, x2, y1, y2;
     
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
       
  }
 /* for multiple MBRs */   
 void SpatialTypes :: mbrUnionS(Envelope *in, Envelope *inout, int *len, MPI_Datatype *type) 
 {  
     double x1, x2, y1, y2;
     
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
  
Envelope SpatialTypes :: reduceByUnionTest(Envelope *env) 
{

   //createOperatorType();
    
  Envelope recv;
   
   //MPI_Reduce(env, &recv, 1, mbrtype, boxUnion, 0, MPI_COMM_WORLD);
   
  MPI_Op boxUnion;
  MPI_Datatype mbrtype;
  
  MPI_Type_contiguous(4, MPI_DOUBLE, &mbrtype);
  MPI_Type_commit(&mbrtype);
  
  MPI_Op_create(mbrUnionS, 0, &boxUnion);
  
  //MPI_Reduce(env, &recv, 1, mbrtype, boxUnion, 0, MPI_COMM_WORLD);
  // int MPI_Allreduce(const void *sendbuf, void *recvbuf, int count,
  //                   MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
  
  MPI_Allreduce(env, &recv, 1, mbrtype, boxUnion, MPI_COMM_WORLD);
  
  return recv;
}
