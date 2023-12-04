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
    SubUnit* s = new SolidCylinder();
    w.Add(s,"c");

    // Sub-unit is now initialized and we can call Validate to check if all terms are defined,
    // and if all SMSD terms match their Guinier expansions of the corresponding scattering terms.
    try {
       s->ValidateDefinedTerms();
    }
    catch (...)  // Errors thrown by GiNaC
    {
       cout << "!!!!!!!!!!!!!!!!!   Validate failed with some bug(s)  !!!!!!!!!!!!!!!!!!!!!!! \n";
    }
    
    ex R=GLEX->getSymbol("R","c");
    ex L=GLEX->getSymbol("L","c");
    ex beta=GLEX->getSymbol("beta","c");

    ParameterList map;
    map[R]=1;
    map[L]=1.5;

    string dir = "Validation/SolidCylinder_R1_L1.5/";
   
    s->ValidateFormFactorFile(map, dir+"F.dat");
    
    s->ValidateFormFactorAmplitudeFile("center",map, dir+"FFA_center.dat");
    s->ValidateFormFactorAmplitudeFile("ends",map, dir+"FFA_ends.dat");
    s->ValidateFormFactorAmplitudeFile("hull",map, dir+"FFA_hull.dat");    
    s->ValidateFormFactorAmplitudeFile("surface",map, dir+"FFA_surface.dat");

    s->ValidatePhaseFactorFile("center","ends",map, dir+"PF_center2ends.dat");
    s->ValidatePhaseFactorFile("center","hull",map, dir+"PF_center2hull.dat");
    s->ValidatePhaseFactorFile("center","surface",map, dir+"PF_center2surface.dat");

    s->ValidatePhaseFactorFile("ends","ends",map, dir+"PF_end2end.dat");
    s->ValidatePhaseFactorFile("ends","hull",map, dir+"PF_end2hull.dat");
    s->ValidatePhaseFactorFile("ends","surface",map, dir+"PF_end2surface.dat");


    s->ValidatePhaseFactorFile("hull","hull",map, dir+"PF_hull2hull.dat");
    s->ValidatePhaseFactorFile("hull","surface",map, dir+"PF_hull2surface.dat");

    s->ValidatePhaseFactorFile("surface","surface",map, dir+"PF_surface2surface.dat");

    map[R]=2;
    map[L]=0.5;

    dir = "Validation/SolidCylinder_R2_L0.5/";
   
    s->ValidateFormFactorFile(map, dir+"F.dat");
    
    s->ValidateFormFactorAmplitudeFile("center",map, dir+"FFA_center.dat");
    s->ValidateFormFactorAmplitudeFile("ends",map, dir+"FFA_ends.dat");
    s->ValidateFormFactorAmplitudeFile("hull",map, dir+"FFA_hull.dat");    
    s->ValidateFormFactorAmplitudeFile("surface",map, dir+"FFA_surface.dat");

    s->ValidatePhaseFactorFile("center","ends",map, dir+"PF_center2ends.dat");
    s->ValidatePhaseFactorFile("center","hull",map, dir+"PF_center2hull.dat");
    s->ValidatePhaseFactorFile("center","surface",map, dir+"PF_center2surface.dat");

    s->ValidatePhaseFactorFile("ends","ends",map, dir+"PF_end2end.dat");
    s->ValidatePhaseFactorFile("ends","hull",map, dir+"PF_end2hull.dat");
    s->ValidatePhaseFactorFile("ends","surface",map, dir+"PF_end2surface.dat");


    s->ValidatePhaseFactorFile("hull","hull",map, dir+"PF_hull2hull.dat");
    s->ValidatePhaseFactorFile("hull","surface",map, dir+"PF_hull2surface.dat");

    s->ValidatePhaseFactorFile("surface","surface",map, dir+"PF_surface2surface.dat");

}

    

catch (const SEBException e)
{
    std::cout << e;                    // Print what the error was, and where it was triggered.
}

}

