#include "World.hpp"

// Include creation function for sub-units.
#include "Subunits/CreateSubunit.hpp"

/* Adds a new sub unit that is not connected to anything. Thus it receives its own graphID.

   Arguments:
           sub a pointer to an uninitialized sub-unit object class, Init initializes it.
           name  a string for naming the sub-unit.
           tag   a string for grouping identical sub-units together.
           
   Returns graphID.
*/

GraphID World::Add(SubUnit *sub, subName name, string tag)
{
try{
    if (!GLEX->testnamestring(name))                 throw SEBException("Bad symbol in subunit name:"+name);
    if (!tag.empty() && !GLEX->testnamestring(tag))  throw SEBException("Bad symbol in subunit tag:"+tag  );
    if (hasName(name))                               throw SEBException("Name "+name+" already exists in the world");
    if (!testSubunitPointer(sub))                    throw SEBException("Supplied subUnit pointer does not point a sub-unit.");

    sub->Init(name, tag, GLEX);

    totalNumberofGraphs++;                // Every add creates a new graph since the sub-unit is not connected to anything.
    list<subName> subList;
    subList.push_back(name);
    subGraphs.insert(pair<GraphID, list<subName>>(totalNumberofGraphs, subList));
    nameCatalog.insert(pair<subName, SubUnit *>(name, sub));
    typeCatalog.insert(pair<subName, int>(name, sub->getType()));

    return totalNumberofGraphs;
}
catch (SEBException& e)
{
   e.PushCallStack("GraphID World::Add(SubUnit*, subName "+name+", string "+tag+")" );
   throw;
}
}

// Helper
GraphID World::Add(string subtype, subName name, string tag)
{
   return Add(CreateSubunit(subtype), name, tag);
}


/*
    Adds a new structure to the world, think of this as a bag that contains a lot of connected sub-units, but gives them a new name.
    Using the structure we can continue to build structures by linking additional structures into it, analogously to linking
    sub-units to sub-units. 

    It is not possible to addAndLink a sub-unit to a structure, but the single sub-unit can be wrapped in a structure and
    linked that way.
    
    Arguments: graphID existing graph in the world
               name a new unique name for this structure

    Returns graphID.
*/

GraphID World::Add(GraphID gid, structName name)
{
try{
    if (!GLEX->testnamestring(name))      throw SEBException("Bad symbol in structure name:"+name );
    if (!testGraphID(gid))                throw SEBException("Bad graphid:"+to_string(gid) );
    if (hasName(name))                    throw SEBException("Name "+name+" already exists in the world");

    Structure *struc = new Structure(name, gid, GLEX);
    totalNumberofGraphs++;
    
    list<structName> struccontainer;
    struccontainer.push_back(name);
    subGraphs.insert(pair<GraphID, list<structName>>(totalNumberofGraphs, struccontainer));
    
    nameCatalog.insert(pair<string, ABSSubUnit *>(name, struc));
    typeCatalog.insert(pair<string, int>(name, struc->getType()));

    return totalNumberofGraphs;   // return new graphID
}
catch (SEBException& e)
{
   e.PushCallStack("GraphID World::Add(GraphID "+to_string(gid)+", structName "+name+")");
   throw;
}

}

/* Adds a structure based on a name, which can be the name of an existing sub-unit or structure. */
/*GraphID World::Add(string oldname, string newname)
{
try{
     return Add(getGraphID(oldname), newname);
   }
catch (SEBException& e)
{
   e.PushCallStack("GraphID World::Add(string "+oldname+", string "+newname+")");
   throw;
}
}
*/


/* addsAndLink a new sub-unit to an existing sub-unit, thus growing the graph by one sub-unit.

   Arguments:
     sub  is the pointer to an uninitialized sub-unit object.
     newr has the form name.ref     name is given to the new sub-unit, ref has to be a well defined reference point on that type of sub-unit given by *sub.
     oldr has the form name.ref     name has to exist already in the world, and ref has to be a well defined reference point on that sub-unit.
     tag is an optional string.     Used for defining groups of identical sub-units.
     
     The new sub-unit is then linked to the existing (old) sub-unit by the two reference points.
     

   Returns graphID which is the same for the new and old sub-units.
*/

GraphID World::Link(SubUnit *sub, refPoint newr, refPoint oldr, string tag)
{
try{
    string newname = getName(newr); // extract name from  name.ref
    string oldname = getName(oldr); 

    if (!GLEX->testnamestring(newname))                  throw SEBException("Bad symbol in sub-unit name:"+newname );
    if (!tag.empty() && !GLEX->testnamestring(tag))      throw SEBException("Bad symbol in sub-unit tag:"+tag);
    if (hasName(newname) )                               throw SEBException("Name "+newname+" already exists in the world");
    if (!hasName(oldname))                               throw SEBException("Name "+oldname+" does not exist in the world");

// Initialize sub-unit, this also sets up the known reference points for this type.
    sub->Init(newname, tag,  GLEX);

// Test that both reference points are good.
    string newref = getReferenceBase(newr); // extract refbase   from e.g. name.refbase#mypoint
    string oldref = getReferenceBase(oldr); 

// test new reference name.   
    if (!sub->hasReference(newref))      throw SEBException("Subunit named "+newname+" does not has reference point "+newref);  
    if (sub->hasDistributedReference(newref) && !hasAHash(newr))
                                         throw SEBException("Missing #... for distributed reference point: "+newref);      
        
// rest old reference name
    SubUnit* olds = getSubunit(oldname);
    if (!olds->hasReference(oldref))     throw SEBException("Subunit "+oldname+" does not have a reference point "+oldref );
    if (olds->hasDistributedReference(oldref) && !hasAHash(oldr))
                                         throw SEBException("Missing #... for distributed reference point: "+oldref);    
// Handle .distributedref#myname
    if(hasAHash(newr))
       {
         refPoint random=getReferenceAfterHash(newr);                // get mypoint
         sub->addRandomDistributedReferencePoint(newref, random);    // add if not already exists.
       }

    if(hasAHash(oldr))
       {
         refPoint random=getReferenceAfterHash(oldr);
         olds->addRandomDistributedReferencePoint(oldref, random);
       }

    GraphID gId = getGraphID(oldname);

    nameCatalog.insert(pair<string, ABSSubUnit *>(newname, sub));
    typeCatalog.insert(pair<string, int>(newname, sub->getType()));
    subGraphs.find(gId)->second.push_back(newname);
    links.push_back(generateLink(oldr, newr));

    return gId;
}
catch (SEBException& e)
{
   e.PushCallStack("GraphID World::Link(SubUnit*, refPoint "+newr+", refPoint "+oldr+", string tag"+tag+")"  );
   throw;
}

}

// helper
GraphID World::Link(string subtype, refPoint newr, refPoint oldr, string tag)
{
   return Link(CreateSubunit(subtype), newr, oldr, tag);
}

