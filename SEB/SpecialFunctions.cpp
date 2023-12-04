/*
    Scattering expressions requires access to various special functions,
    here we extend GiNaCs special function support.
    
    NB. compiling with -DSPECIALFUNCTIONSERIES  replaces these 
    functions with a series expansion which allows Subunit::Validate
    to series automagically expand the scattering expressions to
    validate expressions for radius of gyration, and sigma<R^2> expressions.

*/ 

#include "SpecialFunctions.hpp"
#include <cmath>

using namespace GiNaC;

/*
    Helpers Csc and Sec since mathematica loves to use these.
*/

static ex csc_eval(const ex & x)
{
   return 1/sin(x);
}

static ex sec_eval(const ex & x)
{
   return 1/cos(x);
}

static ex power_eval(const ex & x, const ex& a)
{
   return pow(x,a);
}

REGISTER_FUNCTION(csc, eval_func(csc_eval).latex_name("\\csc"));  
REGISTER_FUNCTION(sec, eval_func(sec_eval).latex_name("\\sec"));  
REGISTER_FUNCTION(power, eval_func(power_eval));  


/*
     Bessel J0, J1, J2 functions.

*/

#ifndef SPECIALFUNCTIONSERIES

static ex BesselJ0_eval(const ex & x)
{
   return BesselJ0(x).hold();
}

static ex BesselJ0_evalf(const ex & x)
{
    if (is_a<numeric>(x))
        return gsl_sf_bessel_J0(ex_to<numeric>(x).to_double() );
    else
        return BesselJ0(x).hold();   
}

static ex BesselJ0_deriv(const ex & x, unsigned diff_param)
{
    return -BesselJ1(x);
}

REGISTER_FUNCTION(BesselJ0, eval_func(BesselJ0_eval).
                            evalf_func(BesselJ0_evalf).
                            derivative_func(BesselJ0_deriv).
                            latex_name("J_0"));   

static ex BesselJ1_eval(const ex & x)
{
   return BesselJ1(x).hold();
}

static ex BesselJ1_evalf(const ex & x)
{
    if (is_a<numeric>(x))
        return gsl_sf_bessel_J1(ex_to<numeric>(x).to_double() );
    else
        return BesselJ1(x).hold();   
}

static ex BesselJ1_deriv(const ex & x, unsigned diff_param)
{
    return (BesselJ0(x)-BesselJ2(x))/2;
}


REGISTER_FUNCTION(BesselJ1, eval_func(BesselJ1_eval).
                       evalf_func(BesselJ1_evalf).
                       derivative_func(BesselJ1_deriv).
                       latex_name("J_1"));

static ex BesselJ2_eval(const ex & x)
{
   return BesselJ2(x).hold();
}

static ex BesselJ2_evalf(const ex & x)
{
    if (is_a<numeric>(x))
        return gsl_sf_bessel_Jn(2, ex_to<numeric>(x).to_double() );
    else
        return BesselJ2(x).hold();   
}

REGISTER_FUNCTION(BesselJ2, eval_func(BesselJ2_eval).
                       evalf_func(BesselJ2_evalf).
                       latex_name("J_2"));

#else   // Series expansions here.

static ex BesselJ0_eval(const ex & x)
{
   return 1-pow(x,2)/4+pow(x,4)/64-pow(x,6)/2304;
}

REGISTER_FUNCTION(BesselJ0, eval_func(BesselJ0_eval).
                            latex_name("J_0"));   

static ex BesselJ1_eval(const ex & x)
{
   return x/2-pow(x,3)/16+pow(x,5)/384;
}


REGISTER_FUNCTION(BesselJ1, eval_func(BesselJ1_eval).
                       latex_name("J_1"));

static ex BesselJ2_eval(const ex & x)
{
  return pow(x,2)/8-pow(x,4)/96+pow(x,6)/3072;
}


REGISTER_FUNCTION(BesselJ2, eval_func(BesselJ2_eval).
                       latex_name("J_2"));

#endif

/*

     Dawson integral:
                        D[x] = exp(-x^2) \int_0 ^x dt exp(t^2)
*/


#ifndef SPECIALFUNCTIONSERIES

