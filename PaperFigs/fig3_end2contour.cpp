#include "SEB.hpp"
int main()
{
  World w;
  GraphID g=w.Add("GaussianPolymer", "A");
  w.Link("GaussianPolymer", "B.contour#r1", "A.end2");
  w.Add(g, "DiBlockCopolymer");
  cout << latex;
  cout << w.FormFactor("DiBlockCopolymer");

  // To evaluate the expression, we need to specify excess scattering length and length of rods.  
  ex F=w.FormFactor("DiBlockCopolymer");
  ParameterList params;
  w.setParameter(params,"beta_A",1);
  w.setParameter(params,"Rg_A",1);
  w.setParameter(params,"beta_B",1);
  w.setParameter(params,"Rg_B",1);

  // Choose a log-distributed set of q-values
  DoubleVector qvec = w.logspace(0.01, 50.0, 400);
    
  // Evaluate form factor and save to file
  w.Evaluate( F, params, qvec, "fig3c.q", "Form factor of end-to-contour linked diblock copolymer. Rg=1");
}