/*
    Grows a structure by linking the structure in gid to it.
    
    Instead of a subunit pointer, the method takes a graphID.
    The first reference point r1 has structure     mystructure:..:sub-unit.refpoint   such that the new structure is named mystructure.
    The second reference point r2  is of the form structure:..:sub-unit.refpoint      and should already exist in a previously defined structure.

    Returns graphID which is the same for the new and old structures.
    
*/
GraphID World::Link(GraphID gid, refPoint r1, refPoint r2)
{
try{
    if (!testGraphID(gid))  throw SEBException("Bad graphid:"+to_string(gid) );

// Check names of structures, at the highest level
    structName newname = prefix(r1);
    structName oldname = prefix(r2);

    if (hasName(newname))  throw SEBException("Name "+newname+" already exists in the world" );
    if (!hasName(oldname)) throw SEBException("Name "+oldname+" does not exist in the world" );

// Test entire path down to sub-unit and reference point base are good. true=> skip test of first structure in r1, since it has not yet been created.
    testPathSyntax(r1,true);
    testPathSyntax(r2);

// could be a new named distributed reference point  subunit.distributedreftype#myref  where we have to handle the unknown myref
    string newsubname = getName(r1);
    string oldsubname = getName(r2);
    string newref = getReference(r1);
    string oldref = getReference(r2);
    string newrefbase = getReferenceBase(r1);
    string oldrefbase = getReferenceBase(r2);

// testPathSyntax ensures that the path is correct and that the reference point exists.
// BUT we could still have a case where the reference point base is distributed, but the user has failed to specify a #.
    if (getSubunit(newsubname)->hasDistributedReference(newrefbase) && !hasAHash(newref))
        throw SEBException("Missing #... for distributed reference point: "+newref);
    if (getSubunit(oldsubname)->hasDistributedReference(oldrefbase) && !hasAHash(oldref))
        throw SEBException("Missing #... for distributed reference point: "+oldref);

    if(hasAHash(newref))                                                        // e.g. contour #mypoint
        {
            refPoint refbase=getReferenceBase(r1);                          // contour
            refPoint random=getReferenceAfterHash(r1);                      // mypoint
            getSubunit(newsubname)->addRandomDistributedReferencePoint(refbase, random);    // add if not already exists. Throw if refbase does not exist
        }
        
    if(hasAHash(oldref))                                                        // e.g. contour #mypoint
        {
            refPoint refbase=getReferenceBase(r2);                          // contour
            refPoint random=getReferenceAfterHash(r2);                      // mypoint
            getSubunit(oldsubname)->addRandomDistributedReferencePoint(refbase, random);    // add if not already exists.;
        }
        
    Structure *struc = new Structure(newname, gid, GLEX);

    GraphID oldgraphID = getGraphID(oldname);
    subGraphs.find(oldgraphID)->second.push_back(newname);
    
    nameCatalog.insert(pair<string, Structure *>(newname, struc));
    typeCatalog.insert(pair<string, int>(newname, struc->getType()));
    links.push_back(generateLink(r1, r2));

    GraphID newgraphID = getGraphID( prefix(r1) );
    if (newgraphID!= oldgraphID)
        throw SEBException("Internal error: mismatch in graphids");

    return newgraphID;
}
catch (SEBException& e)
{
   e.PushCallStack("GraphID World::Link(GraphID "+to_string(gid)+", refPoint "+r1+", refPoint "+r2+")" );
   throw;
}

}





/*  Find a path inside a given structure.   (here using DiBlockStarChain.cpp as an example)

    Example:  chain:star1:diblock1:polyB.end2 and    chain:star5:diblock3:polyB.end2

    The path is between two fully specified sub-units and potentially reference points hence the lengthy expressions.
    That is a path from a common structural parent (chain) down to the specific sub-unit (and reference point).

    These searches are always horizontal in a structure in the sense that a path is defined on the immediate
    children of the parent structure. Here star1-star5 are the immediate children of chain, thus the path is
    defined as the star level, and never involves steps at the diblock level.
    
    Paths always exist and are unique because by construction. They exist because we require the
    searches to be rooted at the same level in the structure, and everything is connected at every
    level. Secondly, paths are unique because with addAndLink we grow graphs and structures by
    adding leafs to an existing graph, thus we can never form a loop by construction.
    

      CASE 1: Path between two reference points:

      Example:
         name1 is star1:diblock1:polyB.end2
         name2 is star1:diblock3:polyB.end2

      The result is diblock1:polyB.end2 -> diblock1:polyA.end1 <-> diblock3:polyA.end1 -> diblock3:polyB.end2
      
      
      Example:
        name1 is chain:star1:diblock1:polyB.end2
        name2 is chain:star5:diblock3:polyB.end2
        
      The result is star1:diblock1:polyB.end2 -> star1:diblock3:polyB.end2 <-> star2:diblock1:polyB.end2 -> star2:diblock3:polyB.end2 <->
                    star3:diblock1:polyB.end2 -> star3:diblock3:polyB.end2 <-> star4:diblock1:polyB.end2 -> star4:diblock3:polyB.end2 <->
                    star5:diblock1:polyB.end2 -> star5:diblock3:polyB.end2

      Paths are always alternating sequences of jumps across sub-units/structures (->) connecting two reference points on the same
      sub-unit/structure and jumps across links (<->).
      
      Example: The lowest level we can search is inside a structure that directly resolves to sub-units
        name1 is  diblock1:polyA.end1
        name1 is  diblock1:polyB.end2
        
      The result is   polyA.end1 -> polyA.end2 <-> polyB.end1 -> polyB.end2
  


      CASE 2: path between a reference point and a structure / sub-unit. Here we flip so name1 is always the refence point.

        name1 is chain:star1:diblock1:polyB.end2
        name2 is chain:star5:diblock3:polyB

       The result is star1:diblock1:polyB.end2 -> star1:diblock3:polyB.end2 <-> star2:diblock1:polyB.end2 -> star2:diblock3:polyB.end2 <-> 
                     star3:diblock1:polyB.end2 -> star3:diblock3:polyB.end2 <-> star4:diblock1:polyB.end2 -> star4:diblock3:polyB.end2 <->
                     star5:diblock1:polyB.end2
                     
       The search terminates because chain:star5:diblock3:polyB is found inside star5. So we have found the structure
       containing the target. Because searches are horizontal we do not search at the diblock level towards the target.
       
       But to find out how to get from star5:diblock1:polyB.end2 to chain:star5:diblock3:polyB we would have again search
       for that path, this is a search at the diblock level one below star5. The result is 
                   diblock1:polyB.end2 -> diblock1:polyA.end1 <-> diblock3:polyA.end1
       
       Again the search terminates at diblock3:polyA.end1 because diblock3:polyB is found inside diblock3. To find the
       path from the reference point to the unit, we have to search diblock3:polyA.end1 to diblock3:polyB 

       The result is polyA.end1 -> polyA.end2 <-> polyB.end1  and we have found which reference point 
       chain:star5:diblock3:polyB.end1 which is closest to chain:star1:diblock1:polyB.end2
        


      CASE 3: path between a pair of structures / sub-unit2.

         name1 is chain:star1:diblock1:polyB
         name2 is chain:star5:diblock3:polyB

         In this case we seed the search with start1:*.refs and then search outwards to other structures, and
         terminate when we hit a star5:* reference point. Hence the path above is identical to the search.

         name1 is chain:star1
         name2 is chain:star5
         
         
         The path found is

                  star1:diblock3:polyB.end2 <-> star2:diblock1:polyB.end2 -> star2:diblock3:polyB.end2 
                                            <-> star3:diblock1:polyB.end2 -> star3:diblock3:polyB.end2
                                            <-> star4:diblock1:polyB.end2 -> star4:diblock3:polyB.end2
                                            <-> star5:diblock1:polyB.end2

         The paths starts at star1:diblock3:polyB.end2   since star1:diblock1:polyB is inside star1.
         The path ends at    star5:diblock1:polyB.end2   since star5:diblock3:polyB is inside star5

         Thus the corresponding scattering expression would be 
 
           Form factor amplitude of star1:diblock1:polyB relative to star1:diblock3:polyB.end2
         x Phase factor of star2:diblock1:polyB.end2 -> star2:diblock3:polyB.end2
         x Phase factor of star3:diblock1:polyB.end2 -> star3:diblock3:polyB.end2
         x Phase factor of star4:diblock1:polyB.end2 -> star4:diblock3:polyB.end2
         x Form factor amplitude of star5:diblock3:polyB relative to star5:diblock1:polyB.end2

         Additional path searches between these are required to resolve the structural
         scattering terms in terms or their substructures / subunits.

      Example:
 
          name1 is star1:diblock1:polyB
          name2 is star1:diblock3:polyB
          
          The path found is diblock1:polyA.end1 <-> diblock3:polyA.end1
          
          The path starts at diblock1:polyA.end1  since diblock1:polyB is inside diblock1
          The path ends at   diblock3:polyA.end1  since diblock3:polyB is inside diblock3


      Example: 
      
          name1 is diblock1:polyB
          name2 is diblock1:polyA.end1
          
          The path is polyB.end1 <-> polyA.end2 -> polyA.end1


      Example:
      
          name1 is diblock3:polyA.end1
          name2 is diblock3:polyB

          The path is polyA.end1 -> polyA.end2 <-> polyB.end1



      The search algorithm is a stack based breadth-first search algorithm. We maintain a list of path candidates,
      we extract the first path candidate in the list, and extract the reference point at the end of the path, and
      then attempt to grow the path by jumping to all non-visited neighbors of the end. 

      CASE1: We seed the search with the starting reference point and terminate when we have found the termination point.
      
      CASE2: We seed the search with the starting/termination reference point, and terminate when we have found a 
             structure containing the target/starting sub-unit.
             
      CASE3: We seed the search with all reference points within a structure, and terminate when we have found a
             structure containing the target sub-unit.

*/

