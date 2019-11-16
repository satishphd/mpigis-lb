#include "filePartition/MPIReadBinMBRStruct.h"

list<Envelope*>* MPIReadBinMBRStruct :: parseEnvs(MBR *mbrs, MPI_Offset nmbrs)
{
   list<Envelope*>* envVec = new list<Envelope*>();
   MPI_Offset counter;
   MBR mbr;
   
   for(counter = 0; counter < nmbrs; counter++) {
     mbr = mbrs[counter];
     Envelope *env = new Envelope(mbr.minx, mbr.maxx, mbr.miny, mbr.maxy);
     envVec->push_back(env);
   }
   free(mbrs);
   
   return envVec;
} 

list<Envelope*>* MPIReadBinMBRStruct :: parseIntEnvs(IntMBR *mbrs, MPI_Offset nmbrs)
{
   list<Envelope*>* envVec = new list<Envelope*>();
   MPI_Offset counter;
   IntMBR mbr;
   
   for(counter = 0; counter < nmbrs; counter++) {
     mbr = mbrs[counter];
     Envelope *env = new Envelope(mbr.minx, mbr.maxx, mbr.miny, mbr.maxy);
     envVec->push_back(env);
   }
   free(mbrs);
   
   return envVec;
} 


MPI_Datatype MPIReadBinMBRStruct :: createIntMBRType()
{
	const int nitems = 4;
    int          blocklengths[4] = {1,1,1,1};
    MPI_Datatype types[4] = {MPI_INT, MPI_INT, MPI_INT, MPI_INT};
    MPI_Datatype MPI_INT_MBR;
    MPI_Aint     offsets[4];
    offsets[0] = offsetof(MBR, minx);
    offsets[1] = offsetof(MBR, miny);
    offsets[2] = offsetof(MBR, maxx);
    offsets[3] = offsetof(MBR, maxy);
    
    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &MPI_INT_MBR);
    MPI_Type_commit(&MPI_INT_MBR);
    return MPI_INT_MBR;
}


MPI_Datatype MPIReadBinMBRStruct :: createMBRType()
{
	const int nitems = 4;
    int          blocklengths[4] = {1,1,1,1};
    MPI_Datatype types[4] = {MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE};
    MPI_Datatype MPI_MBR;
    MPI_Aint     offsets[4];
    offsets[0] = offsetof(MBR, minx);
    offsets[1] = offsetof(MBR, miny);
    offsets[2] = offsetof(MBR, maxx);
    offsets[3] = offsetof(MBR, maxy);
    
    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &MPI_MBR);
    MPI_Type_commit(&MPI_MBR);
    return MPI_MBR;
}

 list<Envelope*>* MPIReadBinMBRStruct :: readMBRs_NonContig(char *mbrPath)
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
   
  MPI_File_open(MPI_COMM_WORLD, mbrPath, MPI_MODE_RDONLY, myinfo, &fh);
  
  //cout<<"In readPoints_NonContig"<<endl;
  
  MPI_Datatype filetype;
  MPI_Offset filesize;
  
  MPI_File_get_size(fh, &filesize);
  
  //filesize = 1048576*128; // 128 MB
  
  MPI_Offset bufsize = filesize/nprocs;
  
  if(bufsize < 0)
	  cerr<<"Negative file size "<<endl; 
  
  MBR *buf = (MBR *) malloc(bufsize);
   
  if(buf == NULL)
    cerr<<"Error no memory"<<endl; 

  MPI_Offset nmbrs = bufsize/sizeof(MBR);
  
  if(rank == 0) {
	  cout<<"Total filesize "<<filesize<<endl;
  
	  cout<<"MBRs per process "<<nmbrs<<endl;
  }
  
  MPI_Datatype MPI_MBR = createMBRType();
  
