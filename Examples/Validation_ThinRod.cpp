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

      
*/

int main()
{
 try{
    SymbolInterface *GLEX = SymbolInterface::instance();

    World w("World");

    // Define symbolic sub-unit s1 and give it two reference points a,b
    SubUnit* s = new ThinRod();
    w.Add(s,"rod");
    
    // Sub-unit is now initialized and we can call Validate to check if all terms are defined,
    // and if all SMSD terms match their Guinier expansions of the corresponding scattering terms.
    try {
    s->ValidateDefinedTerms();
    }
    catch (...)
    {
       cout << "Validate failed with some bug\n";
    }
    ex x=GLEX->getSymbol("x","loop");
    ex beta=GLEX->getSymbol("beta","loop");

    ParameterList pl;
    w.setParameter(pl, "L_rod",1);
 
    s->ValidateFormFactorFile(pl, "Validation/ThinRod/F.dat");
    
    s->ValidateFormFactorAmplitudeFile("contour",pl, "Validation/ThinRod/F.dat");
    s->ValidateFormFactorAmplitudeFile("end1",pl, "Validation/ThinRod/A_end.dat");
    s->ValidateFormFactorAmplitudeFile("end2",pl, "Validation/ThinRod/A_end.dat");
    s->ValidateFormFactorAmplitudeFile("middle",pl, "Validation/ThinRod/A_middle.dat");

    s->ValidatePhaseFactorFile("end1","middle",pl, "Validation/ThinRod/Psi_end2middle.dat");
    s->ValidatePhaseFactorFile("end2","middle",pl, "Validation/ThinRod/Psi_end2middle.dat");
    s->ValidatePhaseFactorFile("contour","end1",pl, "Validation/ThinRod/A_end.dat");
    s->ValidatePhaseFactorFile("contour","end2",pl, "Validation/ThinRod/A_end.dat");
    s->ValidatePhaseFactorFile("contour","middle",pl, "Validation/ThinRod/Psi_contour2middle.dat");
    s->ValidatePhaseFactorFile("contour","contour",pl, "Validation/ThinRod/F.dat");


}

    

catch (const SEBException e)
{
    std::cout << e;                    // Print what the error was, and where it was triggered.
}

}

