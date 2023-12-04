//===========================================================================
// Included guards
#ifndef INCLUDE_GUARD_GAUSSIANPOLYMER
#define INCLUDE_GUARD_GAUSSIANPOLYMER

/*===========================================================================

   This file implements the scattering terms of a polymer model characterized
   by Gaussian chain statistics at all scales.
   
   A polymer has 3 specific reference points and 1 distributed reference points:
         end1, end2     The two ends at l=0 and l=L, respectively.
         middle         A point at l=L/2
         contour        A random selected point along the contour [0:L]

   We choose Rg2, the square radius of gyration of the polymer as the characteristic
   length scale. For random walks Rg^2 = b L/6 where b is the step length,
   L=bN is the contour length if we have N steps.
   
   This means all scattering expressions are expressed via the
   dimensionless number x=q^2 Rg^2
   
   The form factor was originally derived by Debye in
   P. Debye, J. Phys. Colloid Chem., 51 (1947) 18-32.
   
   The form factor amplitude was originally derived by Hammouda in
   B. Hammouda, J. Polym. Sci., Part B: Polym. Phys. 30, 1387 (1992).2

   See RWPolymer.nb / RWPolymer.pdf  for derivations.   
============================================================================= */


class GaussianPolymer : public SubUnit{

    public:
        GaussianPolymer() : SubUnit(){    
            type = SUBUNITCHILD;
           stype = GAUSSIANPOLYMER;
            setName("GaussianPolymer");
        }
        
    virtual ~GaussianPolymer(){}

    virtual void Init(string name, string tag, SymbolInterface *GEX)
    {
        // Initialize base class
        SubUnit::Init(name, tag, GEX);
        string n = getTag();
        
        // ========================================================================================
        // Setup reference points

        // specific reference points for a polymer
        setReferencePointName("end1");
        setReferencePointName("middle");
        setReferencePointName("end2");

        // distributed reference points for a polymer
        setDistReferencePointType("contour");

         
        // ========================================================================================
        // Define symbols via GLEX interface
        symbol q    = GLEX->getSymbol("q");           // Global q
        symbol Rg   = GLEX->getSymbol("Rg", n);    // For polymers we use the Radius of gyration to define the size.
        symbol x    = GLEX->getSymbol("x", n);     // The natural dimensionless variable is x = (q Rg)^2


        // ========================================================================================
        // Define mapping between dimensionless variables and scattering expressions using Rg and q as variables:
        expand[x] = pow(q*Rg ,2);   // x = q^2 Rg^2

        xparameters.insert(x);           // Local structural parameters
        parameters.insert(Rg);           // Local structural parameters

        // ========================================================================================
        // Scattering expressions
        
        // Form factor for polymer: (Debye expression)
        FormFactorExpression = 2*(exp(-x)-1+x)/(x*x); 

        // Form factor amplitude expression relative to all reference points.        
        FormFactorAmplitudeExpressions["end1"]      = (1-exp(-x))/x;
        FormFactorAmplitudeExpressions["end2"]      = (1-exp(-x))/x;
        FormFactorAmplitudeExpressions["middle"]    = 2*(1-exp(-x/2))/x;
        FormFactorAmplitudeExpressions["contour"]   = FormFactorExpression;

        // ========================================================================================
        // Phase factors
        // We need phase factors for all reference point pairs except a specific geometric reference point and itself, since PhaseFactor[X][X]=1.
        // The two reference points must be alfabetically sorted.

        // between pairs of ends
        PhaseFactorExpressions["end1"]["end2"]       = exp(-x);

        // between middle and ends
        PhaseFactorExpressions["end1"]["middle"]     =
        PhaseFactorExpressions["end2"]["middle"]     = exp(-x/2);

        // between ends and contour         
        PhaseFactorExpressions["contour"]["end1"]    =
        PhaseFactorExpressions["contour"]["end2"]    = FormFactorAmplitudeExpressions[ "end1" ];                                               

        // between middle and contour
        PhaseFactorExpressions["contour"]["middle"]  = FormFactorAmplitudeExpressions[ "middle" ];

        // Finally between contour and contour
        PhaseFactorExpressions["contour"]["contour"] =  FormFactorExpression;

        // ========================================================================================
        // Sizes

        ex Rg2 = pow(Rg, 2);   // All sizes below have unit (distance)^2 and are expressed in terms of Rg^2. 

        // Radius of gyration expression for polymer, simple since we express the polymer as function of Rg:
        RadiusOfGyration2 = Rg2;

        // sigma <R^2> for all distances between reference points and scatterers.
        // These are exactly the Guinier expansions of the corresponding amplitudes
        sigmaMSDref2scat["end1"]    =
        sigmaMSDref2scat["end2"]    = 3*Rg2;                   // sigma = 1, <R^2_end_to_random> = bL/2
        sigmaMSDref2scat["middle"]  = 3*Rg2/2;                 // sigma = 1, <R^2_middle_to_random> = bL/4
        sigmaMSDref2scat["contour"] = 2*Rg2;                   // sigma = 2, <R^2_random_to_random> = bL/6 = Rg2

        // sigma <R^2> for all distances between pairs of reference points.
        // These are exactly the Guinier expansions of the corresponding phase factors
        // Again the two reference points must be alfabetically sorted.

        // betweeen pairs of ends
        sigmaMSDref2ref["end1"]["end2"]       = 6*Rg2;          // sigma=1, <R^2_end1_to_end2> = 6Rg^2 = b L

        // between middle and ends
        sigmaMSDref2ref["end1"]["middle"]     =
        sigmaMSDref2ref["end2"]["middle"]     = 3*Rg2;         // sigma=1, <R^2_end_to_middle> = 3Rg^2 = b L/2
        
        // between ends and contour
        sigmaMSDref2ref["contour"]["end1"]    = 
        sigmaMSDref2ref["contour"]["end2"]    = 3*Rg2;         // sigma=!, <R^2_end_to_contour> = < bl > averaged over l in [0:L] = b L/2 = 3Rg^2

        // between middle and contour
        sigmaMSDref2ref["contour"]["middle"]  = 3*Rg2/2;       // sigma=1, <R^2_middle_to_contour> = < b|l-L/2| > averaged over l in [0:L] = b L / 4 = 3Rg^2/2
           
        // Finally between contour and contour
        sigmaMSDref2ref["contour"]["contour"] = 2*Rg2;         // sigma=2, < b |l1-l2| > averaged over l1<l2 in [0:L] = bL/6 = Rg^2
    }

};

#endif // INCLUDE_GUARD_RWPOLYMER
