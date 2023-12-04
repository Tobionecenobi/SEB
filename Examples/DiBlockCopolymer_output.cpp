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
    ex Fexpression=w.FormFactor("diblock");
    
    string F       = to_string(Fexpression);
    string Fcform  = to_string_cform(Fexpression);
    string Fpython = to_string_python(Fexpression);
    string Flatex  = to_string_latex(Fexpression);
    
    cout << "Form Factor string\n";
    cout << F << endl << endl;;
    cout << "Form Factor string for C/C++\n";
    cout << Fcform << endl << endl;;
    cout << "Form Factor string for Python\n";
    cout << Fpython << endl << endl;;
    cout << "Form Factor string for LaTeX\n";
    cout << Flatex << endl << endl;;
      
}

    

catch (const SEBException e)
{
    std::cout << e;                    // Print what the error was, and where it was triggered.
}



}