ReferencePointList World::findpath(string name1, string name2, bool check)
{
try{

    if(prefix(name1) != prefix(name2))    throw SEBException("Both search paths should start in the same structure.");
    if (check) testPathSyntax(name1);
    if (check) testPathSyntax(name2);

    name1=postfix(name1);
    name2=postfix(name2);
        
    if (prefix(name1)==prefix(name2))       // Returns an empty path.
       {
             ReferencePointList Path;
             return Path;
       }

    bool reversesearch=false;
    if (!hasAPeriod(name1) && hasAPeriod(name2))
         {
            swap(name1,name2);                                 // Optimization: Search from referece point to sub-unit, rather than from
            reversesearch=true;                                // sub-unit to reference point, since there are many potential starting points.
         }

    list< ReferencePointList > SearchPaths;                    // These are all the search candidate paths reaching towards the target.
    ReferencePointList VisitedAlready;                         // Reference point already visited, used to avoid back tracking.
    bool istargetref = hasAPeriod(name2);                      // we are searching for a reference point, or a structure/subunit.

    // Seed the search with source point(s)
    if (hasAPeriod(name1))                                     // Starting point of search is a specific reference point.
      {
          ReferencePointList startPath;                        // Then we add that reference point as the only          
          startPath.push_back(name1);                          // source path in our SearchPaths
          SearchPaths.push_back(startPath);
          VisitedAlready.push_back(name1);                     // Mark starting point has having been visited
      }
    else                                                       // Starting point of seeach is a structure or sub-unit
     {                                                         // then we seed the search with all reference points within the structure or sub-unit

          for (auto r : getReferencePoints( prefix(name1) ) )
             {
                ReferencePointList startPath;
                startPath.push_back(r);
                SearchPaths.push_back(startPath);
                VisitedAlready.push_back(r);
             }
     }

    // Stack based recursion, where a path candidate is popped, grown by a single step to a neighbor, checked against target, and pushed back if no target is found.
    while (!SearchPaths.empty())
      {
          ReferencePointList path = SearchPaths.front();                    // Get candidate path from the SearchPaths list
          SearchPaths.pop_front();                                          // remove this path from the SearchPaths list
          refPoint last = path.back();                                      // What is the last step, to grow from?
          
          for (auto next : getNeighbors( last, VisitedAlready) )            // iterate over next possible steps from last, that has not already been visited
             {
                 path.push_back(next);                                      // we push the next ref to the path to propose a new search path
                                                                            // Have we found the target of our search?
                 if (    ( istargetref &&         next == name2          )  //   case of reference point target
                      || (!istargetref && prefix(next) == prefix(name2)) )  //   case of substructure / sub-unit target
                    {
                       if (reversesearch) path.reverse();                   // In case we were search from target to source reverse path.
                       return path;                                         // and return it.
                    }

                 VisitedAlready.push_back(next);                            // mark next has baving been visited
                 SearchPaths.push_back(path);                               // and send the new grown path back to the search list.
                 path.pop_back();                                           // pop next, so we can reuse this path for adding a new step
             }          
      }

    throw SEBException("Didn't find a path between "+name1+"  "+name2+"    This should not happen!");
}
catch (SEBException& e)
{
   e.PushCallStack("ReferencePointList World::findpath(string name1=\""+name1+"\", string name2=\""+name2+"\")" );
   throw;
}
}


/*  Not used by SEB, but provides paths between pairs of reference points at a user specified depth. Thus path is specified at the
    same height as the two starting reference points.  */
ReferencePointList  World::Path( refPoint r1, refPoint r2, int depth, bool doCheck)
{
try{
    string myself=prefix(r1);
    if(myself != prefix(r2))   throw SEBException("Both reference points should be in the same structure / reference point.");
    if (depth<0)               throw SEBException("Depth can not be negative.");
    if (!hasName(myself))      throw SEBException(string("Unknown name ")+myself+" in world.");


    // If we have two paths to a reference point inside a structure:
    if (isStructure(myself))
      {
        // These checks are expensive, and disabled in subsequent recursions.
        if(doCheck) testPathSyntax(r1);
        if(doCheck) testPathSyntax(r2);

        // Trivial cases
        ReferencePointList rpl;
        if ( isLinked(r1, r2) || r1 == r2 || depth==0)
            {
              rpl.push_back(r1);
              rpl.push_back(r2);
              return rpl;
            }
        
        // depth>0   hence we could have XX:YY... XX:ZZ...  or  XX:YY... XX:YY...  in the second case the yy-yo-yy path would be empty, so we have to handle thus case
        if (prefix(postfix(r1)) == prefix(postfix(r2)))   // XX:YY.. and XX:yy..
           {
               rpl = Path( postfix(r1) , postfix(r2), depth - 1, false);              
           }
         else
           {                                                                                    // The two points have different second prefix, hence the path will not be empty.
               ReferencePointList path = findpath(r1, r2, false);                              // Find path, don't check arguments.
               
               auto it=path.begin();
               auto its=it; its++;
               while (its != path.end())
                {                                                                                                           
                    if(!isLinked(*it, *its))                                                                                 // steps across links contribute no phase
                             rpl.splice( rpl.end(), Path(*it, *its, depth-1, false) );

                    it=its; its++;
                }
           }

        for (auto& p : rpl) p=myself+":"+p;   // Prepent myself, since findpath removes the common root structure.
        return rpl;
    }
    else
     if (isSubunit(myself))     // r1,r2 must have the form   r1=subunit.ref1  r2=subunitname.ref2      
          {
              ReferencePointList rpl { r1, r2};
              return rpl;
         }

    throw SEBException("Internal error.");
    
}
catch (SEBException& e)
{
   e.PushCallStack("ReferencePointList  World::Path(refPoint "+r1+", refPoint "+r2+", int "+to_string(depth)+"..)");
   throw;
}
}


/* User front end for generating Phase factor between two given reference points. Both r1 and r2 should start at the same
   level in the structure, and thus share prefix.
*/
ex World::PhaseFactor( refPoint r1, refPoint r2, int depth, int varForm)
{
try{
    string myself=prefix(r1);
    if(myself != prefix(r2)) throw SEBException("Both reference points should be in the same structure / reference point.");
    if (depth<0)             throw SEBException("Depth can not be negative.");
    if (!hasName(myself))    throw SEBException("Unknown name "+myself+" in world.");

    if (isStructure(myself))
      {
        testPathSyntax(r1);
        testPathSyntax(r2);
      }

    betas.clear(); params.clear();

    return GenerateRefToRef( r1, r2, depth, varForm );
}
catch (SEBException& e)
{
   e.PushCallStack("World::PhaseFactor(refPoint r1=\""+r1+"\", refPoint r2=\""+r2+"\", int depth="+to_string(depth)+"..)");
   throw;
}
}


// User front end for generating form factor amplitudes for a specified reference point. Returns normalized scattering expression
ex World::FormFactorAmplitude( refPoint ref, int depth , int varForm  )
{
try{
   string myself=prefix(ref);
   if (depth<0)           throw SEBException("Depth can not be negative.");
   if (!hasName(myself))  throw SEBException("Unknown name "+myself+" in world.");
   if (!hasAPeriod(ref))  throw SEBException("String \""+ref+"\" does not specify a reference point.");     
   if(isStructure(myself)) testPathSyntax(ref);
                          
   betas.clear(); params.clear();

   return GenerateRefToAll( ref,  depth, varForm )/GenerateRefToAll( ref,  depth, BETA );
}
catch (SEBException& e)
{
   e.PushCallStack("World::FormFactorAmplitude(refPoint ref=\""+ref+"\",int depth="+to_string(depth)+", int varForm="+to_string(varForm)+")");
   throw;
}
}               


