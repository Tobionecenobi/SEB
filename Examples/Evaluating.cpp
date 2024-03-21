// Standard C++ headers
#include<iostream>

// Include SEB functionality
#include "SEB.hpp"

/*

    In this example we generate a di-block copolymer and illustate how to evaluate the form factor expression to values.
       
                A                    B           
        x----------------x = x-----------------x 
         end1        end2     end1         end2  
    
*/

int main()
{
 try{
    World w("World");

    // Define diblock copolymer
    GraphID diblock = w.Add(new GaussianPolymer(), "A");
    w.Link(new GaussianPolymer(),  "B.end1", "A.end2");
    
    // Name diblock structure
    w.Add(diblock, "diblock");
    
    // Obtain expression for form factor    
    ex F=w.FormFactor("diblock");
    cout << "Form Factor(q; Rg_A, Rg_B, beta_A, beta_B)= " << F << "\n"; 
    
    // To evaluate the expression, we can get the parameters of the last evaluated expression.
    ParameterList params;
    w.setParameter(params,"beta_A",1);
    w.setParameter(params,"beta_B",1);
    w.setParameter(params,"Rg_A",1);
    w.setParameter(params,"Rg_B",2);

    // Use evaluate to evaluate the F expression at a set of parameters and a single q value.
    double q=0.1;
    cout << "Form Factor= " << w.Evaluate( F, params, q)<< "\n";
    
    // Use evaluate to evaluate the expresion for a vector of log distributed q values.
    DoubleVector qvec = w.logspace(0.01, 10.0, 20);
    DoubleVector Intensity = w.Evaluate( F, params, qvec);
        
    for (int i=0; i<qvec.size() ; i++)
       cout << qvec[i] << " " << Intensity[i] << "\n"; 
   
    // Use evaluate to evaluate the expresion and save the result to a file
    DoubleVector qvec2=w.logspace(0.01, 10.0, 1000 );
    w.Evaluate( F, params, qvec2, "formfactor_diblock.q", "Form factor of a diblock copolymer.");
   
}

    

catch (const SEBException e)
{
    std::cout << e;                    // Print what the error was, and where it was triggered.
}



}
