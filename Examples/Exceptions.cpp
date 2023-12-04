#include "SEB.hpp"

/*
     Example of catching an exception.

       The default behaviour of SEB is to throw an exception if something
       unexpected happens, e.g. faulty input or internal errors.
       
       This is handled by the SEBException class.    
       
       This code outputs:
       Bad symbol in variable name:$%& at getSymbol(const string&)  
*/

int main()
{


try
{
//     SymbolInterface *GLEX = SymbolInterface::instance();
//     ex fail = GLEX->getSymbol("$%&");    // throws an exception since non-alpha numeric characters not allowed in string.
   
//   World w("World");
//   w.Add(new GaussianPolymer(), "polyA"); 
//   w.Link(new GaussianPolymer(), "polyB.end2", "polyA.noend"); 
   
     World w("World");
     GraphID g = w.Add(new GaussianPolymer(), "polyA"); 
     w.Add(2, "structure");
   
}

catch (const SEBException& e)
{
    std::cout << "Exception was caught:" << endl
              << "Error "<< e.what() << endl
              << "Where " << e.where() << endl;
}

}
