// Standard C++ headers
#include<iostream>

// Include SEB functionality
#include "SEB.hpp"

/*
     SEB uses GiNaC to represent symbols and mathematical mathematical expressions.
     
     Symbols are things like x, y, F_chain, Psi_polymer and so on.
     
     A major issue with GiNaC is if say the symbol "x" is defined multiple places.
     But GiNaC identifies symbol via an internal reference, hence it is possible
     to define different symbols that shares the same name, say x, x, and x, that are
     all different symbols, but has the same name. This creates problems!! 

     SEB solves this problem with SymbolInterface where you always request
     symbols via getSymbol. If the symbol is new, it is created, and if it
     is already known, a reference to the previous symbol is returned.

*/

int main()
{
  /*Pointer to symbol interface*/
  SymbolInterface *GLEX = SymbolInterface::instance();

  // generate expression
  ex alpha = GLEX->getSymbol("alpha");

  // generate expression
  ex F = GLEX->getSymbol("F","chain");

  // generate expression
  ex A = GLEX->getSymbol("A","polymer","end1");

  // generate expression
  ex P = GLEX->getSymbol("Psi","polymer","end1","end2");

  // Mathematical expression combining several symbols
  ex test=alpha*F+2*A*P*A;

  cout  << "Ginac expression:" << test << endl;
  cout  << "Ginac expression (latex):"<< latex << test << endl; 
} 
