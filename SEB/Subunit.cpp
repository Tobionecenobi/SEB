#include "Subunit.hpp"



ex SubUnit::FormFactor(ParameterList&  betas, ParameterList&  params, int varForm )
{
  try{
    ex beta=getBeta();
    if      (varForm == GENERIC)   {
                                      ex F=GLEX->getSymbol("F", tag);
                                      betas[beta]=0;
                                      params[F]=0;
                                      
                                      return beta*beta*F;
                                    }
    else if (varForm == XVAR)       {
                                      betas[beta]=0;
                                      for (auto& p:xparameters) params[p]=0;
                                      return beta*beta*FormFactorExpression;
                                    }
    else if (varForm == QVAR)       {
                                      betas[beta]=0;
                                      for (auto& p:parameters) params[p]=0;
                                      return beta*beta*FormFactorExpression.subs( expand );
                                    }
    else if (varForm == BETA)       {
                                      betas[beta]=0;
                                      return beta*beta;
                                    }
    else if (varForm == ONE)        return ex(1);
    else if (varForm == GUINIER)    
                                    {
                                      betas[beta]=0;
                                      for (auto& p:parameters) params[p]=0;

                                      symbol q=GLEX->getSymbol("q");
                                      return beta*beta*(1-q*q*2*RadiusOfGyration2/6);
                                    }
    else
                                    throw SEBException("Wrong varform constant");
    }
    catch (SEBException& e)
     {
       e.PushCallStack("ex Subunit:FormFactor(..)" );  throw;
     }
}

