#include "filePartition/MPI_ReadBinaryMBRFile.h"

// int countNumLines(char *stream, MPI_Offset len)
// {
//    MPI_Offset byteCounter;
//    int rank;
//    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//    long numLines = 0;
//    //for(int i = 0; i< 1000; i++)
//       //printf("%c",stream[i]);
//    //cout<<endl;   
//    for(byteCounter = 0; byteCounter < len; byteCounter++) {
//       if( stream[byteCounter] == '\n')
//          numLines++;
//    }
//    cout<<"Rank "<<rank<<":"<<numLines<<endl;
// }

/* The buf contains x1, y1, x2, y2 */
list<Envelope*>* ReadBinaryMBRFile :: parseEnvs(float *buf, MPI_Offset nfloats)
{
   list<Envelope*>* envVec = new list<Envelope*>();
   MPI_Offset counter;
   
   for(counter = 0; counter < nfloats; counter = counter+4) {
     Envelope *env = new Envelope(buf[counter + 0], buf[counter + 2], buf[counter + 1], buf[counter + 3]);
     envVec->push_back(env);
   }
   
   return envVec;
} 

list<Geometry*>* ReadBinaryMBRFile :: parseMBRs(float *buf, MPI_Offset nfloats)
{
   geos::geom::GeometryFactory factory;
   
   list<Geometry*>* geoms = new list<Geometry*>();
   MPI_Offset counter;
   Geometry *geom;
   
   for(counter = 0; counter < nfloats; counter = counter+4) {
   
   		CoordinateArraySequence* coords = new geos::geom::CoordinateArraySequence();
		try {
       	 	double start_longitude = buf[counter + 0];
    	 	double start_latitude = buf[counter + 1];
    	 	coords->add(Coordinate(start_longitude, start_latitude));
    
    	 	double end_longitude = buf[counter + 2];
    	 	double end_latitude = buf[counter + 3];
    	 	coords->add(Coordinate(end_longitude, end_latitude));

    	 	LineString *line = factory.createLineString(coords);
            geom = line;
            
            geoms->push_back(geom);
        }
        catch(exception &e)
        {
           cout<< e.what() <<endl;
        }
   }
   
   return geoms;
} 

list<Envelope*>* ReadBinaryMBRFile :: readMBRs_Contiguous(char *road_networkPath)
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
   
  MPI_File_open(MPI_COMM_WORLD, road_networkPath, MPI_MODE_RDONLY, myinfo, &fh);
  
  MPI_Offset filesize;
  
  MPI_File_get_size(fh, &filesize);
  
  MPI_Offset bufsize = filesize/nprocs;
  
  if(bufsize < 0)
	  cerr<<"Negative file size "<<endl; 
  
  float *buf = (float *) malloc(bufsize);
   
  if(buf == NULL)
    cerr<<"Error no memory"<<endl; 

  MPI_Offset nfloats = bufsize/sizeof(float);
  
  if(rank == 0) {
	  cout<<"Total filesize "<<filesize<<endl;
  
	  cout<<"floats per process "<<nfloats<<endl;
  }
  
  MPI_Status status;
  MPI_Offset start = nfloats*rank*sizeof(float);
  //MPI_File_read_all(fh, buf, nfloats, MPI_FLOAT, &status);
  
//   int MPI_File_read_at_all(MPI_File fh, MPI_Offset offset, void *buf,
//                          int count, MPI_Datatype datatype, MPI_Status *status);
  MPI_File_read_at_all(fh, start, buf, nfloats, MPI_FLOAT, MPI_STATUS_IGNORE);
  
  //list<Geometry*>* geoms = parseMBRs(buf, nfloats);
  list<Envelope*>*envs = parseEnvs(buf, nfloats);
  MPI_File_close(&fh);
  
