#include<iostream>

using namespace std;

void transform_every_int(int *v, unsigned n, int (*fp)(int))
{
   for(unsigned i = 0; i < n; i++) {
      v[i] = fp(v[i]);
   }	
}

int square_int(int x) 
{
  return x*x;
}

int main()
{
  int a[5] = {1,2,3,4,5};
  transform_every_int(a, 5, square_int);
  
  for(int i = 0; i<5; i++) {
    cout<<a[i]<<" ";
  }
  cout<<endl;
  return 0;
}