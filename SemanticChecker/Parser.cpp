/* 
 * File:   Parser.cpp
 * Author: Michael Frederick (n00725913)
 * 
 * Created on February 24, 2017, 6:15 PM
 */

#include "Parser.h"


Parser::Parser(std::fstream& inputFile) : filestream(inputFile) {
}

bool Parser::parseFile(SymbolTable* symTable) {
    this->symTab = symTable;
    this->lastSymbol = new Symbol();
    this->functionSymbol = new Symbol();
    this->currentScope = "";
    this->lastId = "";
    this->functionId = "";
    this->numberOfParamSeen = -1;
    this->seenMainFunction = false;
    this->seenReturnStmt = false;
    bool result = true;
    if (this->getNextToken()) {
        try {
            this->program();
            if (!this->seenMainFunction) {
                // I have not seen a main function
                result = false;
            }
        }
        catch (int e) {
            std::cout << "Int Exception thrown" << std::endl;
            std::cout << "CurrentToken: " << this->currentToken << std::endl;
            result = false;
        }
        catch (float e) {
            std::cout << "Float Exception thrown" << std::endl;
            std::cout << "CurrentToken: " << this->currentToken << std::endl;
            result = false;
        }
        catch (std::string e) {
            std::cout << e << std::endl;
            result = false;
        }
    }
    else {
        std::cout << "Unable to pull first token" << std::endl;
        result = false;
    }
    return result;
}

bool Parser::getNextToken() {
    std::cout << "Accepted: " << this->currentToken << std::endl;
    bool result = false;
    std::string temp;
    while (temp.length() <= 0) {
        if (!std::getline(this->filestream, temp)) {
            break;
        }
    }
    if (temp.length() > 0) {
        result = true;
        this->currentToken = temp;
    }
    else {
        this->currentToken = "";
    }
    return result;
}

bool Parser::acceptToken(std::string token, bool addSymbol) {
    bool result = true;
    std::size_t findResult;
    std::string keyword;
    Symbol* resultSymbol;
    
    if (token.compare("id") == 0) {
        findResult = this->currentToken.find("ID:");
        if (findResult != std::string::npos) {
            // ADD TO SYMBOL TABLE 
            if (this->symTab != NULL) { 
                Symbol* sym = new Symbol(this->currentToken, this->currentScope);
                resultSymbol = this->symTab->addSymbol(sym);
                if (addSymbol && (resultSymbol != sym)) {
                    this->throwFloatException();
                }
                this->lastSymbol = resultSymbol;
                if (this->lastSymbol->getType().compare("") != 0) {
                    this->lastType = this->lastSymbol->getType();
                }
            }
            this->lastId = token;
            result = this->getNextToken();
        }
        else {
            this->throwBadAcceptToken(this->currentToken, token);
            result = false;
        }
    }
    else if (token.compare("num") == 0) {
        // Check if float
        findResult = this->currentToken.find("FLOAT");
        if (findResult == std::string::npos) {
            // Ok, FLOAT is not in the currentToken
            findResult = this->currentToken.find("INT");
            if (findResult == std::string::npos) {
                // INT is not in the currentToken. This is not a NUM
                this->throwBadAcceptToken(this->currentToken, token);
                result = false;
            }
            else {
                result = this->getNextToken();
                this->lastType = "int";
            }
        }
        else {
            result = this->getNextToken();
            this->lastType = "float";
        }
    }
    else if (this->currentToken.compare(token) == 0) {
        result = this->getNextToken();
    }
    else {
        /*
         * Check for KEYWORD
         */
        findResult = this->currentToken.find(token);
        if (findResult != std::string::npos) {
            //ID: We're not checking if ID has been declared, that's a semantic problem.
            //ERROR: We're never going to accept a error token. Therefore it will never be found in .find()
            //KEYWORD: we need to check the KEYWORD
            //NUM: can be INT or FLOAT and indicates a Number constant
            findResult = this->currentToken.find("KEYWORD: ");
            if (findResult != std::string::npos) {
                // Need to remove "KEYWORD: " from string to check if token is the same
                keyword = this->currentToken.erase(findResult, 9);
                if (keyword.compare(token) == 0) {
                    result = this->getNextToken();
                }
                else {
                    // Not the same keyword
                    this->throwBadAcceptToken(this->currentToken, token);
                    result = false;
                }
            }
        }
        else {
            // token is not inside this->currentToken
            this->throwBadAcceptToken(this->currentToken, token);
            result = false;
        }
    }
    return result;
}

