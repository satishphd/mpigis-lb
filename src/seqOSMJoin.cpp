
//#include "utility.h"
//#include "tut/tut.hpp"
// geos

#include "geom_util/seqOSMJoin.h"

void MyGeos:: printMBRs(list<Envelope*>* mbrs)
{
   for (list<Envelope*>::iterator it = mbrs->begin() ; it != mbrs->end(); ++it) {
        Envelope* p = *it;
        cout<<p->toString()<<"\t";
   }
}

void MyGeos:: testLoadBalance(list<Envelope*>* mbrs, vector<Geometry*> *layerA) 
{    
    //char *filename = "/home/satish/openacc_project/shapefile_data/data/urban_areas/ne_10m_urban_areas.shp";
    //char *filename = "/home/satish/openacc_project/shapefile_data/data/states_provinces/ne_10m_admin_1_states_provinces.shp";
	//vector<Geometry*> *layerA = readShapefile(filename);
    
    Index index;
    index.createRTree(mbrs);
    
    map<Geometry*, vector<void *> >*joinResult = index.query(layerA);
    
    Join joinObj;
    
    joinObj.joinGeomEnv(joinResult);
    
    
}

void MyGeos:: distRead(int numRecords, SHPHandle hSHP, SHPObject ***psShape){
	
    SHPObject **psShape_temp;
    
    int		nShapeType, nEntities;
    double 	adfMinBound[4], adfMaxBound[4];
    
    if( hSHP == NULL ){
        printf( "Unable to open shapefile\n");
        exit( 1 );
    }	
    SHPGetInfo( hSHP, &nEntities, &nShapeType, adfMinBound, adfMaxBound );

    psShape_temp=(SHPObject **)malloc((numRecords)*sizeof(SHPObject*));
    
    for(int i= 0; i<numRecords; i++){
	psShape_temp[i] = SHPReadObject( hSHP, i );
	
       if( psShape_temp[i] == NULL ){
            fprintf( stderr,"Unable to read shape %d, terminating object reading.\n",i );
            exit(1);
        }
     }	
	(*psShape)=psShape_temp;
}

void MyGeos:: destoryObjects(SHPObject **psShape,int num){
	int i;
	for(i=0;i<num;i++){
	    SHPDestroyObject(psShape[i]);
	}
}


void MyGeos:: populateLayer(SHPObject **psShape,int num, vector<Geometry*> *layer){
	int i;

	//printf("%d \n",num);
	
	geos::geom::GeometryFactory factory_;
        
	for( i = 0; i < num; i++ ) {
          int	j;
          // Create sequence of coordiantes
     	  CoordinateArraySequence* coords = new geos::geom::CoordinateArraySequence(psShape[i]->nVertices+1);

          //cout<<i<<" "<<psShape[i]->nVertices<<" ";
          for( j = 0; j < psShape[i]->nVertices; j++ )
          {
            coords->setAt(Coordinate(psShape[i]->padfX[j], psShape[i]->padfY[j]), j);
            //  cout<<psShape[i]->padfX[j]<< ", "<<psShape[i]->padfY[j]<<endl;
          }
          //cout<<endl;
          coords->setAt(Coordinate(psShape[i]->padfX[0], psShape[i]->padfY[0]), j);
          //cout<<psShape[i]->padfX[j]<< ", "<<psShape[i]->padfY[j]<<endl;
        
          // Create exterior ring
          LinearRing* exterior = factory_.createLinearRing(coords);
		
          //tut::ensure("simple ext", exterior->isSimple() );
		
	  Geometry* poly = factory_.createGeometry(exterior);
		
	  // std::cout << "poly empty: " << poly->isEmpty()
 	//	<< ", simple: " << poly->isSimple()
 	//	<< ", points: " << poly->getNumPoints()
 	//	<< std::endl;
		
          layer->push_back(poly);
        }
        cout<<"populate fn "<<layer->size()<<endl;
}

vector<Geometry*>* MyGeos:: readShapefile(char* filename)
{
        vector<Geometry*> *layer = new vector<Geometry*>();
	
	SHPObject	**psShape;
		
	SHPHandle	hSHP;
	hSHP = SHPOpen(filename,"rb");
    
        printf("Number of records = %d \n", hSHP->nRecords);
    
        distRead(hSHP->nRecords, hSHP, &psShape);
	//SHPReadMBR(hSHP, startIndex, endIndex, &mbrs);
        populateLayer(psShape,hSHP->nRecords, layer);
	
        return layer;
}

