/*
    Helper method for creating different subunit types

*/


#ifndef INCLUDE_GUARD_CREATESUBUNIT
#define INCLUDE_GUARD_CREATESUBUNIT

SubUnit*  CreateSubunit(string subtype)
{
        if (subtype == "Point")               return new Point();
   else if (subtype == "GaussianLoop")        return new GaussianLoop();
   else if (subtype == "GaussianPolymer")     return new GaussianPolymer();
   else if (subtype == "ThinRod")             return new ThinRod();
   else if (subtype == "ThinCircle")          return new ThinCircle();
   else if (subtype == "ThinDisk")            return new ThinDisk();
   else if (subtype == "ThinSphericalShell")  return new ThinSphericalShell();
   else if (subtype == "SolidSphere")         return new SolidSphere();
   else if (subtype == "SolidSphericalShell") return new SolidSphericalShell();
   else if (subtype == "SolidCylinder")       return new SolidCylinder();
   else if (subtype == "SymbolicSubunit")     return new SymbolicSubunit();
   else 
      throw SEBException("Unknown sub-unit type "+subtype,"Create");
}

#endif
