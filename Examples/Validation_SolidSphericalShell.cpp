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
    SubUnit* s = new SolidSphericalShell();
    w.Add(s,"shell");

    // Sub-unit is now initialized and we can call Validate to check if all terms are defined,
    // and if all SMSD terms match their Guinier expansions of the corresponding scattering terms.
    try {
       s->ValidateDefinedTerms();
    }
    catch (...)  // Errors thrown by GiNaC
    {
       cout << "!!!!!!!!!!!!!!!!!   Validate failed with some bug(s)  !!!!!!!!!!!!!!!!!!!!!!! \n";
    }

    double Ri_=2.33;
    double Ro_=3.44;

    ex xi=GLEX->getSymbol("xi","shell");
    ex xo=GLEX->getSymbol("xo","shell");
    ex Ri=GLEX->getSymbol("Ri","shell");
    ex Ro=GLEX->getSymbol("Ro","shell");
    ex beta=GLEX->getSymbol("beta","shell");

    ParameterList map;
    map[Ri]=Ri_;
    map[Ro]=Ro_;

    string dir="Validation/SolidSphericalShell_Ri2.33_Ro3.44/";
    s->ValidateFormFactorFile(map, dir+"FF.dat");

    s->ValidateFormFactorAmplitudeFile("center",map,   dir+"FFA_center.dat");
    s->ValidateFormFactorAmplitudeFile("surfacei",map, dir+"FFA_inner.dat");
    s->ValidateFormFactorAmplitudeFile("surfaceo",map, dir+"FFA_outer.dat");
    s->ValidateFormFactorAmplitudeFile("surface",map,  dir+"FFA_surface.dat");

    s->ValidatePhaseFactorFile("center","surface",map, dir+"PF_center_surface.dat");
    s->ValidatePhaseFactorFile("surfacei","surfacei",map, dir+"PF_inner_inner.dat");
    s->ValidatePhaseFactorFile("surfacei","surfaceo",map, dir+"PF_inner_outer.dat");
    s->ValidatePhaseFactorFile("surfaceo","surfaceo",map, dir+"PF_outer_outer.dat");

    s->ValidatePhaseFactorFile("surface","surfacei",map, dir+"PF_inner_surface.dat");
    s->ValidatePhaseFactorFile("surface","surfaceo",map, dir+"PF_outer_surface.dat");
    s->ValidatePhaseFactorFile("surface","surface",map, dir+"PF_surface_surface.dat");

}

    

catch (const SEBException e)
{
    std::cout << e;                    // Print what the error was, and where it was triggered.
}

}

