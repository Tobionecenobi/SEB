// Standard C++ headers
#include<iostream>

// Include SEB functionality
#include "SEB.hpp"

/*
    In this example we generate a random walk polymer by linking N rods together
    at a random point along their contours
*/

int main()
{
    World w;

    // Define first rod in the chain
    GraphID rw = w.Add( "GaussianLoop", "L1","loop");

    // Add 100 more rods linking to a random point on the contour of the past rod
    int N=100;
    for (int i=2; i<N; i++)
      {
          string now  = "L"+to_string(i)  +".contour#r" + to_string(i);
          string last = "L"+to_string(i-1)+".contour#l" + to_string(i-1);
          w.Link( "GaussianLoop",  now, last, "loop");
      }
    
    // Name the structure formed by the rods
    w.Add(rw, "RandomWalkLoops");
    
    // Obtain form factor expression
    ex F=w.FormFactor("RandomWalkLoops");

    // To evaluate the expression, we need to specify excess scattering length and length of rods.
    ParameterList params;
    w.setParameter(params,"beta_loop",1);
    w.setParameter(params,"Rg_loop",1);

    // Choose a log-distributed set of q-values
    DoubleVector qvec = w.logspace(0.01, 50.0, 400);
    
    // Evaluate form factor and save to file
    w.Evaluate( F, params, qvec, "fig10e.q", "Form factor of 100 contour-to-contour linked loops. Rg=1");
}

