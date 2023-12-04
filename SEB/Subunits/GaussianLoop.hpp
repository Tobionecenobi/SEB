//===========================================================================
// Included guards
#ifndef INCLUDE_GUARD_GAUSSIANLOOP
#define INCLUDE_GUARD_GAUSSIANLOOP

/*===========================================================================

   This file implements the scattering terms of a closed random walk loop
   where the internal chain statistics is Gaussian.
      
   A loop has one distributed reference point:
         contour        A random selected point along the contour [0:L]

   We choose the length Rg as the structural parameter, Rg is related
   to the step length of the random walk b and the total contour length
   of the loop L as Rg^2 = bL/12  which is exactly half of that of a
   linear random walk.
   


   See RWloop.nb / RWloop.pdf  for derivations.   
============================================================================= */


class GaussianLoop : public SubUnit{

    public:
        GaussianLoop() : SubUnit(){    
            type = SUBUNITCHILD;
           stype = GAUSSIANLOOP;
        }
        
    virtual ~GaussianLoop(){}

    virtual void Init(string name, string tag, SymbolInterface *GEX)
    {
        // Initialize base class
        SubUnit::Init(name, tag, GEX);
        string n = getTag();
        
        // ========================================================================================
        // Setup reference points

        // distributed reference points for a polymer
        setDistReferencePointType("contour");
         
        // ========================================================================================
        // Define symbols via GLEX interface
        symbol q    = GLEX->getSymbol("q");        // Global q
        symbol Rg   = GLEX->getSymbol("Rg", n);    // For RWloops we use the radius of gyration to define its size
        symbol x    = GLEX->getSymbol("x", n);     // The natural dimensionless variable


        // ========================================================================================
        // Define mapping between dimensionless variables and scattering expressions using Rg and q as variables:
        expand[x] = q*Rg/sqrt(2);        // x = q sqrt(bL/24) = q sqrt(Rg^2/2) = q Rg / sqrt(2)

        xparameters.insert(x);           // Local structural parameters
        parameters.insert(Rg);           // Local structural parameters

        // ========================================================================================
        // Scattering expressions
        
        // Form factor for RWloop
        FormFactorExpression = DawsonF(x)/x;
        
        // Form factor amplitude expression relative to all reference points.        
        FormFactorAmplitudeExpressions["contour"]    = FormFactorExpression;
        PhaseFactorExpressions["contour"]["contour"] =  FormFactorExpression;

        // ========================================================================================
        // Sizes

        // Radius of gyration expression for RWloop:
        RadiusOfGyration2 = Rg*Rg;

        // sigma <R^2> for all distances between reference points and scatterers.
        // These are exactly the Guinier expansions of the corresponding amplitudes
        sigmaMSDref2scat["contour"] = 2*Rg*Rg;                   // sigma = 2, <R^2_random_to_random> = Rg^2
        sigmaMSDref2ref["contour"]["contour"] = 2*Rg*Rg;         // sigma=2 , < R^2 > averaged over all points = Rg^2
    }

};

#endif // INCLUDE_GUARD_RWLOOP