static ex DawsonF_eval(const ex & x)
{
   return DawsonF(x).hold();
}

static ex DawsonF_evalf(const ex & x)
{
    if (is_a<numeric>(x))
        return gsl_sf_dawson( ex_to<numeric>(x).to_double() );
    else
        return DawsonF(x).hold();   
}

#else   // Series expansions for validation of sub-unit scattering

static ex DawsonF_eval(const ex & x)
{
   return x-2*pow(x,3)/3 +4*pow(x,5)/15;
}

static ex DawsonF_evalf(const ex & x)
{
   return x-2*pow(x,3)/3 +4*pow(x,5)/15;
}

#endif


REGISTER_FUNCTION(DawsonF, eval_func(DawsonF_eval).
                           evalf_func(DawsonF_evalf).
                           latex_name("DawsonF"));



/*
   Six(x) denotes the Sin integral divided by x:
   
        six(x) = Si(x)/x = x^-1 \int_0 ^x dt sin(t)/t       
               = 1 - x^2/18 + x^4/600 - x^6/35280
                             
*/


#ifndef SPECIALFUNCTIONSERIES

static ex Six_eval(const ex & x)
{
   return Six(x).hold();
}

static ex Six_evalf(const ex & x)
{
    if (is_a<numeric>(x))
     {
        double z=ex_to<numeric>(x).to_double();
        if (z<1e-4) return 1-z*z/18.0;
        return gsl_sf_Si(z)/z;
     }
    else
        return Six(x).hold();
}

static ex Six_deriv(const ex & x, unsigned diff_param)
{
      return sin(x)/(x*x)-Six(x)/x;
}

#else   // Series expansions for validation of sub-unit scattering

static ex Six_eval(const ex & x)
{
   return 1 - pow(x,2)/18 +pow(x,4)/600-pow(x,6)/35280;
}

static ex Six_evalf(const ex & x)
{
   return 1 - pow(x,2)/18 +pow(x,4)/600-pow(x,6)/35280;
}

static ex Six_deriv(const ex & x, unsigned diff_param)
{
   return -x/9+pow(x,3)/150-pow(x,5)/5880;
}

#endif

REGISTER_FUNCTION(Six, eval_func(Six_eval).
                      evalf_func(Six_evalf).
                      derivative_func(Six_deriv).
                      latex_name("Six"));



/*
    Error functions:
    
    Erf(x) = 2/sqrt(pi) \int_0 ^x dt exp(-t^2)
    
    Erfc(x) = 1-erf(x)
*/

static ex Erf_eval(const ex & x)
{
   return Erf(x).hold();
}

static ex Erf_evalf(const ex & x)
{
    if (is_a<numeric>(x))
        return gsl_sf_erf( ex_to<numeric>(x).to_double() );
    else
        return Erf(x).hold();   
}

REGISTER_FUNCTION(Erf, eval_func(Erf_eval).
                      evalf_func(Erf_evalf).
                      latex_name("Erf"));


static ex Erfc_eval(const ex & x)
{
   return Erfc(x).hold();
}

static ex Erfc_evalf(const ex & x)
{
    if (is_a<numeric>(x))
        return gsl_sf_erfc( ex_to<numeric>(x).to_double() );
    else
        return Erfc(x).hold();   
}


REGISTER_FUNCTION(Erfc, eval_func(Erfc_eval).
                        evalf_func(Erfc_evalf).
                        latex_name("Erfc"));



/*
    Hypergeometric functions:

*/

static ex Hypergeometric0F1Regularized_eval(const ex& a, const ex & x)
{
   return Hypergeometric0F1Regularized(a,x).hold();
}

static ex Hypergeometric0F1Regularized_evalf(const ex& a,const ex & x)
{
    if (is_a<numeric>(x) && is_a<numeric>(a))
        return gsl_sf_hyperg_0F1(ex_to<numeric>(a).to_double(), ex_to<numeric>(x).to_double() );
    else
        return Hypergeometric0F1Regularized(a,x).hold();   
}