// User front end for generating form factor amplitudes for a specified reference point. Returns beta weighted but does not divide by sum beta to normalize
ex World::FormFactorAmplitude_Unnormalized( refPoint ref, int depth, int varForm)
{
try{
   string myself=prefix(ref);
   if (depth<0)            throw SEBException("Depth can not be negative.");
   if (!hasName(myself))   throw SEBException("Unknown name "+myself+" in world.");
   if (!hasAPeriod(ref))   throw SEBException("String \""+ref+"\" does not specify a reference point.");     
   if(isStructure(myself)) testPathSyntax(ref);
   betas.clear(); params.clear();
   
   return GenerateRefToAll( ref,  depth, varForm );
}
catch (SEBException& e)
{
   e.PushCallStack("World::FormFactorAmplitude_Unnormalized( refPoint ref=\""+ref+"\",int depth="+to_string(depth)+", int varForm="+to_string(varForm)+",..)");
   throw;
}
}

// User front end for generating form factors everything is weighed by beta factors and normalized by (sum beta)^2
ex World::FormFactor( structName myself, int depth, int varForm )
{
try{
   if (hasAColon(myself))  throw SEBException("Expected structure/sub-unit name got : in "+myself );
   if (hasAPeriod(myself)) throw SEBException("Expected structure/sub-unit name got . in "+myself);
   if (!hasName(myself))   throw SEBException("Unknown structure/sub-unit "+myself);
   if (depth<0)            throw SEBException("Depth can not be negative.");
   betas.clear(); params.clear();

   return GenerateAllToAll( myself, depth, varForm)/GenerateAllToAll( myself, depth, BETA);
}
catch (SEBException& e)
{
   e.PushCallStack("ex World::FormFactor( structName \""+myself+"\", depth="+to_string(depth)+",..)");
   throw;
}
}

// User front end for generating form factors everything is weighed by beta factors but not normalized
ex World::FormFactor_Unnormalized( string myself, int depth, int varForm)
{
try{
   if (hasAColon(myself))  throw SEBException("Expected structure/sub-unit name got : in "+myself);
   if (hasAPeriod(myself)) throw SEBException("Expected structure/sub-unit name got . in "+myself);
   if (!hasName(myself))   throw SEBException("Unknown structure/sub-unit "+myself);
   if (depth<0)            throw SEBException("Depth can not be negative.");
   betas.clear(); params.clear();
 
   return GenerateAllToAll( myself, depth, varForm);
}
catch (SEBException& e)
{
   e.PushCallStack("ex World::FormFactor_Unnormalized( structName \""+myself+"\", depth="+to_string(depth)+",..)");
   throw;
}
}

ex World::PhaseFactorX( refPoint r1, refPoint r2, int depth)
{
    return PhaseFactor( r1, r2, depth, XVAR );
}
               
ex World::PhaseFactorGeneric( refPoint r1, refPoint r2, int depth)
{
    return PhaseFactor(r1, r2, depth, GENERIC);
};




ex World::FormFactorAmplitudeX       ( refPoint ref, int depth)
{
   return FormFactorAmplitude(ref, depth, XVAR);
}
    
ex World::FormFactorAmplitudeGeneric( refPoint ref, int depth)
{
   return FormFactorAmplitude(ref, depth, GENERIC);
}

ex World::FormFactorAmplitudeX_Unnormalized( refPoint ref, int depth)
{
   return FormFactorAmplitude_Unnormalized(ref, depth, XVAR);
}    
    
ex World::FormFactorAmplitudeGeneric_Unnormalized( refPoint ref, int depth)
{
   return FormFactorAmplitude_Unnormalized(ref, depth, GENERIC);
}

ex World::FormFactorAmplitude_Normalization( refPoint ref, int depth )
{
   return FormFactorAmplitude_Unnormalized(ref, depth, BETA);
}




ex World::FormFactorX( string name, int depth)
{
    return FormFactor(name, depth, XVAR);
}

ex World::FormFactorGeneric(string name, int depth)
{
    return FormFactor(name, depth, GENERIC);
}

ex World::FormFactorX_Unnormalized( string name, int depth)
{
    return FormFactor_Unnormalized(name, depth, XVAR);
}

ex World::FormFactorGeneric_Unnormalized( string name, int depth)
{
    return FormFactor_Unnormalized(name, depth, GENERIC);
}

ex World::FormFactor_Normalization( string name, int depth)
{
    return FormFactor_Unnormalized(name, depth, BETA);
}

ex World::RadiusOfGyration2( string name, int depth )
{
    ex q=GLEX->getSymbol("q");
    ex F = FormFactor_Unnormalized( name, depth, GUINIER ).expand();

// When F terms are weigted by betas, but not normalized, the expansion is
// F = (sum_beta)^2 - q^2 Rg2/3 + ..
// Where Rg2 contributions are weighted by beta terms but not normalized
// Hence the proper normalized Rg2 is obtained by dividing the 2nd. coefficient by the zero coefficient.
    
    ex Rg2 = -3*F.coeff(q, 2)/F.coeff(q, 0);
    return Rg2;
}


ex World::SMSD_ref2scat( refPoint ref, int depth)
{
    ex q=GLEX->getSymbol("q");
    ex A = FormFactorAmplitude_Unnormalized( ref, depth, GUINIER ).expand();

// When A terms is weigted by betas but not normalized, the expansion is
// A = (sum_beta) - q^2 sigma <R2_ref>/6 + ..
// Where <R^2> contributions are weighted by beta terms but not normalized
// Hence the proper normalized <R2> is obtained by dividing by the zero coefficient.
    
    ex sigmaMSD=-6*A.coeff(q, 2)/A.coeff(q, 0);
    return sigmaMSD;
}

ex World::SMSD_ref2ref( refPoint r1, refPoint r2, int depth  )
{
    ex q=GLEX->getSymbol("q");
    ex secondorder = PhaseFactor( r1, r2 , depth, GUINIER ).expand().coeff(q, 2);

    return -6*secondorder;
}


ex World::Count(refPoint ref, int depth)
{
    return FormFactorAmplitude_Unnormalized(ref, depth, ONE);
}

ex World::CountPairs( string name, int depth)
{
    return FormFactor_Unnormalized(name, depth, ONE);
}





/* Is specified name in the nameCatalog? */
bool World::hasName(string n)
{
   return nameCatalog.find(n) != nameCatalog.end();
}

/* Is specified name in the specified subgraph? */
bool World::doesSubgraphContainName(int gid, string n)
{
   if (!testGraphID(gid))    throw SEBException("Bad graphid "+to_string(gid), "World::doesSubgraphContainName(GraphID ="+to_string(gid)+", string=\""+n+"\")");

   for ( auto item : subGraphs.at(gid))
      if (item == n) return true;
   return false;
}


/* Checks that parent structure contains subling (structure / subunit name). Only checks one level down. */
bool World::doesStructureContainName(string parent, string sibling)
{
   if (!isStructure(parent)) throw SEBException("String does not refer to a structure.", "World::doesStructureContainName(string parent=\""+parent+"\", string sibling=\""+sibling+"\")");

   GraphID gid=getStructure(parent)->getGraphID();
   return doesSubgraphContainName(gid, sibling);
}


/* Returns a GraphID from a unique name of a sub unit or a subgraph

   Should be e.g. structure_name or subunit_name, will fail if given structure:sub-unit, since then its unclear what graphID referred to, that of the structure, of the sub-unit.
*/
GraphID World::getGraphID(string name)
{
    if (hasAColon(name))  throw SEBException("The specified name contains a :", "World::getGraphID(string "+name+")");

    for (auto itr : subGraphs)
        for (auto itr2 : itr.second)
            if (itr2 == name) return itr.first;

    throw SEBException("The specified name was not found in any sub-graph", "World::getGraphID(string "+name+")");
}

/*Returns the graphID id given a pointer to a sub-unit or structure */
GraphID World::getGraphID(SubUnit *s)    {  return getGraphID( s->getName() ); }
GraphID World::getGraphID(Structure *s)  {  return getGraphID( s->getName() ); }