// int MPI_Type_vector(int count, int blocklength, int stride, MPI_Datatype oldtype, MPI_Datatype *newtype);

  MPI_Type_vector(nmbrs/MBR_PER_BLK, MBR_PER_BLK, 
                  MBR_PER_BLK*nprocs, MPI_MBR, &filetype);
                  							  
  MPI_Type_commit(&filetype);
  MPI_File_set_view(fh, MBR_PER_BLK*sizeof(MBR)*rank, MPI_MBR, filetype, "native", myinfo);

  MPI_Status status;
  
  int ierr = MPI_File_read_all(fh, buf, nmbrs, MPI_MBR, &status);

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
  	  MPI_Get_count(&status, MPI_MBR, &number);
      
      if(number < 0)
         cout<<"Error "<<endl;
         
      MPI_Offset readCount = number;
  	 // Print off the amount of numbers, and also print additional
 	 // information in the status object

	  MPI_Offset specified = (bufsize/sizeof(MBR));
	
	if ( readCount <  specified) 
	   cout<<rank<<" # Floats read "<<readCount<<" #specified "<<specified<<endl;
		  	  //printf("id = %d actual = %d specified = %d \n", rank, number, bufsize/sizeof(float));

  MPI_File_close(&fh);
  
//  buf[nbytes] = '\0';
  list<Envelope*>* mbrs = parseEnvs(buf, nmbrs);

  MPI_Type_free(&filetype);
  MPI_Type_free(&MPI_MBR);  
  
  // don't free buf
  return mbrs;
 }


list<Envelope*>* MPIReadBinMBRStruct :: readMBRs_Contiguous(char *mbrPath)
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
   
  MPI_File_open(MPI_COMM_WORLD, mbrPath, MPI_MODE_RDONLY, myinfo, &fh);
  
  MPI_Offset filesize;
  
  MPI_File_get_size(fh, &filesize);
  
  int bufSize = filesize/nprocs;
  
  if(bufSize < 0)
	  cerr<<"Negative file size "<<endl; 
  
  MBR *buf = (MBR *) malloc(bufSize);
  
  if(buf == NULL)
    cerr<<"Error no memory"<<endl; 
  
  int nmbrs = bufSize/sizeof(MBR);
  
  int extraBytes = bufSize % nprocs;
  
  int extraMBRs = extraBytes/sizeof(MBR);
  
  if(rank == nprocs-1) {
     nmbrs = nmbrs + extraMBRs;
  }
  
  if(rank == 0) {
	  cout<<"Total filesize "<<filesize<<endl;
  
	  cout<<"MBRs 1st process "<<nmbrs<<endl;
  }
  
  if(rank == (nprocs - 1)) {
  	  cout<<"MBRs last process "<<nmbrs<<endl;
  }

  MPI_Status status;
  MPI_Offset start = (MPI_Offset)(nmbrs * rank * sizeof(MBR));
  
  MPI_Datatype MPI_MBR = createMBRType();
  //MPI_File_read_all(fh, buf, nfloats, MPI_FLOAT, &status);
  
//   int MPI_File_read_at_all(MPI_File fh, MPI_Offset offset, void *buf,
//                          int count, MPI_Datatype datatype, MPI_Status *status);

  int ierr =MPI_File_read_at_all(fh, start, buf, nmbrs, MPI_MBR, &status);
  //printf("\n Status %d \n",status.MPI_ERROR);
  
  		char err_buffer[MPI_MAX_ERROR_STRING];
	    int resultlen;
  
  	    if (ierr != MPI_SUCCESS) {
            MPI_Error_string(ierr,err_buffer,&resultlen);
            fprintf(stderr,err_buffer);
            //MPI_Finalize();             /* abort*/
        }
        
 	  int count;
  	  MPI_Get_count(&status, MPI_MBR, &count);
  
      MPI_Offset readCount = count;
  	 // Print off the amount of numbers, and also print additional information in the status object

	  MPI_Offset specified = (bufSize/sizeof(MBR));
  	
  	    if ( readCount <  specified) 
	    	 cout<<rank<<" # MBRs read "<<readCount<<" #specified "<<specified<<endl;
  
  list<Envelope*>* mbrs = parseEnvs(buf, nmbrs);
  
  MPI_Type_free(&MPI_MBR);  
  
  MPI_File_close(&fh);
  
  // don't free buf
  return mbrs;
}

