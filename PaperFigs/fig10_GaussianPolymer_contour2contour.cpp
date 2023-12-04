// Standard C++ headers
#include<iostream>

// Include SEB functionality
#include "SEB.hpp"

/*
    In this example we generate a random walk polymer by linking N polymers together contour-to-contour
*/

int main()
{
   World w;
   
   // Add one polymer, and link 99 more polymers to it
   GraphID rw = w.Add("GaussianPolymer","P1","p");
   for (int i=2; i<=100; i++)
      {
          string now ="P"+to_string(i)  +".contour#r"+to_string(i);
          string last="P"+to_string(i-1)+".contour#s"+to_string(i-1);       
          w.Link("GaussianPolymer",now,last,"p");
      }
      
    // name the structure
    w.Add(rw, "RandomWalkPolymer");
    
    // Get the form factor expression
    ex F=w.FormFactor("RandomWalkPolymer");
    
    // Set parameters
    ParameterList params;
    w.setParameter(params,"beta_p",1);
    w.setParameter(params,"Rg_p",1);
    
    // Define vector of q values we want to sample.
    DoubleVector qvec = w.logspace(0.01, 50.0, 400);
    
    // Save file
    w.Evaluate(F, params, qvec, "fig10b.q", "Chain of 100 polymers linked contour2contour");
}

