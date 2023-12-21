//===========================================================================
// Included guards
#ifndef INCLUDE_GUARD_SYMBOLINTERFACE
#define INCLUDE_GUARD_SYMBOLINTERFACE

//===========================================================================
// included dependencies

#include <sstream>
#include <ginac/ginac.h>
#include "Exceptions.hpp"


using namespace GiNaC;
using namespace std;

// Helpers to convert GiNaC expressions to string representation

string to_string_cform(ex);
string to_string_python(ex);
string to_string_latex(ex);
string to_string(ex);


/*
    This thin interface ensures that the symbolic variables always has the
    same format, such that they can be simplified automatically.
    
    One argument:
    Rg, X, L                            is Rg, X, L
    
    Two arguments:
    F  <subunit>                        is  F_subunit
    
    Three arguments:
    A <subunit> refpoint                is  A_subunit:refpoint
    
    Four arguments:
    Psi <subunit> refpoint1 refpoint2   is  Psi_subunit:refpoint1,refpoint2  (where refpoints are alphabetically sorted).
        
    The interface also generates latex output strings for these in case
    \beta or \Psi are part of the name.
*/

/*Symbol interface is a singleton that makes sure that a symbol is unique*/
class SymbolInterface{
    private:
    static SymbolInterface* myInstance;
    map<string, symbol> symbolDirectory;

// What is this used for?    
//    map<pair<string, string>, symbol> latexSymbolDirectory;
//    map<idx, ex> indexDirectory;
//    list<ex> expressionList;

    // Transforms a string for a symbol into a Latex formatted string.
    string string2latex(const string& s);

    // replaces all from -> to in str   (NB case sensitive)
    void replaceAll(std::string& str, const std::string& from, const std::string& to);

    // replaces psi. -> \psi_ in str
    void replacePsi(std::string& str);

    // replaces beta. -> \beta_ in str
    void replaceBeta(std::string& str);

    // replaces Rg. -> R_g in str
    void replaceRg(std::string& str);

 public:
     /*Symbol interface constructor*/
    SymbolInterface(){ }
    ~SymbolInterface(){};
    static SymbolInterface* instance();

    // From a string, return the corresponding symbol. This is required to ensure symbol object is unique for a given string.
    const symbol& get( const string& s );

    // Returns a standard utf8 symbol or if input is an array of size 2 it returns utf8 symbol and its latex format
    const symbol& getSymbol( const string& s );
    const symbol& getSymbol( const string& s, const string& index1 );
    const symbol& getSymbol( const string& s, const string& index1, const string& index2 );
    const symbol& getSymbol( const string& s, const string& index1, const string& index2, const string& index3 );

    // tests if user specified name is a valid string
    bool testnamestring(const string& s);

    // tests if user specified reference point is a valid string
    bool teststring(const string& s);

    symtab getSymbolTable();

//    template <typename... Args>
//    const symbol& getIndex( std::string s, std::string latex , Args... indices);
        
    //takes up to 5 ex and the first is a symbol the other 4 is indicies
    /*Variadic function templates in C++*/
    // template <typename ex, typename... exs> const ex  getIndex(ex var1, exs... var2);
    
    // const ex  getIndex( const ex s, const ex s1);
    // const ex  getIndex( const ex s, const ex s1, const ex s2);
    // const ex  getIndex( const ex s, const ex s1, const ex s2, const ex s3);
    // const ex  getIndex( const ex s, const ex s1, const ex s2, const ex s3, const ex s4);

//    list<ex> getSymbolList(){
//        return expressionList;
//    }
};

#endif