int main(int argc, char **argv)
{
    //char lakeMBRPath[] = "/home/satish/data/binary_format/lakesMBR.bin";
    char simulatedMBRPath[] = "/home/satish/nly.bin";
	int rank, nprocs;
	
	MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    //char *filename = "/home/satish/data/cemetery/cemetery";
    char *filename = "/home/satish/data/sports_data/sports";
    //char *filename = "/home/satish/data/postal/postal_codes";

    MyGeos geosObj;
	//vector<Geometry*> *layerA = geosObj.readCSVFile(filename);
    //cout<<endl<<"Layer A geoms "<<layerA->size()<<endl;


    double total_t1, total_t2;
	
	//if(rank == 0)
	// printf("%d ",sizeof(pointNode));
	
	total_t1 = MPI_Wtime();
	
	MPIReadBinMBRStruct binReader;
    
    //list<Envelope*>* mbrs = binReader.readMBRs_Contiguous(simulatedMBRPath);
    list<Envelope*>* mbrs = binReader.readIntMBRs_Contiguous(simulatedMBRPath);
    //list<Envelope*>* mbrs = binReader.readMBRs_NonContig(lakeMBRPath);
    
    printf("first = %d \n", mbrs->size());
    
    for(int i = 0; i < 1; i++)
      printf("  <%f>  ",mbrs[i].getMinY());

	
    
    //geosObj.testLoadBalance( mbrs, layerA); 
    
    total_t2 = MPI_Wtime();
    
    if(rank == 0) {
    	  cout<<"Time = "<<(total_t2 - total_t1)<<endl;
    }
    
	MPI_Finalize();
    
    return 0;  

}

/** New method for Simulated Data */
list<Envelope*>* MPIReadBinMBRStruct :: readIntMBRs_Contiguous(char *mbrPath)
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
   
  MPI_File_open(MPI_COMM_WORLD, mbrPath, MPI_MODE_RDONLY, myinfo, &fh);
  
  MPI_Offset filesize;
  
  MPI_File_get_size(fh, &filesize);
  
  int bufSize = filesize/nprocs;
  
  if(bufSize < 0)
	  cerr<<"Negative file size "<<endl; 
  
  IntMBR *buf = (IntMBR *) malloc(bufSize);
  
  if(buf == NULL)
    cerr<<"Error no memory"<<endl; 
  
  int nmbrs = bufSize/sizeof(IntMBR);
  
  int extraBytes = bufSize % nprocs;
  
  int extraMBRs = extraBytes/sizeof(IntMBR);
  
  if(rank == nprocs-1) {
     nmbrs = nmbrs + extraMBRs;
  }
  
  if(rank == 0) {
	  cout<<"Total filesize "<<filesize<<endl;
  
	  cout<<"MBRs 1st process "<<nmbrs<<endl;
  }
  
  if(rank == (nprocs - 1)) {
  	  cout<<"MBRs last process "<<nmbrs<<endl;
  }

  MPI_Status status;
  MPI_Offset start = (MPI_Offset)(nmbrs * rank * sizeof(IntMBR));
  
  MPI_Datatype MPI_INT_MBR = createIntMBRType();
  //MPI_File_read_all(fh, buf, nfloats, MPI_FLOAT, &status);
  
//   int MPI_File_read_at_all(MPI_File fh, MPI_Offset offset, void *buf,
//                          int count, MPI_Datatype datatype, MPI_Status *status);

  int ierr =MPI_File_read_at_all(fh, start, buf, nmbrs, MPI_INT_MBR, &status);
  //printf("\n Status %d \n",status.MPI_ERROR);
  
  		char err_buffer[MPI_MAX_ERROR_STRING];
	    int resultlen;
  
  	    if (ierr != MPI_SUCCESS) {
            MPI_Error_string(ierr,err_buffer,&resultlen);
            fprintf(stderr,err_buffer);
            //MPI_Finalize();             /* abort*/
        }
        
 	  int count;
  	  MPI_Get_count(&status, MPI_INT_MBR, &count);
  
      MPI_Offset readCount = count;
  	 // Print off the amount of numbers, and also print additional information in the status object

	  MPI_Offset specified = (bufSize/sizeof(IntMBR));
  	
  	    if ( readCount <  specified) 
	    	 cout<<rank<<" # MBRs read "<<readCount<<" #specified "<<specified<<endl;
  
  list<Envelope*>* mbrs = parseIntEnvs(buf, nmbrs);
  
  MPI_Type_free(&MPI_INT_MBR);  
  
  MPI_File_close(&fh);
  
  // don't free buf
  return mbrs;
}