// int main(int argc, char *argv[]) {
// 
//     if( argc != 3 ){
//            cout<< argc<<endl;
//            printf( "./prog [shp_file] [shp_file]\n" );
//            exit( 1 );
// 	}
// 
//     double starttime, endtime;
//     double difference;
//     
//     starttime = MyGeos :: my_difftime();    
// 	
// 	MyGeos geosObj;
// 	vector<Geometry*> *layerA = geosObj.readCSVFile(argv[1]);
//         cout<<layerA->size()<<endl;
// 		
// 	vector<Geometry*> *layerB = geosObj.readCSVFile(argv[2]);
// 	cout<<layerB->size()<<endl;
//         
//         //starttime = MyGeos :: my_difftime();    
// 
// 	geosObj.createIndex(layerA);
// 	cout<<"Create Index done"<<endl;
// 		
// 	geosObj.query(layerB);
//  	
//     endtime = MyGeos :: my_difftime();
// 	 
//     difference = endtime - starttime;
// 
//     cout<<difference<<endl;
// 
//     return 0;
// }

vector<Geometry*>* MyGeos:: readCSVFile(char* filename) 
{
	ifstream in_stream;
    string line;
    int invalid = 0;
    WKTParser parser;
    
    vector<Geometry*> *layer;
    
    in_stream.open(filename);
  
    if (in_stream.is_open()) {
   
      layer = new vector<Geometry*>();
    
      geos::io::WKTReader wktreader;
      //Geometry *geom = NULL;

      for( string line; getline( in_stream, line ); )
      {
        //cout<<line<<endl;
        //geom = wktreader.read(line);
        pair<int, string*> p = parser.extract(line);
        Geometry *geom = parser.parseString(p);
        if(geom != NULL && geom->isValid() && !geom->isEmpty())
	        layer->push_back(geom);
	    else
	       invalid++;
      }
	
	  in_stream.close();
   }	
   
   //cout<<"InValid Geoms "<<invalid<<" in file "<<filename<<endl;
   return layer;
}

vector<Geometry*>* MyGeos:: readOSM(char* filename) {

    ifstream in_stream;
    string line;
    
    vector<Geometry*> *layer;
    
    in_stream.open(filename);
  
    if (in_stream.is_open()) {
   
      layer = new vector<Geometry*>();
    
      geos::io::WKTReader wktreader;
      Geometry *geom = NULL;

      for( string line; getline( in_stream, line ); )
      {
        cout<<line<<endl;
        geom = wktreader.read(line);
        layer->push_back(geom);
      }
	
	  in_stream.close();
   }	
	return layer;
}

double MyGeos::  my_difftime ()
{
    struct timeval tp;
    struct timezone tzp;

    gettimeofday(&tp,&tzp);
    return ( (double) tp.tv_sec + (double) tp.tv_usec * 1.e-6 );
}


void MyGeos:: query(vector<Geometry*> *layer2) {
    int count = 0;
   
    for (vector<Geometry*>::iterator layer2it = layer2->begin() ; layer2it != layer2->end(); ++layer2it) {
      
       Geometry* poly1 = *layer2it;

       prep::PreparedGeometryFactory pgf;
       const prep::PreparedGeometry* pgpoly1 = pgf.create(poly1);
      
       std::vector<void *> results;
      
       //cout<<"In query"<<endl;
      
       index.query(poly1->getEnvelopeInternal(), results);
      
       //cout<<"Query result "<<results.size()<<" "<<endl;
      
       
       for(vector<void *>::iterator it = results.begin() ; it != results.end(); ++it) {
             void *poly2Ptr = *it;
             Geometry* poly2 = (Geometry*)poly2Ptr;
             
             try {
                   if(pgpoly1->intersects(poly2)) {
             
                       count++;
              
                    //cout<<poly1->getEnvelopeInternal()->toString()<<endl<<endl<<poly2->getEnvelopeInternal()->toString()<<endl<<endl;                        
                    //cerr<<"Try "<<poly1->getNumPoints()<<" "<<poly2->getNumPoints()<<endl;              
                   }
                  } 
                  catch(exception ex) {
                   //cout<< " 2 ";
                  }
        }
     }
   	 cout<<"Number of intersections "<<count<<endl;
   	
}

//try { /* */ } catch(const std::exception& e) { /* */ }

void MyGeos::createIndex(vector<Geometry*> *poly) {

    for (vector<Geometry*>::iterator it = poly->begin() ; it != poly->end(); ++it) {
        Geometry* p = *it;
      //const Envelope *env = p->getEnvelopeInternal();
      
      index.insert( p->getEnvelopeInternal(), p );
      
      //cout<<env->toString()<<endl; //<<p->toString();
      //cout<<endl;       cout<<endl;
    }
}