//  buf[nbytes] = '\0';
  //countNumLines(buf, nbytes);
  
  free(buf);
  
  return envs;
}


 list<Envelope*>* ReadBinaryMBRFile :: readMBRs(char *road_networkPath)
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
   
  MPI_File_open(MPI_COMM_WORLD, road_networkPath, MPI_MODE_RDONLY, myinfo, &fh);
  
  //cout<<"In readMBRs"<<endl;
  
  MPI_Datatype filetype;
  MPI_Offset filesize;
  
  MPI_File_get_size(fh, &filesize);
  
  //filesize = 1048576*128; // 128 MB
  
  MPI_Offset bufsize = filesize/nprocs;
  
  if(bufsize < 0)
	  cerr<<"Negative file size "<<endl; 
  
  float *buf = (float *) malloc(bufsize);
   
  if(buf == NULL)
    cerr<<"Error no memory"<<endl; 

  MPI_Offset nfloats = bufsize/sizeof(float);
  
  if(rank == 0) {
	  cout<<"Total filesize "<<filesize<<endl;
  
	  cout<<"floats per process "<<nfloats<<endl;
  }
  
// int MPI_Type_vector(int count,
//                    int blocklength,
//                    int stride, 
//                    MPI_Datatype oldtype,
//                    MPI_Datatype *newtype);

  MPI_Type_vector(nfloats/FLOATS_PER_BLK, FLOATS_PER_BLK, 
                  FLOATS_PER_BLK*nprocs, MPI_FLOAT, &filetype);
                  
  MPI_Type_commit(&filetype);
  MPI_File_set_view(fh, FLOATS_PER_BLK*sizeof(float)*rank, MPI_FLOAT, 
	      filetype, "native", myinfo);

  MPI_Status status;
  
  int ierr = MPI_File_read_all(fh, buf, nfloats, MPI_FLOAT, &status);

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
  	  MPI_Get_count(&status, MPI_FLOAT, &number);
      
      if(number < 0)
         cout<<"Error "<<endl;
         
      MPI_Offset readCount = number;
  	 // Print off the amount of numbers, and also print additional
 	 // information in the status object

	  MPI_Offset specified = (bufsize/sizeof(float));
	
	if ( readCount <  specified) 
	   cout<<rank<<" # Floats read "<<readCount<<" #specified "<<specified<<endl;
		  	  //printf("id = %d actual = %d specified = %d \n", rank, number, bufsize/sizeof(float));

  
  //list<Geometry*>* geoms = parseMBRs(buf, nfloats);
  list<Envelope*>*envs = parseEnvs(buf, nfloats);
  //cout<<"# envelopes "<<envs->size()<<endl;

  MPI_File_close(&fh);
  
//  buf[nbytes] = '\0';
  //countNumLines(buf, nbytes);

  MPI_Type_free(&filetype);
  
  free(buf);
  
  return envs;
 }


int main_real(int argc, char **argv)
{
	//char road_networkPath[] = "/projects/dimos/GeoSpatial/data/SpatialHadoop_DataSets/sample_road_network/road_networkMBRsOnly.bin"; 
    char road_networkPath[] = "/home/satish/data/sample_road_network/road_networkMBRsOnly.bin";
	int rank, nprocs;
	
	MPI_Init(&argc,&argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &nprocs);

    double total_t1, total_t2;
	
	ReadBinaryMBRFile binReader;
    
    // non-contiguous access
   // list<Envelope*>* mbrs = binReader.readMBRs(road_networkPath);

    list<Envelope*>* mbrs = binReader.readMBRs_Contiguous(road_networkPath);
    
  	long longNumber = (long)mbrs->size();
    cout<<" Edges "<<longNumber<<endl;

    long totalLines = 0;
    long max;
    long min;
    
    //MPI_Barrier(MPI_COMM_WORLD);
   // MPI_Reduce(&longNumber, &totalLines, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
//     MPI_Reduce(&longNumber, &min, 1, MPI_LONG, MPI_MIN, 0, MPI_COMM_WORLD);    
//     MPI_Reduce(&longNumber, &max, 1, MPI_LONG, MPI_MAX, 0, MPI_COMM_WORLD);
    
  
    
   if(rank == 0) {
      //cout<<"Total # of MBRs "<<totalLines<<endl;
//      cout<<"Max MBRs "<<max << "Min MBRs "<<min<<endl;
  //cout<<"Time = "<<(total_t2 - total_t1)<<endl;
   }
   
    MyGeos geosObj;
    
    char *filename = "/home/satish/data/cemetery/cemetery";
    //char *filename = "/home/satish/data/sports_data/sports";
    //char *filename = "/home/satish/data/postal/postal_codes";

	vector<Geometry*> *layerA = geosObj.readCSVFile(filename);
    cout<<endl<<"Layer A geoms "<<layerA->size()<<endl;
    
    total_t1 = MPI_Wtime();
    
    geosObj.testLoadBalance( mbrs, layerA); 
    
     //if(rank == 2)
       // geosObj.printMBRs(mbrs);
    //cout<<endl<<endl;
    //MPI_Barrier(MPI_COMM_WORLD);
    
    total_t2 = MPI_Wtime();
    
    if(rank == 0) {
    	  cout<<"Time = "<<(total_t2 - total_t1)<<endl;
    }
    MPI_Finalize();
    return 0; 
}

