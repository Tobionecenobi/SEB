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

    // Define Triblock copolymer
    GraphID triblock = w.Add(new GaussianPolymer(), "A");
    w.Link(new GaussianPolymer(),  "B.end1", "A.end2");
    w.Link(new GaussianPolymer(),  "C.end1", "B.end2");
    
    // Name triblock structure
    w.Add(triblock, "triblock");

    cout << "\nNormalized symbolic Form Factor : -----------------------------------------------------\n";
    cout <<  w.FormFactorGeneric("triblock") << endl;

/*
Outputs after reformatting: 

F_A*beta_A^2+beta_B^2*F_B+F_C*beta_C^2+2*beta_A*beta_B*A_A:end2*A_B:end1+2*beta_A*beta_C*A_A:end2*Psi_B:end1,end2*A_C:end1+2*beta_B*beta_C*A_B:end2*A_C:end1
-------------------------------------------------------------------------------------------------------------------------------------------------------------
                                                      (beta_A+beta_B+beta_C)^2

*/

    cout << "\nUnormalized symbolic Form Factor : -----------------------------------------------------\n";
    cout <<  w.FormFactorGeneric_Unnormalized("triblock") << endl;

/*
Outputs the numerator from the expression above: 

F_A*beta_A^2+beta_B^2*F_B+F_C*beta_C^2+2*beta_A*beta_B*A_A:end2*A_B:end1+2*beta_A*beta_C*A_A:end2*Psi_B:end1,end2*A_C:end1+2*beta_B*beta_C*A_B:end2*A_C:end1

*/

    cout << "\nUnormalized reduced Form Factor : -----------------------------------------------------\n";
    cout <<  w.FormFactorX_Unnormalized("triblock") << endl;

/*
Outputs the numerator, but using the polymer specific expressions for Psi, A, F using x= Rg^2 q^2 for the three blocks:  

2*x_C^(-1)*beta_A*exp(-x_B)*(-1+exp(-x_C))*(-1+exp(-x_A))*x_A^(-1)*beta_C+2*x_B^(-2)*(-1+x_B+exp(-x_B))*beta_B^2+2*x_C^(-2)*(-1+x_C+exp(-x_C))*beta_C^2+2*x_C^(-1)*x_B^(-1)*(-1+exp(-x_C))*beta_B*beta_C*(-1+exp(-x_B))+2*beta_A*x_B^(-1)*(-1+exp(-x_A))*beta_B*x_A^(-1)*(-1+exp(-x_B))+2*(-1+exp(-x_A)+x_A)*beta_A^2*x_A^(-2)

*/

    cout << "\nUnormalized Form Factor : -----------------------------------------------------\n";
    cout <<  w.FormFactor_Unnormalized("triblock") << endl;

/*
Outputs the numerator, but using the polymer specific expressions for Psi, A, F expressing everything in terms of q, and Rg^2_A, Rg^2_B, Rg^2_C

2*q^(-4)*beta_A^2*Rg_A^(-4)*(-1+exp(-q^2*Rg_A^2)+q^2*Rg_A^2)+2*q^(-4)*(-1+exp(-q^2*Rg_C^2)+q^2*Rg_C^2)*Rg_C^(-4)*beta_C^2+2*Rg_B^(-4)*q^(-4)*(-1+Rg_B^2*q^2+exp(-Rg_B^2*q^2))*beta_B^2+2*(-1+exp(-q^2*Rg_A^2))*Rg_B^(-2)*q^(-4)*beta_A*Rg_A^(-2)*(-1+exp(-Rg_B^2*q^2))*beta_B+2*(-1+exp(-q^2*Rg_A^2))*(-1+exp(-q^2*Rg_C^2))*q^(-4)*beta_A*exp(-Rg_B^2*q^2)*Rg_A^(-2)*Rg_C^(-2)*beta_C+2*(-1+exp(-q^2*Rg_C^2))*Rg_B^(-2)*q^(-4)*(-1+exp(-Rg_B^2*q^2))*beta_B*Rg_C^(-2)*beta_C

*/

    cout << "\nNormalization constant for Form Factor : ------------------------------------------\n";
    cout <<  w.FormFactor_Normalization("triblock") << endl;

/*
Outputs after reformatting:    (beta_A+beta_B+beta_C)^2
*/


    cout << "\nApparent Radius of gyration2  -----------------------------------------------------\n";
    cout <<  w.RadiusOfGyration2("triblock") << endl;


/*
Outputs after reformatting:

6*beta_C*Rg_B^2*beta_A+3*beta_B*Rg_C^2*beta_C+3*beta_B*beta_A*Rg_A^2+Rg_C^2*beta_C^2+beta_B^2*Rg_B^2+beta_A^2*Rg_A^2+3*beta_B*Rg_B^2*beta_A+3*Rg_C^2*beta_C*beta_A+3*beta_C*beta_A*Rg_A^2+3*beta_B*beta_C*Rg_B^2
------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
                                                      (beta_A+beta_B+beta_C)^2
*/


    cout << "\nNumber of sub-units pairs -----------------------------------------------------\n";
    cout <<  w.CountPairs("triblock") << endl;

/*
Outputs after reformatting:   9   which is N^2 since there are 3 blocks in this structure.
*/



}
catch (const SEBException e)
{
    std::cout << e;                    // Print what the error was, and where it was triggered.
}



}
