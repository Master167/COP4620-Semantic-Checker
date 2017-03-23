/* 
 * File:   Symbol.cpp
 * Author: Michael Frederick (n00725913)
 * 
 * Created on February 20, 2017, 5:49 PM
 */

#include "Symbol.h"

Symbol::Symbol() {
    this->identifier = "$DEAD$";
    this->declaredScope = "";
    this->type = "";
    this->value = "";
    this->isFunction = false;
    this->numberOfParams = -1;
    this->nextSymbol = NULL;
}

Symbol::Symbol(std::string identifier, std::string scope) {
    this->identifier = identifier;
    this->declaredScope = scope;
    this->type = "";
    this->value = "";
    this->isFunction = false;
    this->numberOfParams = -1;
    this->nextSymbol = NULL;
}

std::string Symbol::getIdentifier() {
    return this->identifier;
}

std::string Symbol::getDeclaredScope() {
    return this->declaredScope;
}

std::string Symbol::getType() {
    return this->type;
}

std::string Symbol::getValue() {
    return this->value;
}

void Symbol::setType(std::string type) {
    if (this->type.compare("") == 0) {
        // Hasn't been set yet
        this->type = type;
    }
    else {
        this->throwFloatException();
    }
    return;
}

void Symbol::setValue(std::string value) {
    if (this->identifier.compare("$DEAD$") != 0) {
        this->value = value;
    }
    else {
        this->throwFloatException();
    }
    return;
}

void Symbol::changeIsFunction() {
    if (!this->isFunction && this->identifier.compare("$DEAD$") != 0) {
        this->isFunction = true;
    }
    else {
        this->throwFloatException();
    }
    return;
}

void Symbol::setNumberOfParams(int number) {
    if (this->isFunction) {
        this->numberOfParams = number;
    }
    else {
        this->throwFloatException();
    }
    return;
}

void Symbol::linkNextSymbol(Symbol* nextSym) {
    if (!this->hasNextSymbol()) {
        this->nextSymbol = nextSym;
    }
    else {
        this->throwFloatException();
    }
    return;
}

bool Symbol::hasNextSymbol() {
    return (this->nextSymbol == NULL);
}

bool Symbol::isEqual(Symbol* sym) {
    bool equal = false;
    
    // Check size
    if (sym->getIdentifier().length() == this->getIdentifier().length() && sym->declaredScope == this->declaredScope) {
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

void Symbol::throwFloatException() throw(float) {
    throw -1.0;
    return;
}
