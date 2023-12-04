//===========================================================================
// Included guards
#ifndef INCLUDE_GUARD_SOLIDSPHERICALSHELL
#define INCLUDE_GUARD_SOLIDSPHERICALSHELL


/*===========================================================================

   This file implements the scattering terms of a spherical shell.
   
   The SphericalShell has two reference points:
          center     The geometric point at the center of the SphericalShell.
          surfaceo   A randomly selected point on the exterior surface at radius Router.
          surfacei   A randomly selected point on the interior surface at radius Rinnner.
          surface    A randomly selected point on any surface (area weighted).
   
   We choose Ri, Ro, the inner and outer radia as the characteristic length scales.
   Hence the dimensionless variables xi=qRi and xo=qRo are useed to express scattering expressions.

   
   See SphericalShell.nb / SphericalShell.pdf for derivations.
============================================================================= */

class SolidSphericalShell : public SubUnit {
    public: 
    /*Constructor*/
    public: 
        SolidSphericalShell() : SubUnit(){    
            type = SUBUNITCHILD;
           stype = SOLIDSPHERICALSHELL;
        }
    virtual ~SolidSphericalShell(){};

    virtual void Init(string name, string tag, SymbolInterface *GEX)
    {
        // Initialize base class
        SubUnit::Init(name, tag, GEX);
        string n = getTag();

        // ========================================================================================
        // Setup reference points

        // specific reference points for a SphericalShell
        setReferencePointName("center");

        // distributed reference points for a SphericalShell        
        setDistReferencePointType("surface");
        setDistReferencePointType("surfaceo");
        setDistReferencePointType("surfacei");

        // ========================================================================================
        // Define symbols
        symbol q    = GLEX->getSymbol("q");
        symbol xi   = GLEX->getSymbol("xi", n); 
        symbol xo   = GLEX->getSymbol("xo", n); 
        symbol Ri   = GLEX->getSymbol("Ri", n);
        symbol Ro   = GLEX->getSymbol("Ro", n);

        // ========================================================================================
        // Define mapping between dimensionless variables and scattering expressions using R and q as variables:

        expand[xi] = q*Ri;          // x=qR
        expand[xo] = q*Ro;          // x=qR

        xparameters.insert(xo);     // Structural parameter.
        xparameters.insert(xi);     // Structural parameter.
        xparameters.insert(Ro);     // Structural parameter.
        xparameters.insert(Ri);     // Structural parameter.
        
        parameters.insert(Ro);      // Structural parameter.
        parameters.insert(Ri);      // Structural parameter.

        // ========================================================================================
        // Scattering expressions
        
        ex Ac = (3*(xi*cos(xi) - xo*cos(xo) - sin(xi) + sin(xo)))/(pow(xo,3)-pow(xi,3));  // Amplitude relative to center of the solid shell
        
        FormFactorExpression = Ac*Ac;

        // Form factor amplitude expression relative to all reference points.        

        FormFactorAmplitudeExpressions["center"]   = Ac;
        FormFactorAmplitudeExpressions["surfacei"] = Ac*sin(xi)/xi;
        FormFactorAmplitudeExpressions["surfaceo"] = Ac*sin(xo)/xo;
        
        ex Areaout=4*Pi*Ro*Ro;
        ex Areain =4*Pi*Ri*Ri;
        ex PsiSurf = ( Areaout*sin(xo)/xo + Areain*sin(xi)/xi )/(Areaout+Areain);
        
        FormFactorAmplitudeExpressions["surface"] = Ac*PsiSurf;
        
        // ========================================================================================
        // Phase factors
        // We need phase factors for all reference point pairs except a specific reference point and itself, since PhaseFactor[X][X]=1.

        // between the center and surface
        PhaseFactorExpressions["center"]["surface"]   = PsiSurf;
        PhaseFactorExpressions["center"]["surfacei"]  = sin(xi)/xi;
        PhaseFactorExpressions["center"]["surfaceo"]  = sin(xo)/xo;

        // between two random points on the surface
        PhaseFactorExpressions["surfacei"]["surfacei"]  = pow(sin(xi)/xi,2);
        PhaseFactorExpressions["surfaceo"]["surfaceo"]  = pow(sin(xo)/xo,2);

        PhaseFactorExpressions["surface"]["surfacei"]  = PsiSurf*sin(xi)/xi;
        PhaseFactorExpressions["surface"]["surfaceo"]  = PsiSurf*sin(xo)/xo;
        PhaseFactorExpressions["surface"]["surface"]   = pow(PsiSurf,2);
        PhaseFactorExpressions["surfacei"]["surfaceo"] = sin(xi)*sin(xo)/xi/xo;
        
        // ========================================================================================
        // Sizes

        // Radius of gyration expression for polymer, simple since we express the polymer as function of R:
        RadiusOfGyration2 = (3*(Ro*pow(Ri,3) + pow(Ri,4) + pow(Ri,2)*pow(Ro,2) + Ri*pow(Ro,3) + pow(Ro,4))*
     pow(Ri*Ro + pow(Ri,2) + pow(Ro,2),-1))/5;

        // sigma <R^2> for all distances between reference points and scatterers.
        // These are exactly the Guinier expansions of the corresponding amplitudes
        sigmaMSDref2scat["center"]    = RadiusOfGyration2; 
        sigmaMSDref2scat["surfacei"]  = (8*Ri*Ri*Ri*Ri + 8*Ri*Ri*Ri*Ro + 8*Ri*Ri*Ro*Ro + 3*Ri*Ro*Ro*Ro + 3*Ro*Ro*Ro*Ro)/(5*(Ri*Ri+Ri*Ro+Ro*Ro));
        sigmaMSDref2scat["surfaceo"]  = (3*Ri*Ri*Ri*Ri + 3*Ri*Ri*Ri*Ro + 8*Ri*Ri*Ro*Ro + 8*Ri*Ro*Ro*Ro + 8*Ro*Ro*Ro*Ro)/(5*(Ri*Ri+Ri*Ro+Ro*Ro));
        sigmaMSDref2scat["surface"]   = (8*Ro*pow(Ri,5) + 8*pow(Ri,6) + 11*pow(Ri,4)*Ro*Ro + 6*pow(Ri*Ro,3) +  11*Ri*Ri*pow(Ro,4) + 8*Ri*pow(Ro,5) + 8*pow(Ro,6))
                                        /(Ro*pow(Ri,3) + pow(Ri,4) + 2*Ri*Ri*Ro*Ro + Ri*pow(Ro,3) + pow(Ro,4))/5;

        // sigma <R^2> for all distances between pairs of reference points.
        // These are exactly the Guinier expansions of the corresponding phasefactors

        // between the center and surface
        sigmaMSDref2ref["center"]["surface"]   = (Areaout*Ro*Ro+Areain*Ri*Ri)/(Areaout+Areain);
        sigmaMSDref2ref["center"]["surfacei"]  = Ri*Ri;
        sigmaMSDref2ref["center"]["surfaceo"]  = Ro*Ro;

        // between two random points on the surface
        sigmaMSDref2ref["surfacei"]["surfacei"]  = 2*Ri*Ri;
        sigmaMSDref2ref["surfaceo"]["surfaceo"]  = 2*Ro*Ro;


        sigmaMSDref2ref["surface"]["surfacei"]  = (2*Ri*Ri*Ri*Ri+Ri*Ri*Ro*Ro+  Ro*Ro*Ro*Ro)/(Ri*Ri+Ro*Ro);
        sigmaMSDref2ref["surface"]["surfaceo"]  = (  Ri*Ri*Ri*Ri+Ri*Ri*Ro*Ro+2*Ro*Ro*Ro*Ro)/(Ri*Ri+Ro*Ro);
        sigmaMSDref2ref["surface"]["surface"]   = 2*(Ri*Ri*Ri*Ri+Ro*Ro*Ro*Ro)/(Ri*Ri+Ro*Ro);
        sigmaMSDref2ref["surfacei"]["surfaceo"] = Ri*Ri+Ro*Ro;

    }

};

#endif // INCLUDE_GUARD_SphericalShell
