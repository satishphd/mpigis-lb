#include "filePartition/MPIReadBinaryAllPointNodes.h"

MPI_Datatype ReadBinaryPointFile :: createPtNodeType()
{
	const int nitems = 4;
    int          blocklengths[4] = {1,1,1,32};
    MPI_Datatype types[4] = {MPI_DOUBLE, MPI_DOUBLE, MPI_LONG, MPI_CHAR};
    MPI_Datatype MPI_PTNODE;
    MPI_Aint     offsets[4];
    offsets[0] = offsetof(pointNode, longi);
    offsets[1] = offsetof(pointNode, lati);
    offsets[2] = offsetof(pointNode, nodeId);
    offsets[3] = offsetof(pointNode, tag);
    
    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &MPI_PTNODE);
    MPI_Type_commit(&MPI_PTNODE);
    return MPI_PTNODE;
}

pair<int, pointNode*>* ReadBinaryPointFile :: readPoints_NonContig(char *pointPath)
 {
  int nprocs, rank; 

  MPI_Comm_size(MPI_COMM_WORLD, &nprocs); 
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
  
    MPI_Info myinfo;
    MPI_Info_create(&myinfo);
    MPI_Info_set(myinfo, "access_style", "read_once,sequential"); 
    MPI_Info_set(myinfo, "collective_buffering", "true"); 
    MPI_Info_set(myinfo, "romio_cb_read", "enable");
  
  MPI_File fh;
   
  MPI_File_open(MPI_COMM_WORLD, pointPath, MPI_MODE_RDONLY, myinfo, &fh);
  
  //cout<<"In readPoints_NonContig"<<endl;
  
  MPI_Datatype filetype;
  MPI_Offset filesize;
  
  MPI_File_get_size(fh, &filesize);
  
  //filesize = 1048576*128; // 128 MB
  
  MPI_Offset bufsize = filesize/nprocs;
  
  if(bufsize < 0)
	  cerr<<"Negative file size "<<endl; 
  
  pointNode *buf = (pointNode *) malloc(bufsize);
   
  if(buf == NULL)
    cerr<<"Error no memory"<<endl; 

  MPI_Offset npoints = bufsize/sizeof(pointNode);
  
  if(rank == 0) {
	  cout<<"Total filesize "<<filesize<<endl;
  
	  cout<<"floats per process "<<npoints<<endl;
  }
  
  MPI_Datatype MPI_PTNODE = createPtNodeType();
  
// int MPI_Type_vector(int count, int blocklength, int stride, MPI_Datatype oldtype, MPI_Datatype *newtype);

  MPI_Type_vector(npoints/POINT_NODES_PER_BLK, POINT_NODES_PER_BLK, 
                  POINT_NODES_PER_BLK*nprocs, MPI_PTNODE, &filetype);
                  							  
  MPI_Type_commit(&filetype);
  MPI_File_set_view(fh, POINT_NODES_PER_BLK*sizeof(pointNode)*rank, MPI_PTNODE, 
	      filetype, "native", myinfo);

  MPI_Status status;
  
  int ierr = MPI_File_read_all(fh, buf, npoints, MPI_PTNODE, &status);

  char err_buffer[MPI_MAX_ERROR_STRING];
  int resultlen;
  
  	  if (ierr != MPI_SUCCESS) {
           MPI_Error_string(ierr,err_buffer,&resultlen);
           fprintf(stderr,err_buffer);
           MPI_Finalize();             /* abort*/
       }
  
  		//MPI_File_write_all(fh, buf, nfloats, MPI_FLOAT, &status);
  
  		// After receiving the message, check the status to determine
    	// how many numbers were actually received
	  int number;
  	  MPI_Get_count(&status, MPI_PTNODE, &number);
      
      if(number < 0)
         cout<<"Error "<<endl;
         
      MPI_Offset readCount = number;
  	 // Print off the amount of numbers, and also print additional
 	 // information in the status object

	  MPI_Offset specified = (bufsize/sizeof(pointNode));
	
	if ( readCount <  specified) 
	   cout<<rank<<" # Floats read "<<readCount<<" #specified "<<specified<<endl;
		  	  //printf("id = %d actual = %d specified = %d \n", rank, number, bufsize/sizeof(float));

  
  //list<Geometry*>* geoms = parseMBRs(buf, nfloats);
 
  // list<Envelope*>*envs = parseEnvs(buf, npoints);
  //cout<<"# envelopes "<<envs->size()<<endl;

  MPI_File_close(&fh);
  
//  buf[nbytes] = '\0';
  //countNumLines(buf, nbytes);

  MPI_Type_free(&filetype);
  MPI_Type_free(&MPI_PTNODE);  
  
  // don't free buf
  return new pair<int, pointNode*>(npoints, buf);
 }


