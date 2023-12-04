//===========================================================================
// Included guards
#ifndef INCLUDE_GUARD_SUBUNIT
#define INCLUDE_GUARD_SUBUNIT

//===========================================================================
// included dependencies

#include <cmath>
#include <fstream>

#include "Abstract_subunit.hpp"
#include "Subunits/Constants.hpp"

//===========================================================================
// The actutual class

class SubUnit : public ABSSubUnit
{
    protected:
    
    /* Every subunit has a unique name, so we can e.g. uniquely refer to its reference points when linking.  */
    subName sub_name;
    
    /* Groups of sub-units might represent structurally idential sub-units, we tag them with the same string to mark
    them as being identical. The tags are also used sub-scripts in scattering expressions. If no tag is specified
    the sub_unit name is/ used automatically making all sub-units in a structure unique. */
    string tag;
    
    /* Sub-unit type, this specifies the specific type of sub-unit. This is different from type from the base class,
       which is used to distingish between sub-units and structures. */
    int stype=-1;

    /*
       Reference points comes in 3 varieties.
         1) Specific geometric reference points, for instance polymer has end1,end2 sphere has a centre.
         We can directly link to other sub-units to these points.
         
         2) Distributed reference points, these are distributions of points we have to average over, for
         instance polymer has contour, sphere has surface. These are not used directly, since we can not link
         a sub-unit to contour or surface.
         
         3) Specific randomly chosen reference points. To link a sub-unit to contour we must first pick
         a random point on the contour, and link the sub-unit to this now specific, but random point
         on the contour. This is a specific reference point named e.g. polymer.contour#mypoint or sphere.surface#mypoint.

       Defining disorder.  We could choose to link say the centre of two spheres to polymer.contour#mypoint, in
       that case, they  are all linked to the same random point on the polymer, and the two spheres would always
       be on top of each other because they are on the SAME random point.
       
       We could also choose to link the two spheres to polymer.contour#mypoint1 and polymer.contour#mypoint2,
       in which case they would be on two different random points along the polymer, and hence rarely be in contact.

       1+3 defines the connectivity of a structure that we need to traverse.
       2 is defined by sub-units, and we always turn them into 3 before using these.s
    */
    
    ReferencePointSet refsSpecific;        // These are specific geometric (1) + randomly chosen distributed (3)      e.g.  end1, end2, contour#mypkt1, contour#mypkt2
    ReferencePointSet refsDistributed;     // These are the names of distributed unspecified reference points (2)     e.g. polymer contour, or sphere surface.

    /* Structural parameter maps.
    
      A sub-unit for instance a polymer is naturally represented by the structural parameter Rg2. Hence the scattering
      expressions are most naturally expressed in terms of the dimensionless parameter x = q^2 Rg^2. Other objects such
      as spheres, rods, cyllinders are characterized by dimensionless structural parameters x = qL, y=qR ...
      
      Thus for a polymer expand[x] = q^2 Rg^2 which allows for expanding reduced scattering expressions (XVAR, LHS)
      into expressions with explicit structural parameters (QVAR, RHS).                    
       */

    exmap expand;   
    
    // Set of local structural parameter expressions for this sub-unit, e.g. Rg for polymer, R for sphere, L for rod and so on.
    set<ex> parameters;
    set<ex> xparameters;  



    /* A sub-unit stores a lot of scattering expressions, expressed consisely in dimensionless variables (XVAR) */

    ex FormFactorExpression;                                      // expression for the form factor
    map<refPoint,ex> FormFactorAmplitudeExpressions;              // each reference point has an associated form factor amplitude expression
    map<refPoint ,map<refPoint, ex> > PhaseFactorExpressions;     // each pair of reference points has an associated phase factor expression equation