// return sub-unit pointer to specified sub-unit. Throws if name unknown, or pointer is not to a sub-unit
SubUnit* World::getSubunit(string n)
{
   if (!hasName(n)) throw SEBException("The specified subunit is unknown", "World::getSubunit(string "+n+")");
   SubUnit* sptr = reinterpret_cast<SubUnit*>( nameCatalog.find(n)->second );
   
   if (sptr->getType() != SUBUNIT && sptr->getType() != SUBUNITCHILD)
       throw SEBException("The specified name does not point to a sub-unit!", "World::getSubunit(string "+n+")");
   
   return sptr;
}

// return structure pointer to specified structure.  Throws if name unknown, or pointer is not to a structue
Structure* World::getStructure(string n)         
{
   if (!hasName(n)) throw SEBException("The specified structure is unknown", "World::getStructure(string "+n+")");
   Structure* sptr = reinterpret_cast<Structure*>( nameCatalog.find(n)->second );
   
   if (sptr->getType() != STRUCTURE)
       throw SEBException("The specified name does not point to a structure!", "World::getStructure(string "+n+")");
   
   return sptr;
}

// Helper returning whether "name" is a sub-unit. Throws if name does not exist.
bool World::isSubunit(string n)
{
   if (!hasName(n)) throw SEBException("The specified name is unknown", "World::isSubunit(string "+n+")");

   ABSSubUnit* aptr=nameCatalog.find(n)->second;
   int typ=aptr->getType();
   return  typ== SUBUNIT || typ == SUBUNITCHILD;
}

// Helper returning whether "name" is a structure. Throws if name does not exist.
bool World::isStructure(string n)
{
   if (!hasName(n)) throw SEBException("The specified name is unknown", "World::isStructure(string "+n+")");

   ABSSubUnit* aptr=nameCatalog.find(n)->second;
   return aptr->getType() == STRUCTURE;
}


/* Checks if to reference point are linked */
bool World::isLinked(refPoint r1, refPoint r2)
{
    if (r2<r1) swap(r1,r2);   // We store links alphabetically sorted.
    for(auto link : links)
        if(link.first == r1 && link.second == r2)  return true;

    return false;
}


// Helper function that gets all reference points inside a structure, that is all paths
// connecting structure to a reference point.
ReferencePointList World::getReferencePoints( refPoint structure)
{
// test structure string is good.

    ReferencePointList references;
    addStructureReferences(structure, references );
    return references;
}



// Methods for handling parameters, and evaluating expressions.



// Returns a copy of the parameter lists.
ParameterList World::getParams()
{
   ParameterList pl(betas);
   pl.insert(params.begin(), params.end());
   return pl;
}
       
// Returns a copy of the parameter lists.
ParameterList World::getParamsq()
{
   ParameterList pl(betas);
   pl.insert(params.begin(), params.end());
   ex q=GLEX->getSymbol("q");
   pl[q]=0;
   return pl;
}
       
ParameterList World::getq(double q) { 
   ParameterList pl;
   ex Q=GLEX->getSymbol("q");
   pl[Q]=q;
   return pl;
}

ParameterList World::q(double q) { return getq(q); }

double World::setParameter(ParameterList& pl, string str, double x)
{
   // Throw if str not known??
   ex e=GLEX->get(str);
   pl[e]=x;               
   return x;         
}


DoubleVector World::linspace(double q1,double q2,int n)
{
   if (q2<q1) swap(q1,q2);
   if (q1 <=0) throw SEBException("Negative values not allowed","World::linspace("+to_string(q1)+","+to_string(q2)+","+to_string(n)+ ")");
   if (q2 <=0) throw SEBException("Negative values not allowed","World::linspace("+to_string(q1)+","+to_string(q2)+","+to_string(n)+ ")");
   if (n <=1)  throw SEBException("Number of points must be larger than 1","World::linspace("+to_string(q1)+","+to_string(q2)+","+to_string(n)+ ")");
   
   DoubleVector q;
   for (int i=0;i<n;i++)
         q.push_back( (q2-q1)*i/(n-1) + q1 );
     
   return q;
}

DoubleVector World::logspace(double q1,double q2,int n)
{
   if (q2<q1) swap(q1,q2);
   if (q1 <=0) throw SEBException("Negative values not allowed","World::logspace("+to_string(q1)+","+to_string(q2)+","+to_string(n)+ ")");
   if (q2 <=0) throw SEBException("Negative values not allowed","World::logspace("+to_string(q1)+","+to_string(q2)+","+to_string(n)+ ")");
   if (n <=1)  throw SEBException("Number of points must be larger than 1","World::logspace("+to_string(q1)+","+to_string(q2)+","+to_string(n)+ ")");
   
   DoubleVector q;
   for (int i=0;i<n;i++)
       {
         q.push_back( pow( 10.0,  log10(q2/q1)*i/(n-1) + log10(q1)  ) );
       }
   return q;
}

DoubleVector World::logspace(double q1,double q2, double base)
{
   if (q2<q1) swap(q1,q2);
   if (q1 <=0) throw SEBException("Negative values not allowed","World::logspace("+to_string(q1)+","+to_string(q2)+","+to_string(base)+ ")");
   if (q2 <=0) throw SEBException("Negative values not allowed","World::logspace("+to_string(q1)+","+to_string(q2)+","+to_string(base)+ ")");
   if (base<=1)  throw SEBException("Base must be larger than one","World::logspace("+to_string(q1)+","+to_string(q2)+","+to_string(base)+ ")");
   
   DoubleVector q;
   double x=q1;
   while (x<=q2)
     {
        q.push_back(x);
        x*=base;
     }
     
   return q;
}

double World::Evaluate(ex e, ParameterList& pl)
{
   ex result=e.subs(pl).evalf();

   // ex_to<numeric> is an unsafe cast, so check the type first
   if (is_a<numeric>(result)) 
        return ex_to<numeric>(result).to_double();
    else
        throw SEBException("Expression did not evaluate to number, since not all parameters were specified", "World::Evaluate(ex,ParameterList)");
}

double World::Evaluate(ex e, ParameterList& pl, double q)
{
   ex result=e.subs(pl).subs(getq(q)).evalf();

   // ex_to<numeric> is an unsafe cast, so check the type first
   if (is_a<numeric>(result)) 
        return ex_to<numeric>(result).to_double();
    else
        throw SEBException("Expression did not evaluate to number, since not all parameters were specified", "World::Evaluate(ex,ParameterList, double)");
}

DoubleVector World::Evaluate(ex e, ParameterList& pl, DoubleVector& q)
{
   DoubleVector I(q.size());   
   ex eval=e.subs(pl);   
   for (int i=0; i<q.size() ; i++)
      {
        ex result = eval.subs(getq(q[i])).evalf();

        if (is_a<numeric>(result)) I[i]=ex_to<numeric>(result).to_double();
        else
        throw SEBException("Expression did not evaluate to number, since not all parameters were specified", "World::Evaluate(ex,ParameterList, DoubleVector)");
      }
   
   return I;
} 

DoubleVector World::Evaluate(ex e, ParameterList& pl, DoubleVector& q, string fname, string comment, string prefix)
{
   DoubleVector I=Evaluate(e,pl,q);
   
   ofstream fo(fname);
   if (!fo.is_open()) throw SEBException("Could not open file "+fname, "World::Evaluate(ex, Parameterlist&, DoubleVector&, string "+fname+" )");
   
   fo << prefix << "File geneated by SEB\n";
   fo << prefix << comment << "\n";
   fo << prefix << "Expression = " << e << "\n";
   fo << prefix << "Parameters:\n";
   for (auto const& p : pl)
       fo << prefix << "\t" << p.first << "=" << p.second << "\n";

   for (int i=0;i<q.size(); i++)
      fo << q[i] << " " << I[i] << "\n";
   fo.close();
   
   return I;
}


/*
Prints out all nested structures in a structure using a directory format. 

Here casting is used to get a structure pointer from namecatalog that contains ABSSubunits pointers
 https://stackoverflow.com/questions/36802354/print-binary-tree-in-a-pretty-way-using-c
 Prints the the sub units with a print inspired by folder structure
 */

