/* 
 * File:   Symbol.h
 * Author: Michael Frederick (n00725913)
 *
 * Created on February 20, 2017, 5:49 PM
 */

#ifndef SYMBOL_H
#define SYMBOL_H

#include <string>

class Symbol {
public:
    //Constructor
    Symbol();
    Symbol(std::string identifier, int depth);
    //Variables
    
    //Functions
    std::string getIdentifier();
    int getDepth();
    bool isEqual(Symbol* sym);
private:
    //Variables
    std::string identifier;
    int depth;
    //std::string type; // I'll build unto this later
    //std::string value;
    
    //Functions
};

#endif /* SYMBOL_H */
