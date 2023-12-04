//===========================================================================
// Included guards
#ifndef INCLUDE_GUARD_THINDISK
#define INCLUDE_GUARD_THINDISK


/*===========================================================================

   This file implements the scattering terms of a infinitesimal thin disk.
   
   The Shell has two reference points:
          center     The geometric point at the center of the disk.
          surface    A randomly selected point on the surface of the disk.
   
   We choose R the radius as the characteristic length scales.
   Hence the dimensionless variable is x=qR.

   See SolidSphere.nb / SolidSphere.pdf for derivations.
============================================================================= */

class ThinDisk : public SubUnit {
    public: 
    /*Constructor*/
    public: 
        ThinDisk() : SubUnit(){    
            type = SUBUNITCHILD;
           stype = THINDISK;
        }
    virtual ~ThinDisk(){};

    virtual void Init(string name, string tag, SymbolInterface *GEX)
    {
        // Initialize base class
        SubUnit::Init(name, tag, GEX);
        string n = getTag();

        // ========================================================================================
        // Setup reference points

        // specific reference points for a ThinDisk
        setReferencePointName("center");

        // distributed reference points for a ThinDisk        
        setDistReferencePointType("surface");
        setDistReferencePointType("rim");

        // Define symbols
        symbol q    = GLEX->getSymbol("q");
        symbol x    = GLEX->getSymbol("x", n); 
        symbol R    = GLEX->getSymbol("R", n);
        symbol t    = GLEX->getSymbol("t", n);

        // ========================================================================================
        // Define mapping between dimensionless variables and scattering expressions using R and q as variables:

        expand[x] = q*R;            // x=qR

        xparameters.insert(x);      // Structural parameter.
        parameters.insert(R);       // Structural parameter.

        // ========================================================================================
        // Scattering expressions
        
        ex Ac = integral(t, 0, Pi/2, 2*BesselJ1(x*sin(t))/x );                                       // Amplitude relative to center of a thin spherical shell
        
        FormFactorExpression = 2*(x-BesselJ1(2*x))/pow(x,3);

        // Form factor amplitude expression relative to all reference points.        

        FormFactorAmplitudeExpressions["center"]   = Ac;
        FormFactorAmplitudeExpressions["surface"]  = FormFactorExpression;
        FormFactorAmplitudeExpressions["rim"]      = integral(t, 0, Pi/2, 2*BesselJ0(x*sin(t))*BesselJ1(x*sin(t))/x ); 
        
        // ========================================================================================
        // Phase factors
        // We need phase factors for all reference point pairs except a specific reference point and itself, since PhaseFactor[X][X]=1.

        // between the center and surface
        PhaseFactorExpressions["center"]["surface"]    = Ac;
        PhaseFactorExpressions["surface"]["surface"]   = FormFactorExpression;
        PhaseFactorExpressions["center"]["rim"]        = sin(x)/x;
        PhaseFactorExpressions["rim"]["surface"]       = integral(t, 0, Pi/2, 2*BesselJ0(x*sin(t))*BesselJ1(x*sin(t))/x ); 
        PhaseFactorExpressions["rim"]["rim"]           = BesselJ0(2*x)+Pi/2*(BesselJ1(2*x)*StruveH0(2*x)-BesselJ0(2*x)*StruveH1(2*x));

        // ========================================================================================
        // Sizes

        // Radius of gyration expression for polymer, simple since we express the polymer as function of R:
        RadiusOfGyration2 = R*R/2;

        // sigma <R^2> for all distances between reference points and scatterers.
        // These are exactly the Guinier expansions of the corresponding amplitudes
        sigmaMSDref2scat["center"]    = R*R/2;
        sigmaMSDref2scat["surface"]   = R*R;
        sigmaMSDref2scat["rim"]       = 3*R*R/2;
        
        // sigma <R^2> for all distances between pairs of reference points.
        // These are exactly the Guinier expansions of the corresponding phasefactors

        // between the center and surface
        sigmaMSDref2ref["center"]["surface"]   = R*R/2;
        sigmaMSDref2ref["surface"]["surface"]  = R*R;
        sigmaMSDref2ref["center"]["rim"]       = R*R;
        sigmaMSDref2ref["rim"]["surface"]      = 3*R*R/2;;
        sigmaMSDref2ref["rim"]["rim"]          = 2*R*R;
    }

};

#endif // INCLUDE_GUARD_THINDISK
