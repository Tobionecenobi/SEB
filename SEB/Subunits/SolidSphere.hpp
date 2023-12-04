//===========================================================================
// Included guards
#ifndef INCLUDE_GUARD_SOLIDSPHERE
#define INCLUDE_GUARD_SOLIDSPHERE


/*===========================================================================

   This file implements the scattering terms of a homogeneous solid sphere.
   
   The sphere has two reference points:
          center     The geometric point at the center of the sphere.
          surface    A randomly selected point on the surface at radius R.
   
   We choose R, the radius as the characteristic length scale. Hence the
   dimensionless variable x=qR is used to express scattering expressions.

   The form factor was originally derived by Releigh in
   Lord Rayleigh, Proc. Roy. Soc. London, Ser. A, 84 (1911) 25-38.
   
   See Sphere.nb / Sphere.pdf for derivations.
============================================================================= */

class SolidSphere : public SubUnit {
    public: 
    /*Constructor*/
    public: 
        SolidSphere() : SubUnit(){    
            type = SUBUNITCHILD;
           stype = SOLIDSPHERE;
        }
    virtual ~SolidSphere(){};

    virtual void Init(string name, string tag, SymbolInterface *GEX)
    {
        // Initialize base class
        SubUnit::Init(name, tag, GEX);
        string n = getTag();                  // n could be name but will be overwritten by tag if defined.

        // ========================================================================================
        // Setup reference points

        // specific reference points for a sphere
        setReferencePointName("center");

        // distributed reference points for a sphere        
        setDistReferencePointType("surface");

        // ========================================================================================
        // Define symbols
        symbol q    = GLEX->getSymbol("q");
        symbol x    = GLEX->getSymbol("x", n); 
        symbol R    = GLEX->getSymbol("R", n);

        // ========================================================================================
        // Define mapping between dimensionless variables and scattering expressions using R and q as variables:

        expand[x] = q*R;          // x=qR

        xparameters.insert(x);     // Structural parameter.
        parameters.insert(R);     // Structural parameter.

        // ========================================================================================
        // Scattering expressions
        
        ex A = 3*(sin(x)-x*cos(x))/pow(x,3);  // Amplitude relative to center of sphere as derived by Rayleigh.
        
        FormFactorExpression = A*A;

        // Form factor amplitude expression relative to all reference points.        

        FormFactorAmplitudeExpressions["center"]  = A;
        FormFactorAmplitudeExpressions["surface"] = A*sin(x)/x;
        
        // ========================================================================================
        // Phase factors
        // We need phase factors for all reference point pairs except a specific reference point and itself, since PhaseFactor[X][X]=1.

        // between the center and surface
        PhaseFactorExpressions["center"]["surface"]  = sin(x)/x;

        // between two random points on the surface
        PhaseFactorExpressions["surface"]["surface"] = pow(sin(x)/x,2);
        

        // ========================================================================================
        // Sizes

        // Radius of gyration expression for polymer, simple since we express the polymer as function of R:
        RadiusOfGyration2 = 3*R*R/5;

        // sigma <R^2> for all distances between reference points and scatterers.
        // These are exactly the Guinier expansions of the corresponding amplitudes
        sigmaMSDref2scat["center"]   = 3*R*R/5;                 // sigma=1, <R^2_centre_to_scatterer> = 3R^2/5
        sigmaMSDref2scat["surface"]  = 8*R*R/5;                 // sigma=1, <R^2_surface_to_scatterer> = 8R²/5

        // sigma <R^2> for all distances between pairs of reference points.
        // These are exactly the Guinier expansions of the corresponding phasefactors

        // betweeen the centre and a random point on the surface
        sigmaMSDref2ref["center"]["surface"]       = R*R;           // sigma=1, <R^2_surface_to_center = R^2

        // between two random points on the surface
        sigmaMSDref2ref["surface"]["surface"]     = 2*R*R;          // sigma=2, <R^2_surface_to_surface> = R^2
       
    }

};

#endif // INCLUDE_GUARD_SPHERE