    /* We also need to store mean-square-distances, these are obtained by Guinier expanding the Debye formula, that is the
       various scattering expressions. We note SEB requires   sigma <R^2>  to be defined, which are trivially defined by Guinier expansions. 
       
       Below sigma refers to a symmetry number which are either 1 or 2.
       
       F             = 1 - q^2 sigma_F         <Rg^2> /6 + ..     
       A_ref         = 1 - q^2 sigma_ref       <R^2_ref_scatter>/6 + ..     
       PSI_ref1_ref2 = 1 - q^2 sigma_ref1_ref2 <R^2_ref_ref>/6 + ..     
       
       sigma_F is by definition 2, because the form factor is <sin(qR_ij)/qR_ij> where <..> is average over conformations and i,j denotes scatterers.
       Thus both ij and ji contributes, and we DEFINE Rg^2 as <R_ij^2> for the set of unique ij
       
       
       
       
    */

    ex RadiusOfGyration2;                                    // expression for Rg^2
    map<refPoint, ex> sigmaMSDref2scat;                      // sigma_ref <R^2_ref,scat>       mean-square distance between specified reference point and all scatterers.
    map<refPoint, map<refPoint, ex>> sigmaMSDref2ref;        // sigma_ref,ref <R^2_ref,ref>    mean-square distance between specified reference points

    public:
    
    /*Constructor generating an SubUnit. The user constructs no-name sub-unit pointers, but it is world that 
      is told what the sub-units should be named, and how they should be linked to existing sub-units,
      hence the constructors are very thin, and the real initialization is delayed until world can call Init()
      with the proper information. */

    SubUnit() : ABSSubUnit(){
        type = SUBUNIT;
       stype = UNKNOWN;
    }; 
    virtual ~SubUnit(){}

    /* Initialize sub unit by adding scattering properties to it*/
    virtual void Init(string name, string tag, SymbolInterface *GEX)
      {
           GLEX=GEX;
           setName(name);
           if(!tag.empty()) setTag(tag);
                     else setTag(name);

           // Initialize scattering expressions.      
      }

    /* Gets/Sets the name of a sub unit */
    virtual subName getName() { return sub_name; }
    virtual void setName(subName name) { sub_name = name; }

    /* Gets/Sets the tag of a sub-unit */
    string getTag() { return tag; }
    virtual void setTag(string group) { tag = group; }

    // returns the type of a derived sub-unit instance.
    int getSubunitType() { return stype; }

    /*Returns the scattering length of the sub unit as the greek letter beta with the sub units tag as an index*/
    ex getBeta(){
        return GLEX->getSymbol("beta", tag);
    }

    /*returns the form factor */
    virtual ex FormFactor(ParameterList&  betas, ParameterList&  params, int varForm = GENERIC );
    virtual ex FormFactor( int varForm = QVAR );
    
    /*returns the form factor amplitude relative to r */
    virtual ex FormFactorAmplitude(refPoint r, ParameterList&  betas, ParameterList&  params, int varForm = GENERIC );
    virtual ex FormFactorAmplitude(refPoint r,int varForm = QVAR );

    // Return the phase factor between r1 and r2
    virtual ex PhaseFactor(refPoint r1, refPoint r2, ParameterList&  betas, ParameterList&  params, int varForm = GENERIC );
    virtual ex PhaseFactor(refPoint r1, refPoint r2, int varForm = QVAR );


    /* Returns the radius of gyration of a sub unit. This always uses explicit structural parameters. */
    ex virtual getRadiusOfGyration2() { return RadiusOfGyration2; }

    // Returns sigma <R^2> between the reference point and all scatterers in th
    ex  virtual getSigmaMSDRef2Scat(refPoint r);

    // Returns sigma <R^2> between the two reference points.
    ex  virtual getSigmaMSDRef2Ref(refPoint r1, refPoint r2 );



    /* Tests if this sub-unit has a distributed reference point e.g. contour for polymer*/
    bool hasDistributedReference(const refPoint& r)
       {
          return refsDistributed.find(r) != refsDistributed.end();
       }

    /* Tests if this sub-unit has a specific or random reference point. e.g. end1 or contour#mypkt for polymer */
    bool hasSpecificReference(const refPoint& r)
       {
          return refsSpecific.find(r) != refsSpecific.end();
       }

    /* Is this reference point known by this sub-unit */
    bool hasReference(const refPoint r)
       {
          return hasSpecificReference(r) || hasDistributedReference(r);
       }

