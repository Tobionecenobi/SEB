// Standard C++ headers
#include<iostream>

// Include SEB functionality
#include "SEB.hpp"

/*
    In this example we generate a random walk polymer by linking N rods together.     
*/

int main()
{
    World w;

    // Define first rod in the chain
    GraphID rw = w.Add(new ThinRod(), "R1","rod");

    // Add 100 more rods end-to-end
    int N=100;
    for (int i=2; i<N; i++)
      {
          string now  = "R"+to_string(i);
          string last = "R"+to_string(i-1);       
          w.Link(new ThinRod(),  now+".end1", last+".end2", "rod");
      }
    
    // Name the structure formed by the rods
    w.Add(rw, "RandomWalkRods");
    
    // Obtain form factor expression
    ex F=w.FormFactor("RandomWalkRods");

    // To evaluate the expression, we need to specify excess scattering length and length of rods.
    ParameterList params;
    w.setParameter(params,"beta_rod",1);
    w.setParameter(params,"L_rod",sqrt(12));   // Rg2 = L^2/12 = 1 => L=sqrt(12)

    // Choose a log-distributed set of q-values
    DoubleVector qvec = w.logspace(0.01, 10.0, 200);
    
    // Evaluate form factor and save to file
    w.Evaluate( F, params, qvec, "formfactor_rwrods.q", "Form factor of 100 end-to-end linked rods. L=0.1");

    // Define polymer chain
    GraphID poly = w.Add(new GaussianPolymer(), "Polymer");
    ex Fpoly=w.FormFactor("Polymer");

    ex Rg2expression=w.RadiusOfGyration2("RandomWalkRods");
    double Rg2value=w.Evaluate(Rg2expression, params);    
    w.setParameter(params,"beta_Polymer",100);
    w.setParameter(params,"Rg_Polymer",sqrt(Rg2value));
    
    // Evaluate form factor and save to file
    w.Evaluate( Fpoly, params, qvec, "formfactor_polymer.q", "Form factor of single Gaussian polymer chain");

/*
    ex q=w.getSymbol("q");
    ex Fguinier=1-q*q*Rg2value/6;
    w.Evaluate( Fguinier, params, qvec, "formfactor_guinier.q", "Form factor Guinier expanded.");
*/
}

