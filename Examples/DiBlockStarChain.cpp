// Standard C++ headers
#include<iostream>

// Include SEB functionality
#include "SEB.hpp"

/*

    In this example we generate a 5-chain of stars, where each star has 4 di-block copolymer arms.

    
    A diblock copolymer is generated by a structure consisting of polyA and polyB:
    
                 polyA                           polyB
        x----------------------------x = x----------------------------x                     gid=1
         end1                    end2     end1                    end2
    
     The reference points defining the link (=) is polyB.end1 <-> polyA.end2,
     and both polyA and polyB are GaussianPolymers.
    
 
 
    A star structure created by linking four diblock copolymers, which requires 3 links.
               
   Star centre                    corona
   
              polyA         polyB           name      direction below
        x------------x---------------x     diblock1  (left)
        | end1
        x------------x---------------x     diblock2  (up)                                   gid=2
        | end1
        x------------x---------------x     diblock3  (right)
        | end1
        x------------x---------------x     diblock4  (down)
          end1   end2  end1         end2
          
     The reference points defining the star is the linkages (|):
     
         diblock2.polyA.end1 <-> diblock1.polyA.end1
         diblock3.polyA.end1 <-> diblock1.polyA.end1
         diblock4.polyA.end1 <-> diblock1.polyA.end1
    
    Such that all *.polyA.end1 is the centre, halfway out from the centre are are *.polyA.end2 = *.polyB.end1,
    and the free end of the polymers are *.polyB.end2
    
    
    A chain of stars is created by linking (4) give starts by the ends of two arms:
    
         
         |       |       |       |       |
       --*-- = --*-- = --*-- = --*-- = --*--                                                  gid=3
         |       |       |       |       |
       
      star1    star2    star3  star4    star5
      
     Linkage points are
         star2:diblock1.polyB.end2 <-> star1:diblock3:polyB.end2
         star3:diblock1.polyB.end2 <-> star2:diblock3:polyB.end2
         star4:diblock1.polyB.end2 <-> star3:diblock3:polyB.end2
         star5:diblock1.polyB.end2 <-> star4:diblock3:polyB.end2
               
*/

int main()
{
 try{
    World w = World("w");

    // Define diblock copolymer
    GraphID diblock = w.Add(new GaussianPolymer(), "polyA");
    w.Link(new GaussianPolymer(),  "polyB.end1", "polyA.end2");
    
    // Define star
    GraphID star = w.Add(diblock, "diblock1");
    w.Link(diblock,"diblock2:polyA.end1", "diblock1:polyA.end1");
    w.Link(diblock,"diblock3:polyA.end1", "diblock1:polyA.end1");
    w.Link(diblock,"diblock4:polyA.end1", "diblock1:polyA.end1");

    // Define chain
    GraphID chain = w.Add(star, "star1");
    w.Link(star,"star2:diblock1:polyB.end2", "star1:diblock3:polyB.end2");
    w.Link(star,"star3:diblock1:polyB.end2", "star2:diblock3:polyB.end2");
    w.Link(star,"star4:diblock1:polyB.end2", "star3:diblock3:polyB.end2");
    w.Link(star,"star5:diblock1:polyB.end2", "star4:diblock3:polyB.end2");

    // Name chain structure name
    w.Add(chain, "chain");

    // Define latex as ouput format
    cout << latex;
    cout << "Phasefactor: -----------------------------------------------------\n";
    cout <<  w.PhaseFactor("chain:star1:diblock1:polyB.end2","chain:star1:diblock3:polyB.end2") << endl;

    cout << "Formfactor Amplitude: -----------------------------------------------------\n";
    cout <<  w.FormFactorAmplitude("chain:star1:diblock1:polyB.end2") << endl;

    cout << "Formfactor : -----------------------------------------------------\n";
    cout <<  w.FormFactor("chain") << endl;

}
catch (const SEBException e)
{
    std::cout << e;                    // Print what the error was, and where it was triggered.
}



}
