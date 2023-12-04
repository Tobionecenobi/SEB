// Standard C++ headers
#include<iostream>

// Include SEB functionality
#include "SEB.hpp"

/*

    In this example we generate a f x f dendrimer, where f is the functionality of
    nodes and g is the number of generations.
    
    Generation 0 is a f functional star
    
      end1  poly0   end2
    x -------------------
    |
    x end1  poly1   end2
    |
    x end1  poly2   end2
    :
    x end1  polyf-1 end2
    
   The recursively we add f-1   polymers to each tip of the f gen0 polymers., 
      
       polyi.end2   polyi0.end1
   -------------- x --------------
                  | polyi1.end1
                  x --------------
                  : polyif-1.end1
                  x --------------
                                                    
*/

// Recursive add branches.
void AddBranches(int g, int f, string name, World& w)
{
   if (g==0) return;
   
   // Add f-1 branches to the tip of the previous generation polymer.
   for (int i=1;i<f;i++)
       {
          string newname = name+to_string(i);
          w.Link(new GaussianPolymer(), newname+".end1", name+".end2", "poly");
          AddBranches( g-1, f, newname, w);
       }
}

int main()
{
 try{
    World w = World("w");

    int f = 4;  // 4 functional links
    int g= 3;   // 3 generations         
        
    // Make initial f-functional star (gen0)
    GraphID dendrimer = w.Add(new GaussianPolymer(), "poly0", "poly");
    for (int i=1;i<f;i++)
       {
          string myname = "poly"+to_string(i);
          w.Link(new GaussianPolymer(), myname+".end1", "poly0.end1", "poly");
       }

    // Add branches to each of the f branches
    for (int i=0;i<f;i++)
       {
          string myname = "poly"+to_string(i);
          AddBranches(g-1, f, myname, w);
       }

    // Name the first generation
    w.Add(dendrimer, "dendrimer");

    cout << "Form Factor of dendrimer:" << endl;
    cout <<  w.FormFactor("dendrimer") << endl;

    cout << "Form Factor amplitudeof dendrimer relative to core:" << endl;
    cout <<  w.FormFactorAmplitude("dendrimer:poly0.end1") << endl;

    cout << "Number of pairs of polymers" << endl;
    cout << w.CountPairs("dendrimer") << endl;

    cout << "Number of sub-units" << endl;
    cout << w.Count("dendrimer:poly0.end1") << endl;

}
catch (const SEBException e)
{
    std::cout << e;                    // Print what the error was, and where it was triggered.
}



}
