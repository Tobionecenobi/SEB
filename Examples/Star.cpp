// Standard C++ headers
#include<iostream>

// Include SEB functionality
#include "SEB.hpp"

/*

    The example creates a micelle consisting of 1 sphere with N identical polymers attached to it.
    The polymers are attached at random points on the surface.
    
    
     sphere surface
        |
        | sphere.surface#r0 <-> poly0.end1 x----------------------------x
        |
        | sphere.surface#r1 <-> poly1.end1 x----------------------------x
        |
        | sphere.surface#r2 <-> poly2.end1 x----------------------------x
        |
        |  :

      All the polymers are tagged 'poly' because we assume they are identical.        

*/

int main()
{
    World w("world");

    int N=3;

    // Point core.
    GraphID g1 = w.Add(new Point(), "p");
    for (int i=0; i<N; i++)
          {
             string name = string("p")+to_string(i)+".end1";             
             w.Link(new GaussianPolymer(), name, "p.point", "poly" );
          }
    w.Add(g1, "star");

    ex F1=w.FormFactor_Unnormalized("star");
    ParameterList p1=w.getParams();

    for (auto const& p: p1) cout << " " << p.first;
    cout << endl;

    cout << "Form factor of star with centre point:" << endl;
    cout << F1 << endl;

    
    cout << "Counting sub-units (automatically neglecting points):" << endl;
    cout << w.Count("star:p.point") << endl;

    cout << "Counting sub-units pairs (automatically neglecting points):" << endl;
    cout << w.CountPairs("star") << endl;

}