// 
// geos::index::strtree::STRtree index;
// 	index.insert( poly->getEnvelopeInternal(), nullptr );
// 
// 	std::vector<void *> results;
// 	geos::geom::Point * point = factory.createPoint( geos::geom::Coordinate( 50, 50 ) );
// 	index.query( point->getEnvelopeInternal(), results );
// 

/*
void MyGeos:: distRead(int numRecords, SHPHandle hSHP, SHPObject ***psShape){
	
    SHPObject **psShape_temp;
    
    int		nShapeType, nEntities;
    double 	adfMinBound[4], adfMaxBound[4];
    
    if( hSHP == NULL ){
        printf( "Unable to open shapefile\n");
        exit( 1 );
    }	
    SHPGetInfo( hSHP, &nEntities, &nShapeType, adfMinBound, adfMaxBound );

    psShape_temp=(SHPObject **)malloc((numRecords)*sizeof(SHPObject*));
    
    for(int i= 0; i<numRecords; i++){
	psShape_temp[i] = SHPReadObject( hSHP, i );
	
       if( psShape_temp[i] == NULL ){
            fprintf( stderr,"Unable to read shape %d, terminating object reading.\n",i );
            exit(1);
        }
     }	
	(*psShape)=psShape_temp;
}

void MyGeos:: destoryObjects(SHPObject **psShape,int num){
	int i;
	for(i=0;i<num;i++){
	    SHPDestroyObject(psShape[i]);
	}
}


void MyGeos:: populateLayer(SHPObject **psShape,int num, vector<Geometry*> *layer){
	int i;

	//printf("%d \n",num);
	
	geos::geom::GeometryFactory factory_;
        
	for( i = 0; i < num; i++ ) {
          int	j;
          // Create sequence of coordiantes
     	  CoordinateArraySequence* coords = new geos::geom::CoordinateArraySequence(psShape[i]->nVertices+1);

          //cout<<i<<" "<<psShape[i]->nVertices<<" ";
          for( j = 0; j < psShape[i]->nVertices; j++ )
          {
            coords->setAt(Coordinate(psShape[i]->padfX[j], psShape[i]->padfY[j]), j);
            //  cout<<psShape[i]->padfX[j]<< ", "<<psShape[i]->padfY[j]<<endl;
          }
          //cout<<endl;
          coords->setAt(Coordinate(psShape[i]->padfX[0], psShape[i]->padfY[0]), j);
          //cout<<psShape[i]->padfX[j]<< ", "<<psShape[i]->padfY[j]<<endl;
        
          // Create exterior ring
          LinearRing* exterior = factory_.createLinearRing(coords);
		
          //tut::ensure("simple ext", exterior->isSimple() );
		
	  Geometry* poly = factory_.createGeometry(exterior, 0);
		
	  // std::cout << "poly empty: " << poly->isEmpty()
 	//	<< ", simple: " << poly->isSimple()
 	//	<< ", points: " << poly->getNumPoints()
 	//	<< std::endl;
		
          layer->push_back(poly);
        }
        cout<<"populate fn "<<layer->size()<<endl;
}

vector<Geometry*>* MyGeos:: readShapefile(char* filename)
{
        vector<Geometry*> *layer = new vector<Geometry*>();
	
	SHPObject	**psShape;
		
	SHPHandle	hSHP;
	hSHP = SHPOpen(filename,"rb");
    
        printf("Number of records = %d \n", hSHP->nRecords);
    
        distRead(hSHP->nRecords, hSHP, &psShape);
	//SHPReadMBR(hSHP, startIndex, endIndex, &mbrs);
        populateLayer(psShape,hSHP->nRecords, layer);
	
        return layer;
}

int main1(int argc, char *argv[]) {

        if( argc != 3 ){
           cout<< argc<<endl;
           printf( "./prog [shp_file] [shp_file]\n" );
           exit( 1 );
	}

        double starttime, endtime;
        double difference;

	MyGeos geosObj;
	vector<Geometry*> *layerA = geosObj.readShapefile(argv[1]);
        cout<<layerA->size()<<endl;
		
	vector<Geometry*> *layerB = geosObj.readShapefile(argv[2]);
	cout<<layerB->size()<<endl;
        
        starttime = MyGeos :: my_difftime();    

	geosObj.createIndex(layerA);
	cout<<"Create Index done"<<endl;
		
	geosObj.query(layerB);
 	
        endtime = MyGeos :: my_difftime();
	 
        difference = endtime - starttime;

        cout<<difference<<endl;

        return 0;
}

*/






