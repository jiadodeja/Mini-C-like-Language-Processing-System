#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include "lex.h"
using namespace std;

enum states{
    start, inint, inreal, inid, instr
};


extern LexItem getNextToken(istream& in, int& linenum) {
    string lexeme = "";
    char i;
    states state = start;


    while(true){
        if(!in.get(i)){
            break;
        }
        if(i=='\n'){
            linenum++;
        }


        switch (state) {
            case start:

                //check for a digit
                if (isdigit(i)) {
                    state = inint;
                    lexeme += i;
                    continue;
                }


                //check for letter
                if (isalpha(i)) {
                    state = inid;
                    lexeme += i;
                    continue;
                }


                //check for string
                if (i== '\'') {
                    state = instr;
                    continue;
                }



                if (i == '+') {
                    return LexItem{PLUS, "+", linenum};
                }
                if (i == '-') {
                    return LexItem{MINUS, "-", linenum};
                }
                if (i == '*') {
                    return LexItem{MULT, "*", linenum};
                }
                if (i == '/') {
                    return LexItem{DIV, "/", linenum};
                }
                if (i == '=') {
                    return LexItem{EQ, "=", linenum};
                }
                if (i == '<') {
                    return LexItem{LTHAN, "<", linenum};
                }
                if (i == '>') {
                    return LexItem{GTHAN, ">", linenum};
                }
                if (i == ',') {
                    return LexItem{COMMA, ",", linenum};
                }
                if (i == ';') {
                    return LexItem{SEMICOL, ";", linenum};
                }
                if (i == '(') {
                    return LexItem{LPAREN, "(", linenum};
                }
                if (i == ')') {
                    return LexItem{RPAREN, ")", linenum};
                }
                if (i == ':'){
                    in.get(i);
                    if(i=='='){
                        return LexItem{ASSOP, ":=", linenum};
                    }
                }
                if (i == '.') {
                    return LexItem{DOT, ".", linenum};
                }


                //error
                if(!isspace(i) && i!='\0'){
                    lexeme+=i;
                    return LexItem{ERR,lexeme,linenum};
                }


                continue;

            case inint:


                if (isdigit(i)) { //int continues
                    lexeme += i;
                    continue;
                } else if(i=='.'){
                    lexeme += i;
                    state = inreal;
                    continue;
                } else{
                    if(i!='\n'){
                        in.putback(i);
                    }
                    return LexItem{ICONST,lexeme,linenum};
                }


            case inid:
                if (isalnum(i) || i == '_' || i == '$') {
                    lexeme += i;
                    continue;
                } else {//ident is over
                    if(i!='\n'){
                        in.putback(i);
                    }
                    return id_or_kw(lexeme,linenum);
                }




            case instr:
                if(i=='\"'){
                    return LexItem{SCONST,lexeme,linenum};


                }else if(i=='\n'){
                    linenum--;
                    return LexItem{ERR,"\'"+lexeme,linenum};
                }else{
                    lexeme += i;
                }
                continue;


            case inreal:
                if(i=='.'){
                    return LexItem{ERR,lexeme+".",linenum};
                }else if(!isdigit(i)){
                    if(i!='\n'){
                        in.putback(i);
                    }
                    return LexItem{RCONST,lexeme,linenum};
                }
                else{
                    lexeme +=i;
                }
                continue;


        }
    }

    return LexItem{DONE,"",linenum};
}






extern ostream& operator<<(ostream& out, const LexItem& tok) {
    map<Token, string> m;
    // keywords OR RESERVED WORDS
    m[IF] = "IF";
    m[ELSE] = "ELSE";
    m[STRING] = "STRING";
    m[PROGRAM] = "PROGRAM";


    // the arithmetic operators, logic operators, relational operators
    m[PLUS] = "PLUS";
    m[MINUS] = "MINUS";
    m[MULT] = "MULT";
    m[DIV] = "DIV";
    m[ASSOP] = "ASSOP";
    m[EQ] = "EQ";
    m[GTHAN] = "GTHAN";
    m[LTHAN] = "LTHAN";
    m[AND] = "AND";
    m[OR] = "OR";
    m[NOT] = "NOT";
    m[COMMA] = "COMMA";
    m[SEMICOL] = "SEMICOL";
    m[LPAREN] = "LPAREN";
    m[RPAREN] = "RPAREN";
    m[DOT] = "DOT";




    if(tok.GetToken()==IDENT){
        out<<"IDENT: \""<<tok.GetLexeme()<<"\"";
    }else if(tok.GetToken()==SCONST){
        out<<"SCONST: \""<<tok.GetLexeme()<<"\"";
    }else if(tok.GetToken()==ERR){
        out<<"Error in line "<<tok.GetLinenum()<<": Unrecognized Lexeme {"<<tok.GetLexeme()<<"}";
    }else if(tok.GetToken()==ICONST){
        out<<"ICONST: \""<<tok.GetLexeme()<<"\"";
    }else if(tok.GetToken()==RCONST){
        out<<"RCONST: \""<<tok.GetLexeme()<<"\"";
    }else if(tok.GetToken()==BCONST){
        out<<"BCONST: \""<<tok.GetLexeme()<<"\"";
    }
    else if (tok.GetToken()==CCONST){
        out<<"CCONST: \""<<tok.GetLexeme()<<"\"";
    }
    }else {
        out<<m[tok.GetToken()];
    }


    return out;


}


extern LexItem id_or_kw(const string& lexeme, int linenum){
    map<string, Token> m;
    // keywords OR RESERVED WORDS
    m["if"]=IF;
    m["else"]=ELSE;

    m["string"]=STRING;

    m["program"]=PROGRAM;
    m["true"]=TRUE;
    m["false"]=FALSE;

    m["and"]=AND;

    m["not"]=NOT;
    m["or"]=OR;


    map<string, Token>::iterator it=m.find(lexeme);
    if(lexeme=="true"){
        return LexItem{BCONST, lexeme, linenum};
    }else if(lexeme=="false"){
        return LexItem{BCONST, lexeme, linenum};
    }


    if(it==m.end())
        return LexItem{IDENT, lexeme,linenum};
    else
        return LexItem{m[lexeme],lexeme,linenum};


}