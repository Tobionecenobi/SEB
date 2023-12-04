/*
   ginac_extension  introduces various special functions which are required to
   evaluate scattering expressions numerically. We use GNU Scientific LIbrary's
   implementation for these to extend Ginac.

   https://www.gnu.org/software/gsl/doc/html/specfunc.html
*/

//===========================================================================
// Included guards
#ifndef INCLUDE_GINAC_EXTENSION_SPECIALFUNCTIONS
#define INCLUDE_GINAC_EXTENSION_SPECIALFUNCTIONS

#include <ginac/ginac.h>

#include <gsl/gsl_sf_bessel.h>          
#include <gsl/gsl_sf_dawson.h>          
#include <gsl/gsl_sf_expint.h>          
#include <gsl/gsl_sf_erf.h>             
#include <gsl/gsl_sf_hyperg.h>

// These functions simplifying cut'n'pasting from Matematica into GiNaC.
DECLARE_FUNCTION_1P(csc)                // csc(x) = 1/sin(x)
DECLARE_FUNCTION_1P(sec)                // sec(x) = 1/cos(x)
DECLARE_FUNCTION_2P(power)              // power(x) = pow(x)


DECLARE_FUNCTION_1P(BesselJ0)           // Bessel functions  BesselJ0, BesselJ1, BesselJ2(x)
DECLARE_FUNCTION_1P(BesselJ1)
DECLARE_FUNCTION_1P(BesselJ2)
DECLARE_FUNCTION_1P(DawsonF)            // Dawson integral   DawsonF(x)
DECLARE_FUNCTION_1P(Si)                 // Sin integral      Si(x)          not used
DECLARE_FUNCTION_1P(Six)                // Sin integral/x    Si(x)/x
DECLARE_FUNCTION_1P(Erf)                // Error functions   Erf(x)
DECLARE_FUNCTION_1P(Erfc)               //                   Erfc(x)=1-Erf(x)

DECLARE_FUNCTION_2P(Hypergeometric0F1Regularized)     //  0F1 hypergeometric function   https://reference.wolfram.com/language/ref/Hypergeometric0F1Regularized.html


// http://jean-pierre.moreau.pagesperso-orange.fr/c_function2.html :

DECLARE_FUNCTION_1P(StruveH0)           // StruveH0 function
DECLARE_FUNCTION_1P(StruveH1)           // StruveH1 function

#endif
