// Standard C++ headers
#include<iostream>

// Include SEB functionality
#include "SEB.hpp"

/*

    In this example we define three symbolic sub-units and generate a linear structure with them.
    We calculate the analytic expressions for various scattering terms and distances.
*/

int main()
{
 try{
    World w("World");

    // Define symbolic sub-unit s1 and give it two reference points a,b
    SymbolicSubunit* s1 = new SymbolicSubunit();
    s1-> setReferencePointName("a");
    s1-> setReferencePointName("b");

    // Define symbolic sub-unit s2 and give it two reference points c,d
    SymbolicSubunit* s2 = new SymbolicSubunit();
    s2-> setReferencePointName("c");
    s2-> setReferencePointName("d");

    // Define symbolic sub-unit s3 and give it two reference points e,f
    SymbolicSubunit* s3 = new SymbolicSubunit();
    s3-> setReferencePointName("e");
    s3-> setReferencePointName("f");

    // Add it to the world
    GraphID structure = w.Add(s1, "X");
    w.Link(s2,  "Y.c", "X.b");
    w.Link(s3,  "Z.e", "Y.d");
    
    // Name structure triblock
    w.Add(structure, "triblock");
    
    // Obtain expression for form factor for the symbolic triblock
    cout << "Form Factor = " << w.FormFactor("triblock") << endl << endl; 

    // Form factor amplitude
    cout << "Form Factor amplitude(\"triblock:Y.c\")= " << w.FormFactorAmplitude("triblock:Y.c") << endl << endl; 

    // Phase Factor
    cout << "Phasefactor(\"triblock:Y.c\", \"triblock:Z.f\")= " << w.PhaseFactor("triblock:Y.c","triblock:Z.f") << endl << endl; 
    
    // Obtain symbolic expression for the radius of gyration
    cout << "Radius of gyration^2 = " << w.RadiusOfGyration2("triblock") << endl << endl; 

    // Obtain symbolic expression for mean-square distance between Y.c and all scatterers in the structure 
    cout << "sigma <R^2(\"triblock:Y.c\")> = " << w.SMSD_ref2scat("triblock:Y.c") << endl << endl; 

    // Obtain symbolic expression for the mean-square distance between the two reference points
    cout << "sigma <R^2(\"triblock:Y.c\",\"triblock:Z.f\")> = " << w.SMSD_ref2ref("triblock:Y.c","triblock:Z.f") << endl << endl; 
}

    

catch (const SEBException e)
{
    std::cout << e;                    // Print what the error was, and where it was triggered.
}



}
