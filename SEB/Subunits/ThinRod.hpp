//===========================================================================
// Included guards
#ifndef INCLUDE_GUARD_THINROD
#define INCLUDE_GUARD_THINROD

/*===========================================================================

   This file implements the scattering terms of a straight rod.
   
   A rod had 2 specific reference points
         end1, end2     The two ends at l=0 and l=L, respectively.
         contour        A random selected point along the contour [0:L]

   We choose the length L as the structural parameter. Scattering
   expressions are expressed via the dimensionless number x=qL
   
   Reference for this form factor: T. Neugebauer, Ann. Phys. (Leipzig) 42, 509 (1943).
   P. I. Teixeira, D. J. Read, and T. C. B. McLeish, J. Chem. Phys. 126, 074901 (2007).

   The form factor amplitude was originally derived by Hammouda in

   See rod.nb / rod.pdf  for derivations.   
============================================================================= */


class ThinRod : public SubUnit{

    public:
        ThinRod() : SubUnit(){    
            type = SUBUNITCHILD;
           stype = THINROD;
        }
        
    virtual ~ThinRod(){}

    virtual void Init(string name, string tag, SymbolInterface *GEX)
    {
        // Initialize base class
        SubUnit::Init(name, tag, GEX);
        string n = getTag();
        
        // ========================================================================================
        // Setup reference points

        // specific reference points for a polymer
        setReferencePointName("end1");
        setReferencePointName("end2");
        setReferencePointName("middle");

        // distributed reference points for a polymer
        setDistReferencePointType("contour");

         
        // ========================================================================================
        // Define symbols via GLEX interface
        symbol q    = GLEX->getSymbol("q");        // Global q
        symbol L    = GLEX->getSymbol("L", n);     // For rods we use the length L to define its size
        symbol x    = GLEX->getSymbol("x", n);     // The natural dimensionless variable is qL


        // ========================================================================================
        // Define mapping between dimensionless variables and scattering expressions using Rg and q as variables:
        expand[x] = q*L;                 // x = q L

        xparameters.insert(x);           // Local structural parameters
        parameters.insert(L);            // Local structural parameters

        // ========================================================================================
        // Scattering expressions
        
        // Form factor for rod
        FormFactorExpression = 2*(cos(x)-1)/(x*x)+2*Six(x);
        
        ex FormFactorAmplitudeEnd = Six(x);
        ex FormFactorAmplitudeMiddle = Six(x/2);

        // Form factor amplitude expression relative to all reference points.        
        FormFactorAmplitudeExpressions["end1"]      = FormFactorAmplitudeEnd;
        FormFactorAmplitudeExpressions["end2"]      = FormFactorAmplitudeEnd;        
        FormFactorAmplitudeExpressions["middle"]    = FormFactorAmplitudeMiddle;
        FormFactorAmplitudeExpressions["contour"]   = FormFactorExpression;

        // ========================================================================================
        // Phase factors
        // We need phase factors for all reference point pairs except a specific geometric reference point and itself, since PhaseFactor[X][X]=1.
        // The two reference points must be alfabetically sorted.

        // between pairs of ends
        PhaseFactorExpressions["end1"]["end2"]       = sin(x)/x;

        PhaseFactorExpressions["end1"]["middle"]     = 2*sin(x/2)/x;
        PhaseFactorExpressions["end2"]["middle"]     = 2*sin(x/2)/x;

        // between ends and contour         
        PhaseFactorExpressions["contour"]["end1"]    = FormFactorAmplitudeEnd;
        PhaseFactorExpressions["contour"]["end2"]    = FormFactorAmplitudeEnd;

        PhaseFactorExpressions["contour"]["middle"]  = FormFactorAmplitudeMiddle;

        // Finally between contour and contour
        PhaseFactorExpressions["contour"]["contour"] =  FormFactorExpression;

        // ========================================================================================
        // Sizes

        // Radius of gyration expression for rod:
        RadiusOfGyration2 = L*L/12;

        // sigma <R^2> for all distances between reference points and scatterers.
        // These are exactly the Guinier expansions of the corresponding amplitudes
        sigmaMSDref2scat["end1"]    =
        sigmaMSDref2scat["end2"]    = L*L/3;                   // sigma = 1, <R^2_end_to_random> = L^2/3
        sigmaMSDref2scat["middle"]  = L*L/12;                  // sigma = 1, <R^2_middle_to_random> = L^2/12
        sigmaMSDref2scat["contour"] = L*L/6;                   // sigma = 2, <R^2_random_to_random> = L^2/12

        // sigma <R^2> for all distances between pairs of reference points.
        // These are exactly the Guinier expansions of the corresponding phase factors
        // Again the two reference points must be alfabetically sorted.

        // betweeen pairs of ends
        sigmaMSDref2ref["end1"]["end2"]       = L*L;           // sigma=1, <R^2_end1_to_end2> = L^2

        // between middle and ends
        sigmaMSDref2ref["end1"]["middle"]     =
        sigmaMSDref2ref["end2"]["middle"]     = L*L/4;         // sigma=1, <R^2_end_to_middle> = (L/2)^2
        
        // between ends and contour
        sigmaMSDref2ref["contour"]["end1"]    = 
        sigmaMSDref2ref["contour"]["end2"]    = L*L/3;         // sigma=!, <R^2_end_to_contour> = L^2/3

        // between middle and contour
        sigmaMSDref2ref["contour"]["middle"]  = L*L/12;        // sigma=1, <R^2_middle_to_contour> = L^2/12  (by definition this is the same as Rg^2 for a rod)
           
        // Finally between contour and contour
        sigmaMSDref2ref["contour"]["contour"] = L*L/6;         // sigma=2, < R^2 > averaged over all points = L^2/12
    }

};

#endif // INCLUDE_GUARD_ROD
