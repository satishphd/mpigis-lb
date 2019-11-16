#include "filePartition/ShapeFilePartitioner.h"

  
 pair<FileSplits*, FileSplits*> ShapeFilePartitioner :: partition()
  {
    pair<FileSplits*, FileSplits*> emptyPair(nullptr, nullptr);
    return emptyPair;
  } 

  int ShapeFilePartitioner :: initialize(Config &args)
  {
    
    const char *layer1 = args.getLayer1()->front().c_str();
//	const char *layer2 = args.getLayer2()->front().c_str();
	
	hSHP = SHPOpen(layer1,"rb");
	
    return 0;
  }
  
  
  void ShapeFilePartitioner ::distRead(int startIndex,int endIndex,char * filename,SHPObject	***psShape)
{
	
	SHPObject **psShape_temp;
	SHPHandle	hSHP;
	int		nShapeType, nEntities;
	double 	adfMinBound[4], adfMaxBound[4];
	    	    
	int i;
    hSHP = SHPOpen(filename,"rb");
    if( hSHP == NULL ){
        printf( "Unable to open:%s\n", filename );
        exit( 1 );
    }	
    SHPGetInfo( hSHP, &nEntities, &nShapeType, adfMinBound, adfMaxBound );

    psShape_temp=(SHPObject **)malloc((endIndex-startIndex+1)*sizeof(SHPObject*));
    
    for(i=startIndex;i<=endIndex;i++){
		psShape_temp[i-startIndex] = SHPReadObject( hSHP, i );
		if( psShape_temp[i-startIndex] == NULL ){
            fprintf( stderr,"Unable to read shape %d, terminating object reading.\n",i );
            exit(1);
        }
	}	
	(*psShape)=psShape_temp;
}

void ShapeFilePartitioner :: destoryObjects(SHPObject **psShape,int num)
{
	int i;
	for(i=0;i<num;i++){
	    SHPDestroyObject(psShape[i]);
	}
}

void ShapeFilePartitioner :: printBaseObjects(SHPObject	**psShape,int num)
{
	int i;
	int iPart;
	const char 	*pszPlus;
	//printf("%d \n",num);
	
	for( i = 0; i < num; i++ )
    {
        int	j;
        printf( "b %d %.15g %.15g %.15g %.15g ",
                    i, psShape[i]->dfXMin, psShape[i]->dfYMin,
                       psShape[i]->dfXMax, psShape[i]->dfYMax);


        for( j = 0, iPart = 1; j < psShape[i]->nVertices; j++ )
        {
          printf(" %.15g %.15g ",
                       psShape[i]->padfX[j],
                       psShape[i]->padfY[j]);
            
        }
        printf("\n");
    }
}
