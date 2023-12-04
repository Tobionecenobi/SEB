// Standard C++ headers
#include<iostream>

// Include SEB functionality
#include "SEB.hpp"

int main()
{
 try{
    SymbolInterface *GLEX = SymbolInterface::instance();

    World w("World");

    // Define symbolic sub-unit s1 and give it two reference points a,b
    SubUnit* s = new ThinDisk();
    w.Add(s,"disk");
    
    // Sub-unit is now initialized and we can call Validate to check if all terms are defined,
    // and if all SMSD terms match their Guinier expansions of the corresponding scattering terms.
    try {
       s->ValidateDefinedTerms();
//       s->ValidateSymbolically();
    }
    catch (...)  // Errors thrown by GiNaC
    {
       cout << "!!!!!!!!!!!!!!!!!   Validate failed with some bug(s)  !!!!!!!!!!!!!!!!!!!!!!! \n";
    }
    
    
   ParameterList pl;
   w.setParameter(pl, "R_disk",1);
 
   s->ValidateFormFactorFile(pl,                      "Validation/ThinDisk_R1/FF.dat");
   s->ValidateFormFactorAmplitudeFile("surface",pl,   "Validation/ThinDisk_R1/FF.dat");
   s->ValidatePhaseFactorFile("surface","surface",pl, "Validation/ThinDisk_R1/FF.dat");
   
   s->ValidateFormFactorAmplitudeFile("center",pl, "Validation/ThinDisk_R1/FFA_center.dat");
   s->ValidateFormFactorAmplitudeFile("rim",pl, "Validation/ThinDisk_R1/FFA_rim.dat");
   s->ValidatePhaseFactorFile("rim","surface",pl, "Validation/ThinDisk_R1/FFA_rim.dat");

   s->ValidatePhaseFactorFile("center","rim",pl, "Validation/ThinDisk_R1/Psi_center2rim.dat");
   s->ValidatePhaseFactorFile("rim","rim",pl, "Validation/ThinDisk_R1/Psi_rim2rim.dat");
}

    

catch (const SEBException e)
{
    std::cout << e;                    // Print what the error was, and where it was triggered.
}

}

