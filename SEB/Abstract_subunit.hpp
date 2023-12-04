//===========================================================================
// Included guards
#ifndef INCLUDE_GUARD_ABSSUBUNIT
#define INCLUDE_GUARD_ABSSUBUNIT

//===========================================================================
// included dependencies
#include "SymbolInterface.hpp"
#include "Types.hpp"
#include "Constants.hpp"

//===========================================================================
// used namespaces
using namespace std;
using namespace GiNaC;

//===========================================================================
// Forward declarations
class World;

/* ===========================================================================

   This is the base class for both structures and sub-units, since they have the same API in
   terms of providing expressions for scattering contributions.
   
   Hence everything that is common the both structure and sub-unit is defined
   here for instance relevant methods.

*/

class ABSSubUnit{ 

    protected:  

    /* What is my type */
    int type;
    
    /*Pointer to symbol interface*/
    SymbolInterface *GLEX;

    public:
    /*Constructor*/    
    ABSSubUnit( ){    
        type = ABSSUBUNIT;
    } 
    virtual ~ABSSubUnit(){}

    /*Returns the type of the absoloute sub unit. Here the type is either a sub-unit or a structure*/
    int getType() { return type; }

// REPLACE THESE BY ABSTRACT COMMON METHODS SHARED BETWEEN SUB_UNIT AND STRUCTURE

   /*Returns a list of reference point contained in the absolute sub unit*/
    ReferencePointSet* getReferencePoints() { return nullptr; }

    /*Returns the formfactor*/
    virtual ex Formfactor()  {return 1;}

    /*Returns the formfactor amplitude*/
    virtual ex FormfactorAmplitude(refPoint ref) {return 1;}

    /*Returns the formfactor amplitude*/
    virtual ex PhaseFactor(refPoint ref1, refPoint ref2)  {return 1;}

};

#endif // INCLUDE_GUARD_ABSSUBUNIT
