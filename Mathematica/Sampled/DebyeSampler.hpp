#include <valarray>
#include <vector>
#include <fstream>
#include <string>

using namespace std;

class Sampler
{
    long count=0;
    double R2=0;
    valarray<double> q;
    valarray<double> I;
    string fnam="";
    
   public:
    Sampler(vector<double> &qin, string fn)
      {
          fnam=fn;
          
          q.resize(qin.size());
          I.resize(qin.size());
          
          for (int i=0;i<qin.size();i++)
            {
               q[i]=qin[i];
               I[i]=0.0;
            }
      }

    ~Sampler()
      {
          // Normalize          
          R2/=count; I/=count;
          
          ofstream fo(fnam.c_str());
          fo << "# <R^2> = " << R2 << endl; 
       
          for (int i=0;i<q.size();i++)
               fo << q[i] << " " << I[i] << endl;

          fo.close();            
      }
      
     
     void add(double dx,double dy,double dz)
      {
         double r2=dx*dx+dy*dy+dz*dz;
         double r=sqrt(r2);
         
         R2+=r2;
         I += sin(q*r)/(q*r);
         
         count++;
      }

};

