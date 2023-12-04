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
    SubUnit* s = new ThinSphericalShell();
    w.Add(s,"shell");
    
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

}

    

catch (const SEBException e)
{
    std::cout << e;                    // Print what the error was, and where it was triggered.
}

}

