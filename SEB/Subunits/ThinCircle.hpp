//===========================================================================
// Included guards
#ifndef INCLUDE_GUARD_THINCIRCLE
#define INCLUDE_GUARD_THINCIRCLE


/*===========================================================================

   This file implements the scattering terms of a thin circle
   
   A circle has two reference points:
          center     The geometric point at the center of the cicle
          contour    A randomly selected point on the circle
   
   We choose R the radius as the characteristic length scales.
   Hence the dimensionless variable is x=qR.

   See Circle.nb / Circle.pdf for derivations.
============================================================================= */

class ThinCircle : public SubUnit {
    public: 
    /*Constructor*/
    public: 
        ThinCircle() : SubUnit(){    
            type = SUBUNITCHILD;
           stype = THINCIRCLE;
        }
    virtual ~ThinCircle(){};

    virtual void Init(string name, string tag, SymbolInterface *GEX)
    {
        // Initialize base class
        SubUnit::Init(name, tag, GEX);
        string n = getTag();

        // ========================================================================================
        // Setup reference points

        // specific reference points for a Circle
        setReferencePointName("center");

        // distributed reference points for a Circle        
        setDistReferencePointType("contour");

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
                
        FormFactorExpression = BesselJ0(2*x)+Pi/2*(BesselJ1(2*x)*StruveH0(2*x)-BesselJ0(2*x)*StruveH1(2*x));

        // Form factor amplitude expression relative to all reference points.        

        FormFactorAmplitudeExpressions["center"]   = sin(x)/x;
        FormFactorAmplitudeExpressions["contour"]  = FormFactorExpression;
        
        // ========================================================================================
        // Phase factors
        // We need phase factors for all reference point pairs except a specific reference point and itself, since PhaseFactor[X][X]=1.

        // between the center and surface
        PhaseFactorExpressions["center"]["contour"]        = sin(x)/x;
        PhaseFactorExpressions["contour"]["contour"]    = FormFactorExpression;

        // ========================================================================================
        // Sizes

        // Radius of gyration expression for polymer, simple since we express the polymer as function of R:
        RadiusOfGyration2 = R*R/2;

        // sigma <R^2> for all distances between reference points and scatterers.
        // These are exactly the Guinier expansions of the corresponding amplitudes
        sigmaMSDref2scat["center"]    = R*R;
        sigmaMSDref2scat["contour"]   = 2*R*R;
        
        // sigma <R^2> for all distances between pairs of reference points.
        // These are exactly the Guinier expansions of the corresponding phasefactors

        // between the center and surface
        sigmaMSDref2ref["center"]["contour"]   = R*R;
        sigmaMSDref2ref["contour"]["contour"]  = 2*R*R;
    }

};

#endif // INCLUDE_GUARD_Circle
