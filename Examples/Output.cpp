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


    cout << "\nForm Factor (default ginac output) --------------------------------------\n";
    cout <<  dflt << w.FormFactor("diblock") << endl;
    
    cout << "\nForm Factor (latex formatted) -------------------------------------------\n";
    cout <<  latex << w.FormFactor("diblock") << endl;

    cout << "\nForm Factor (c formatted) -----------------------------------------------\n";
    cout <<  csrc << w.FormFactor("diblock") << endl;

    cout << "\nForm Factor (Python) ----------------------------------------------------\n";
    cout <<  python << w.FormFactor("diblock") << endl;

}
catch (const SEBException e)
{
    std::cout << e;                    // Print what the error was, and where it was triggered.
}



}