REGISTER_FUNCTION(Hypergeometric0F1Regularized, eval_func(Hypergeometric0F1Regularized_eval).
                                                evalf_func(Hypergeometric0F1Regularized_evalf).
                                                latex_name("\\_{0}F\\_{1}Regularized"));





// Defined below
void STVH0(double X, double *SH0);
void STVH1(double X, double *SH1);

#ifndef SPECIALFUNCTIONSERIES

static ex StruveH0_eval(const ex & x)
{
   return StruveH0(x).hold();
}

static ex StruveH0_evalf(const ex & x)
{
    if (is_a<numeric>(x))
        {
           double z=ex_to<numeric>(x).to_double();
           double y;
           STVH0(z, &y);
        return y;
        }
    else
        return StruveH0(x).hold();
}

static ex StruveH1_eval(const ex & x)
{
   return StruveH1(x).hold();
}

static ex StruveH1_evalf(const ex & x)
{
    if (is_a<numeric>(x))
        {
           double z=ex_to<numeric>(x).to_double();
           double y;
           STVH1(z, &y);
           return y;
        }
    else
        return StruveH1(x).hold();
}

#else    // Series approximation

static ex StruveH0_eval(const ex & x)
{
   static double P=2.0/3.14159265358979323846264338327950288419716939;
   return P*(x-pow(x,3)/9.0+pow(x,5)/225.0-pow(x,7)/11025.0+pow(x,9)/893025.0);
}

static ex StruveH0_evalf(const ex & x)
{
    if (is_a<numeric>(x))
        {
           static double P=2.0/3.14159265358979323846264338327950288419716939;
           double x=ex_to<numeric>(x).to_double();
           return P*(x-pow(x,3)/9.0+pow(x,5)/225.0-pow(x,7)/11025.0+pow(x,9)/893025.0);
        }
    else
        return StruveH0(x).hold();
}

static ex StruveH1_eval(const ex & x)
{
   static double P=2.0/3.14159265358979323846264338327950288419716939;
   return P*(pow(x,2)/3.0-pow(x,4)/45.0+pow(x,6)/1575.0-pow(x,8)/99225.0);
}

static ex StruveH1_evalf(const ex & x)
{
    if (is_a<numeric>(x))
        {
           double x=ex_to<numeric>(x).to_double();
           static double P=2.0/3.14159265358979323846264338327950288419716939;
           return P*(pow(x,2)/3.0-pow(x,4)/45.0+pow(x,6)/1575.0-pow(x,8)/99225.0);
        }
    else
        return StruveH1(x).hold();
}


#endif


REGISTER_FUNCTION(StruveH0, eval_func(StruveH0_eval).
                        evalf_func(StruveH0_evalf).
                        latex_name("StruveH0"));

REGISTER_FUNCTION(StruveH1, eval_func(StruveH1_eval).
                           evalf_func(StruveH1_evalf).
                          latex_name("StruveH1"));



/**************************************************************
!*       Purpose: This program computes Struve function       * 
!*                H0(x) using subroutine STVH0                *
!*       Input :  x   --- Argument of H0(x) ( x ò 0 )         *
!*       Output:  SH0 --- H0(x)                               *
!*       Example:                                             *
!*                   x          H0(x)                         *
!*                ----------------------                      *
!*                  0.0       .00000000                       * 
!*                  5.0      -.18521682                       *
!*                 10.0       .11874368                       *
!*                 15.0       .24772383                       *
!*                 20.0       .09439370                       *
!*                 25.0      -.10182519                       *
!* ---------------------------------------------------------- *
!* REFERENCE: "Fortran Routines for Computation of Special    *
!*             Functions, jin.ece.uiuc.edu/routines/routines  *
!*             .html".                                        *
!*                                                            *
!*                          C++ Release By J-P Moreau, Paris. *
!*                                  (www.jpmoreau.fr)         *
!*************************************************************/ 