/* Returns the form factor amplitude of sub-unit relative to a reference point r

   CASES for r:
   If the reference point is specific e.g. polymer end1, return the corressponding expression for end1.
   If the reference point is distributed e.g. polymer contour, return the corresponding expression for contour.
   If the reference point is a randomly chosen from a distribution e.g. polymer contour#mypoint, remove mypoint, and return the corresponding expression for contour.
*/
ex SubUnit::FormFactorAmplitude(refPoint r, ParameterList&  betas, ParameterList&  params, int varForm)
{
  try{

    if (!testReference(r)) throw SEBException("Invalid reference point ");

    if (hasAHash(r)) r = removeHashString(r);
    
    if ( (varForm == XVAR || varForm == QVAR)  && FormFactorAmplitudeExpressions.find(r)==FormFactorAmplitudeExpressions.end()) 
                             throw SEBException("Invalid reference point not found in FormFactorAmplitudesExpressions table");

    if (varForm == GUINIER)
         if (sigmaMSDref2scat.find(r)==sigmaMSDref2scat.end()) 
                             throw SEBException("Invalid reference point not found in sigmaMSDref2scat table");


    ex beta=getBeta();
    if      (varForm == GENERIC)   {
                                       ex A=GLEX->getSymbol("A", tag, r);
                                       betas[beta]=0;
                                       params[A]=0;
                                       
                                       return beta*A;
                                    }
    else if (varForm == XVAR)       {        
                                       betas[beta]=0;
                                       for (auto& p:xparameters) params[p]=0;
                                       return beta*FormFactorAmplitudeExpressions[r];
                                    }
    else if (varForm == QVAR)       
                                    {
                                       betas[beta]=0;
                                       for (auto& p:parameters) params[p]=0;
                                       return beta*FormFactorAmplitudeExpressions[r].subs( expand );
                                    }
    else if (varForm == BETA)       {
                                       betas[beta]=0;
                                       return beta;
                                    }
    else if (varForm == ONE)           return ex(1);
    else if (varForm == GUINIER)    
                                    {
                                       betas[beta]=0;
                                       symbol q=GLEX->getSymbol("q");
                                       for (auto& p:parameters) params[p]=0;

                                       return beta*(1-q*q*getSigmaMSDRef2Scat(r)/6);
                                    }
    else                            throw SEBException("Wrong varform constant)");
    }
    catch (SEBException& e)
     {
       e.PushCallStack("ex Subunit:FormFactorAmplitude(refPoint "+r+", ..)" );  throw;
     }
}


    /* Returns the phase factor of a sub unit between two reference points

   CASES:
     both r1 and r2 can be specific (e.g. end1), distributed (contour), or randomly chosen on distributed (contour#mypoint)
     and r1 and r2 can be the same or different.
     
     Specific r1 vs. all options for r2  (same, specific, distributed, random):
       r1= poly.end1    r2=poly.end1          return 1
                        r2=poly.end2          return Psi_poly,end1,end2
                        r2=poly.contour       return Psi_poly,contour,end1
                        r2=poly.contour#mypkt return Psi_poly,contour,end1

     Distributed r1 vs. all options for r2 (same, specific, another distributed, random)
       r1= poly.contour r2=poly.contour         return Psi_poly,contour,contour
                        r2=poly.end2            return Psi_poly,contour,end2
                        r2=poly.contour2        return Psi_poly,contour,contour2
                        r2=poly.contour#mypkt   return Psi_poly,contour,contour

     Randomly chosen point on distributed r1 vs. all options for r2 (same, specific, distributed, another random)
       r1= poly.contour#mypkt   r2=poly.contour#mypkt    return 1
                                r2=poly.end2             return Psi_poly,contour,end2
                                r2=poly.contour          return Psi_poly,contour,contour
                                r2=poly.contour#mypkt2   return Psi_poly,contour,contour


     Boiling this down if the both reference points are specific (geometric / random) and identical then return 1,
     else return the phase factor with #.. removed.

*/
ex SubUnit::PhaseFactor(refPoint r1, refPoint r2, ParameterList&  betas, ParameterList&  params, int varForm )
{
try{    
    if (!testReference(r1)) throw SEBException("Wrong starting reference point");
    if (!testReference(r2)) throw SEBException("Wrong ending reference point");
    // Reference points are now garentied to be known!

    if (r1==r2 && hasSpecificReference(r1) && hasSpecificReference(r2)) return ex(1);

    //For removing the # and the name of the randomly chosen reference point
    if (hasAHash(r1)) r1 = removeHashString(r1);
    if (hasAHash(r2)) r2 = removeHashString(r2);

    if(r2 < r1) swap (r1, r2);   // The arrays stores expressions with alphabetically sorted reference points.

    if (varForm == XVAR || varForm == QVAR)
      {
         if (PhaseFactorExpressions.find(r1)==PhaseFactorExpressions.end()) 
                             throw SEBException("Invalid reference point "+r1+" not found in PhaseFactorExpressions table");

         if (PhaseFactorExpressions[r1].find(r2)==PhaseFactorExpressions[r1].end()) 
                             throw SEBException("Invalid reference point "+r2+" not found in PhaseFactors table");
                             
      }

    if (varForm == GUINIER)
      {
         if (sigmaMSDref2ref.find(r1)==sigmaMSDref2ref.end()) 
                             throw SEBException("Invalid reference point "+r1+" not found in sigmaMSDref2ref table");

         if (sigmaMSDref2ref[r1].find(r2)==sigmaMSDref2ref[r1].end()) 
                             throw SEBException("Invalid reference point "+r2+" not found in sigmaMSDref2ref table");   
      }


    if      (varForm == GENERIC)   {
                                       ex psi=GLEX->getSymbol("Psi", tag, r1, r2);
                                       params[psi]=0;
                                       return psi;
                                    }
    else if (varForm == XVAR)       {
                                       for (auto& p:xparameters) params[p]=0;
                                       return PhaseFactorExpressions[r1][r2];
                                    }
    else if( varForm == QVAR )      {
                                       for (auto& p:parameters) params[p]=0;
                                       return PhaseFactorExpressions[r1][r2].subs(expand);
                                    }
    else if (varForm == BETA)       return ex(1);
    else if (varForm == ONE)        return ex(1);
    else if (varForm == GUINIER)    
                                   {
                                       for (auto& p:parameters) params[p]=0;
                                   
                                       symbol q=GLEX->getSymbol("q");
                                       return ex(1)-q*q*getSigmaMSDRef2Ref(r1,r2)/6;
                                   }
    else
                                    throw SEBException("Unknown varform constant");
    }
    catch (SEBException& e)
     {
       e.PushCallStack("SubUnit::PhaseFactor("+r1+","+r2+",..)");  throw;
     }
}



