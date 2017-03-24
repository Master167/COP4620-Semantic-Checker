/* 
 * File:   Parser.h
 * Author: Michael Frederick (n00725913)
 *
 * Created on February 24, 2017, 6:15 PM
 */

#ifndef PARSER_H
#define PARSER_H

#include <fstream>
#include <string>
#include <iostream>
#include <new>
#include <cctype>
#include <sstream>
#include "SymbolTable.h"
#include "Symbol.h"

class Parser {
public:
    Parser(std::fstream& inputFile);
    bool parseFile(SymbolTable* symTable);
private:
    // Class Variables
    std::string currentToken;
    std::fstream& filestream;
    
    // Added for Project Three
    Symbol* lastSymbol;
    Symbol* functionSymbol;
    SymbolTable* symTab;
    std::string currentScope;
    std::string functionId;
    std::string lastId;
    std::string lastType;
    int numberOfParamSeen;
    // Class Methods
    bool getNextToken();
    bool acceptToken(std::string token);
    void throwBadAcceptToken(std::string badToken, std::string expectedToken) throw(std::string);
    void throwException() throw (int);
    void throwFloatException() throw(float);
    bool searchArray(int arraySize, std::string *array, std::string key);
    
    // Grammar Methods
    void program();
    void declarationList();
    void declarationListPrime();
    void declaration();
    void callDeclaration();
    void idSpecifier();
    void typeSpecifier();
    void params();
    void paramList();
    void paramListPrime();
    void param();
    void array();
    void compountStmt();
    void localDeclarations();
    void localDeclarationsPrime();
    void statementList();
    void statementListPrime();
    void statement();
    void expressionStmt();
    void selectionStmt();
    void danglingElse();
    void iterationStmt();
    void returnStmt();
    void returnStmtEnd();
    void expression();
    void variable();
    void variableFactor();
    void varArray();
    void relopExpression();
    void relop();
    void additiveExpression();
    void additiveExpressionPrime();
    void addop();
    void term();
    void termPrime();
    void factor();
    void mulop();
    void varCall();
    void args();
    void argList();
    void argListPrime();
};

#endif /* PARSER_H */

