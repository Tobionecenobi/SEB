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
    w.Add(g1, "star1");

    // Using one end of a polymer to define the core.
    GraphID g2 = w.Add(new GaussianPolymer(), "poly0" , "poly");
    for (int i=1; i<N; i++)
          {
             string name = string("poly")+to_string(i)+string(".end1");             
             w.Link(new GaussianPolymer(), name, "poly0.end1", "poly");
          }

    // Define micelle structure
    w.Add(g2, "star2");

    ex F1=w.FormFactor_Unnormalized("star1");
    ParameterList p1=w.getParams();

    ex F2=w.FormFactor_Unnormalized("star2");
    ParameterList p2=w.getParams();

    for (auto const& p: p1) cout << " " << p.first;
    cout << endl;
    for (auto const& p: p2) cout << " " << p.first;
    cout << endl;

    cout << "Form factor of star with centre point:" << endl;
    cout << F1 << endl;

    cout << "Form factor of using poly0.end1 as link:" << endl;
    cout << F2 << endl;

    cout << "Form factor difference:" << endl;
    cout << F1-F2 << endl;
    
    cout << "Counting sub-units (automatically neglecting points):" << endl;
    cout << w.Count("star1:p.point") << endl;
    cout << w.Count("star2:poly0.end1") << endl;

    cout << "Counting sub-units pairs (automatically neglecting points):" << endl;
    cout << w.CountPairs("star1") << endl;
    cout << w.CountPairs("star2") << endl;

}