pair<int, pointNode*>* ReadBinaryPointFile :: readPoints_Contiguous(char *pointPath)
{
  int nprocs, rank; 

  MPI_Comm_size(MPI_COMM_WORLD, &nprocs); 
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); 
  
    MPI_Info myinfo;
    MPI_Info_create(&myinfo);
    MPI_Info_set(myinfo, "access_style", "read_once,sequential"); 
    MPI_Info_set(myinfo, "collective_buffering", "true"); 
    MPI_Info_set(myinfo, "romio_cb_read", "enable");
  
  MPI_File fh;
   
  MPI_File_open(MPI_COMM_WORLD, pointPath, MPI_MODE_RDONLY, myinfo, &fh);
  
  MPI_Offset filesize;
  
  MPI_File_get_size(fh, &filesize);
  
  int bufSize = filesize/nprocs;
  
  if(bufSize < 0)
	  cerr<<"Negative file size "<<endl; 
  
  pointNode *buf = (pointNode *) malloc(bufSize);
  
  if(buf == NULL)
    cerr<<"Error no memory"<<endl; 
  
  int nPointNodes = bufSize/sizeof(pointNode);
  
  if(rank == 0) {
	  cout<<"Total filesize "<<filesize<<endl;
  
	  cout<<"pt Nodes per process "<<nPointNodes<<endl;
  }

  MPI_Status status;
  MPI_Offset start = (MPI_Offset)(nPointNodes * rank * sizeof(pointNode));
  
  MPI_Datatype MPI_PTNODE = createPtNodeType();
  //MPI_File_read_all(fh, buf, nfloats, MPI_FLOAT, &status);
  
//   int MPI_File_read_at_all(MPI_File fh, MPI_Offset offset, void *buf,
//                          int count, MPI_Datatype datatype, MPI_Status *status);

  int ierr =MPI_File_read_at_all(fh, start, buf, nPointNodes, MPI_PTNODE, &status);
  //printf("\n Status %d \n",status.MPI_ERROR);
  
  		char err_buffer[MPI_MAX_ERROR_STRING];
	    int resultlen;
  
  	    if (ierr != MPI_SUCCESS) {
            MPI_Error_string(ierr,err_buffer,&resultlen);
            fprintf(stderr,err_buffer);
            //MPI_Finalize();             /* abort*/
        }
        
 	  int count;
  	  MPI_Get_count(&status, MPI_PTNODE, &count);
  
      MPI_Offset readCount = count;
  	 // Print off the amount of numbers, and also print additional information in the status object

	  MPI_Offset specified = (bufSize/sizeof(pointNode));
  	
  	    if ( readCount <  specified) 
	    	 cout<<rank<<" # Floats read "<<readCount<<" #specified "<<specified<<endl;

    
  MPI_File_close(&fh);
  MPI_Type_free(&MPI_PTNODE);  
  
  // don't free buf
  return new pair<int, pointNode*>(nPointNodes, buf);
}

int main_real2(int argc, char **argv)
{
    char pointNodePath[] = "/home/satish/data/allnodes_small/allNodesPointsSmall.bin";
    //char pointNodePath[] = "/home/satish/data/allnodes_small/allNodesPoints.bin";
	int rank, nprocs;
	
	MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    double total_t1, total_t2;
	
	//if(rank == 0)
	// printf("%d ",sizeof(pointNode));
	
	ReadBinaryPointFile binReader;
    //pair<int, pointNode*> *p = binReader.readPoints_Contiguous(pointNodePath);
    
    pair<int, pointNode*> *p = binReader.readPoints_NonContig(pointNodePath);
    
    printf("first = %d \n", p->first);
    pointNode* ptnodes = p->second;
    
    for(int i = 0; i < 1; i++)
      printf("  <%ld>  ",ptnodes[i].nodeId);

    
	MPI_Finalize();
    
    return 0;  

}