// int main2(int argc, char **argv)
// {
//   int rank, nprocs;
//   
//   MPI_Init(&argc,&argv);
//   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//   MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
// 
//   double total_t1, total_t2;
//   
//   total_t1 = MPI_Wtime();
//   
// //  char lakeFilePath[] = "/projects/dimos/GeoSpatial/data/SpatialHadoop_DataSets/lakes/new_lakes.txt";
// //  char road25GBFilePath[] = "/projects/dimos/GeoSpatial/data/SpatialHadoop_DataSets/roads25GB/roads"; 
// //  char allobjPath[] = "/projects/dimos/GeoSpatial/data/SpatialHadoop_DataSets/allobjects/all_objects"; 
// //	char road_networkPath[] = "/projects/dimos/GeoSpatial/data/SpatialHadoop_DataSets/road_networkdirectory/road_network"; 
//   char road_networkPath[] = "/projects/dimos/GeoSpatial/data/SpatialHadoop_DataSets/sample_road_network/road_networkMBRsOnly.bin"; 
//   MPI_File fh;
//    
//   MPI_File_open(MPI_COMM_WORLD, road_networkPath, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
//  
//   MPI_Datatype filetype;
//   MPI_Offset filesize;
//   
//   MPI_File_get_size(fh, &filesize);
//   
//   //filesize = 1048576*60; // 64 MB
//   
//   MPI_Offset bufsize = filesize/nprocs;
//   
//   if(bufsize < 0)
// 	  cerr<<"Negative file size "<<endl; 
//   
//   float *buf = (float *) malloc(bufsize);
//    
//   if(buf == NULL)
//     cerr<<"Error no memory"<<endl; 
// 
//   MPI_Offset nfloats = bufsize/sizeof(float);
//   
//   if(rank == 0) {
// 	  cout<<"Total filesize "<<filesize<<endl;
//   
// 	  cout<<"floats per process "<<nfloats<<endl;
//   }
//   
// // int MPI_Type_vector(int count,
// //                    int blocklength,
// //                    int stride, 
// //                    MPI_Datatype oldtype,
// //                    MPI_Datatype *newtype);
// 
//   MPI_Type_vector(nfloats/FLOATS_PER_BLK, FLOATS_PER_BLK, 
//                   FLOATS_PER_BLK*nprocs, MPI_FLOAT, &filetype);
//                   
//   MPI_Type_commit(&filetype);
//   MPI_File_set_view(fh, FLOATS_PER_BLK*sizeof(float)*rank, MPI_FLOAT, 
// 	      filetype, "native", MPI_INFO_NULL);
// 
//   MPI_Status status;
//   
//   MPI_File_read_all(fh, buf, nfloats, MPI_FLOAT, &status);
//   
//   // After receiving the message, check the status to determine
//     // how many numbers were actually received
//   int number;
//   MPI_Get_count(&status, MPI_FLOAT, &number);
// 
//   // Print off the amount of numbers, and also print additional
//   // information in the status object
//    printf("%d read %d FLOAT \n",rank, number);
//   
//   MPI_File_close(&fh);
//   
// //  buf[nbytes] = '\0';
//   //countNumLines(buf, nbytes);
// 
//   MPI_Type_free(&filetype);
//   free(buf);
//   
//   total_t2 = MPI_Wtime();
//   
//   long longNumber = (long)number;
//   
//   long totalLines;
//   //MPI_Barrier(MPI_COMM_WORLD);
//   MPI_Reduce(&longNumber, &totalLines, 1, MPI_LONG, MPI_SUM, 0, MPI_COMM_WORLD);
//   
//   if(rank == 0) {
//       cout<<"Total # of MBRs "<<totalLines/4<<endl;
// 	  cout<<"Time = "<<(total_t2 - total_t1)<<endl;
//   }
//   
//   MPI_Finalize();
//   return 0; 
// }

