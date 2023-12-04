//===========================================================================
// Included guards
#ifndef INCLUDE_GUARD_STRUCTURE
#define INCLUDE_GUARD_STRUCTURE

//===========================================================================
// included dependencies
#include <list>
#include "Abstract_subunit.hpp"

/*
    A structure is an name assigned to a graph of connected sub-units / structures.
    World keeps tracks of graphs and their links. Thus structure just contains name
    and a graphid of what the name corresponds to.

    Just like the same type of sub-unit can be used multiple times to define
    a graph, then the same type of graph can be used multiple times to define structures.
    
    This creates a natural nesting, where we recursively can build structures of structures
    of ... of sub-units without ever actually replicating the sub-units when they are
    reused.

    After a structure is defined, it can never change its graphID, but you can still
    add sub-units and structure to the graphid automatically changing everything
    that is build with that structure/graphID.
*/


class Structure : public ABSSubUnit{
    
    private:
    int gid;
    structName name;

    public:

    /*Constructor*/
    Structure(structName n, GraphID id, SymbolInterface *GEX)
    { 
        name=n;
        gid=id;
        GLEX = GEX;
        type = STRUCTURE;
    } 
    virtual ~Structure(){}
    
    Structure(){    
        type = STRUCTURE;
    }
    
    /* Gets / Sets the name of a structure*/
    void setName(structName N) { name = N; }
    structName getName() { return name; }

    // Gets the ID of the graph inside the this structure.
    GraphID getGraphID() { return gid; }

    

    /*Returns the formfactor*/
    virtual ex Formfactor(){
        return GLEX -> getSymbol("F", name);
    }

    /*Returns the formfactor amplitude*/
    virtual ex FormfactorAmplitude(refPoint ref){
    
        if (!hasReference(ref)) throw SEBException("Invalid reference:"+ref,"Structure::getFormFactorAmplitude");
    
        return GLEX -> getSymbol("A", ref);
    }

    /*Returns the phase factor*/
    virtual ex PhaseFactor(refPoint ref1, refPoint ref2){

        if (!hasReference(ref1)) throw SEBException("Invalid reference:"+ref1,"Structure::getPhaseFactor");
        if (!hasReference(ref2)) throw SEBException("Invalid reference:"+ref2,"Structure::getPhaseFactor");

        return GLEX -> getSymbol("Psi", ref1, ref2);
    }
    
    /* Is this a valid reference point in this context? 
    
       format: structure1:structure2:structure3:subunit.referencept
       
       structure1 should match the name of the present structure. We need help from world to drill down,
       so structure probably needs a pointer for a call back to world.
    */
    bool hasReference(refPoint& ref)
       {
           // Check the ref point is valid in the structure, such that a sub-unit exists with that reference point somewhere.
           // Throw Exception if falled with a reference point that is invalid.

           //Check if the prefix of the repoint is equal to the structure name
           // I AM NOT SURE WE WANT TO HELP THE USER BY NOT ANCHORING REFERENCE POINTS AT THE CURRENT STRUCTURE LEVEL.
           if(ref.substr(0, name.length()) != name){
               ref = name + "." + ref; 
           }

           return true;
       }

    void Print()
       {
           cout << "Name=" << name << " is gid=" << gid << endl;       
       }

};

#endif // INCLUDE_GUARD_STRUCTURE
