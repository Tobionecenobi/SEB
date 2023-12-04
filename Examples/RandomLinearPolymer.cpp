// Standard C++ headers
#include<iostream>

// Include SEB functionality
#include "SEB.hpp"

/*

    In this example we generate a random linear polymer formed by linking polymer 0 and 1
    by a random point along their contur, then linking polymer 2 by a random point to a random
    point on polymer 1 and so on.
       
*/

int main()
{
 try{
    World w("World");

    // Define first polymer in the chain
    GraphID diblock = w.Add(new GaussianPolymer(), "poly0","poly");

    // Link N polymers contour to contour.
    int N=3;
    for (int i=1; i<N; i++)
      {
          string now  = "poly"+to_string(i)+".contour#r";
          string last = "poly"+to_string(i-1)+".contour#r";
          
          w.Link(new GaussianPolymer(),  now, last, "poly");
      }
    
    // Name the structure
    w.Add(diblock, "RandomLinear");
    
    // Obtain form factor expression
    ex F=w.FormFactor("RandomLinear");
    cout << "F = " << F << "\n";   
}

    

catch (const SEBException e)
{
    std::cout << e;                    // Print what the error was, and where it was triggered.
}



}
