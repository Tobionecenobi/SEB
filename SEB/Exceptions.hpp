//===========================================================================
// Included guards
#ifndef INCLUDE_GUARD_EXCEPTIONS
#define INCLUDE_GUARD_EXCEPTIONS


#include <exception>
#include <iostream>
#include <string>

#ifdef EXCEPTIONCOREDUMP
#include <cstdlib>
#endif

// If EXCEPTIONCOREDUMP defined in the make file, then SEB will abort when an exception is raised.
// This is useful for debugging. The default is to raise an exception and then let the user worry about the response in a standard catch.
//
// NB. to store cores remember:
// ulimit -c unlimited
//

using namespace std;

class SEBException : public exception
{
    string e="";      // What is the error
    string w="";      // Where did it occur.
    
public:

    SEBException(const char* err) : exception(), e(err)
       {
#ifdef EXCEPTIONCOREDUMP
        cout << err << "\n";
        abort(); 
#endif        
       }
    
    SEBException(const string err) : exception(), e(err)
       {
#ifdef EXCEPTIONCOREDUMP
        cout << err << "\n";
        abort(); 
#endif        
       }
    
    SEBException(const char* err, const char* where) : exception(), e(err),w(where)
       {
#ifdef EXCEPTIONCOREDUMP
        cout << err << " at " << where << "\n";
        abort(); 
#endif        
       }
    
    SEBException(const string err, const string where) : exception(), e(err),w(where)
       {
#ifdef EXCEPTIONCOREDUMP
        cout << err << " at " << where << "\n";
        abort(); 
#endif        
       }

    void PushCallStack(string c) { w+=c+"\n"; }

    virtual const char* what()  const noexcept { return e.c_str(); }
    virtual const char* where() const noexcept { return w.c_str(); }
    
    friend ostream& operator<<(ostream& out, const SEBException e)
    {
      out << "Exception \'"+e.e+"\' triggered at \'"+e.w << "\'" << endl;
      return out;
    }
};

#endif