void Parser::throwBadAcceptToken(std::string badToken, std::string expectedToken) throw(std::string) {
    std::string message = "Expected: " + expectedToken + " Found: " + badToken;
    throw message;
    return;
}

void Parser::throwException() throw (int) {
    throw -1;
    return;
}

void Parser::throwFloatException() throw(float){
    throw (float)-1.0;
    return;
}

bool Parser::searchArray(int arraySize, std::string *array, std::string key) {
    bool result = false;
    // Need to clean the key a tad. To remove text added by LexicalAnalyzer
    //  -Ex: Remove 'KEYWORD: ' from a keyword
    std::string temp;
    std::string compareToken = this->currentToken;
    std::size_t findResult = compareToken.find("KEYWORD: ", 0);
    if (findResult != std::string::npos) {
        compareToken.erase(findResult, 9);
    }
    else {
        findResult = compareToken.find("INT: ", 0);
        if (findResult != std::string::npos) {
            compareToken = "num";
        }
        else {
            findResult = compareToken.find("FLOAT: ", 0);
            if (findResult != std::string::npos) {
                compareToken = "num";
            }
            else {
                findResult = compareToken.find("ID: ", 0);
                if (findResult != std::string::npos) {
                    compareToken = "id";
                }
            }
        }
    }
    
    for (int i = 0; i < arraySize; i++) {
        temp = array[i];
        if (temp.compare(compareToken) == 0) {
            result = true;
            break;
        }
    }
    return result;
}

// Grammar Methods
void Parser::program() {
    this->declarationList();
    return;
}

void Parser::declarationList() {
    this->declaration();
    this->declarationListPrime();
    return;
}

void Parser::declarationListPrime() {
    std::string firstSet[3] = { "int", "float", "void" };
    if (this->searchArray(3, firstSet, this->currentToken)) {
        this->declaration();
        this->declarationListPrime();
    }
    else {
        // Follow set for declaration-list-prime is the end of file and it can go empty
    }
    return;
}

