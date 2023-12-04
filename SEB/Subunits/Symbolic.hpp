//===========================================================================
// Included guards
#ifndef INCLUDE_GUARD_GENERIC
#define INCLUDE_GUARD_GENERIC

/*===========================================================================

   This file implements a GENERIC sub-unit. The user must define its reference
   points, before it is passed to world.
   

============================================================================= */


class SymbolicSubunit : public SubUnit{

    public:
        SymbolicSubunit() : SubUnit(){    
            type = SUBUNITCHILD;
           stype = SYMBOLICSUBUNIT;
        }
        
    virtual ~SymbolicSubunit(){}

    virtual void Init(string name, string tag, SymbolInterface *GEX)
    {
        // The user should have defined some reference points before passing the sub-unit to world.
        SubUnit::Init(name, tag, GEX);
        string n = getTag();
    }

    /*returns the form factor, ginac tutorial: 5.3 Substituting expressions*/
    virtual ex FormFactor(ParameterList&  betas, ParameterList&  params, int varForm = GENERIC )
    {
      try{
        ex beta=getBeta();
        if (varForm == BETA)            {
                                          betas[beta]=0;
                                          return beta*beta;
                                        }
        else if (varForm == ONE)        return ex(1);
        else if (varForm == GUINIER)    
                                        {
                                         ex beta=GLEX->getSymbol("beta", tag);
                                         ex q=GLEX->getSymbol("q");
                                         ex Rg2=GLEX->getSymbol("Rg2", tag);

                                         betas[beta]=0;
                                         params[Rg2]=0;
                                      
                                         return beta*beta*( 1-q*q*Rg2/3 );
                                        }
        else
                                        {
                                          ex F=GLEX->getSymbol("F", tag);
                                          betas[beta]=0;
                                          params[F]=0;
                                          
                                          return beta*beta*F;
                                        }
        }
        catch (SEBException& e)
         {
           e.PushCallStack("ex GENERIC::FormFactor(..)" );  throw;
         }
    }

    /* Returns the form factor amplitude of sub-unit relative to a reference point r
    
       CASES for r:
       If the reference point is specific e.g. polymer end1, return the corressponding expression for end1.
       If the reference point is distributed e.g. polymer contour, return the corresponding expression for contour.
       If the reference point is a randomly chosen from a distribution e.g. polymer contour#mypoint, remove mypoint, and return the corresponding expression for contour.
    */
    virtual ex FormFactorAmplitude(refPoint r, ParameterList&  betas, ParameterList&  params, int varForm = GENERIC )
{
  try{

        if (!testReference(r)) throw SEBException("Invalid reference point ");
        if (hasAHash(r)) r = removeHashString(r);

        ex beta=getBeta();
 
        if (varForm == BETA)       {
                                           betas[beta]=0;
                                           return beta;
                                        }
        else if (varForm == ONE)           return ex(1);
        else if (varForm == GUINIER)    
                                        {
                                           ex beta=GLEX->getSymbol("beta", tag);
                                           ex q=GLEX->getSymbol("q");
                                           ex R2=GLEX->getSymbol("sigmaRrs2", tag, r);
     
                                           params[R2]=0;
                                           betas[beta]=0;
     
                                          return beta*(1-q*q*R2/6);
                                        }
        else                            {
                                           ex A=GLEX->getSymbol("A", tag, r);
                                           betas[beta]=0;
                                           params[A]=0;
                                           
                                           return beta*A;
                                        }
        }
        catch (SEBException& e)
         {
           e.PushCallStack("ex GENERIC:FormFactorAmplitude(refPoint "+r+", ..)" );  throw;
         }
    }

    virtual ex PhaseFactor(refPoint r1, refPoint r2, ParameterList&  betas, ParameterList&  params, int varForm = GENERIC )
    {
try{    
        if (!testReference(r1)) throw SEBException("Wrong starting reference point");
        if (!testReference(r2)) throw SEBException("Wrong ending reference point");
        // Reference points are now garentied to be known!

        if (hasSpecificReference(r1) && hasSpecificReference(r2) && r1==r2) return ex(1);

        //For removing the # and the name of the randomly chosen reference point
        if (hasAHash(r1)) r1 = removeHashString(r1);
        if (hasAHash(r2)) r2 = removeHashString(r2);

        if (varForm == BETA)            return ex(1);
        else if (varForm == ONE)        return ex(1);
        else if (varForm == GUINIER)    
                                       {
                                           for (auto& p:parameters) params[p]=0;
                                       
                                           ex q=GLEX->getSymbol("q");
                                           ex R2=GLEX->getSymbol("sigmaRrr2", tag, r1, r2);
                                           params[R2]=0;
                                           return ex(1)-q*q*R2/6;
                                       }
        else
                                       {
                                           ex psi=GLEX->getSymbol("Psi", tag, r1, r2);
                                           params[psi]=0;
                                           return psi;
                                       }
        }
        catch (SEBException& e)
         {
           e.PushCallStack("GENERIC::PhaseFactor("+r1+","+r2+",..)");  throw;
         }
    }

    /* Returns the radius of gyration of a sub unit. This always uses explicit structural parameters. */
    ex virtual getRadiusOfGyration2() { return GLEX->getSymbol("Rg2", tag); }

    /*Returns mean square square distances from r to all scatterers in the sub-unit */
    ex  virtual getSigmaMSDRef2Scat(refPoint r)
     {
        if (!testReference(r)) throw SEBException("Wrong reference point:"+r, "GENERIC::getSigmaMSDRef2Ref()");
        if(hasAHash(r)) r = removeHashString(r);

        return GLEX->getSymbol("sigmaRrs2", tag, r);
    }

    /* Returns the mean square distance between pairs of reference points */
    
    ex  virtual getSigmaMSDRef2Ref(refPoint r1, refPoint r2 ){

        if (!testReference(r1)) throw SEBException("Wrong reference point:"+r1, "GENERIC::getSigmaMSDRef2Ref()");
        if (!testReference(r2)) throw SEBException("Wrong reference point:"+r2, "GENERIC::getSigmaMSDRef2Ref()");

        if(r2 < r1) swap (r1, r2);
        if (hasSpecificReference(r1) && hasSpecificReference(r2) && r1==r2) return ex(1);

        if(r1.find(".") != string::npos)  throw SEBException("Wrong input in getRefToRefMSD()", "getSigmaMSDRef2Ref()");

        if(hasAHash(r1)) r1 = removeHashString(r1);
        if(hasAHash(r2)) r2 = removeHashString(r2);

        return GLEX->getSymbol("sigmaRrr2",tag, r1, r2);
    }

};

#endif // INCLUDE_GUARD_GENERIC