/*
int main1() {
 geos::io::WKTReader wktreader;
 geos::io::WKTWriter wkt;
// geos::io::WKTWriter *wkt = new io::WKTWriter();
 //typedef std::auto_ptr<geos::geom::Geometry> GeomPtr;
 //GeomPtr geom(wktreader.read("POINT(-117 33)"));
//Geometry *geom = wktreader.read("POINT(-117 33)");

//Geometry *geom1 = wktreader.read("GEOMETRYCOLLECTION (Geometry ((8.6877453 50.5889002, 8.6876368 50.5889066, 8.6873752 50.5886577, 8.6870302 50.5883526, 8.6869588 50.5883036, 8.6869154 50.5882776, 8.6868697 50.5882458, 8.6866597 50.5881093, 8.6863815 50.5879466, 8.6861977 50.5878153, 8.6861715 50.5877051, 8.6860612 50.5876683, 8.6859878 50.5876526, 8.6858408 50.5875686, 8.6850137 50.5870223, 8.6857327 50.5866218, 8.6860667 50.5868703, 8.6863237 50.5869754, 8.6865495 50.5870331, 8.6872581 50.5878642, 8.6875507 50.5880765, 8.6877445 50.5882129, 8.6881499 50.5885672, 8.6885809 50.5888923, 8.6891434 50.5893269, 8.6894173 50.5895387, 8.6894721 50.5896885, 8.6896584 50.5897287, 8.6898162 50.5898052, 8.6902827 50.5901467, 8.6904035 50.5902338, 8.6905708 50.590407, 8.6908031 50.5905477, 8.690956 50.5906742, 8.6911087 50.5907436, 8.6912232 50.5908061, 8.6912163 50.5909102, 8.6911087 50.5909761, 8.6909734 50.5909866, 8.6907408 50.5909345, 8.6903313 50.5909206, 8.6900397 50.590872, 8.6896857 50.5906464, 8.6891165 50.5901744, 8.6886549 50.5897961, 8.6882303 50.5893788, 8.6880019 50.5892083, 8.6878538 50.589047, 8.6878601 50.5889513, 8.6877453 50.5889002)), Geometry ((8.6873316 50.5909134, 8.6873975 50.5911807, 8.6873489 50.591247, 8.6873825 50.5913615, 8.6871657 50.5914266, 8.6869191 50.591481, 8.6867178 50.5915946, 8.6866697 50.5917675, 8.6869465 50.5920629, 8.687126 50.5922752, 8.6872518 50.592395, 8.6873776 50.5924341, 8.6875372 50.5925226, 8.6878902 50.5924441, 8.6882006 50.5923024, 8.6883251 50.5921908, 8.6884482 50.5920054, 8.6885256 50.5918991, 8.6885815 50.591817, 8.6886483 50.5917924, 8.6887689 50.5917362, 8.6887616 50.5916528, 8.6887961 50.5915726, 8.6888861 50.591513, 8.6889398 50.5915154, 8.6889992 50.5915276, 8.689026 50.5915835, 8.6889302 50.5916066, 8.6889091 50.5916383, 8.68898 50.5916322, 8.689026 50.5916018, 8.689118 50.591682, 8.6891585 50.591693, 8.6892521 50.5916237, 8.689185 50.5916018, 8.6891793 50.5915823, 8.6892444 50.5915215, 8.6893364 50.591496, 8.6894379 50.5915057, 8.6895567 50.5915616, 8.6896238 50.591485, 8.6897953 50.591533, 8.6898018 50.5916112, 8.6899318 50.5916593, 8.6900782 50.5916215, 8.6905012 50.5918998, 8.690753 50.5921006, 8.6908963 50.5922593, 8.6913471 50.5924573, 8.6915277 50.592669, 8.6917284 50.5927785, 8.6917661 50.5928479, 8.6917016 50.5929081, 8.6912406 50.5929469, 8.690891 50.5929648, 8.6904759 50.5929158, 8.6901529 50.592791, 8.6897015 50.5927154, 8.689476 50.5926005, 8.6892138 50.592466, 8.6890306 50.5924506, 8.688932 50.5924746, 8.6888585 50.5925172, 8.6887608 50.5926409, 8.6886834 50.5929572, 8.6885852 50.5929408, 8.6885202 50.593001, 8.6883387 50.5931231, 8.6882032 50.5931454, 8.6881018 50.5930882, 8.6879748 50.5931079, 8.6878562 50.5931411, 8.6877517 50.59315, 8.6875969 50.5931755, 8.6875305 50.5931908, 8.6874682 50.5932189, 8.6874018 50.5932011, 8.6873476 50.5931985, 8.6870678 50.5932416, 8.6867699 50.5932404, 8.6864692 50.5932038, 8.6865723 50.5930611, 8.6861132 50.5929381, 8.6861187 50.5927706, 8.6859408 50.5924888, 8.6858576 50.5921409, 8.6860207 50.5918903, 8.6862081 50.5916214, 8.6866545 50.5913567, 8.6868016 50.5912869, 8.6868881 50.5913202, 8.6870222 50.5912023, 8.6868984 50.5911584, 8.6870043 50.5910226, 8.6871335 50.5908321, 8.6874492 50.5906078, 8.6874281 50.590517, 8.6874221 50.5903999, 8.6876767 50.5903413, 8.6880876 50.5903076, 8.6881798 50.5902334, 8.6880209 50.589869, 8.6879893 50.58978, 8.6880607 50.5897619, 8.6883234 50.5899775, 8.6884851 50.5901796, 8.6887007 50.5903345, 8.6887676 50.5904815, 8.6887901 50.5905583, 8.6888022 50.5906633, 8.6887266 50.5907175, 8.688401 50.5907404, 8.6880093 50.5907111, 8.6876084 50.5906831, 8.6874813 50.5907568, 8.6873413 50.5908305, 8.6873316 50.5909134)))");
Geometry *geom1 = wktreader.read("Geometry ((30 10, 40 40, 20 40, 10 20, 30 10))");
Geometry *geom2 = wktreader.read("Geometry ((20 20, 30 10, 10 0, 20 20))");
Geometry *geom3 = wktreader.read("Geometry ((200 200, 300 100, 100 0, 200 200))");

cout<<geom1->intersects(geom3)<<endl;

//int pCount = geom1->getNumGeometries();
//cout<< "Number of Geometrys "<<pCount<<endl;

//const Geometry *geom = geom1->getGeometryN(0);

//CoordinateSequence* seq = geom->getCoordinates();
//int cords = seq->size();
//cout<< "Size of seq "<<cords;

//for(int i=0; i<cords; i++) {
  
  //Coordinate &cord = seq[i];
  //cout<<cord.x <<" "<<cord.y;
  //cout<<seq->getX(i)<<" "<<seq->getY(i);
//}
  
 //string tmp=wkt.write(geom);
 //cout<<tmp<<endl;
 return 0;
}
*/