void Parser::declaration() {
    std::string firstSet[3] = { "int", "float", "void" };
    if (this->searchArray(3, firstSet, this->currentToken)) {
        this->typeSpecifier();
        this->acceptToken("id", true);
        this->lastSymbol->setType(this->lastType);
        this->callDeclaration();
        this->lastId = "";
        this->lastType = "";
        this->lastSymbol = new Symbol();
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::callDeclaration() {
    std::string idFirst[2] = { ";", "[" };
    if (this->searchArray(2, idFirst, this->currentToken)) {
        this->idSpecifier();
    }
    else if (this->currentToken.compare("(") == 0) {
        if (this->lastSymbol->getIdentifier().compare("ID: main") == 0) {
            this->seenMainFunction = true;
        }
        this->lastSymbol->changeIsFunction();
        this->functionSymbol = this->lastSymbol;
        this->seenReturnStmt = false;
        this->currentScope = this->functionSymbol->getIdentifier();
        this->acceptToken("(", false);
        this->params();
        this->acceptToken(")", false);
        this->compountStmt();
        // The function is a non-void type and no returnStmt was seen
        if (this->functionSymbol->getType().compare("void") != 0 && !this->seenReturnStmt) {
            this->throwFloatException();
        }
        this->functionSymbol = new Symbol();
        this->currentScope = "";
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::idSpecifier() {
    if (this->currentToken.compare(";") == 0) {
        this->acceptToken(";", false);
    }
    else if (this->currentToken.compare("[") == 0) {
        this->acceptToken("[", false);
        this->acceptToken("num", false);
        this->acceptToken("]", false);
        this->lastSymbol->changeIsArray();
        this->acceptToken(";", false);
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::typeSpecifier() {
    std::string first[1] = { "int" };
    std::string second[1] = { "float" };
    std::string third[1] = { "void" };
    if (this->searchArray(1, first, this->currentToken)) {
        this->acceptToken("int", false);
        this->lastType = "int";
    }
    else if (this->searchArray(1, second, this->currentToken)) {
        this->acceptToken("float", false);
        this->lastType = "float";
    }
    else if (this->searchArray(1, third, this->currentToken)) {
        this->acceptToken("void", false);
        this->lastType = "void";
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::params() {
    std::string first[1] = { "int" };
    std::string second[1] = { "float" };
    std::string third[1] = { "void" };
    this->numberOfParamSeen = 0;
    if (this->searchArray(1, first, this->currentToken)) {
        this->acceptToken("int", false);
        this->acceptToken("id", true);
        this->lastSymbol->setType("int");
        this->numberOfParamSeen++;
        this->array();
        this->paramListPrime();
    }
    else if (this->searchArray(1, second, this->currentToken)) {
        this->acceptToken("float", false);
        this->acceptToken("id", true);
        this->lastSymbol->setType("float");
        this->numberOfParamSeen++;
        this->array();
        this->paramListPrime();
    }
    else if (this->searchArray(1, third, this->currentToken)) {
        this->acceptToken("void", false);
        this->paramList();
    }
    else {
        this->throwException();
    }
    this->functionSymbol->setNumberOfParams(this->numberOfParamSeen);
    return;
}

void Parser::paramList() {
    std::string first[1] = { "id" };
    if (this->searchArray(1, first, this->currentToken)) {
        // Can't have a void param
        this->throwFloatException();
        // From Parser
        //this->acceptToken("id", true);
        //this->numberOfParamSeen++;
        //this->array();
        //this->paramListPrime();
    }
    else if (this->currentToken.compare(")") == 0) {
        // Go to empty
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::paramListPrime() {
    if (this->currentToken.compare(",") == 0) {
        this->acceptToken(",", false);
        this->param();
        this->paramListPrime();
    }
    else if (this->currentToken.compare(")") == 0) {
        // Go to empty
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::param() {
    std::string first[3] = { "int", "float", "void" };
    if (this->searchArray(3, first, this->currentToken)) {
        this->typeSpecifier();
        this->acceptToken("id", true);
        this->lastSymbol->setType(this->lastType);
        this->array();
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::array() {
    std::string follow[2] = { ",", ")" };
    if (this->currentToken.compare("[") == 0) {
        this->acceptToken("[", false);
        this->acceptToken("]", false);
        this->lastSymbol->changeIsArray();
    }
    else if (this->searchArray(2, follow, this->currentToken)) {
        // Go to empty
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::compountStmt() {
    if (this->currentToken.compare("{") == 0) {
        this->acceptToken("{", false);
        this->localDeclarations();
        this->statementList();
        this->acceptToken("}", false);
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::localDeclarations() {
    std::string first[3] = { "int", "float", "void" };
    std::string follow[9] = { "id", "(", "num", ";", "{", "if", "while", "return", "}" };
    if (this->searchArray(3, first, this->currentToken)) {
        this->localDeclarationsPrime();
    }
    else if (this->searchArray(9, follow, this->currentToken)) {
        // Go to empty
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::localDeclarationsPrime() {
    std::string first[3] = { "int", "float", "void" };
    std::string follow[9] = { "id", "(", "num", ";", "{", "if", "while", "return", "}" };
    if (this->searchArray(3, first, this->currentToken)) {
        this->typeSpecifier();
        this->acceptToken("id", true);
        this->lastSymbol->setType(this->lastType);
        this->idSpecifier();
        this->localDeclarationsPrime();
        this->lastId = "";
        this->lastType = "";
        this->lastSymbol = new Symbol();
    }
    else if (this->searchArray(9, follow, this->currentToken)) {
        // Go to empty
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::statementList() {
    std::string first[8] = { "id", "(", "num", ";", "{", "if", "while", "return" };
    if (this->searchArray(8, first, this->currentToken)) {
        this->statementListPrime();
    }
    else if (this->currentToken.compare("}") == 0) {
        // Go to empty
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::statementListPrime() {
    std::string first[8] = { "id", "(", "num", ";", "{", "if", "while", "return" };
    if (this->searchArray(8, first, this->currentToken)) {
        this->statement();
        this->statementListPrime();
    }
    else if (this->currentToken.compare("}") == 0) {
        // Go to empty
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::statement() {
    std::string first[4] = { "id", "(", "num", ";" };
    std::string second[1] = { "{" };
    std::string third[1] = { "if" };
    std::string fourth[1] = { "while" };
    std::string fifth[1] = { "return" };
    if (this->searchArray(4, first, this->currentToken)) {
        this->expressionStmt();
    }
    else if (this->searchArray(1, second, this->currentToken)) {
        this->compountStmt();
    }
    else if (this->searchArray(1, third, this->currentToken)) {
        this->selectionStmt();
    }
    else if (this->searchArray(1, fourth, this->currentToken)) {
        this->iterationStmt();
    }
    else if (this->searchArray(1, fifth, this->currentToken)) {
        this->returnStmt();
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::expressionStmt() {
    std::string result = "";
    std::string first[3] = { "id", "(", "num" };
    if (this->searchArray(3, first, this->currentToken)) {
        result = this->expression();
        this->acceptToken(";", false);
    }
    else if (this->currentToken.compare(";") == 0) {
        this->acceptToken(";", false);
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::selectionStmt() {
    std::string result = "";
    std::string first[1] = { "if" };
    if (this->searchArray(1, first, this->currentToken)) {
        this->acceptToken("if", false);
        this->acceptToken("(", false);
        result = this->expression();
        this->acceptToken(")", false);
        this->statement();
        this->danglingElse();
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::danglingElse() {
    std::string first[1] = { "else" };
    std::string follow[9] = { "id", "(", "num", ";", "{", "if", "while", "return", "}" };
    if (this->searchArray(1, first, this->currentToken)) {
        this->acceptToken("else", false);
        this->statement();
    }
    else if (this->searchArray(9, follow, this->currentToken)) {
        // Go to empty
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::iterationStmt() {
    std::string result = "";
    std::string first[1] = { "while" };
    if (this->searchArray(1, first, this->currentToken)) {
        this->acceptToken("while", false);
        this->acceptToken("(", false);
        result = this->expression();
        this->acceptToken(")", false);
        this->statement();
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::returnStmt() {
    std::string first[1] = { "return" };
    if (this->searchArray(1, first, this->currentToken)) {
        this->acceptToken("return", false);
        this->seenReturnStmt = true;
        this->returnStmtEnd();
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::returnStmtEnd() {
    std::string result = "";
    std::string first[3] = { "id", "(", "num" };
    if (this->searchArray(3, first, this->currentToken)) {
        result = this->expression();
        this->acceptToken(";", false);
        if (this->functionSymbol->getType().compare("void") != 0) {
            // It's a float or int function
            if (this->functionSymbol->getType().compare(result) != 0) {
                // Wrong expression type
                this->throwFloatException();
            }
        }
        else {
            // It's a void function
            if (result.compare("") != 0) {
                // Returning something on void
                this->throwFloatException();
            }
        }
    }
    else if (this->currentToken.compare(";") == 0) {
        this->acceptToken(";", false);
    }
    else {
        this->throwException();
    }
    return;
}

std::string Parser::expression() {
    std::string result = this->lastType;
    std::string first[1] = { "id" };
    std::string third[1] = { "num" };
    if (this->searchArray(1, first, this->currentToken)) {
        this->acceptToken("id", false);
        result = this->variable();
    }
    else if (this->currentToken.compare("(") == 0) {
        this->acceptToken("(", false);
        result = this->expression();
        this->acceptToken(")", false);
        result = this->termPrime(result);
        result = this->additiveExpressionPrime(result);
        result = this->relopExpression(result);
    }
    else if (this->searchArray(1, third, this->currentToken)) {
        this->acceptToken("num", false);
        result = this->termPrime(this->lastType);
        result = this->additiveExpressionPrime(result);
        result = this->relopExpression(result);
    }
    else {
        this->throwException();
    }
    return result;
}

std::string Parser::variable() {
    std::string result = this->lastType;
    std::string first[12] = { "[", "=", "*", "/", "+", "-", "<=", "<", ">", ">=", "==", "!=" };
    std::string follow[4] = { ";", ")", "]", "," };
    if (this->searchArray(12, first, this->currentToken)) {
        this->varArray();
        result = this->variableFactor();
    }
    else if (this->currentToken.compare("(") == 0) {
        this->acceptToken("(", false);
        result = this->args();
        this->acceptToken(")", false);
        result = this->termPrime(result);
        result = this->additiveExpressionPrime(result);
        result = this->relopExpression(result);
    }
    else if (this->searchArray(4, follow, this->currentToken)) {
        this->varArray();
        result = this->variableFactor();
    }
    else {
        this->throwException();
    }
    return result;
}

std::string Parser::variableFactor() {
    Symbol* assignmentVariable;
    std::string result = this->lastType;
    std::string second[10] = { "*", "/", "+", "-", "<=", "<", ">", ">=", "==", "!=" };
    std::string follow[4] = { ";", ")", "]", "," };
    if (this->currentToken.compare("=") == 0) {
        assignmentVariable = this->lastSymbol;
        this->acceptToken("=", false);
        this->lastType = "";
        result = this->expression();
        // A data type was seen but didn't make it all the way down
        if (result.compare("") == 0 && this->lastType.compare("") != 0) {
            result = this->lastType;
        }
        // Check if expression result is the same as variable declared type
        if (assignmentVariable->getType().compare(result) != 0) {
            this->throwFloatException();
        }
    }
    else if (this->searchArray(10, second, this->currentToken)) {
        result = this->termPrime(result);
        result = this->additiveExpressionPrime(result);
        result = this->relopExpression(result);
    }
    else if (this->searchArray(4, follow, this->currentToken)) {
        result = this->termPrime(result);
        result = this->additiveExpressionPrime(result);
        result = this->relopExpression(result);
    }
    else {
        this->throwException();
    }
    return result;
}

void Parser::varArray() {
    std::string result = this->lastType;
    std::string follow[15] = { "=", "*", "/", "+", "-", "<=", "<", ">", ">=", "==", "!=", ";", ")", "]", "," };
    if (this->currentToken.compare("[") == 0) {
        // The lastSymbol seen is not an array
        if (!this->lastSymbol->getIsArray()) {
            this->throwFloatException();
        }
        this->acceptToken("[", false);
        result = this->expression();
        // Check if result is an INT
        if (result.compare("int") != 0) {
            this->throwFloatException();
        }
        this->acceptToken("]", false);
    }
    else if (this->searchArray(15, follow, this->currentToken)) {
        // Go to Empty
    }
    else {
        this->throwException();
    }
    return;
}

std::string Parser::relopExpression(std::string leftType) {
    std::string result = leftType;
    std::string first[6] = { "<=", "<", ">", ">=", "==", "!=" };
    std::string follow[4] = { ";", ")", "]", ",", };
    if (this->searchArray(6, first, this->currentToken)) {
        this->relop();
        result = this->additiveExpression();
        if (leftType.compare(result) != 0) {
            this->throwFloatException();
        }
    }
    else if (this->searchArray(4, follow, this->currentToken)) {
        // Go to empty
    }
    else {
        this->throwException();
    }
    return result;
}

void Parser::relop() {
    if (this->currentToken.compare("<=") == 0) {
        this->acceptToken("<=", false);
    }
    else if (this->currentToken.compare("<") == 0) {
        this->acceptToken("<", false);
    }
    else if (this->currentToken.compare(">") == 0) {
        this->acceptToken(">", false);
    }
    else if (this->currentToken.compare(">=") == 0) {
        this->acceptToken(">=", false);
    }
    else if (this->currentToken.compare("==") == 0) {
        this->acceptToken("==", false);
    }
    else if (this->currentToken.compare("!=") == 0) {
        this->acceptToken("!=", false);
    }
    else {
        this->throwException();
    }
    return;
}

std::string Parser::additiveExpression() {
    std::string result = this->lastType;
    std::string second[1] = { "id" };
    std::string third[1] = { "num" };
    if (this->currentToken.compare("(") == 0) {
        this->acceptToken("(", false);
        result = this->expression();
        this->acceptToken(")", false);
        result = this->termPrime(result);
        result = this->additiveExpressionPrime(result);
    }
    else if (this->searchArray(1, second, this->currentToken)) {
        this->acceptToken("id", false);
        result = this->varCall();
        result = this->termPrime(result);
        result = this->additiveExpressionPrime(result);
    }
    else if (this->searchArray(1, third, this->currentToken)) {
        this->acceptToken("num", false);
        result = this->termPrime(this->lastType);
        result = this->additiveExpressionPrime(result);
    }
    else {
        this->throwException();
    }
    return result;
}

std::string Parser::additiveExpressionPrime(std::string leftType) {
    std::string result = leftType;
    std::string first[2] = { "+", "-" };
    std::string follow[10] = { "<=", "<", ">", ">=", "==", "!=", ";", ")", "]", "," };
    if (this->searchArray(2, first, this->currentToken)) {
        this->addop();
        result = this->term();
        if (leftType.compare(result) != 0) {
            this->throwFloatException();
        }
        result = this->additiveExpressionPrime(result);
    }
    else if (this->searchArray(10, follow, this->currentToken)) {
        // Go to empty
    }
    else {
        this->throwException();
    }
    return result;
}

void Parser::addop() {
    if (this->currentToken.compare("+") == 0) {
        this->acceptToken("+", false);
    }
    else if (this->currentToken.compare("-") == 0) {
        this->acceptToken("-", false);
    }
    else {
        this->throwException();
    }
    return;
}

std::string Parser::term() {
    std::string result = this->lastType;
    std::string second[1] = { "id" };
    std::string third[1] = { "num" };
    if (this->currentToken.compare("(") == 0) {
        this->acceptToken("(", false);
        result = this->expression();
        this->acceptToken(")", false);
        result = this->termPrime(result);
    }
    else if (this->searchArray(1, second, this->currentToken)) {
        this->acceptToken("id", false);
        result = this->varCall();
        result = this->termPrime(result);
    }
    else if (this->searchArray(1, third, this->currentToken)) {
        this->acceptToken("num", false);
        result = this->termPrime(this->lastType);
    }
    else {
        this->throwException();
    }
    return result;
}

std::string Parser::termPrime(std::string leftType) {
    std::string result = leftType;
    std::string first[2] = { "*", "/" };
    std::string follow[12] = { "<=", "<", ">", ">=", "==", "!=", "+", "-", ";", ")", "]", "," };
    if (this->searchArray(2, first, this->currentToken)) {
        this->mulop();
        result = this->factor();
        if (leftType.compare(result) != 0) {
            // Operands don't match
            this->throwFloatException();
        }
        result = this->termPrime(result);
    }
    else if (this->searchArray(12, follow, this->currentToken)) {
        // Go to Empty
    }
    else {
        this->throwException();
    }
    return result;
}

std::string Parser::factor() {
    std::string result = this->lastType;
    std::string second[1] = { "id" };
    std::string third[1] = { "num" };
    if (this->currentToken.compare("(") == 0) {
        this->acceptToken("(", false);
        result = this->expression();
        this->acceptToken(")", false);
    }
    else if (this->searchArray(1, second, this->currentToken)) {
        this->acceptToken("id", false);
        result = this->varCall();
    }
    else if (this->searchArray(1, third, this->currentToken)) {
        this->acceptToken("num", false);
    }
    else {
        this->throwException();
    }
    return result;
}

void Parser::mulop() {
    if (this->currentToken.compare("*") == 0) {
        this->acceptToken("*", false);
    }
    else if (this->currentToken.compare("/") == 0) {
        this->acceptToken("/", false);
    }
    else {
        this->throwException();
    }
    return;
}

std::string Parser::varCall() {
    std::string result = this->lastType;
    std::string follow[14] = { "+", "-", "*", "/", "<=", "<", ">", ">=", "==", "!=", ";", ")", "]", "," };
    if (this->currentToken.compare("(") == 0) {
        this->acceptToken("(", false);
        result = this->args();
        this->acceptToken(")", false);
    }
    else if (this->currentToken.compare("[") == 0) {
        this->varArray();
    }
    else if (this->searchArray(14, follow, this->currentToken)) {
        this->varArray();
    }
    else {
        this->throwException();
    }
    return result;
}

std::string Parser::args() {
    std::string result = this->lastType;
    std::string first[3] = { "id", "(", "num"};
    if (this->searchArray(3, first, this->currentToken)) {
        result = this->argList();
    }
    else if (this->currentToken.compare(")") == 0) {
        // Go to empty
    }
    else {
        this->throwException();
    }
    return result;
}

std::string Parser::argList() {
    std::string result = this->lastType;
    std::string first[3] = { "id", "(", "num"};
    if (this->searchArray(3, first, this->currentToken)) {
        result = this->expression();
        result = this->argListPrime();
    }
    else {
        this->throwException();
    }
    return result;
}

std::string Parser::argListPrime() {
    std::string result = this->lastType;
    if (this->currentToken.compare(",") == 0) {
        this->acceptToken(",", false);
        result = this->expression();
        result = this->argListPrime();
    }
    else if (this->currentToken.compare(")") == 0) {
        // Go to empty
    }
    else {
        this->throwException();
    }
    return result;
}