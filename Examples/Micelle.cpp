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

    // Spherical core
    GraphID g = w.Add(new SolidSphere(), "sphere");

    int N=100;
    for (int i=0; i<N; i++)
          {
             string name = string("poly")+to_string(i)+string(".end1");
             string ref  = string("sphere.surface#r")+to_string(i);
             
             w.Link(new GaussianPolymer(), name, ref, "poly");
          }

    // Define micelle structure
    GraphID g1 = w.Add(g, "micelle");

    // Define latex as ouput format
    cout << latex;
    cout <<  "Form factor= ";
    cout <<  w.FormFactor("micelle") << endl << endl;

    cout <<  "Form factor amplitude relative to centre= ";
    cout <<  w.FormFactorAmplitude("micelle:sphere.center") << endl << endl;

    cout <<  "Phase factor tip-to-tip= ";
    cout <<  w.PhaseFactor("micelle:poly0.end2","micelle:poly1.end2") << endl << endl;

}
