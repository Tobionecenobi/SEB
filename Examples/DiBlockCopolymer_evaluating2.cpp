// Standard C++ headers
#include<iostream>

// Include SEB functionality
#include "SEB.hpp"

/*

    In this example we generate a tri-block copolymer and derive various expressions
       
                A                    B                     C
        x----------------x = x-----------------x = x-----------------x 
         end1        end2     end1         end2     end1         end2
    
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
    ParameterList params=w.getParams();

    cout << "Parameters: (default value is zero)\n";
    for (auto& p: params) 
       {
          cout << "\t" << p.first << "=" << p.second << "\n";
       }
    cout << endl;

    // Use SetParameter to define values
    w.setParameter(params,"beta_A",1);
    w.setParameter(params,"beta_B",1);
    w.setParameter(params,"Rg_A",1);
    w.setParameter(params,"Rg_B",2);

    cout << "Parameters:\n";
    for (auto& p: params) 
       {
          cout << "\t" << p.first << "=" << p.second << "\n";
       }
    cout << endl;
    
    // Below we use .subs to substite expressions for values:

    // Insert parameter values into the expression for F, producing an expression that only depends on q.
    ex Fq=F.subs(params);
    cout << "Form Factor(q)= " << Fq << "\n";

    // w.q( value ) returns a Parameterlist where the symbol q is set to the desired value.
    ex Fvalue=Fq.subs( w.q(0.1) );
    cout << "Form Factor= " << Fvalue << "\n";
}

    

catch (const SEBException e)
{
    std::cout << e;                    // Print what the error was, and where it was triggered.
}



}
