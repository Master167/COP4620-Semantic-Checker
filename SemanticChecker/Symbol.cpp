/* 
 * File:   Symbol.cpp
 * Author: Michael Frederick (n00725913)
 * 
 * Created on February 20, 2017, 5:49 PM
 */

#include "Symbol.h"

Symbol::Symbol() {
    this->identifier = "$DEAD$";
    this->depth = 0;
}

Symbol::Symbol(std::string identifier, int depth) {
    this->identifier = identifier;
    this->depth = depth;
}

std::string Symbol::getIdentifier() {
    return this->identifier;
}
int Symbol::getDepth() {
    return this->depth;
}

bool Symbol::isEqual(Symbol* sym) {
    bool equal = false;
    
    // Check size
    if (sym->getIdentifier().length() == this->getIdentifier().length() && sym->depth == this->depth) {
        equal = true;
        for (int i = 0; i < this->getIdentifier().length(); i++) {
            if (this->getIdentifier()[i] != sym->getIdentifier()[i]) {
                equal = false;
                break;
            }
        }
    }
    return equal;
}