void STVH0(double X, double *SH0) {
/*      =============================================
!       Purpose: Compute Struve function H0(x)
!       Input :  x   --- Argument of H0(x) ( x ò 0 )
!       Output:  SH0 --- H0(x)
!       ============================================= */
        double A0,BY0,P0,PI,Q0,R,S,T,T2,TA0;
	int K, KM;
	    
	PI=3.14159265358979323846264338327950288419716939;
        S=1.0;
        R=1.0;
        if (X <= 20.0) {
           A0=2.0*X/PI;
           for (K=1; K<61; K++) {
              R=-R*X/(2.0*K+1.0)*X/(2.0*K+1.0);
              S=S+R;
              if (fabs(R) < fabs(S)*1.0e-12) goto e15;
           }
e15:       *SH0=A0*S;
        }
        else {
           KM=int(0.5*(X+1.0));
           if (X >= 50.0) KM=25;
           for (K=1; K<=KM; K++) {
              R=-R*pow((2.0*K-1.0)/X,2.0);
              S=S+R;
              if (fabs(R) < fabs(S)*1.0e-12) goto e25;
           }
e25:       T=4.0/X;
           T2=T*T;
           P0=((((-.37043e-5*T2+.173565e-4)*T2-.487613e-4)*T2+.17343e-3)*T2-0.1753062e-2)*T2+.3989422793;
           Q0=T*(((((.32312e-5*T2-0.142078e-4)*T2+0.342468e-4)*T2-0.869791e-4)*T2+0.4564324e-3)*T2-0.0124669441);
           TA0=X-0.25*PI;
           BY0=2.0/sqrt(X)*(P0*sin(TA0)+Q0*cos(TA0));
           *SH0=2.0/(PI*X)*S+BY0;
        }
}


/**************************************************************
!*       Purpose: This program computes Struve function       *
!*                H1(x) using subroutine STVH1                *
!*       Input :  x   --- Argument of H1(x) ( x ò 0 )         *
!*       Output:  SH1 --- H1(x)                               *
!*       Example:                                             *
!*                   x          H1(x)                         *
!*                -----------------------                     *
!*                  0.0       .00000000                       *
!*                  5.0       .80781195                       *
!*                 10.0       .89183249                       *
!*                 15.0       .66048730                       *
!*                 20.0       .47268818                       *
!*                 25.0       .53880362                       *
!* ---------------------------------------------------------- *
!* REFERENCE: "Fortran Routines for Computation of Special    *
!*             Functions, jin.ece.uiuc.edu/routines/routines  *
!*             .html".                                        *
!*                                                            *
!*                          C++ Release By J-P Moreau, Paris. *
!*                                  (www.jpmoreau.fr)         *
!*************************************************************/ 



void STVH1(double X, double *SH1) {
/*      =============================================
!       Purpose: Compute Struve function H1(x)
!       Input :  x   --- Argument of H1(x) ( x ò 0 )
!       Output:  SH1 --- H1(x)
!       ============================================= */
        double A0,BY1,P1,PI,Q1,R,S,T,T2,TA1;
	int K, KM;

 	PI=3.14159265358979323846264338327950288419716939;
        R=1.0;
        if (X <= 20.0) {
           S=0.0;
           A0=-2.0/PI;
           for (K=1; K<=60; K++) {
              R=-R*X*X/(4.0*K*K-1.0);
              S=S+R;
              if (fabs(R) < fabs(S)*1.0e-12) goto e15;
           }
e15:       *SH1=A0*S;
        }
        else {
           S=1.0;
           KM=int(0.5*X);
           if (X > 50.0)  KM=25;
           for (K=1; K<=KM; K++) {
              R=-R*(4.0*K*K-1.0)/(X*X);
              S=S+R;
              if (fabs(R) < fabs(S)*1.0e-12) goto e25;
           }
e25:       T=4.0/X;
           T2=T*T;
           P1=((((0.42414e-5*T2-0.20092e-4)*T2+0.580759e-4)*T2-0.223203e-3)*T2+0.29218256e-2)*T2+0.3989422819;
           Q1=T*(((((-0.36594e-5*T2+0.1622e-4)*T2-0.398708e-4)*T2+0.1064741e-3)*T2-0.63904e-3)*T2+0.0374008364);
           TA1=X-0.75*PI;
           BY1=2.0/sqrt(X)*(P1*sin(TA1)+Q1*cos(TA1));
           *SH1=2.0/PI*(1.0+S/(X*X))+BY1;
        }
}