    /* adds a specific reference point.  For polymer could be end1, end2, middle or contour#mypoint  */
    void addSpecificReferencePoint(const refPoint ref ){
        if (hasSpecificReference(ref)) throw SEBException("reference point "+ref+" already exists!", "SubUnit::addSpecificReferencePoint");

        refsSpecific.insert(ref);
    }

    /* adds a distributed reference point.   Could be contour for polymer  or surface for sphere */
    void addDistributedReferencePoint(const refPoint ref ){
        if (hasDistributedReference(ref)) throw SEBException("reference point "+ref+" already exists!", "SubUnit::addDistributedReferencePoint");
        if (hasAHash(ref))                throw SEBException("distributed reference point "+ref+" contains #!", "SubUnit::addDistributedReferencePoint");

        refsDistributed.insert(ref);
    }

    /* adds a random point (random) from a distributed reference point (refbase). If it already exists, do nothing.
       That is given contour#mypoint checks contour is distributed, and adds whole string to specific reference points.
    */
    void addRandomDistributedReferencePoint(refPoint refbase, refPoint random)
    {
        if(!hasDistributedReference(refbase))
            throw SEBException("Type of distributed reference:"+refbase+" does not exist for sub-unit "+sub_name, "World::addRandomDistributedReferencePOint(\""+refbase+"\",\""+random+"\")");
    
        refPoint newref =refbase+"#"+random;
        if (!hasSpecificReference(newref))
                 addSpecificReferencePoint(newref);   // Add contour#mypoint or surface¤mypt as a specific reference point if not already existing.
    }    

    /* Returns a list of all the SPECIFIC reference points on a sub unit, this is need for traversing a structure. */
    // DO WE WANT OTHER CLASSES TO ACCESS THIS, and is this just just the specific points + randomly chosen points.
    // SHould be replaced by returning a begin and end const iterator, so other parts of the code can iterate along the data, without changing them.
    ReferencePointSet* getReferencePoints(){
        return &refsSpecific;
    }

    // Prefix is structure1:...:mysubunitname.
    void addSubunitReferences( string prefix, ReferencePointList& ret )
    {
        for (auto ref : refsSpecific)
              ret.push_back(prefix+"."+ref);
    }


    // Const iterators over specific reference points.
    ReferencePointSet::const_iterator SpecificReferencepoints_cbegin()    { return refsSpecific.cbegin(); }
    ReferencePointSet::const_iterator SpecificReferencepoints_cend()      { return refsSpecific.cend(); }

    // Const iterators over distributed reference points.
    ReferencePointSet::const_iterator DistributedReferencepoints_cbegin() { return refsDistributed.cbegin(); }
    ReferencePointSet::const_iterator DistributedReferencepoints_cend()   { return refsDistributed.cend(); }

    // Tests if all the required terms and cross-terms are defined.
    void ValidateDefinedTerms( );

    // Guiner expands all scattering terms and compares to the Rg2 or sigma<R^2> terms.
    void ValidateSymbolically (ParameterList pl=ParameterList() );

    // Saves a graph of all scattering expressions for the given parameters and qvec.
    // The graphs can be compared to output from sampling or e.g. mathematica derived expressions.
    void ValidateGraphically(ParameterList pl, vector<double> qvec, string base );
    
     
     
    bool ValidateFormFactorFile(ParameterList &pl, string filename, double tolerance=1e-4)
     {
         return ValidateExpressionFile( FormFactorExpression.subs(expand).subs(pl), RadiusOfGyration2.subs(pl), filename, true, "FormFactor" , tolerance);
     }


    bool ValidateFormFactorAmplitudeFile(refPoint r, ParameterList &pl, string filename, double tolerance=1e-4)
     {
         if (!hasAmplitudeRef(r))
              throw SEBException("Refpoint "+r+" not valid for sub-unit form factor","ValidateFormFactorAmplitudeFile(refPoint r, ParameterList &pl, string filename, double tolerance=1e-4)");
         return ValidateExpressionFile( FormFactorAmplitudeExpressions[r].subs(expand).subs(pl), sigmaMSDref2scat[r].subs(pl), filename, false, "FormFactorAmplitude["+r+"]", tolerance);
     }