void World::folderprint(string name, string StructureString, bool isLast)
{
    if (isStructure(name))
       {
            cout << StructureString;
            cout << (isLast ? "└──" : "├──");
            cout << name + "\n";

            GraphID gid=getStructure(name)->getGraphID();                           // get gid inside "name".
            for (auto it = subgraph_cbegin(gid); it != subgraph_cend(gid); it++)    // traverse that subgraph
                {
                    string n=*it;
                    auto itt=it; itt++;  // Next element in list.
                    folderprint(n, StructureString + (isLast ? "    " : "│   "), itt==subgraph_cend(gid) );       
                }
       }
     else if (isSubunit(name))
       {
            cout << StructureString;
            cout << (isLast ? "└──" : "├──");
            cout << name << "\n";       
       }
     else
       throw SEBException("Should never happen!","void World::print(folderprintname=\""+name+"\", StructureString=\""+StructureString+"\", isLast="+to_string(isLast)+")" );
}


// Prints out which sub-unit / structure names belong to which sub-graphs
void World::printGraphIDs()
{
   for (auto g : subGraphs)
     {
        cout << g.first << " -> ";
        for (auto n : g.second )     // g is pair<GraphID,list<string>>
           {              
              cout << n;
              if (isStructure(n)) cout << "(" << getStructure(n)->getGraphID() << ")";
              cout << " ";
           }
        cout << endl;
     }

}


void World::printPath(ReferencePointList path, string prefix, string postfix)
{
   for (auto p : path)
         cout << prefix << p << postfix;
}


/*Prints all links*/
void World::printLinks()
{
    for (auto it : links)
    {
        cout.width(50);
        cout << it.first << right << "<--->" + it.second + "\n";
    }
}


void World::printList(ReferencePointList path, string prefix, string postfix)
{   
   for (auto it=path.begin() ; it!= path.end(); ++it )
     {
        cout << prefix << *it;
        if (*it != path.back()) cout << postfix;
     }
}

























// --------------------------------------------------------------------------------------------------------------------------------------------------
// Private helper functions below

/*  Returns the phase factor at a given level of nested structures between two reference points

     r1 and r2 should have the same prefix, thus start at the same structure or be on the same sub-unit.

     We can not calculate phase factors inside graphs, only if they ware wrapped in a structure.     

     We should have a front-end function that tests input, and then calls a recursive helper,
     since there is no need to keep test input at every recursion.
     
*/
ex World::GenerateRefToRef( refPoint r1, refPoint r2, int depth, int varForm)
{
    string myself=prefix(r1);

    // If we have two paths to a reference point inside a structure:
    if (isStructure(myself))
      {
        // Trivial case
        if(isLinked(r1, r2) || r1 == r2 ) return ex(1);
    
        // We are still in a structure, so return GENERIC expression for psi.
        if(depth == 0) return getPsi( myself, postfix(r1), postfix(r2), varForm);
        
        // depth>0   hence we could have XX:YY... XX:ZZ...  or  XX:YY... XX:YY...  in the second case the yy-yo-yy path would be empty, so we have to handle thus case
        if (prefix(postfix(r1)) == prefix(postfix(r2)))   // XX:YY.. and XX:yy..
               return GenerateRefToRef( postfix(r1) , postfix(r2), depth - 1, varForm);          
         else
           {                                                                                    // The two points have different second prefix, hence the path will not be empty.
               ReferencePointList  path = findpath(r1, r2, false);                              // Find path, don't check arguments.
               return PhaseFactor(path, depth-1, myself, varForm);                            // Use helper to geneerate product of Psi terms.
         }
    }
    else
     if (isSubunit(myself))     // r1,r2 must have the form   r1=subunit.ref1  r2=subunitname.ref2      
        return getSubunit(getName(r1))->PhaseFactor(getReference(r1), getReference(r2), betas, params,  varForm);  

    throw SEBException("Internal error.",
                        "World::GenerateRefToRef( refPoint r1=\""+r1+"\", refPoint r2=\""+r2+"\", int depth="+to_string(depth)+", int varForm="+to_string(varForm)+")");
    
}





/* Returns the form factor amplitude at a certain level of the nested structures with respect to a given reference point 

   For example  (using examples from DiBlockCopolymerStar)
   
   polyA.end1        => beta_polyA*A_polyA:end1  (GENERIC)
   
   Which is the scattering length weighted contribution of polyA relative to end1.
   
   diblock1:polyA.end1  => beta_polyA*A_polyA:end1 + beta_polyB*Psi_polyA:end1,end2*A_polyB:end1
   
   This time we want the form factor amplitude of diblock1 relative to polyA.end1 inside the structure.
   Each sub-unit contributes a scattering length weighted term, polyA.end1 is on polyA, hence there
   are no phase shifts and hence no Psi factors, to get to polyB, we first have jump accross polyA
   from end1 to end2 until we reach end1 on polyB. Hence the Psi term, and A_polyB:end1.
   
   
   star1:diblock1:polyA.end1 =>   (depth 1)
                                     beta_diblock1*A_diblock1:polyA.end1
                                    +beta_diblock2*A_diblock2:polyA.end1
                                    +beta_diblock3*A_diblock3:polyA.end1
                                    +beta_diblock4*A_diblock4:polyA.end1

   star1 contains 4 diblocks, and since they are all tethered by diblock*:polyA.end1 <-> polyA.end1 there are no phase factors.
   If we progress one level deeper (depth 2) the result is
   
                            =>     4*beta_polyA*A_polyA:end1
                                  +4*beta_polyB*Psi_polyA:end1,end2*A_polyB:end1
   
   Each diblock copolymer has an A block close to the centre, and a B block away from the centre. Hence there are no phase
   factors for the A_polyA:end1 which are directly connected to the reference point, whereas the to get from the reference
   point to a polyB we have to jump across an A block first, which encurs a Psi_polyA:end1,end2 phase factor.
   
   Same structure and level but a different reference point this time:
   
   star1:diblock1:polyB.end2   =>       beta_polyB*A_polyB:end2
                                       +beta_polyA*Psi_polyB:end1,end2*A_polyA:end2                                       
                                      +3*Psi_polyB:end1,end2*Psi_polyA:end1,end2 * beta_polyA*A_polyA:end1 
                                      +3*Psi_polyB:end1,end2*Psi_polyA:end1,end2 * Psi_polyA:end1,end2* A_polyB:end1*beta_polyB)

   Here diblock1:polyB is directly connected to the reference point producing the first term.
   and diblock1:polyA can be reached by jumping across polyB from end1 to end2, thus producing the second term.
   furtermore, there are 3 polyA sub-units in diblock[2-4] but to reach these, we first have to jump across polyB and polyA in diblock1, thus producing the third term.
   finally, there are 3 polyB sub-units in diblock[2-4] but to reach these, we have to jump across two polyA and one polyB thus producing the final term.




*/
ex World::GenerateRefToAll( refPoint ref, int depth, int varForm )
{
   string myself=prefix(ref);
   if(isStructure(myself))
    {
        // Depth 0 reached, but we are still in structure, so return a GENERIC expression.
        if(depth == 0) return getFFA( myself, postfix(ref), varForm);

        ex A = 0;
        Structure *sptr = getStructure(myself);
        GraphID gid = sptr->getGraphID();
        
        for (auto child =subgraph_cbegin(gid) ; child!= subgraph_cend(gid) ; ++child)                          // Loop over all children
           {
               ReferencePointList  path = findpath(ref, myself+":"+*child);                             // Find path from child to reference point, don't check arguments again.

               ex term=1;
               if (path.empty())                                                                               // CASE: ref is within myself:child
                     term = GenerateRefToAll(postfix(ref), depth-1, varForm);                     // hence return form factor amplitude of child relative to ref
                 else                                                                                          // CASE: ref is not within myself:child, so we have a path.
                     term = PhaseFactor(path, depth-1, myself, varForm)                              // Make product of psi terms for jumps along path
                           *GenerateRefToAll( path.back(), depth-1, varForm);                      // times form factor amplitude relative to last ref point in path.
                  
               A+=term;                                                                                         // Each child contribute a form factor amplitude term
           }
           
        return A;
    }
    else if (isSubunit(myself))                                                                                // We have reached a sub-unit. Just return the equation.
        return getSubunit(myself)->FormFactorAmplitude(getReference(ref), betas, params, varForm);  
    else
    throw SEBException("Something wrong with the object given", "ex World::GenerateRefToAll( refPoint "+ref+", int "+to_string(depth)+",..)");
}


