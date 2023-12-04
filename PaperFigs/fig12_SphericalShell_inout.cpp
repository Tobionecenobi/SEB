// Standard C++ headers
#include<iostream>

// Include SEB functionality
#include "SEB.hpp"

/*
    In this example we generate a spherical shell, and attach polymers to the outside and rods to the inside.
*/

int main()
{
    World w;

    // Define first rod in the chain
    GraphID str = w.Add( "SolidSphericalShell", "shell");

    // Add 100 more rods linking to a random point on the contour of the past rod
    int N=50;
    for (int i=0; i<N; i++)
      {
          string polyname    = "poly"+to_string(i)+".end1";
          string shellref1  = "shell.surfaceo#p"+to_string(i);
          w.Link( "GaussianPolymer",  polyname, shellref1, "poly");

          string rodname     = "rod"+to_string(i)+".end1";
          string shellref2  = "shell.surfacei#r"+to_string(i);
          w.Link( "ThinRod",  rodname, shellref2, "rod");          
      }
    
    // Name the structure formed by the rods
    w.Add(str, "Structure");
    
    // Obtain form factor expression
    ex F=w.FormFactor("Structure");

    // To evaluate the expression, we need to specify excess scattering length and length of rods.
    ParameterList params;
    w.setParameter(params,"beta_shell",0);
    w.setParameter(params,"Ri_shell",8);
    w.setParameter(params,"Ro_shell",12);
    
    w.setParameter(params,"beta_poly",1);
    w.setParameter(params,"Rg_poly",1);

    w.setParameter(params,"beta_rod",1);
    w.setParameter(params,"L_rod",sqrt(12));

    // Choose a log-distributed set of q-values
    DoubleVector qvec = w.logspace(0.05, 5.0, 200);
    
    // Evaluate form factor and save to file
    w.Evaluate( F, params, qvec, "fig12d.q", "Form factor of 50 polymers on the outer surface of a spherical shell, and 50 rods on the interiour surface of the shell.");
}

