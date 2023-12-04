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
    SubUnit* s = new GaussianLoop();
    w.Add(s,"loop");
    
    // Sub-unit is now initialized and we can call Validate to check if all terms are defined,
    // and if all SMSD terms match their Guinier expansions of the corresponding scattering terms.
    try {
       s->ValidateDefinedTerms();
    }
    catch (...)  // Errors thrown by GiNaC
    {
       cout << "!!!!!!!!!!!!!!!!!   Validate failed with some bug(s)  !!!!!!!!!!!!!!!!!!!!!!! \n";
    }
    
    
    cout << endl << endl;
    
    string dir="Validation/GaussianLoop/";
    ParameterList pl;
    w.setParameter(pl, "Rg_loop",1);
    s->ValidateFormFactorFile(pl, dir+"F.dat");
    s->ValidateFormFactorAmplitudeFile("contour",pl, dir+"F.dat");
    s->ValidatePhaseFactorFile("contour","contour",pl, dir+"F.dat");
    
    ex x=GLEX->getSymbol("x","loop");
    ex beta=GLEX->getSymbol("beta","loop");

    cout << "F(x=0.1)= " << s->FormFactor(XVAR).subs(beta==1).subs(x==0.1).evalf() << endl;
    cout << "F(x=1)= " << s->FormFactor(XVAR).subs(beta==1).subs(x==1).evalf()  << endl;
    cout << "F(x=10)= " << s->FormFactor(XVAR).subs(beta==1).subs(x==10).evalf()  << endl;
}

    

catch (const SEBException e)
{
    std::cout << e;                    // Print what the error was, and where it was triggered.
}

}