/*    Calculates form factor of a structure or sub-unit within the world.

*/
ex World::GenerateAllToAll( structName myself, int depth , int varForm)
{
   if(isStructure(myself))
    {
        // Depth 0 reached, but we are still in structure, so return a GENERIC expression.
        if(depth == 0) return getFF( myself, varForm );

        ex F = 0;
        Structure *sptr = getStructure(myself);
        GraphID gid = sptr->getGraphID();
        
        for (auto child1=subgraph_cbegin(gid) ; child1!= subgraph_cend(gid) ; ++child1)                    // Loop over pairs of children
           {
              for (auto child2=child1 ; child2!=subgraph_cend(gid) ; ++child2)                       
               {
                    string child1name = *child1;
                    string child2name = *child2;

                    ex Fterm=0;
                    if (child1==child2)                                                                                  // Diagonal term
{
                             Fterm=GenerateAllToAll( child1name, depth-1, varForm);                                         // Add form factor of child.

}
                      else                                                                                               
                        {                                                                                                // Add interference term between the two children.
                             ReferencePointList  path = findpath(myself+":"+child1name, myself+":"+child2name, false);   // find path connecting the two children
                             
                             ex A1 =GenerateRefToAll( path.front(), depth-1, varForm);                      // Amplitude of child1 relative to first step in path
                             ex Psi=PhaseFactor(      path        , depth-1, myself, varForm);              // phase factors due to path
                             ex A2 =GenerateRefToAll( path.back() , depth-1, varForm);                      // Amplitude of child2 relative to last step in path
                             
                             Fterm=2*A1*Psi*A2;
                                    
                        }


                    F+=Fterm;
               }
             
           }
        return F;
    }
    else if (isSubunit(myself))                                                                                // We have reached a sub-unit. Just return the equation.
    {    
        return getSubunit(myself)->FormFactor(betas, params, varForm);
    }
    else
    throw SEBException("Something wrong with the object given", "ex World::GenerateAllToAll( structName "+myself+", int depth , int varForm)");
}





ex World::PhaseFactor(ReferencePointList& path, int depth, string myself, int varForm, bool doCheck)
{
// Iterate through all steps in the path, the path is alternating 1) steps across a link, 2) steps across a structure/subunit.
//    1) we have not moved in space, so we do not need to do anything.
//    2) recursively drill down noting that the step is always within the same sibling structure or sub-unit.
// Currently we do no checks, since we assume this is a private function.

    if (depth<0)  throw SEBException("Depth can not be negative.",
                           "World::getPhaseFactor( path,"+to_string(depth)+", int varForm="+to_string(varForm)+")");

//    cout << "getPhaseFactor called with path: ";
//    printList(path); cout << "\n";

    ex Psi=1;
    auto it=path.begin();
    auto its=it; its++;
    while (its != path.end())
       {                                                                                                           // Is this a step across a sub-unit or via a link?
          if(!isLinked(*it, *its))                                                                                 // steps across links contribute no phase
               if (depth==0)                                                                                       // GENERIC expression
               {               
                   if (hasAColon(*it))          
                               Psi *= getPsi(prefix(*it), postfix(*it), postfix(*its), varForm);                    // it = XX:yy.r1  its = XX:zz.r2   Psi_XX:yy.r1,zz.r2
                          else            
                               Psi *= getPsi(getName(*it), getReference(*it), getReference(*its), varForm );        // it = XX.r1     its = XX.r2      Psi_XX:r1,r2
               }
               else            Psi *= GenerateRefToRef(*it, *its, depth, varForm);                          // otherwise recurse down
            it=its;
            its++;
         }
         
    return Psi;
}        
        

/* Sorts the link in ascii order so the smallest ascii number comes first in the link where a link = pair<refpoint, refpoint>
   The sanity of the strings should have been tested before here*/
link World::generateLink(refPoint r1, refPoint r2)
{
    if (r1==r2) throw SEBException("A reference point cannot be linked with itself", "World::generateLink(refPoint r1="+r1+", refPoint r2="+r2+")");

    if (r1<r2) return link(r1,r2);
          else return link(r2,r1);  
}



// Const iterator over subgraphs
list<string>::const_iterator World::subgraph_cbegin(GraphID id)
{
   return (subGraphs.find(id)->second).cbegin();
}

// const iterator over subgraphs termination point.
list<string>::const_iterator World::subgraph_cend(GraphID id)
{
   return (subGraphs.find(id)->second).cend();
}



/* Checks for various characters */
bool World::hasAHash(refPoint r)  {  return r.find("#") != string::npos; }
bool World::hasAColon(refPoint r) {  return r.find(":") != string::npos; }
bool World::hasAPeriod(refPoint r){  return r.find(".") != string::npos; }


/* Checks if a sub-unit pointer is good. This could still coredump if pointer points to a bad place in memory. */
bool World::testSubunitPointer(SubUnit* s)
{
   if (s==nullptr) return false;
   if (s->getType() != SUBUNIT && s->getType() != SUBUNITCHILD ) return false;
   
   return true;
}



/* Recursively checks a path through a structure.

      structure1:structure2:...:subunitname [.refname]

      Checks that all structure names are known,
             that the subunit name is known.             
             that the sub-unit has the basename of the reference point (if defined). 
             and that structure1 has a structure2 in it and so on.

    In case we are bulding a new structure, the prefix structure does not exist yet.   (skipfirst=true in this case)
*/

void World::testPathSyntax(string r, bool skipfirst)  // default false, that is check everything
{
    if (hasAColon(r))
           {           // CASE: structure1:X                       
                                                                    
               if (!skipfirst)                                       // If skipfirst is true then we skip testing structure1, since it hasn't been created yet.
                   {
                      if (!hasName(prefix(r))) 
                               throw SEBException("Structure "+prefix(r)+" is not known");
                      
                      if (!doesStructureContainName(prefix(r), prefix(postfix(r))))
                               throw SEBException("Structure "+ prefix(r)+" does not contain a "+prefix(postfix(r))+" inside it");
                   }
                   
               return testPathSyntax(postfix(r));                    // Its known, so continue to test X.
               
           }
      else
           {              // r=subunit.ref
               string name=getName(r);
               if (!hasName(name))
                      throw SEBException("Subunit "+name+" is not known");
               
               // Does the sub-unit recognize the base reference point?
               if (hasAPeriod(r) && !getSubunit(name)->hasReference( getReferenceBase(r) ))
                      throw SEBException("Subunit "+name+" does not have a reference point "+getReferenceBase(r));
           }
}


bool World::testGraphID(int g)
{
   return g>=1 && g<=totalNumberofGraphs;
}


/*  Returns the prefix before ":" or if not found before "." or the entire string.

    If called with
        structure1:structure2:subunit.ref     returns structure1
        subunit.ref                           returns subunit
        subunit                               returns subunit
        
    Throws if called with strings :xxx  or .xxx that has an empty prefix.

*/
string World::prefix(string n)
{
    int start=n.find(":");

    if (start==0) throw SEBException("String of form :xx has empty prefix", "World::prefix(\""+n+"\")");
    if (start != string::npos)     return n.substr(0, start);
    
    start=n.find(".");
    if (start==0) throw SEBException("String of form .xx has empty prefix", "World::prefix(\""+n+"\")");
    if (start != string::npos)     return n.substr(0, start);
    
    return n;
}

/*
Need a helper to count to occurances of a character to make the logic below more sharp.
*/

// Returns everything what is after the first :
// Throws if : does not exist, or : is at the end of the string.
string World::postfix(string n)
{
    int from=n.find(":");
    if (from == string::npos)     throw SEBException(": not found in string", "World::postfix(\""+n+"\")");
    from++;
    if (from == n.length())       throw SEBException("Strings of form xx: has empty postfix", "World::postfix(\""+n+"\")");

    return n.substr(from);      // return the postfix
}


// Returns the path part of a string like "structure:structure:subunit.ref" by discarding the reference part.
//  Throws if no .
string World::getPath(string n)
{
    int end=n.find(".");
    if (end == string::npos)     throw SEBException(". not found in string", "World::getPath(\""+n+"\")");
    if (end == 0)                throw SEBException("String of form .xx has no path", "World::getPath(\""+n+"\")");
    
    return n.substr(0, end);   // return the path
}

