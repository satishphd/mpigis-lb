#include<string.h>
#include<stdio.h>
#include<iostream>
#include <cstdlib>

using namespace std;

typedef long long mbr_t;

void convert(double a1, double b1, double a2, double b2);

//================================ CoordToMBR ===================================
mbr_t CoordToMBR(char* ct, char type){
  mbr_t retVal=0;
  const char fracNum=12;
  int fCount=0, fDot=0;
  switch(type){
    case 0:
      retVal=atof(ct);
      break;
    case 1:
      //"float" to "long long"
      char buff[50];
      int i=0;
      while(*ct){
        if(*ct!='.'){
           if(fCount<fracNum){buff[i++]=*(ct);if(fDot)fCount++;}
           else break;
        }
        else fDot=1;
        ct++;
        }
      for(;fCount<fracNum;fCount++)buff[i++]='0';
      buff[i]='\0';
      retVal=atoll(buff);
      retVal+=9000000000000000;
      //printf("\n%s %ld\n", buff, retVal);
      break;
  }
  return(retVal);
}


int main()
{
 double a1 = 114.17;
 double b1 = 37.7654;
 double a2 = 144.951;
 double b2 = 37.7648; 
 
 convert(a1, b1, a2, b2);
 
 return 0;
}

void convert(double a1, double b1, double a2, double b2)
{
    char buffer[50];  // make sure this is big enough!!!
    
    snprintf(buffer, sizeof(buffer), "%f", a1);
    mbr_t mbr1 = CoordToMBR(buffer, 1);
    memset(buffer, 0, sizeof(buffer));

    snprintf(buffer, sizeof(buffer), "%f", b1);
    mbr_t mbr2 = CoordToMBR(buffer, 1);
    memset(buffer, 0, sizeof(buffer));
    
    snprintf(buffer, sizeof(buffer), "%f", a2);
    mbr_t mbr3 = CoordToMBR(buffer, 1);
    memset(buffer, 0, sizeof(buffer));

    snprintf(buffer, sizeof(buffer), "%f", b2);
    mbr_t mbr4 = CoordToMBR(buffer, 1);
    
    cout<<mbr1<<" "<<mbr2<<" "<<mbr3<<" "<<mbr4<<endl;
}