// In rare case we do not want to track which parameters the expressions depend on:
ex SubUnit::FormFactor( int varForm)                            {  ParameterList pl; return FormFactor(pl, pl, varForm); }
ex SubUnit::FormFactorAmplitude(refPoint r, int varForm)        {  ParameterList pl; return FormFactorAmplitude(r, pl, pl, varForm); }
ex SubUnit::PhaseFactor(refPoint r1, refPoint r2, int varForm)  {  ParameterList pl; return PhaseFactor(r1,r2, pl, pl, varForm); }


/*Returns mean square square distances from r to all scatterers in the sub-unit */
ex  SubUnit::getSigmaMSDRef2Scat(refPoint r)
 {
    if(hasAHash(r)) r = removeHashString(r);
    if (sigmaMSDref2scat.find(r)==sigmaMSDref2scat.end()) 
                throw SEBException("Invalid reference point "+r+" not found in sigmaMSDref2scat table", "SubUnit::getSigmaMSDRef2Scat()");

    return sigmaMSDref2scat[r];
}

/* Returns the mean square distance between pairs of reference points */    
ex  SubUnit::getSigmaMSDRef2Ref(refPoint r1, refPoint r2 )
  {

    if (!testReference(r1)) throw SEBException("Wrong reference point:"+r1, "SubUnit::getSigmaMSDRef2Ref()");
    if (!testReference(r2)) throw SEBException("Wrong reference point:"+r2, "SubUnit::getSigmaMSDRef2Ref()");

    if(r2 < r1) swap (r1, r2);
    if (hasSpecificReference(r1) && hasSpecificReference(r2) && r1==r2) return ex(1);

    if(r1.find(".") != string::npos)  throw SEBException("Wrong input in getRefToRefMSD()", "getSigmaMSDRef2Ref()");

    if(hasAHash(r1)) r1 = removeHashString(r1);
    if(hasAHash(r2)) r2 = removeHashString(r2);

    if (sigmaMSDref2ref.find(r1)==sigmaMSDref2ref.end()) 
                             throw SEBException("Invalid reference point "+r1+" not found in PhaseFactors table", "SubUnit::getSigmaMSDRef2Ref()");

    if (sigmaMSDref2ref[r1].find(r2)==sigmaMSDref2ref[r1].end()) 
                             throw SEBException("Invalid reference point "+r2+" not found in PhaseFactors table", "SubUnit::getSigmaMSDRef2Ref()");                                 

    return  sigmaMSDref2ref[r1][r2];
}


// Tests if all the required terms and cross-terms are defined.
void SubUnit::ValidateDefinedTerms( )
{
   bool isOK=true;

   ReferencePointSet refs;       
   for (auto const& r : refsSpecific)
      {
         if (refs.find(r)!=refs.end()) { isOK=false; cout << "WARNING: reference point " << r << " was defined twice\n"; }
         refs.insert(r);
      }
   for (auto const& r : refsDistributed)
      {
         if (refs.find(r)!=refs.end())  { isOK=false; cout << "WARNING: reference point " << r << " was defined twice\n"; }
         refs.insert(r);
      }
   
   // Validate all Form factor amplitude expressions and the corresponding sigmaMSDref2scat expressions.
   for (auto const& r : refs)
      {
           if (FormFactorAmplitudeExpressions.find(r) == FormFactorAmplitudeExpressions.end()) 
                  { isOK=false; cout << "WARNING: FormFactorAmplitude Expression missing for ref=" << r << "\n"; }
           if (sigmaMSDref2scat.find(r) == sigmaMSDref2scat.end()) 
                   { isOK=false; cout << "WARNING: sigmaMSDref2scat missing for ref=" << r << "\n"; }
      }
      
      
   // Validate all phase factor expressions and the corresponding sigmaMSDref2ref expressions.
   for (auto const& r1 : refs)
     for (auto const& r2 : refs)
      {
           if (r2<r1) continue;   // We only store alfabetically sorted, so eventually we will test the other version.
           if (r1==r2 && hasSpecificReference(r1))   // r1=r2 for specific references are = 1 by definition so these should not be stored
                {
                   if (   PhaseFactorExpressions.find(r1) != PhaseFactorExpressions.end()
                      &&  PhaseFactorExpressions[r1].find(r2)!=PhaseFactorExpressions[r1].end())
                           cout << "Warning: PhaseFactor for spefic reference points r1=r2=" << r1 << " is defined, but is not used since its unity for spby definition\n";
                   continue;
                }
           
           if (PhaseFactorExpressions.find(r1)==PhaseFactorExpressions.end()) 
                              { isOK=false; cout << "WARNING: PhaseFactor expression missing for r1=" << r1 << endl; }
           if (PhaseFactorExpressions[r1].find(r2)==PhaseFactorExpressions[r1].end()) 
                              { isOK=false; cout << "WARNING: PhaseFactor expression missing for r1=" << r1 << "  r2=" << r2 << endl; }
  
           if (sigmaMSDref2ref.find(r1)==sigmaMSDref2ref.end()) 
                              { isOK=false; cout << "WARNING: sigmaMSDref2ref expression missing for r1=" << r1 << endl; }
  
           if (sigmaMSDref2ref[r1].find(r2)==sigmaMSDref2ref[r1].end()) 
                              { isOK=false; cout << "WARNING: sigmaMSDref2ref expression missing for r1=" << r1 << "  r2=" << r2 << endl; }
                               }
      
     if (!isOK) cout << "WARNING some scattering terms were missing. Please fix those!\n\n!";
           else cout << "OK! All required terms are present\n";
}

