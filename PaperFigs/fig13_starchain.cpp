#include "SEB.hpp"

int main()
{ 
  // Define the world
  World w;

  // Make diblock
  GraphID d = w.Add("GaussianPolymer","A");
  w.Link( "GaussianPolymer","B.end1","A.end2");

  // Make star with diblocks
  GraphID s = w.Add(d,"diblock1");
  w.Link(d,"diblock2:A.end1","diblock1:A.end1");
  w.Link(d,"diblock3:A.end1","diblock1:A.end1");
  w.Link(d,"diblock4:A.end1","diblock1:A.end1");

  // Make chain with stars
  GraphID c = w.Add(s,"star1");
  w.Link(s,"star2:diblock1:B.end2","star1:diblock3:B.end2");
  w.Link(s,"star3:diblock1:B.end2","star2:diblock3:B.end2");
  w.Link(s,"star4:diblock1:B.end2","star3:diblock3:B.end2");
  w.Link(s,"star5:diblock1:B.end2","star4:diblock3:B.end2");

  // Name the chain
  w.Add(c, "chain");
  
  // To evaluate the expression, we need to specify excess scattering length and length of rods.  
  ex F=w.FormFactor("chain");
  ParameterList params;
  w.setParameter(params,"Rg_A",1);
  w.setParameter(params,"Rg_B",1);
  w.setParameter(params,"beta_A",1);
  w.setParameter(params,"beta_B",1);

  // Choose a log-distributed set of q-values
  DoubleVector qvec = w.logspace(0.01, 50.0, 400);
    
  // Evaluate form factor and save to file
  w.Evaluate( F, params, qvec, "fig13_A1_B1.q", "Form factor of chain of stars of diblock copolymers. Rg=1 BetaA=BetaB=1");
  
  // Now change constrast and generate files
  w.setParameter(params,"beta_A",1);
  w.setParameter(params,"beta_B",0);
  w.Evaluate( F, params, qvec, "fig13_A1_B0.q", "Form factor of chain of stars of diblock copolymers. Rg=1 BetaA=1 BetaB=0");
  
  w.setParameter(params,"beta_A",0);
  w.setParameter(params,"beta_B",1);
  w.Evaluate( F, params, qvec, "fig13_A0_B1.q", "Form factor of chain of stars of diblock copolymers. Rg=1 BetaA=0 BetaB=1");
  
  // Estimate sizes:
  cout << "<Rg2>=" << latex << w.RadiusOfGyration2("chain") << endl;
  cout << "<R_center_to_all>=" << latex << w.SMSD_ref2scat("chain:star3:diblock1:A.end1") << endl;
  cout << "<R2_length>="  << latex << w.SMSD_ref2ref("chain:star1:diblock1:B.end2","chain:star5:diblock3:B.end2") << endl;
  cout << "<R2_breath>="  << latex << w.SMSD_ref2ref("chain:star3:diblock2:B.end2","chain:star3:diblock4:B.end2") << endl;
}
