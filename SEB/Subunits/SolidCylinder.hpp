//===========================================================================
// Included guards
#ifndef INCLUDE_GUARD_SOLIDCYLINDER
#define INCLUDE_GUARD_SOLIDCYLINDER


/*===========================================================================

   This file implements the scattering terms of a homogeneous solid cylinder.
   
   The center has   reference points:
          center       The geometric point at the center of the cylinder.
          hull         A randomly selected point on circular hull of the cylinder.
          ends         A random selected point on the two flat ends of the cylinder
          surface      A randomly selected point on the hull/ends weighted by their areas.

   We choose R, the radius, and L the cylinder length as the characteristic length scale.
   Hence the dimensionless variable x=qR and y=qL is used to express scattering expressions.
   
   See SolidCylinder.nb / SolidCylinder.pdf for derivations.
============================================================================= */

static const double SMALL = 1e-5;

class SolidCylinder : public SubUnit {
    public: 
    /*Constructor*/
    public: 
        SolidCylinder() : SubUnit(){    
            type = SUBUNITCHILD;
           stype = SOLIDCYLINDER;
        }
    virtual ~SolidCylinder(){};

    virtual void Init(string name, string tag, SymbolInterface *GEX)
    {
        // Initialize base class
        SubUnit::Init(name, tag, GEX);
        string n = getTag();                  // n could be name but will be overwritten by tag if defined.

        // ========================================================================================
        // Setup reference points

        // specific reference points for a cylinder
        setReferencePointName("center");

        // distributed reference points for a cylinder    
        setDistReferencePointType("hull");
        setDistReferencePointType("ends");
        setDistReferencePointType("surface");

        // ========================================================================================
        // Define symbols
        symbol q    = GLEX->getSymbol("q");
        symbol x    = GLEX->getSymbol("x", n); 
        symbol y    = GLEX->getSymbol("y", n); 
        symbol R    = GLEX->getSymbol("R", n);
        symbol L    = GLEX->getSymbol("L", n);
        symbol t    = GLEX->getSymbol("t", n);

        // ========================================================================================
        // Define mapping between dimensionless variables and scattering expressions using R and q as variables:

        expand[x] = q*R;          // x=qR
        expand[y] = q*L;          // y=qL

        xparameters.insert(x);     // Structural parameter.
        xparameters.insert(y);     // Structural parameter.
        parameters.insert(R);      // Structural parameter.
        parameters.insert(L);      // Structural parameter.

        // ========================================================================================
        // Scattering expressions
        
        ex Surface_hull= 2*Pi*R*L;
        ex Surface_ends= 2*Pi*R*R;   // Two ends

        // Primitives for cylinder scattering expressions, all with the cylinder axis along z, and the orientational average over qvec yet to be made. 
                       
        ex A_qvec =(4*BesselJ1(x*sin(t))*csc(t)*sec(t)*sin((y*cos(t))/2))/(x*y);
        ex Psi_center2hull_qvec = (2*BesselJ0(x*sin(t))*sin(y*cos(t)/2))/(cos(t)*y); 
        ex Psi_center2ends_qvec = cos(y*cos(t)/2)*Hypergeometric0F1Regularized(2, -pow(x*sin(t),2)/4 );
        ex Psi_center2surface_qvec = (Surface_hull*Psi_center2hull_qvec+Surface_ends*Psi_center2ends_qvec)/(Surface_hull+Surface_ends);

        // Form factor is the orientational averege of A_qvec^2. 
        FormFactorExpression = integral(t, SMALL, Pi/2, sin(t)* A_qvec*A_qvec ); 

        // Form factor amplitude expression relative to all reference points are orientational integrals over A_qvec and Psi_center_to_X_qvec
        FormFactorAmplitudeExpressions["center"]      = integral(t, SMALL, Pi/2, sin(t)*A_qvec );
        FormFactorAmplitudeExpressions["ends"]        = integral(t, SMALL, Pi/2, sin(t)*A_qvec*Psi_center2ends_qvec );       
        FormFactorAmplitudeExpressions["hull"]        = integral(t, SMALL, Pi/2, sin(t)*A_qvec*Psi_center2hull_qvec );
        FormFactorAmplitudeExpressions["surface"]     = integral(t, SMALL, Pi/2, sin(t)*A_qvec*Psi_center2surface_qvec );       
        
        // ========================================================================================
        // Phase factors
        // We need phase factors for all reference point pairs except a specific reference point and itself, since PhaseFactor[X][X]=1.
      
        PhaseFactorExpressions["center"]["ends"]     = integral(t, SMALL, Pi/2, sin(t)*Psi_center2ends_qvec );
        PhaseFactorExpressions["center"]["hull"]     = integral(t, SMALL, Pi/2, sin(t)*Psi_center2hull_qvec );
        PhaseFactorExpressions["center"]["surface"]  = integral(t, SMALL, Pi/2, sin(t)*Psi_center2surface_qvec );

        PhaseFactorExpressions["ends"]["ends"]       = integral(t, SMALL, Pi/2, sin(t)*Psi_center2ends_qvec*Psi_center2ends_qvec );
        PhaseFactorExpressions["ends"]["hull"]       = integral(t, SMALL, Pi/2, sin(t)*Psi_center2hull_qvec*Psi_center2ends_qvec );
        PhaseFactorExpressions["ends"]["surface"]    = integral(t, SMALL, Pi/2, sin(t)*Psi_center2ends_qvec*Psi_center2surface_qvec );

        PhaseFactorExpressions["hull"]["hull"]       = integral(t, SMALL, Pi/2, sin(t)*Psi_center2hull_qvec*Psi_center2hull_qvec );
        PhaseFactorExpressions["hull"]["surface"]    = integral(t, SMALL, Pi/2, sin(t)*Psi_center2hull_qvec*Psi_center2surface_qvec );
        
        PhaseFactorExpressions["surface"]["surface"] = integral(t, SMALL, Pi/2, sin(t)*Psi_center2surface_qvec*Psi_center2surface_qvec );

        // ========================================================================================
        // Sizes

        // Radius of gyration expression for solid cylinder
        RadiusOfGyration2 = (L*L+6*R*R)/12;

        // sigma <R^2> for all distances between reference points and scatterers.
        // These are exactly the Guinier expansions of the corresponding amplitudes
        sigmaMSDref2scat["axispoints"]  = L*L/3+R*R/2;                                // sigma=1
        sigmaMSDref2scat["center"]      = (L*L+6*R*R)/12;                             // sigma=1
        sigmaMSDref2scat["ends"]        = (L*L+3*R*R)/3;                              // sigma=1
        sigmaMSDref2scat["hull"]        = (L*L+9*R*R)/6;                              // sigma=1
        sigmaMSDref2scat["surface"]     = (L*L*L+2*L*L*R+9*L*R*R+6*R*R*R)/(6*(L+R));  // sigma=1

        // sigma <R^2> for all distances between pairs of reference points.
        // These are exactly the Guinier expansions of the corresponding phasefactors

        // betweeen the centre and a random point on the surface
        sigmaMSDref2ref["axispoints"]["axispoints"]  = L*L/2;                                       // sigma=2
        sigmaMSDref2ref["axispoints"]["center"]      = L*L/4;                                       // sigma=1
        sigmaMSDref2ref["axispoints"]["ends"]        = (L*L+R*R)/2;                                 // sigma=1
        sigmaMSDref2ref["axispoints"]["hull"]        = L*L/3+R*R;                                   // sigma=1
        sigmaMSDref2ref["axispoints"]["surface"]     = (2*L*L*L+3*L*L*R+6*L*R*R+3*R*R*R)/(6*(L+R)); // sigma=1 

        sigmaMSDref2ref["center"]["axispoints"]       = (L*L+2*R*R)/4;                              // sigma=1
        sigmaMSDref2ref["center"]["ends"]       = (L*L+2*R*R)/4;                                    // sigma=1
        sigmaMSDref2ref["center"]["hull"]       = L*L/12+R*R;                                       // sigma=1
        sigmaMSDref2ref["center"]["surface"]    = (L*L*L+3*L*L*R+12*L*R*R+6*R*R*R)/(12*(L+R));      // sigma=1 

        sigmaMSDref2ref["ends"]["ends"]         = L*L/2+R*R;                                        // sigma=2
        sigmaMSDref2ref["ends"]["hull"]         = (2*L*L+9*R*R)/6;                                  // sigma=1
        sigmaMSDref2ref["ends"]["surface"]      = (2*L*L*L+3*L*L*R+9*L*R*R+6*R*R*R)/(6*(L+R));      // sigma=1 

        sigmaMSDref2ref["hull"]["hull"]         = L*L/6+2*R*R;                                      // sigma=2
        sigmaMSDref2ref["hull"]["surface"]      = (L*L*L+2*L*L*R+12*L*R*R+9*R*R*R)/(6*(L+R));       // sigma=1
 
        sigmaMSDref2ref["surface"]["surface"]   = (L*L*L+3*L*L*R+12*L*R*R+6*R*R*R)/(6*(L+R));       // sigma=2
    }

};

#endif // INCLUDE_GUARD_SPHERE
