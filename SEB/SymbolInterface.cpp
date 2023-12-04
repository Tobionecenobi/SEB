#include "SymbolInterface.hpp"

using namespace GiNaC;
using namespace std;

/*Resets Symbol interface*/
SymbolInterface* SymbolInterface::myInstance = 0;

/*In first call to instance, generate symbol interface class*/
SymbolInterface* SymbolInterface::instance(){
    if( !myInstance ){
        myInstance = new SymbolInterface();
    }
    return myInstance;
}

/*
    Replaces all occurances with the from string to the to string in str.
    PROBLEM: we need case insensitive replace.
*/
void SymbolInterface::replaceAll(std::string& str, const std::string& from, const std::string& to) {
    if(from.empty()) return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

/*Replace psi only is the first four characters are psi_*/
void SymbolInterface::replacePsi(std::string& str){
    //Check that "psi" is the first characters in the string and the next character is "_"
    if(str.substr(0,4) == "psi_" || str.substr(0,4) == "Psi_"){
        str.replace(0,4,"\\psi_");
    }
}

/*Replace beta only if the first five characters are beta_*/
void SymbolInterface::replaceBeta(std::string& str){
    //Check that "beta" is the first characters in the string and the next character is "_"
    if(str.substr(0,5) == "beta_" || str.substr(0,5) == "Beta_"){
        str.replace(0,5,"\\beta_");
    }
}

/*Replace Rg only if the first two characters are Rg*/
void SymbolInterface::replaceRg(std::string& str){
    //Check that "Rg" is the first characters in the string and the next character is "_"
    if(str.substr(0,3) == "Rg_" || str.substr(0,3) == "rg_"){
        str.replace(0,2,"R_g");
        str.insert(str.size(), "}");
    }
}

string SymbolInterface::string2latex(const string& s)
/*
   String is one of F_<string>, A_<string>,<refstring>, Psi_<string>,<refstring>,<refstring>
   Here we transform one of these strings into LaTeX formatted expressions.
   
   f_<string>   ->  F_{string}
      
*/
{
   string latex(s);
   
   //Find the first occurance of _
    size_t pos=latex.find("_");

   // Transforms greek symbols
   replacePsi(latex);
   replaceBeta(latex);
   replaceRg(latex);
   replaceAll(latex, "#", "\\#" );
   
   // replace subscript _<string> with _{string}
   size_t found=latex.find("_" );
   if (found!=std::string::npos)     // We have a subscript
       {
          replaceAll(latex, "_", "_{");
          latex+="}";
       }
   
   return latex;
}

/*   A-Za-z0-9 and # is allowed.*/
bool SymbolInterface::testnamestring(const string& s)
{
    size_t pos=0;
    
    while (pos<s.size()-1)   // Don't test trailing zero.
      {
        if(!isalnum(s[pos])) return false;
        pos++;
      }
   return true;
}


/*Test strings which can contain A-Za-z0-9, :.# is also allowed */
bool SymbolInterface::teststring(const string& s)
{
    size_t pos=0;
    
    while (pos<s.size()-1)   // Don't test trailing zero.
      {
        if(!isalnum(s[pos]) && s[pos]!=':'  && s[pos]!='#' && s[pos]!='.') return false;
        pos++;
      }
   return true;
}


    
/*
   Gets a symbol based on a supplied string. If the string is already known, return the
   symbol we created the last time.
*/

const symbol& SymbolInterface::get( const string& s ){

// Store lowercase name as key to retrieve symbol
    string ss(s);  //transform(ss.begin(), ss.end(), ss.begin(), ::tolower);

    auto it = symbolDirectory.find( ss );
    if( it != symbolDirectory.end()) return it -> second; 
                                else return symbolDirectory.insert( make_pair( ss , symbol(s,string2latex(s)) )).first->second;
}


// For instance Rg, L, X
const symbol& SymbolInterface::getSymbol( const string& s ){

    if (!testnamestring(s)) throw SEBException("Bad symbol in variable name:"+s,"getSymbol(const string&)");  
    return get(s);
}

// For instance F  <subunit>
const symbol& SymbolInterface::getSymbol( const string& s, const string& tag ){

    if (!teststring(s))      throw SEBException("Bad symbol in variable name:"+s,"getSymbol(const string&,const string&)");
    if (!teststring(tag))    throw SEBException("Bad symbol in variable name:"+tag,"getSymbol(const string&,const string&)");

    string si = s+"_"+tag;
    return get(si);
}

// s_tag:ref
const symbol& SymbolInterface::getSymbol( const string& s, const string& tag, const string& ref ){

    if (!teststring(s))      throw SEBException("Bad symbol in variable name:"+s  , "getSymbol(const string&,const string&,const string&)");
    if (!teststring(tag))    throw SEBException("Bad symbol in variable name:"+s  , "getSymbol(const string&,const string&,const string&)");
    if (!teststring(ref))     throw SEBException("Bad symbol in index name:"+tag , "getSymbol(const string&,const string&,const string&)");

    string si;
    si = s+"_"+tag+":"+ref;
    return get(si);
}

//  E.g.  psi   sub_unit/structure   r1, r2
const symbol& SymbolInterface::getSymbol(const string& s, const string& tag, const string& index1, const string& index2 ){

    if (!teststring(s))      throw SEBException("Bad symbol in variable name:"+s,"getSymbol(const string&)");  
    if (!teststring(tag))    throw SEBException("Bad symbol in variable name:"+tag  , "getSymbol(const string&,const string&,const string&)");
    if (!teststring(index1))  throw SEBException("Bad symbol in index name:"+index1 , "getSymbol(const string&,const string&,const string&)");
    if (!teststring(index2))  throw SEBException("Bad symbol in index name:"+index2 , "getSymbol(const string&,const string&,const string&)");

    string si;
    if (index1<index2) si = s+"_"+tag+":"+index1+","+index2;
                  else si = s+"_"+tag+":"+index2+","+index1;

    return get(si);
}

// Provide symbol table in GiNAC data structure. GINAC Parsing fails because : not allowed in identifiers.
symtab SymbolInterface::getSymbolTable()
{
    symtab table;
    
    for (auto const& x : symbolDirectory)
         table[x.first]=x.second;

    return table;
}




string to_string_cform(ex e)
{
    ostringstream oss;
    oss << csrc_double << e;
    string tmp=oss.str();
    return tmp;
}

string to_string_python(ex e)
{
    ostringstream oss;
    oss << python << e;
    string tmp=oss.str();
    return tmp;
}

string to_string_latex(ex e)
{
    ostringstream oss;
    oss << latex << e;
    string tmp=oss.str();
    return tmp;
}

string to_string(ex e)
{
    ostringstream oss;
    oss << e;
    string tmp=oss.str();
    return tmp;
}



