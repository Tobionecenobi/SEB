// Standard C++ headers
#include<iostream>
#include<string>

// Include SEB functionality
#include "SEB.hpp"

/*
    In this example we generate a dendrimer by linking together 4 generations of
    three functional connections of polymers.
*/


void Attach(int g, int f, string ref, int& c, World& w)
{
  int arm=f-1;
  if (ref=="center.point") arm=f;
  for (int i=0;i<arm;i++)
   {
    string name = "S"+to_string(c)+".contour#r"+to_string(c);
    w.Link("GaussianPolymer", name, ref, "poly");
    string newref  = "S"+to_string(c)+".contour#l"+to_string(c);
    c++;
    if (g>1) Attach( g-1, f, newref, c, w);
   }
}

int main()
{
   World w;
   
   GraphID dendrimer = w.Add("Point","center");
   int count=0;
   Attach(4, 3, "center.point", count, w);
   w.Add(dendrimer, "Dendrimer");

   ex F=w.FormFactor("Dendrimer");
   cout << "Form Factor of dendrimer:" << endl;
   cout <<  F << endl;

   ParameterList params;
   w.setParameter(params,"beta_poly",1);
   w.setParameter(params,"Rg_poly",1);

   // Choose a log-distributed set of q-values
   DoubleVector qvec = w.logspace(0.01, 50.0, 400);
    
   // Evaluate form factor and save to file
   w.Evaluate( F, params, qvec, "fig11b.q", "Form factor of g=4 f=4 dendrimer of contour-to-contour linked polymers. Rg=1");


}