// 	std::cout << "query results: " << results.size() << std::endl;

// void MyGeos::createIndex(vector<tut::GeometryPtr> *poly) {
//     
//     Geometry *geom;
//     
//     int i=0;
//     
//     for (vector<tut::GeometryPtr>::iterator it = poly->begin() ; it != poly->end(); ++it) {
//       tut::GeometryPtr p = *it;
//       //geom = p->getEnvelope();
//       
//       //cout<<geom->getNumPoints()<<" "<<p->getNumPoints()<<"; ";
//       
//        //if(geom->isRectangle()) {
// //          //for(int i = 0; i<geom->getNumPoints(); i++) {
// 				//cout<<geom->getNumPoints();
//               const LineString *line = p->getExteriorRing (); 
//              CoordinateSequence* coords = line->getCoordinates();
//              //tut::CoordArrayPtr coords = geom->getCoordinates();
// //             
//              Rect rect = {{coords->getX(0), coords->getY(0), coords->getX(1), coords->getY(1)}};
//              RTreeInsertRect(&rect, i+1, &root, 0);
//              i++;
// //          //}
//  //      } 
//     }
// }

// 
// int rtreeIndex()
// {
//     
//     int i, nhits;
//     //printf("nrects = %d\n", nrects);
//     /*
//      * Insert all the data rects.
//      * Notes about the arguments:
//      * parameter 1 is the rect being inserted,
//      * parameter 2 is its ID. NOTE: *** ID MUST NEVER BE ZERO ***, hence the +1,
//      * parameter 3 is the root of the tree. Note: its address is passed
//      * because it can change as a result of this call, therefore no other parts
//      * of this code should stash its address since it could change undernieth.
//      * parameter 4 is always zero which means to add from the root.
//      */
//     for(i=0; i<nrects; i++) {
//                 /* i+1 is rect ID. Note: root can change */
//         RTreeInsertRect(&rects[i], i+1, &root, 0);
//     }
//     nhits = RTreeSearch(root, &search_rect, MySearchCallback, 0);
//     printf("Search resulted in %d hits\n", nhits);
// 
//     return 0;
// }
