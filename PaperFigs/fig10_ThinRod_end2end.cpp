// Standard C++ headers
#include<iostream>

// Include SEB functionality
#include "SEB.hpp"

/*
    In this example we generate a random walk polymer by linking N rods together end-to-end
*/

int main()
{
   World w;
   
   // Add one polymer, and link 99 more polymers to it
   GraphID rw = w.Add("ThinRod","R1","r");
   for (int i=2; i<=100; i++)
      {
          string now ="R"+to_string(i)  +".end1";
          string last="R"+to_string(i-1)+".end2";       
          w.Link("ThinRod",now,last,"r");
      }
      
    // name the structure
    w.Add(rw, "RandomWalkRods");
    
    // Get the form factor expression
    ex F=w.FormFactor("RandomWalkRods");
    
    // Set parameters
    ParameterList params;
    w.setParameter(params,"beta_r",1);
    w.setParameter(params,"L_r",sqrt(12.0));  // Rg2=1
    
    // Define vector of q values we want to sample.
    DoubleVector qvec = w.logspace(0.01, 50.0, 400);
    
    // Save file
    w.Evaluate(F, params, qvec, "fig10a.q", "Chain of 100 rods linked end2end");
}