    bool ValidatePhaseFactorFile(refPoint r1, refPoint r2,ParameterList &pl, string filename, double tolerance=1e-4)
     {
         if (!hasPhaseFactorRefs(r1,r2))
              throw SEBException("Refpoints "+r1+" and "+r2+" not valid for sub-unit phase factor","bool ValidatePhaseFactorFile(refPoint r1, refPoint r2,ParameterList &pl, string filename, double tolerance=1e-4)");
         return ValidateExpressionFile( PhaseFactorExpressions[r1][r2].subs(expand).subs(pl), sigmaMSDref2ref[r1][r2].subs(pl), filename, false, "PhaseFactor["+r1+"]["+r2+"]", tolerance);
     }


    /* Sets the reference point name */
    void setReferencePointName( refPoint ref ){   // WHAT DOES THIS DO??
        // Test ref exists in sub-unit
        if(hasAHash(ref)){
            setRandRefPoint(ref.substr(0, ref.find("#")), ref.substr(ref.find("#")+1, ref.size()));
        }
        refsSpecific.insert(ref);
    }

    /*Check if the subunit has a reference point with the name ref. If so it increments the number of that
    reference point. If not it creates a new reference point with the name ref and sets the number to 1.*/
    void setDistReferencePointType( distRefPoint ref ){
       refsDistributed.insert(ref);
    }

    /*Adds a reference point which is sat randomly on a distributed reference point that has a name. If the distributed reference point is not defined see 'setDistReferencePointType( distRefPoint ref )' function.*/
    void setRandRefPoint(refPoint ref, string name)
    {
        if(refsDistributed.find(ref) != refsDistributed.end() && ref.find("#") == string::npos)
        {
            refsSpecific.insert(ref + "#" + name);
        }
        else throw SEBException("Distributed reference point type not found", "setRandRefPoint()");
    }




   protected:   // Helper functions used by class methods

    // Test if scattering functions have sensible Guinier expansions, and compare these to the hard coded Rg2 and sigma<R^2> expressions.
    bool ValidateFunctionSymbolically(ex& F, ex& SMSD, bool FormFactor, ex Fsymb, ParameterList pl);


    // Helper
    bool ValidateExpressionFile( ex term, ex SRMS2ex, string filename, bool FF, string str, double tolerance);

    // Helper that generates a graph of a given expression, and subtract the Guinier expansion within the range where the expansion should be valid.
    void ValidateGraph(ex Fex, ex sigmaR2ex, bool FormFactor, vector<double>& qvec, string filename);


    /* tests that a user specificed reference point is valid for this sub-unit. */
    bool testReference(const refPoint& r)
      {
          if (!hasSpecificReference(r) && !hasDistributedReference(r)) 
                     throw SEBException("Referencepoint "+r+" not known by subunit"+sub_name, "SubUnit::testReference()");

          return true;
      }

    /* Returns the characters before '#' in a distributed reference point e.g. contour if called with contour#mypoint*/
    refPoint removeHashString(const refPoint ref){

        auto hashpos = ref.find("#");
        if (hashpos == string::npos) throw SEBException("refpoint missing #string", "SubUnit::removeHashString");

        return ref.substr(0, hashpos);
    }

    /* Checks if a reference point string contains a hash e.g. contour#mypoint */
    bool hasAHash(const refPoint ref){
        return ref.find("#") != string::npos;
    }

 
    // Tests that form factor amplitude terms exists for refpoint r  
    bool hasAmplitudeRef(refPoint r)
      {
          return FormFactorAmplitudeExpressions.find(r) != FormFactorAmplitudeExpressions.end()
              && sigmaMSDref2scat.find(r) != sigmaMSDref2scat.end();
      }
      
    // Tests that phase factor terms exists for refpoint r  
   bool hasPhaseFactorRefs(refPoint r1, refPoint r2)
      {
         return PhaseFactorExpressions.find(r1) != PhaseFactorExpressions.end()
            &&  PhaseFactorExpressions[r1].find(r2)!=PhaseFactorExpressions[r1].end()
            &&  sigmaMSDref2ref.find(r1)!=sigmaMSDref2ref.end()
            &&  sigmaMSDref2ref[r1].find(r2)!=sigmaMSDref2ref[r1].end();
      }


};

#endif // INCLUDE_GUARD_SUBUNIT
