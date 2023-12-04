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
#include "DebyeSampler.hpp"

using namespace std;

default_random_engine re;
uniform_real_distribution<double> unit(0,1);

#include "GenerateSamples.hpp"

void Sample(double Ri, double Ro)
{
   string dir="SolidSphericalShell_Ri"+to_string(Ri)+"_Ro"+to_string(Ro)+"/";
   fs::create_directory(dir);

   uniform_real_distribution<double> dist(-Ro,Ro);
 
   int N = 100000000;
  
   vector<double> qvec;
   double qmin=0.1;
   double qmax=100;
 
   double q=qmin;
   while (q<qmax)
      {
         qvec.push_back(q);
         q*=1.03;
      }
   
   Sampler FF(qvec,                  dir+"FF.q" );
   Sampler FFAcenter(qvec,           dir+"FFAcenter.q" );
   Sampler FFAinner(qvec,            dir+"FFAinner.q" );
   Sampler FFAouter(qvec,            dir+"FFAouter.q" );
   Sampler FFAsurface(qvec,          dir+"FFAsurface.q" );
   Sampler Psi_inner_inner(qvec,     dir+"Pinner_inner.q" );
   Sampler Psi_outer_outer(qvec,     dir+"Pouter_outer.q" );
   Sampler Psi_inner_outer(qvec,     dir+"Pinner_outer.q" );
   Sampler Psi_center_surface(qvec,  dir+"Pcenter_surface.q" );
   Sampler Psi_inner_surface(qvec,   dir+"Pinner_surface.q" );
   Sampler Psi_outer_surface(qvec,   dir+"Pouter_surface.q" );
   Sampler Psi_surface_surface(qvec, dir+"Psurface_surface.q" );
  
   double Pout = Ro*Ro/(Ri*Ri+Ro*Ro);   // Area fraction of outside is [0:Pout] whereas [Pout:1] is the inner surface
   
   for (int i=0;i<N;i++)
      {
          if (i % (N/100)==0)
                cout << 100.0*i/N << "% done\n";
      
         double x1,y1,z1, x2,y2,z2;
         
         RandomSolidShell(Ri,Ro, x1,y1,z1, dist);
         RandomSolidShell(Ri,Ro, x2,y2,z2, dist);
         
         double xi1,yi1,zi1, xi2,yi2,zi2;
         double xo1,yo1,zo1, xo2,yo2,zo2;
         
         RandomSurface(Ri, xi1,yi1,zi1, dist);
         RandomSurface(Ri, xi2,yi2,zi2, dist);
         RandomSurface(Ro, xo1,yo1,zo1, dist);
         RandomSurface(Ro, xo2,yo2,zo2, dist);

         // Random points inside shell
         FF.add(x1-x2,y1-y2,z1-z2);

         // Form factor amplitudes
         
         // Center to scatterer
         FFAcenter.add(x1,y1,z1);
         FFAcenter.add(x2,y2,z2);
         
         // Inner surface to scatterer
         FFAinner.add(x1-xi1,y1-yi1,z1-zi1);
         FFAinner.add(x2-xi1,y2-yi1,z2-zi1);
         FFAinner.add(x1-xi2,y1-yi2,z1-zi2);
         FFAinner.add(x2-xi2,y2-yi2,z2-zi2);

         // Outer surface to scatterer
         FFAouter.add(x1-xo1,y1-yo1,z1-zo1);
         FFAouter.add(x2-xo1,y2-yo1,z2-zo1);
         FFAouter.add(x1-xo2,y1-yo2,z1-zo2);
         FFAouter.add(x2-xo2,y2-yo2,z2-zo2);

         // Any surface to scatterer
         double r=unit(re);
         if (r<Pout)   FFAsurface.add( x1-xo1,y1-yo1,z1-zo1 );
                 else  FFAsurface.add( x1-xi1,y1-yi1,z1-zi1 );

         r=unit(re);
         if (r<Pout)   FFAsurface.add( x2-xo2,y2-yo2,z2-zo2 );
                 else  FFAsurface.add( x2-xi2,y2-yi2,z2-zi2 );


         // Phase factors
         
         Psi_inner_inner.add( xi2-xi1,yi2-yi1,zi2-zi1);
         Psi_outer_outer.add( xo2-xo1,yo2-yo1,zo2-zo1);

         Psi_inner_outer.add( xo1-xi1,yo1-yi1,zo1-zi1);
         Psi_inner_outer.add( xo2-xi2,yo2-yi2,zo2-zi2);

         r=unit(re);
         if (r<Pout)   Psi_center_surface.add( xo1,yo1,zo1 );
                 else  Psi_center_surface.add( xi1,yi1,zi1 );

         r=unit(re);
         if (r<Pout)   Psi_center_surface.add( xo2,yo2,zo2 );
                 else  Psi_center_surface.add( xi2,yi2,zi2 );


         r=unit(re);
         if (r<Pout)   Psi_inner_surface.add( xo1-xi1,yo1-yi1,zo1-zi1 );
                 else  Psi_inner_surface.add( xi2-xi1,yi2-yi1,zi2-zi1 );

         r=unit(re);
         if (r<Pout)   Psi_outer_surface.add( xo2-xo1,yo2-yo1,zo2-zo1 );
                 else  Psi_outer_surface.add( xi2-xo1,yi2-yo1,zi2-zo1 );


         double r1=unit(re);
         double r2=unit(re);
         if (r1<Pout && r2<Pout)   Psi_surface_surface.add( xo2-xo1,yo2-yo1,zo2-zo1 );    // 1 and 2 are outside
         if (r1<Pout && r2>Pout)   Psi_surface_surface.add( xi2-xo1,yi2-yo1,zi2-zo1 );    // 1 is outside, 2 is inside
         if (r1>Pout && r2<Pout)   Psi_surface_surface.add( xo2-xi1,yo2-yi1,zo2-zi1 );    // 2 is outside, 1 is inside
         if (r1>Pout && r2>Pout)   Psi_surface_surface.add( xi2-xi1,yi2-yi1,zi2-zi1 );    // 1 and 2 are inside
      }
}

int main()
{
    Sample(2.33,3.44);
}
