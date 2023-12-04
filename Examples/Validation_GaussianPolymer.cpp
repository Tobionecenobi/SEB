// Standard C++ headers
#include<iostream>

// Include SEB functionality
#include "SEB.hpp"

/*
      Use the Validate method to check sub-units in particular
      
          * All terms are defined for the set of reference points.
               * That is one form factor amplitude for each reference point.
               * A phase factor[r1][r2] is defined for each pair r1<r2 reference points.
               * For r1=r2 reference point pairs, phase factors should be defined for
                 distributed reference points, and NOT for specific reference point pairs.
                 
          * Normalization of all scattering terms. We assume F,A,Psi -> 1 for q->0
          * Guinier expansions should have a zero q term.
          * Rg²  should match the Guinier expansion of the form factor = 1 - q^2 <Rg^2>/3
          * sigma <R^2> should match the Guinier expansions of form factor amplitudes
            and phase factors as 1 - q^2 sigma<R^2> / 6.

      Here we validate the expressions for the RWpolymer sub-unit which has a mathematically
      well behaved Guinier expansions, but for some of the special functions GiNaC is unable
      to automagically derive the series expansion, this typically occurs when one has to
      take the q->0 limit carefully since the expansion has divergent terms that cancel when
      taking the limit. GiNaC can't take limits, but just inserts q=0 and throws a
      division by zero exception.
      
      A work around for this behaviour is to temporarily replace the special functions by a
      series expansion approximation. This is done with the compile flag -DSPECIALFUNCTIONSERIES
      which allows the expressions for Rods and RWloops to be validated as well.
      
*/

int main()
{
 try{
    SymbolInterface *GLEX = SymbolInterface::instance();

    World w("World");

    // Define symbolic sub-unit s1 and give it two reference points a,b
    SubUnit* s = new GaussianPolymer();
    w.Add(s,"poly");
    
    
    
    // Sub-unit is now initialized and we can call Validate to check if all terms are defined,
    // and if all SMSD terms match their Guinier expansions of the corresponding scattering terms.
    try {
       s->ValidateDefinedTerms();
       s->ValidateSymbolically();
    }
    catch (...)  // Errors thrown by GiNaC
    {
       cout << "!!!!!!!!!!!!!!!!!   Validate failed with some bug(s)  !!!!!!!!!!!!!!!!!!!!!!! \n";
    }
    
    
    cout << endl << endl;
    string dir = "Validation/GaussianPolymer/";
    ParameterList pl;
    w.setParameter(pl, "Rg_poly",1);
    s->ValidateFormFactorFile(pl, dir+"F.dat");

    s->ValidateFormFactorAmplitudeFile("end1",pl, dir+"Aend.dat");
    s->ValidateFormFactorAmplitudeFile("end2",pl, dir+"Aend.dat");
    s->ValidateFormFactorAmplitudeFile("middle",pl, dir+"Amiddle.dat");
    s->ValidateFormFactorAmplitudeFile("contour",pl, dir+"F.dat");

    s->ValidatePhaseFactorFile("end1","end2",pl, dir+"Psi_end2end.dat");
    s->ValidatePhaseFactorFile("end1","middle",pl, dir+"Psi_middle2end.dat");
    s->ValidatePhaseFactorFile("end2","middle",pl, dir+"Psi_middle2end.dat");
    s->ValidatePhaseFactorFile("contour","end1",pl, dir+"Aend.dat");
    s->ValidatePhaseFactorFile("contour","end2",pl, dir+"Aend.dat");
    s->ValidatePhaseFactorFile("contour","middle",pl, dir+"Psi_middle2contour.dat");
    s->ValidatePhaseFactorFile("contour","contour",pl, dir+"F.dat");
}

    

catch (const SEBException e)
{
    std::cout << e;                    // Print what the error was, and where it was triggered.
}

}