// Helper for Symbolically testing scattering expression. Fails if GiNaC can not simplify the expression in which case -DSPECIALFUNCTIONSERIES
// can help, as it replaces the special functions by series expansions that are valid in the Guinier regime. It also fails if the
// scattering expressions involves numerical integrals.
bool SubUnit::ValidateFunctionSymbolically(ex& F, ex& SMSD, bool FormFactor, ex Fsymb, ParameterList pl)
{
   cout << Fsymb << "=" << F.subs(expand) << endl;
   
   symbol q = GLEX->getSymbol("q");
   ex Fe = series_to_poly(F.subs(expand).series( q==0, 3));

   ex c0 =    Fe.coeff(q,0);   // Should be 1
   ex c1 =    Fe.coeff(q,1);   // Should be 0
   ex c2 = -6*Fe.coeff(q,2);   // should be sigma<R^2>  for form factor 2<Rg^2>
   if (FormFactor) c2/=2;

   bool isOK=true;       
   if (!is_a<numeric>(c0) || (is_a<numeric>(c0) && fabs(ex_to<numeric>(c0).to_double()-1)>1e-5  )) 
      {
         cout << "WARNING NORMALIZATION Expected " << c0 << "==1" << endl;
         isOK=false;
      }

   if (!is_a<numeric>(c1) || (is_a<numeric>(c1) && fabs(ex_to<numeric>(c1).to_double())>1e-5  )) 
      {
         cout << "WARNING LINEAR Expected " << c1 << "==0" << endl;
         isOK=false;
      }

   ex diff= SMSD.expand()-c2.expand();

   if (!is_zero(diff.expand())) 
      {
         if (pl.empty())
            cout << "WARNING SIZE Expected " << c2 << "==" << SMSD << endl;
          else
            cout << "WARNING SIZE GINAC comparison of sizes fails. (This could be false positive, so testing using supplied hint)" << endl;
          
         isOK=false;
         
         // If the user has supplied some parameter values we can test if their difference is zero.
         
         if (!pl.empty() )
            {
                 ex diffnumeric=diff.subs(pl);
                 if (is_a<numeric>(diffnumeric))
                     {
                         double d = ex_to<numeric>(diffnumeric).to_double();
                         if (d<1e-5) cout << "OK for numerical hint supplied!" << endl;
                                else cout << "WARNING fails for numerical hint supplied!" << endl;
                     } else cout << "Fails to evaluate to number for numerical hint supplied!" << endl;          
               
            }             
      }

   if (isOK) cout << "OK for Guinier tests!" << endl;
        else cout << endl;
   return isOK;
}


