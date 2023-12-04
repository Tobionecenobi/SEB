//===========================================================================
// Included guards
#ifndef INCLUDE_GUARD_THINSPHERICALSHELL
#define INCLUDE_GUARD_THINSPHERICALSHELL


/*===========================================================================

   This file implements the scattering terms of a infinitesimal spherical shell.
   
   The Shell has two reference points:
          center     The geometric point at the center of the ThinSphericalShell.
          surface    A randomly selected point on any surface.
   
   We choose R the radius as the characteristic length scales.
   Hence the dimensionless variable is x=qR.

   See SolidSphere.nb / SolidSphere.pdf for derivations.
============================================================================= */

class ThinSphericalShell : public SubUnit {
    public: 
    /*Constructor*/
    public: 
        ThinSphericalShell() : SubUnit(){    
            type = SUBUNITCHILD;
           stype = THINSPHERICALSHELL;
        }
    virtual ~ThinSphericalShell(){};

    virtual void Init(string name, string tag, SymbolInterface *GEX)
    {
        // Initialize base class
        SubUnit::Init(name, tag, GEX);
        string n = getTag();

        // ========================================================================================
        // Setup reference points

        // specific reference points for a ThinSphericalShell
        setReferencePointName("center");

        // distributed reference points for a ThinSphericalShell        
        setDistReferencePointType("surface");

        // ========================================================================================
        // Define symbols
        symbol q    = GLEX->getSymbol("q");
        symbol x    = GLEX->getSymbol("x", n); 
        symbol R    = GLEX->getSymbol("R", n);

        // ========================================================================================
        // Define mapping between dimensionless variables and scattering expressions using R and q as variables:

        expand[x] = q*R;            // x=qR

        xparameters.insert(x);      // Structural parameter.
        parameters.insert(R);       // Structural parameter.

        // ========================================================================================
        // Scattering expressions
        
        ex Ac = sin(x)/x;                                       // Amplitude relative to center of a thin spherical shell
        
        FormFactorExpression = Ac*Ac;

        // Form factor amplitude expression relative to all reference points.        

        FormFactorAmplitudeExpressions["center"]   = Ac;
        FormFactorAmplitudeExpressions["surface"]  = Ac*Ac;
        
        // ========================================================================================
        // Phase factors
        // We need phase factors for all reference point pairs except a specific reference point and itself, since PhaseFactor[X][X]=1.

        // between the center and surface
        PhaseFactorExpressions["center"]["surface"]   = Ac;
        PhaseFactorExpressions["surface"]["surface"]   = Ac*Ac;
        
        // ========================================================================================
        // Sizes

        // Radius of gyration expression for polymer, simple since we express the polymer as function of R:
        RadiusOfGyration2 = R*R;

        // sigma <R^2> for all distances between reference points and scatterers.
        // These are exactly the Guinier expansions of the corresponding amplitudes
        sigmaMSDref2scat["center"]    = R*R; 
        sigmaMSDref2scat["surface"]   = 2*R*R;

        // sigma <R^2> for all distances between pairs of reference points.
        // These are exactly the Guinier expansions of the corresponding phasefactors

        // between the center and surface
        sigmaMSDref2ref["center"]["surface"]   = R*R;

        // between two random points on the surface
        sigmaMSDref2ref["surface"]["surface"]   = 2*R*R;
    }

};

#endif // INCLUDE_GUARD_THINSPHERICALSHELL
