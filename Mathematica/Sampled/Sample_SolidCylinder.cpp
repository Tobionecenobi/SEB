/*

    Samples scattering contributions from a spherical shell with inner radius Ri and outer radius Ro.


*/ 
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
namespace fs = std::filesystem;

#include <iostream>
#include <random>
#include <numbers>
#include "DebyeSampler.hpp"

using namespace std;

default_random_engine re;
#include "GenerateSamples.hpp"

void Sample(double R, double L)
{
   string dir="SolidCylinder_R"+to_string(R)+"_L"+to_string(L)+"/";
   fs::create_directory(dir);
   
   uniform_real_distribution<double> dist(-1,1);
   uniform_real_distribution<double> unit(0,1);
   int N = 100000000;

   vector<double> qvec;
   double qmin=1;
   double qmax=50;
 
   double q=qmin;
   while (q<qmax)
      {
         qvec.push_back(q);
         q*=1.03;
      }

   Sampler FF(qvec,          dir+"FF.q");
   Sampler FFA_center(qvec,  dir+"FFA_center.q");
   Sampler FFA_end(qvec,     dir+"FFA_end.q");
   Sampler FFA_hull(qvec,    dir+"FFA_hull.q");
   Sampler FFA_surface(qvec, dir+"FFA_surface.q");
   Sampler FFA_point(qvec,   dir+"FFA_endpoint.q");

   Sampler PF_center_hull(qvec,    dir+"PF_center_hull.q");
   Sampler PF_center_end(qvec,     dir+"PF_center_end.q");
   Sampler PF_center_surface(qvec, dir+"PF_center_surface.q");   
   Sampler PF_end_end(qvec,        dir+"PF_end_end.q");
   Sampler PF_end_hull(qvec,       dir+"PF_end_hull.q");
   Sampler PF_end_point(qvec,      dir+"PF_end_point.q");
   Sampler PF_end_surface(qvec,    dir+"PF_end_surface.q");
   Sampler PF_hull_hull(qvec,      dir+"PF_hull_hull.q");
   Sampler PF_hull_point(qvec,     dir+"PF_hull_point.q");
   Sampler PF_hull_surface(qvec,   dir+"PF_hull_surface.q");
   Sampler PF_point_surface(qvec,  dir+"PF_point_surface.q");
   Sampler PF_surface_surface(qvec, dir+"PF_surface_surface.q");

   double Ahull=2*std::numbers::pi*R*L;
   double Aend=std::numbers::pi*R*R;
  
   double Phull = Ahull/(Ahull+2*Aend);   // Area fraction of outside is [0:Pout] whereas [Pout:1] is the inner surface
   
   for (int i=0;i<N;i++)
      {
          if (i % (N/100)==0)
                cout << 100.0*i/N << "% done\n";
      
         // Inside points
         double x1,y1,z1, x2,y2,z2;         
         RandomSolidCylinder(L, R, x1,y1,z1, dist);
         RandomSolidCylinder(L, R, x2,y2,z2, dist);
                
         // Points on the ends:
         double xe1,ye1,ze1;
         double xe2,ye2,ze2;
         RandomDisk(R,xe1,ye1,ze1, dist);
         RandomDisk(R,xe2,ye2,ze2, dist);

         if (unit(re)>0.5)  ze1=L/2; else ze1=-L/2;
         if (unit(re)>0.5)  ze2=L/2; else ze2=-L/2;

         // Points on the end axis:
         double xa1,ya1,za1;
         double xa2,ya2,za2;

         xa1= ya1=0; za1=L/2;
         xa2= ya2=0; za2=-L/2;

         double xh1,yh1,zh1;
         double xh2,yh2,zh2;

         RandomCylinderHull(L,R, xh1,yh1,zh1, dist);
         RandomCylinderHull(L,R, xh2,yh2,zh2, dist);

         // Random points inside shell
         FF.add(x1-x2,y1-y2,z1-z2);

         // Form factor amplitudes
         
         // Center to scatterer
         FFA_center.add(x1,y1,z1);
         FFA_center.add(x2,y2,z2);
         
         // Ends to scatterer
         FFA_end.add(x1-xe1,y1-ye1,z1-ze1);
         FFA_end.add(x2-xe1,y2-ye1,z2-ze1);
         FFA_end.add(x1-xe2,y1-ye2,z1-ze2);
         FFA_end.add(x2-xe2,y2-ye2,z2-ze2);

         // hull to scatterer
         FFA_hull.add(x1-xh1,y1-yh1,z1-zh1);
         FFA_hull.add(x2-xh1,y2-yh1,z2-zh1);
         FFA_hull.add(x1-xh2,y1-yh2,z1-zh2);
         FFA_hull.add(x2-xh2,y2-yh2,z2-zh2);

         // Axis points as the end
         FFA_point.add(x1-xa1,y1-ya1,z1-za1);
         FFA_point.add(x1-xa2,y1-ya2,z1-za2);
         FFA_point.add(x2-xa1,y2-ya1,z2-za1);
         FFA_point.add(x2-xa2,y2-ya2,z2-za2);

         // Any surface to scatterer
         if (unit(re)<Phull)   FFA_surface.add( x1-xh1,y1-yh1,z1-zh1 );  // Hull
                         else  FFA_surface.add( x1-xe1,y1-ye1,z1-ze1 );  // ends

         if (unit(re)<Phull)   FFA_surface.add( x2-xh2,y2-yh2,z2-zh2 );  // Hull
                         else  FFA_surface.add( x2-xe2,y2-ye2,z2-ze2 );  // ends

         if (unit(re)<Phull)   FFA_surface.add( x2-xh1,y2-yh1,z2-zh1 );  // Hull
                         else  FFA_surface.add( x2-xe1,y2-ye1,z2-ze1 );  // ends

         if (unit(re)<Phull)   FFA_surface.add( x1-xh2,y1-yh2,z1-zh2 );  // Hull
                         else  FFA_surface.add( x1-xe2,y1-ye2,z1-ze2 );  // ends


         // Phase factors
                           
         PF_center_hull.add( xh1,yh1,zh1);
         PF_center_hull.add( xh1,yh1,zh1);

         PF_center_end.add( xe1,ye1,ze1);
         PF_center_end.add( xe2,ye2,ze2);

         if (unit(re)<Phull)   PF_center_surface.add( xh1,yh1,zh1);  // Hull
                         else  PF_center_surface.add( xe1,ye1,ze1);  // ends

         if (unit(re)<Phull)   PF_center_surface.add( xh2,yh2,zh2);  // Hull
                         else  PF_center_surface.add( xe2,ye2,ze2);  // ends
         
         // end2end
         PF_end_end.add( xe2-xe1,ye2-ye1,ze2-ze1);                  
         
         // end2hull
         PF_end_hull.add( xe1-xh1,ye1-yh1,ze1-zh1);
         PF_end_hull.add( xe1-xh2,ye1-yh2,ze1-zh2);
         PF_end_hull.add( xe2-xh2,ye2-yh2,ze2-zh2);
         PF_end_hull.add( xe2-xh1,ye2-yh1,ze2-zh1);

         // end to axis point
         PF_end_point.add(xe1-xa1,ye1-ya1,ze1-za1);
         PF_end_point.add(xe2-xa1,ye2-ya1,ze2-za1);
         PF_end_point.add(xe1-xa2,ye1-ya2,ze1-za2);
         PF_end_point.add(xe2-xa2,ye2-ya2,ze2-za2);


         // end2surface
         if (unit(re)<Phull)   PF_end_surface.add( xe1-xh1,ye1-yh1,ze1-zh1 );  // Hull
                         else  PF_end_surface.add( xe1-xe2,ye1-ye2,ze1-ze2 );  // ends
 
         // hull2hull
         PF_hull_hull.add( xh2-xh1,yh2-yh1,zh2-zh1);

         // hull to axis point
         PF_hull_point.add(xh1-xa1,yh1-ya1,zh1-za1);
         PF_hull_point.add(xh2-xa1,yh2-ya1,zh2-za1);
         PF_hull_point.add(xh1-xa2,yh1-ya2,zh1-za2);
         PF_hull_point.add(xh2-xa2,yh2-ya2,zh2-za2);

         
         // hull2surface
         if (unit(re)<Phull)   PF_hull_surface.add( xh2-xh1,yh2-yh1,zh2-zh1 );  // Hull
                         else  PF_hull_surface.add( xe1-xh1,ye1-yh1,ze1-zh1 );  // ends


         // axis point 2 surface
         if (unit(re)<Phull)   PF_point_surface.add( xh1-xa1,yh1-ya1,zh1-za1 );  // Hull
                         else  PF_point_surface.add( xe1-xa1,ye1-ya1,ze1-za1 );  // ends

         if (unit(re)<Phull)   PF_point_surface.add( xh2-xa1,yh2-ya1,zh2-za1 );  // Hull
                         else  PF_point_surface.add( xe2-xa1,ye2-ya1,ze2-za1 );  // ends

         if (unit(re)<Phull)   PF_point_surface.add( xh1-xa2,yh1-ya2,zh1-za2 );  // Hull
                         else  PF_point_surface.add( xe1-xa2,ye1-ya2,ze1-za2 );  // ends

         if (unit(re)<Phull)   PF_point_surface.add( xh2-xa2,yh2-ya2,zh2-za2 );  // Hull
                         else  PF_point_surface.add( xe2-xa2,ye2-ya2,ze2-za2 );  // ends

         // surface2surface
         bool s1=unit(re)<Phull;
         bool s2=unit(re)<Phull;
         if (s1 && s2)   PF_surface_surface.add( xh2-xh1,yh2-yh1,zh2-zh1 );  // Hull 2 hull
         if (!s1 && s2)  PF_surface_surface.add( xe2-xh1,ye2-yh1,ze2-zh1 );  // end 2 hull
         if (s1 && !s2)  PF_surface_surface.add( xh2-xe1,yh2-ye1,zh2-ze1 );  // Hull 2 end
         if (!s1 && !s2) PF_surface_surface.add( xe2-xe1,ye2-ye1,ze2-ze1 );  // end 2 end        
      }

}

int main()
{
    Sample(2.0,0.5);
    Sample(1.0,1.5);
}
