/* 
 * File:   SymbolTable.cpp
 * Author: Michael Frederick (n00725913)
 * 
 * Created on February 20, 2017, 5:50 PM
 */

#include "SymbolTable.h"

SymbolTable::SymbolTable(int size) {
    this->deadSymbol = new Symbol();
    this->size = this->getNextPrime(size);
    this->table = new Symbol[this->size];
    this->assignedValues = 0;
}

bool SymbolTable::addSymbol(Symbol* sym) {
    bool insertSuccess = false;
    int index = this->HashFunction(sym->getIdentifier());
    int collisions = 0;
    bool searching = true;
    
    while (searching) {
        if (this->table[index].isEqual(this->deadSymbol)) {
            searching = false;
            insertSuccess = true;
            this->table[index] = *sym;
            this->assignedValues++;
        }
        else if (collisions > 10) {
            index = (index + 1) % this->size;
        }
        else if (collisions == this->size) {
            // Something done broke
            searching = false;
        }
        else {
            collisions++;
            index = ((index * index) * collisions) % this->size;
        }
    }
        
    return insertSuccess;
}

Symbol SymbolTable::getSymbol(std::string) {
    return *this->deadSymbol;
}

Symbol SymbolTable::getDeadSymbol() {
    return *this->deadSymbol;
}

int SymbolTable::HashFunction(std::string str) {
    int value = 1;
    for (int i = 0; i < str.length(); i++) {
        value = (value * (26 + str[i])) % this->size;
    }
    return value;
}

int SymbolTable::getNextPrime(int num) {
    // 9973 is the highest prime number
    for (int i = num; i <= 9973; i++) {
        if (this->isPrime(i) && (i / 2) > num) {
            return i;
        }
    }
    return 9973;
}

bool SymbolTable::isPrime(int p) {
    for (int i = 2; (i * i) <= p; i++) {
        if ((p % i) == 0) {
            return false;
        }
    }
    return true;
}

void SymbolTable::printTable() {
    Symbol sym;
    std::cout << " ---Symbol Table--- " << std::endl;
    std::cout << "Identifier | Scope | Location" << std::endl;
    for (int i = 0; i < this->size; i++) {
        sym = this->table[i];
        if (!sym.isEqual(this->deadSymbol)) {
            std::cout << sym.getIdentifier() << " | " << sym.getDepth() << " | " << i << std::endl;
        }
    }
    return;
}