//  Returns the subname from a string of the form  [structures:]subunitname[.referencepoint.]  Thus everything from start or rigtht most : to . or end.
string World::getName(string n)
{
    int start=0;
    if (n.rfind(":") != string::npos)  start=n.rfind(":")+1;

    int end=n.length();
    if (n.find(".") != string::npos)  end=n.find(".");

    if (end<start)          throw SEBException("String of form .xxx: has no name", "World::getName(\""+n+"\")");
    if (start == end)       throw SEBException("String of form :. has no name", "World::getName(\""+n+"\")");

    return n.substr(start, end-start);   // return name part
}


/*  Returns the reference point part from a string, that is everything after . 
    Hence referencepoint#mypoint from e.g. structurenamae:..;subunitname.referencepoint[#mypoint]    
    Throws if the string does not contain .
*/
string World::getReference(string n)
{
    int start=n.rfind(".");
    if (start == -1)               start=n.find(".");
    if (start == string::npos)     throw SEBException("String without . has no reference found", "World::getReference(\""+n+"\")");
    if (start == n.length()-1)     throw SEBException("String of form xx. has no reference found", "World::getReference(\""+n+"\")");

    return n.substr(start+1);   // return reference part
}


//  Returns the refbase from a string of the form  [structures:]subunitname.refbase#mypoint.
//   Thus everything after . until #/end. Throws if no . found or if # is before .
string World::getReferenceBase(string n)
{
    int start=n.find(".");
    if (start == string::npos)     throw SEBException("String without .", "World::getReferenceBase(\""+n+"\")");
    start++;

    int end=n.find("#");
    if (end == string::npos)       return  n.substr(start);                  // CASE: .refbase
    if (start==end)                throw SEBException("String of form .# has no refpoint", "World::getReferenceBase(\""+n+"\")");
    if (end<start)                 throw SEBException("String of form #xx. has no refpoint", "World::getReferenceBase(\""+n+"\")");
    
    return  n.substr(start, end-start);       // CASE: .refbase#something
}


/*  Returns the refbase from a distributed reference point. The return value is identical to the getReferenceBase above, except that this
    method throws an exception unless both . and # are found.
*/
string World::getReferenceBaseHash(string n)
{
    int start=n.find(".");
    if (start == string::npos)     throw SEBException("String without .", "World::getReferenceBaseHash(\""+n+"\")");
    start++;

    int end=n.find("#");
    if (end == string::npos)       throw SEBException("String without #",  "World::getReferenceBaseHash(\""+n+"\")");
    if (end<start)                 throw SEBException("String of form #xx.", "World::getReferenceBaseHash(\""+n+"\")");
    if (start==end)                throw SEBException("String of form .# has no refpoint", "World::getReferenceBaseHash(\""+n+"\")");
    
    return n.substr(start, end-start);   // return reference type partm
}


/*  Returns everything from # to the end of the string.
    Throws if # or not found, or at the end of the string.
*/
string World::getReferenceAfterHash(string n)
{
    int start=n.find("#");
    if (start == string::npos)       throw SEBException("String without #",  "World::getReferenceAfterHash(\""+n+"\")");
    if (start+1 == n.length())       throw SEBException("String of form xx#", "World::getReferenceAfterHash(\""+n+"\")");

    return n.substr(start+1);   // return reference type partm
}



// Helper to check if r is in VisitedAlready
bool World::isVisited( refPoint& r, ReferencePointList& VisitedAlready)
{
   for (auto v : VisitedAlready)
       if (v==r) return true;

   return false;
}

/*
   Returns all the non-visited neighbours of the last reference point, that is

      Last is structure1:structure2:....:subunitname.reference

      then potentially we can jump through a link to another structure,
      or we can jump to any reference point inside this structure.
*/

ReferencePointList World::getNeighbors( refPoint last, ReferencePointList& VisitedAlready)
{
   ReferencePointList neighbors;
   
// First go through all links that could connect last out of the current structure
   for (auto itr : links)
     {
        refPoint next;
        if (itr.first == last) next = itr.second;
        if (itr.second == last) next = itr.first;

        if (next.empty()) continue;
        if (isVisited(next, VisitedAlready)) continue;

        neighbors.push_back(next);     
     }

// Then traverse inside the structure and find all reference points below prefix. (not already known)
   for (auto it : getReferencePoints(prefix(last)) )
         if (!isVisited(it, VisitedAlready) && it!=last)
               neighbors.push_back(it);
     
   return neighbors;
}

// Recursive funktion that does the search for structures / sub-units into a structure.
void World::addStructureReferences( refPoint r, ReferencePointList& ret )
{
    refPoint name=getName(r);                                                          // get right most structure/sub-unit in path.
    if (!hasName(name)) throw SEBException("Unknown name "+name,"addStructureReferences(\""+r+"\", ..)");
    
    if (isSubunit(name)) getSubunit(name)->addSubunitReferences(r, ret);
    else 
    if (isStructure(name))
            {
                GraphID gid=getStructure(name)->getGraphID();                           // get gid inside "name".
                for (auto it = subgraph_cbegin(gid); it != subgraph_cend(gid); it++)    // traverse that subgraph
                     addStructureReferences( r+":"+*it , ret);                          // and recursive call function to add below.
            }
    else throw SEBException("Should not happen!","addStructureReferences()");
}


// Code for generating a GENERIC Psi for a structure depending on various varforms
ex World::getPsi(string myself, refPoint r1, refPoint r2, int varform)
{
     if      (varform == BETA)     return ex(1);
     else if (varform == GUINIER)
                                   {
                                      ex q=GLEX->getSymbol("q");
                                      ex R2=GLEX->getSymbol("sigmaRrr2", myself, r1, r2);
                                      params[R2]=0;
                                      return ex(1)-q*q*R2/6;
                                   }
     else if (varform == ONE)      return ex(1);                                  
     else                          
                                   {
                                      ex psi=GLEX->getSymbol("Psi", myself, r1, r2);
                                      params[psi]=0;
                                      return psi;
                                   }
}        

// Code for generating a GENERIC Form Factor Amplitude for a structure depending on various varforms
ex World::getFFA(string myself, refPoint r, int varform)
{
     if      (varform == BETA)     
                                   {
                                      ex beta=GLEX->getSymbol("beta", myself);
                                      betas[beta]=0;
                                      return beta;
                                   }
     else if (varform == GUINIER)  {
                                      ex beta=GLEX->getSymbol("beta", myself);
                                      ex q=GLEX->getSymbol("q");
                                      ex R2=GLEX->getSymbol("sigmaRrs2", myself, r);
     
                                      params[R2]=0;
                                      betas[beta]=0;
     
                                      return beta*(1-q*q*R2/6);
                                   }
     else if (varform == ONE)      return ex(1);
     else                          {
                                      ex beta=GLEX->getSymbol("beta", myself);
                                      ex A=GLEX->getSymbol("A", myself, r );
                                      
                                      betas[beta]=0;
                                      params[A]=0;

                                      return beta*A;
                                   }
}        

// Code for generating a GENERIC Form Factor for a structure depending on various varforms
ex World::getFF(string myself, int varform)
{
     if      (varform == BETA)     {
                                      ex beta=GLEX->getSymbol("beta", myself);
                                      betas[beta]=0;
                                      return beta*beta;
                                   }
     else if (varform == GUINIER)  {
                                      ex beta=GLEX->getSymbol("beta", myself);
                                      ex q=GLEX->getSymbol("q");
                                      ex Rg2=GLEX->getSymbol("Rg2", myself);

                                      betas[beta]=0;
                                      params[Rg2]=0;
                                      
                                      return beta*beta*( 1-q*q*Rg2/3 );
                                   }
     else if (varform == ONE)      return ex(1);     
     else                          {  
                                      ex beta=GLEX->getSymbol("beta", myself);
                                      ex F=GLEX->getSymbol("F", myself );

                                      betas[beta]=0;                                      
                                      params[F]=0;

                                      return beta*beta*F;
                                   }
}        