// Guiner expands all scattering terms and compares to the Rg2 or sigma<R^2> terms.
void SubUnit::ValidateSymbolically(ParameterList pl )
{
   bool isOK=true;

   // Valdate Form factor normalization and Rg^2 expression.
   isOK = isOK && ValidateFunctionSymbolically( FormFactorExpression, RadiusOfGyration2, true, GLEX->getSymbol("F", tag) , pl);

   ReferencePointSet refs;       
   for (auto const& r : refsSpecific)
      {
         if (refs.find(r)!=refs.end()) { isOK=false; cout << "WARNING: reference point " << r << " was defined twice\n"; }
         refs.insert(r);
      }
   for (auto const& r : refsDistributed)
      {
         if (refs.find(r)!=refs.end())  { isOK=false; cout << "WARNING: reference point " << r << " was defined twice\n"; }
         refs.insert(r);
      }
   
   // Validate all Form factor amplitude expressions and the corresponding sigmaMSDref2scat expressions.
   for (auto const& r : refs)
      {
           isOK = ValidateFunctionSymbolically( FormFactorAmplitudeExpressions[r], sigmaMSDref2scat[r], false, GLEX->getSymbol("A", tag, r) ,pl) && isOK;
      }
      
   // Validate all phase factor expressions and the corresponding sigmaMSDref2ref expressions.
   for (auto const& r1 : refs)
     for (auto const& r2 : refs)
      {
           if (r2<r1) continue;   // We only store alfabetically sorted, so eventually we will test the other version.
           if (r1==r2 && hasSpecificReference(r1))   // r1=r2 for specific references are = 1 by definition so these should not be stored
                   continue;
                     
           isOK = ValidateFunctionSymbolically( PhaseFactorExpressions[r1][r2], sigmaMSDref2ref[r1][r2], false, GLEX->getSymbol("Psi", tag, r1,r2) , pl)  && isOK;
      }
      
     if (!isOK) cout << "WARNING issues were found, please manually check the code is correct\n\n!";
}



void SubUnit::ValidateGraph(ex Fex, ex sigmaR2ex, bool FormFactor, vector<double>& qvec, string filename)
 {
    symbol q = GLEX->getSymbol("q");

    if (!is_a<numeric>(sigmaR2ex) || !is_a<numeric>(Fex.subs(q==0.1).evalf())) 
        {
           cout << "Fails to evaluate to number for numerical hint supplied!" << filename << endl;          
           return;
        }

    double sigmaR2=ex_to<numeric>(sigmaR2ex).to_double();

    string f=filename+".q";
    ofstream fo(f.c_str());
    for (int i=0;i<qvec.size(); i++)
       fo << qvec[i] << " " << Fex.subs(q==qvec[i]).evalf() << "\n";        
    fo.close();
    
    f=filename+"_guinierdiff.q";
    fo.open(f.c_str());
    
    double maxerror=0;
    for (int i=0;i<qvec.size(); i++)
       {
           double q2=qvec[i]*qvec[i];
           double Fval= ex_to<numeric>(Fex.subs(q==qvec[i]).evalf()).to_double();
           double Fg;
           if (FormFactor) Fg= q2*sigmaR2/3;
                      else Fg= q2*sigmaR2/6;

           if (Fg>1e-2) break;  // Don't subtract Guinier if second order term deviates by more than 1e-2.
           Fg-=1;

           fo << qvec[i] << " " << Fval+Fg << "\n"; 
           if (fabs(Fval+Fg)>maxerror) maxerror=fabs(Fval+Fg);
       }
    fo.close();

    if (maxerror>1e-4) cout << "WARNING GUINIER deviation is " << maxerror << ">1e-4\n";
                  else cout << "GUINIER tests OK for " << filename << "\n";
 }

