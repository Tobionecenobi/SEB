#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
namespace fs = std::filesystem;

#include <random>
#include "DebyeSampler.hpp"

using namespace std;

default_random_engine re;
#include "GenerateSamples.hpp"


void Sample(double R)
{
   string dir="SolidSphere_R"+to_string(R)+"/";
   fs::create_directory(dir);
 
   uniform_real_distribution<double> dist(-R,R);
   uniform_real_distribution<double> unit(0,1);

   vector<double> qvec;
   double qmin=1;
   double qmax=50;
 
   double q=qmin;
   while (q<qmax)
      {
         qvec.push_back(q);
         q*=1.03;
      }

   Sampler FF(qvec,                  dir+"FF.q");
   Sampler FFA_center(qvec,          dir+"FFA_center.q");
   Sampler FFA_surface(qvec,         dir+"FFA_surface.q");
   Sampler PF_center_surface(qvec,   dir+"PF_center_surface.q");
   Sampler PF_surface_surface(qvec,  dir+"PF_surface_surface.q");

   int N = 100000000;
   
   double sample_center_sphere=0;
   double sample_center_surface=0;
   double sample_surface_sphere=0;
   double sample_sphere_sphere=0;
   double sample_surface_surface=0;
   double sample_Rg2=0;

   for (int i=0;i<N;i++)
      {
         double x,y,z;
         double x1,y1,z1;
         RandomSphere(R, x,y,z    , dist);
         RandomSphere(R, x1,y1,z1 , dist);

         double xs,ys,zs;
         double xs1,ys1,zs1;
         RandomSurface(R, xs,ys,zs    , dist);
         RandomSurface(R, xs1,ys1,zs1 , dist);

         // Random vectors between points inside sphere
         FF.add(x1-x,y1-y,z1-z);

         // Random vectors from center to points inside sphere.
         FFA_center.add(x1,y1,z1);
         FFA_center.add(x,y,z);

         // Random vectors from surface to points inside sphere.
         FFA_surface.add(x1-xs1,y1-ys1,z1-zs1);
         FFA_surface.add(x1-xs,y1-ys,z1-zs);
         FFA_surface.add(x-xs,y-ys,z-zs);
         FFA_surface.add(x-xs1,y-ys1,z-zs1);

         // Random vectors from center to surface
         PF_center_surface.add(xs,ys,zs);
         PF_center_surface.add(xs1,ys1,zs1);

         // Random vectors between points on surface  
         PF_surface_surface.add(xs1-xs,ys1-ys,zs1-zs);

// Distance from centre of mass to point inside sphere.
         sample_Rg2 +=   0.5*(   x*x+y*y+z*z +  x1*x1+y1*y1+z1*z1);
 
// Distance from centre to point inside sphere.
         sample_center_sphere +=   0.5*(   x*x+y*y+z*z +  x1*x1+y1*y1+z1*z1);
 
// Distance from surface to any point inside sphere.
         sample_surface_sphere += 0.5*(   (x-xs)*(x-xs)+(y-ys)*(y-ys)+(z-zs)*(z-zs) +   (x1-xs1)*(x1-xs1)+(y1-ys1)*(y1-ys1)+(z1-zs1)*(z1-zs1) );

// Distance between two random points inside sphere
         sample_sphere_sphere += (x-x1)*(x-x1)+(y-y1)*(y-y1)+(z-z1)*(z-z1);

// Distance between two random points on surface of sphere.
         sample_surface_surface += (xs-xs1)*(xs-xs1)+(ys-ys1)*(ys-ys1)+(zs-zs1)*(zs-zs1);

// Distance between the centre and the surface.
         sample_center_surface += 0.5*(    xs*xs+ys*ys+zs*zs + xs1*xs1+ys1*ys1+zs1*zs1);
      }

    sample_Rg2/=N;
    sample_center_sphere/=N;
    sample_center_surface/=N;

    sample_sphere_sphere/=N;

    sample_surface_sphere/=N;
    sample_surface_surface/=N;

    cout << "<Rg2_cm> = "              << sample_Rg2               << " (sigma=2)\n";
    cout << "<centre-to-sphere^2> = "  << sample_center_sphere     << "\n";
    cout << "<centre-to-surface^2> = " << sample_center_surface    << "\n";
    cout << "<surface_sphere^2>="      << sample_surface_sphere    << "\n";
    cout << "<sphere_sphere^2>="       << sample_sphere_sphere/2   << " (signa=2)\n";
    cout << "<surface_surface^2>="     << sample_surface_surface/2 << " (sigma=2)\n";

}

int main()
{
   Sample(1.0);
}
