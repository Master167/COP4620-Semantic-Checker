/* 
 * File:   Parser.cpp
 * Author: Michael Frederick (n00725913)
 * 
 * Created on February 24, 2017, 6:15 PM
 */

#include "Parser.h"


Parser::Parser(std::fstream& inputFile) : filestream(inputFile) {
}

bool Parser::parseFile() {
    bool result = true;
    if (this->getNextToken()) {
        try {
            this->program();
        }
        catch (int e) {
            std::cout << "Int Exception thrown" << std::endl;
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
    //if (this->currentLine > 0) {
    //    std::cout << "Accepted: " << this->currentToken << std::endl;
    //}
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

bool Parser::acceptToken(std::string token) {
    bool result = true;
    std::size_t findResult;
    std::string keyword;
    
    if (token.compare("id") == 0) {
        findResult = this->currentToken.find("ID:");
        if (findResult != std::string::npos) {
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
            }
        }
        else {
            result = this->getNextToken();
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
            //ERROR: We're never going to accept a error token. Therefore it never be found in .find()
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
        return;
    }
    return;
}

void Parser::declaration() {
    std::string firstSet[3] = { "int", "float", "void" };
    if (this->searchArray(3, firstSet, this->currentToken)) {
        this->typeSpecifier();
        this->acceptToken("id");
        this->callDeclaration();
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
        this->acceptToken("(");
        this->params();
        this->acceptToken(")");
        this->compountStmt();
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::idSpecifier() {
    if (this->currentToken.compare(";") == 0) {
        this->acceptToken(";");
    }
    else if (this->currentToken.compare("[") == 0) {
        this->acceptToken("[");
        this->acceptToken("num");
        this->acceptToken("]");
        this->acceptToken(";");
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
        this->acceptToken("int");
    }
    else if (this->searchArray(1, second, this->currentToken)) {
        this->acceptToken("float");
    }
    else if (this->searchArray(1, third, this->currentToken)) {
        this->acceptToken("void");
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
    if (this->searchArray(1, first, this->currentToken)) {
        this->acceptToken("int");
        this->acceptToken("id");
        this->array();
        this->paramListPrime();
    }
    else if (this->searchArray(1, second, this->currentToken)) {
        this->acceptToken("float");
        this->acceptToken("id");
        this->array();
        this->paramListPrime();
    }
    else if (this->searchArray(1, third, this->currentToken)) {
        this->acceptToken("void");
        this->paramList();
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::paramList() {
    std::string first[1] = { "id" };
    if (this->searchArray(1, first, this->currentToken)) {
        this->acceptToken("id");
        this->array();
        this->paramListPrime();
    }
    else if (this->currentToken.compare(")") == 0) {
        // Go to empty
        return;
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::paramListPrime() {
    if (this->currentToken.compare(",") == 0) {
        this->acceptToken(",");
        this->param();
        this->paramListPrime();
    }
    else if (this->currentToken.compare(")") == 0) {
        // Go to empty
        return;
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
        this->acceptToken("id");
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
        this->acceptToken("[");
        this->acceptToken("]");
    }
    else if (this->searchArray(2, follow, this->currentToken)) {
        // Go to empty
        return;
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::compountStmt() {
    if (this->currentToken.compare("{") == 0) {
        this->acceptToken("{");
        this->localDeclarations();
        this->statementList();
        this->acceptToken("}");
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
        return;
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
        this->acceptToken("id");
        this->idSpecifier();
        this->localDeclarationsPrime();
    }
    else if (this->searchArray(9, follow, this->currentToken)) {
        // Go to empty
        return;
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
        return;
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
        return;
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
    std::string first[3] = { "id", "(", "num" };
    if (this->searchArray(3, first, this->currentToken)) {
        this->expression();
        this->acceptToken(";");
    }
    else if (this->currentToken.compare(";") == 0) {
        this->acceptToken(";");
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::selectionStmt() {
    std::string first[1] = { "if" };
    if (this->searchArray(1, first, this->currentToken)) {
        this->acceptToken("if");
        this->acceptToken("(");
        this->expression();
        this->acceptToken(")");
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
        this->acceptToken("else");
        this->statement();
    }
    else if (this->searchArray(9, follow, this->currentToken)) {
        // Go to empty
        return;
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::iterationStmt() {
    std::string first[1] = { "while" };
    if (this->searchArray(1, first, this->currentToken)) {
        this->acceptToken("while");
        this->acceptToken("(");
        this->expression();
        this->acceptToken(")");
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
        this->acceptToken("return");
        this->returnStmtEnd();
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::returnStmtEnd() {
    std::string first[3] = { "id", "(", "num" };
    if (this->searchArray(3, first, this->currentToken)) {
        this->expression();
        this->acceptToken(";");
    }
    else if (this->currentToken.compare(";") == 0) {
        this->acceptToken(";");
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::expression() {
    std::string first[1] = { "id" };
    std::string third[1] = { "num" };
    if (this->searchArray(1, first, this->currentToken)) {
        this->acceptToken("id");
        this->variable();
    }
    else if (this->currentToken.compare("(") == 0) {
        this->acceptToken("(");
        this->expression();
        this->acceptToken(")");
        this->termPrime();
        this->additiveExpressionPrime();
        this->relopExpression();
    }
    else if (this->searchArray(1, third, this->currentToken)) {
        this->acceptToken("num");
        this->termPrime();
        this->additiveExpressionPrime();
        this->relopExpression();
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::variable() {
    std::string first[12] = { "[", "=", "*", "/", "+", "-", "<=", "<", ">", ">=", "==", "!=" };
    std::string follow[4] = { ";", ")", "]", "," };
    if (this->searchArray(12, first, this->currentToken)) {
        this->varArray();
        this->variableFactor();
    }
    else if (this->currentToken.compare("(") == 0) {
        this->acceptToken("(");
        this->args();
        this->acceptToken(")");
        this->termPrime();
        this->additiveExpressionPrime();
        this->relopExpression();
    }
    else if (this->searchArray(4, follow, this->currentToken)) {
        this->varArray();
        this->variableFactor();
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::variableFactor() {
    std::string second[10] = { "*", "/", "+", "-", "<=", "<", ">", ">=", "==", "!=" };
    std::string follow[4] = { ";", ")", "]", "," };
    if (this->currentToken.compare("=") == 0) {
        this->acceptToken("=");
        this->expression();
    }
    else if (this->searchArray(10, second, this->currentToken)) {
        this->termPrime();
        this->additiveExpressionPrime();
        this->relopExpression();
    }
    else if (this->searchArray(4, follow, this->currentToken)) {
        this->termPrime();
        this->additiveExpressionPrime();
        this->relopExpression();
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::varArray() {
    std::string follow[15] = { "=", "*", "/", "+", "-", "<=", "<", ">", ">=", "==", "!=", ";", ")", "]", "," };
    if (this->currentToken.compare("[") == 0) {
        this->acceptToken("[");
        this->expression();
        this->acceptToken("]");
    }
    else if (this->searchArray(15, follow, this->currentToken)) {
        // Go to Empty
        return;
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::relopExpression() {
    std::string first[6] = { "<=", "<", ">", ">=", "==", "!=" };
    std::string follow[4] = { ";", ")", "]", ",", };
    if (this->searchArray(6, first, this->currentToken)) {
        this->relop();
        this->additiveExpression();
    }
    else if (this->searchArray(4, follow, this->currentToken)) {
        // Go to empty
        return;
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::relop() {
    if (this->currentToken.compare("<=") == 0) {
        this->acceptToken("<=");
    }
    else if (this->currentToken.compare("<") == 0) {
        this->acceptToken("<");
    }
    else if (this->currentToken.compare(">") == 0) {
        this->acceptToken(">");
    }
    else if (this->currentToken.compare(">=") == 0) {
        this->acceptToken(">=");
    }
    else if (this->currentToken.compare("==") == 0) {
        this->acceptToken("==");
    }
    else if (this->currentToken.compare("!=") == 0) {
        this->acceptToken("!=");
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::additiveExpression() {
    std::string second[1] = { "id" };
    std::string third[1] = { "num" };
    if (this->currentToken.compare("(") == 0) {
        this->acceptToken("(");
        this->expression();
        this->acceptToken(")");
        this->termPrime();
        this->additiveExpressionPrime();
    }
    else if (this->searchArray(1, second, this->currentToken)) {
        this->acceptToken("id");
        this->varCall();
        this->termPrime();
        this->additiveExpressionPrime();
    }
    else if (this->searchArray(1, third, this->currentToken)) {
        this->acceptToken("num");
        this->termPrime();
        this->additiveExpressionPrime();
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::additiveExpressionPrime() {
    std::string first[2] = { "+", "-" };
    std::string follow[10] = { "<=", "<", ">", ">=", "==", "!=", ";", ")", "]", "," };
    if (this->searchArray(2, first, this->currentToken)) {
        this->addop();
        this->term();
        this->additiveExpressionPrime();
    }
    else if (this->searchArray(10, follow, this->currentToken)) {
        // Go to empty
        return;
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::addop() {
    if (this->currentToken.compare("+") == 0) {
        this->acceptToken("+");
    }
    else if (this->currentToken.compare("-") == 0) {
        this->acceptToken("-");
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::term() {
    std::string second[1] = { "id" };
    std::string third[1] = { "num" };
    if (this->currentToken.compare("(") == 0) {
        this->acceptToken("(");
        this->expression();
        this->acceptToken(")");
        this->termPrime();
    }
    else if (this->searchArray(1, second, this->currentToken)) {
        this->acceptToken("id");
        this->varCall();
        this->termPrime();
    }
    else if (this->searchArray(1, third, this->currentToken)) {
        this->acceptToken("num");
        this->termPrime();
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::termPrime() {
    std::string first[2] = { "*", "/" };
    std::string follow[12] = { "<=", "<", ">", ">=", "==", "!=", "+", "-", ";", ")", "]", "," };
    if (this->searchArray(2, first, this->currentToken)) {
        this->mulop();
        this->factor();
        this->termPrime();
    }
    else if (this->searchArray(12, follow, this->currentToken)) {
        // Go to Empty
        return;
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::factor() {
    std::string second[1] = { "id" };
    std::string third[1] = { "num" };
    if (this->currentToken.compare("(") == 0) {
        this->acceptToken("(");
        this->expression();
        this->acceptToken(")");
    }
    else if (this->searchArray(1, second, this->currentToken)) {
        this->acceptToken("id");
        this->varCall();
    }
    else if (this->searchArray(1, third, this->currentToken)) {
        this->acceptToken("num");
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::mulop() {
    if (this->currentToken.compare("*") == 0) {
        this->acceptToken("*");
    }
    else if (this->currentToken.compare("/") == 0) {
        this->acceptToken("/");
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::varCall() {
    std::string follow[14] = { "+", "-", "*", "/", "<=", "<", ">", ">=", "==", "!=", ";", ")", "]", "," };
    if (this->currentToken.compare("(") == 0) {
        this->acceptToken("(");
        this->args();
        this->acceptToken(")");
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
    return;
}

void Parser::args() {
    std::string first[3] = { "id", "(", "num"};
    if (this->searchArray(3, first, this->currentToken)) {
        this->argList();
    }
    else if (this->currentToken.compare(")") == 0) {
        // Go to empty
        return;
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::argList() {
    std::string first[3] = { "id", "(", "num"};
    if (this->searchArray(3, first, this->currentToken)) {
        this->expression();
        this->argListPrime();
    }
    else {
        this->throwException();
    }
    return;
}

void Parser::argListPrime() {
    if (this->currentToken.compare(",") == 0) {
        this->acceptToken(",");
        this->expression();
        this->argListPrime();
    }
    else if (this->currentToken.compare(")") == 0) {
        // Go to empty
        return;
    }
    else {
        this->throwException();
    }
    return;
}