// Saves a graph of all scattering expressions for the given parameters and qvec.
// The graphs can be compared to output from sampling or e.g. mathematica derived expressions.
void SubUnit::ValidateGraphically(ParameterList pl, vector<double> qvec, string base )
{
   bool isOK=true;
   ValidateGraph(FormFactorExpression.subs(expand).subs(pl), RadiusOfGyration2.subs(pl).evalf(), true, qvec, base+string("_FormFactor_"));

   ReferencePointSet refs;       
   for (auto const& r : refsSpecific)
      {
         if (refs.find(r)!=refs.end()) { isOK=false; cout << "WARNING: reference point " << r << " was defined twice\n"; }
         refs.insert(r);
      }
   for (auto const& r : refsDistributed)
      {
         if (refs.find(r)!=refs.end())  { isOK=false; cout << "WARNING: reference point " << r << " was defined twice\n"; }
         refs.insert(r);
      }
   
   // Validate all Form factor amplitude expressions and the corresponding sigmaMSDref2scat expressions.
   for (auto const& r : refs)
      {
           ValidateGraph(FormFactorAmplitudeExpressions[r].subs(expand).subs(pl), sigmaMSDref2scat[r].subs(pl).evalf(), false, qvec, base+string("_FormFactorAmplitude_")+r+"_");               
      }
      
   // Validate all phase factor expressions and the corresponding sigmaMSDref2ref expressions.
   for (auto const& r1 : refs)
     for (auto const& r2 : refs)
      {
           if (r2<r1) continue;   // We only store alfabetically sorted, so eventually we will test the other version.
           if (r1==r2 && hasSpecificReference(r1))   // r1=r2 for specific references are = 1 by definition so these should not be stored
                   continue;

           ValidateGraph(PhaseFactorExpressions[r1][r2].subs(expand).subs(pl), sigmaMSDref2ref[r1][r2].subs(pl).evalf(), false, qvec, base+string("PhaseFactor_")+r1+"_"+r2+"_");
      }
      
      if (!isOK) cout << "WARNING issues were found, please manually check the code is correct\n\n!";
}



bool SubUnit::ValidateExpressionFile( ex term, ex SRMS2ex, string filename, bool FF, string str, double tolerance)
 {
    int count=0;
    int countg=0;
    double dev=0.0;
    double devg=0.0;
    
    symbol q = GLEX->getSymbol("q");
    if (!is_a<numeric>(SRMS2ex)) throw SEBException("Expression for sigma<R^2> did not evaluate to number","bool ValidateExpressionFile( ex term, ex SRMS, string filename, bool FF, double tolerance)");
    double SRMS2=ex_to<numeric>( SRMS2ex ).to_double();
    if (FF) SRMS2/=3;
       else SRMS2/=6;

    ifstream fi(filename);
    if (!fi.is_open()) throw SEBException("Can not open input file "+filename,"bool ValidateExpressionFile( ex term, ex SRMS, string filename, bool FF, double tolerance)");
 
    double qval,Ival;
    while (fi >> qval >> Ival)
       {
           ex Iex = term.subs(q==qval).evalf();
           if (!is_a<numeric>(Iex)) throw SEBException("Expression for scattering term did not evaluate to number","bool ValidateExpressionFile( ex term, ex SRMS, string filename, bool FF, double tolerance)");
           double I=ex_to<numeric>( Iex).to_double();

           // Test max deviation between data in file and SEB calculation
           if (fabs(I-Ival) > dev) dev=fabs(I-Ival);

           // Test Guinier expansion using only data points where the 2nd order terms deviate by less than 0.01 from the leading term.
           if (qval*qval*SRMS2<0.01)
                 {
                    I=1-qval*qval*SRMS2;
                    if (fabs(I-Ival) > devg) devg=fabs(I-Ival);
                    countg++;
                 }
                          
           count++;
       }
 
     if (dev<tolerance)   cout << "OK validated " << str << " against " << filename << "   max|dev|=" << dev << "  for " << count << " data points" << endl;      
                     else cout << "WARNING validation " << str << " against "<< filename << "   max|dev|=" << dev << "  for " << count << " data points" << endl;

     if (devg<tolerance)   cout << "OK GUINIER validated against "      << filename << "   max|dev|=" << devg << "  for " << countg << " data points tested where  Guinier expansion applies" << endl;      
                      else cout << "WARNING GUINIER validation against "<< filename << "   max|dev|=" << devg << "  for " << countg << " data points tested where Guinier expansion applies" << endl;
     
     return dev<tolerance && devg<tolerance;
 }


