//===========================================================================
// Included guards
#ifndef INCLUDE_GUARD_POINT
#define INCLUDE_GUARD_POINT

/*===========================================================================

   This file implements the scattering from a single invisible point, it can be
   useful e.g. as the centre of a star polymer, but one could equally well build
   structures without it.

   Invisible because we assume beta_point=0, hence it contributes nothing to
   form factors, form factor amplitudes, and has a fase factor of 1.
   For this reason we don't include points in Count and CountPairs.

============================================================================= */


class Point : public SubUnit{

    public:
        Point() : SubUnit(){    
            type = SUBUNITCHILD;
           stype = POINT;
            setName("Point");
        }
        
    virtual ~Point(){}

    virtual void Init(string name, string tag, SymbolInterface *GEX)
    {
        // Initialize base class
        SubUnit::Init(name, tag, GEX);
        string n = getTag();
        
        // ========================================================================================
        // Setup reference points

        // specific reference points for a polymer
        setReferencePointName("point");

        // ========================================================================================
        // Scattering expressions
        
        // Form factor
        FormFactorExpression = 0;

        // Form factor amplitude expression relative to all reference points.        
        FormFactorAmplitudeExpressions["point"]      = 0;

        // ========================================================================================
        // Phase factors
        PhaseFactorExpressions["point"]["point"]       = 1.0;

        // ========================================================================================
        // Sizes

        RadiusOfGyration2 = 0;
        sigmaMSDref2scat["point"]    = 0;
        sigmaMSDref2ref["point"]["point"]  = 0;
    }
    
    // Override form factor, since we assume beta=0 for a point we just return zero.
    virtual ex FormFactor(ParameterList&  betas, ParameterList&  params, int varForm = GENERIC )
    {
      return ex(0); 
    }

    //  Override form factor amplitude, since we assume beta=0 for a point we just return zero.
    virtual ex FormFactorAmplitude(refPoint r, ParameterList&  betas, ParameterList&  params, int varForm = GENERIC )
    {
      return ex(0); 
    }

    // A point introduces no distance, hence traversing it causes a phase factor of 1.
    virtual ex PhaseFactor(refPoint r1, refPoint r2, ParameterList&  betas, ParameterList&  params, int varForm = GENERIC )
    {
      return ex(1); 
    }

};

#endif // INCLUDE_GUARD_Point
