
#include <geos/geom/Geometry.h>
#include <geos/io/WKTReader.h>
#include <geos/io/WKTWriter.h>

#include <geos/io/WKBReader.h>
#include <geos/io/WKBWriter.h>

#include <sstream>
//#include <mpi.h>
#include <unistd.h>
#include <string>
#include <string.h>
#include <iostream>
#include <string.h>

using namespace geos;
using namespace geos::geom;
using namespace std;

struct info
{
  int l;
  int b;
}info;

int main()
{
	io::WKBReader wkbreader;
	//string s = "010500000001000000010200000002000000A465A4DE539A55C0DA8B683BA6364040455D6BEF539A55C0C96B257497364040";
	string s = "010500000001000000010200000013000000D4298F6E84A555C017601F9DBA464040CE52B29C84A555C04DA1F31ABB464040F7CB272B86A555C09E93DE37BE46404050FD834886A555C0FD67CD8FBF4640407F677BF486A555C0F0517FBDC24640402CF3565D87A555C037FE4465C3464040C03C64CA87A555C0BE310400C74640406725ADF886A555C0CF68AB92C8464040CD1E680586A555C07BDAE1AFC9464040CE52B29C84A555C0D9AED007CB4640403E75AC527AA555C03604C765DC464040B184B53176A555C02D431CEBE24640406362F3716DA555C0938D075BEC4640409FC893A46BA555C027A3CA30EE464040779D0DF967A555C089981249F44640400892770E65A555C0A67D737FF54640403830B95164A555C03BC780ECF546404072DD94F25AA555C0041E1840F846404001840F255AA555C04641F0F8F6464040";
	istringstream is(s);
    Geometry *parsed = wkbreader.readHEX(is);
    cout<<parsed->toString()<<endl;
}

// int main2()
// {
//   struct info one;
//   one.l = 3;
//   one.b = 5;
// 
//   struct info two;
//   two.l = 13;
//   two.b = 15;
// 
//   vector<info> vec;
//   vec.push_back(one);
//   vec.push_back(two);
//   
//   info* myInfo = vec.data();
//   return 0;
// }
// 
// int main1(int argc, char **argv)
// {
//   int rank;
//   char hostname[256];
//   
//   MPI_Init(&argc,&argv);
//   MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//   gethostname(hostname,255);
// 
//   printf("Hello world!  I am process number: %d on host %s\n", rank, hostname);
//   
//  geos::io::WKTReader wktreader;
//  Geometry *geom = wktreader.read("POINT(-117 33)");
// 
//  io::WKTWriter wktWriter;
// 
//  
//  string str = wktWriter.write(geom);
//  
//  const char *oldArray = str.c_str();
//  
// 
//     
// //    Geometry *parsed = wktreader.read(string(oldArray));
// //    cout<<"After "<<parsed->toString()<<endl;
//  
//  if(rank == 0) {
//    //printf("%s\n", charray);
//    //cout<<str.size()<<endl;
//    MPI_Send(oldArray, str.size(), MPI_CHAR, 1, 12, MPI_COMM_WORLD);
//  } else {
//    char *recv = new char[str.size()];
//    //recv[str.size()] = '\0';
//    MPI_Status stat;
//    MPI_Recv(recv, str.size(), MPI_CHAR, 0, 12, MPI_COMM_WORLD, &stat);
//    
//    Geometry *parsed = wktreader.read(string(recv));
//    cout<<"DOn "<<parsed->toString()<<endl;
// 
//  }
//  
//  
//  //io::WKBReader wkbreader;
//  //Geometry *parsed = wkbreader.read(s);
//  //cout<<parsed->toString()<<endl;
//  MPI_Finalize();
//  
//  //return 0;
// }