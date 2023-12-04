// Included guards
#ifndef INCLUDE_GUARD_CONSTANTS
#define INCLUDE_GUARD_CONSTANTS


const int WORLDMAXDEPTH=2147483647; //Largest int

/*
   vartypes determines output:
           GENERIC : Scattering expressions expressed using F, A, PSI symbols for structures and sub-units.           
           XVAR    : Scattering expressions where F,A,Psi are expressed with equations involved dimensionless variables
           QVAR    : Scattering expressions where F,A,Psi are expressed with explicit structural parameters and q.
           BETA    : Collect only beta terms, assume q=0 and F=A=Psi=1
           GUINIER : Everything has been Guinier expanded. Hence return   F= 1 - q^2 <Rg^2>/3 + .. and similar for A and P.  Terms are weighted by beta.          
           ONE     : return 1 for all terms, correspond to q->0 and beta_X = 1.0 for all X.
                     Hence for an unnormalized form factor it should produce N^2 where N is the number of sub-units in the structure (or structures depending on level).
                           for an unnormalized form factor amplitude it should produce N.
                           for a phase factor it will just be 1.0

*/

enum vartypes{GENERIC=1, XVAR = 2, QVAR = 3, BETA=4, GUINIER=5, GUINIERA=6, ONE=7 };

/* Type defines:
         ABSSUBUNIT   : Abstract sub-unit (should never be instantiated),
         STRUCTURE    : A structure (graph of sub-units)
         SUBUNIT      : The parent class of all sub-units. Only instantiated if the user manually creates their own sub-units from an empty class.
         SUBUNITCHILD : RWpolymer, sphere, rod, and other sub-units.
*/
enum types{ABSSUBUNIT, STRUCTURE, SUBUNIT, SUBUNITCHILD};
    
#endif    


