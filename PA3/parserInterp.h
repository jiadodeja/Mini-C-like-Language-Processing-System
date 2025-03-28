/* 
 * parserInterp.h
 * Building an Interpreter for Mini C-Like Language
 * Programming Assignment 3
 * Fall 2024
*/

#ifndef PARSE_H_
#define PARSE_H_

#include <iostream>

using namespace std;

#include "lex.h"
#include "val.h"



extern bool Prog(istream& in, int& line);
extern bool StmtList(istream& in, int& line);
extern bool Stmt(istream& in, int& line);
extern bool DeclStmt(istream& in, int& line);
extern bool VarList(istream& in, int& line, LexItem & idtok);
extern bool ControlStmt(istream& in, int& line);
extern bool PrintStmt(istream& in, int& line);
extern bool CompStmt(istream& in, int& line);
extern bool IfStmt(istream& in, int& line);
extern bool AssignStmt(istream& in, int& line);
extern bool Var(istream& in, int& line, LexItem & idtok);
extern bool ExprList(istream& in, int& line);
extern bool Expr(istream& in, int& line, Value & retVal);
extern bool LogANDExpr(istream& in, int& line, Value & retVal);
extern bool EqualExpr(istream& in, int& line, Value & retVal);
extern bool RelExpr(istream& in, int& line, Value & retVal);
extern bool AddExpr(istream& in, int& line, Value & retVal);
extern bool MultExpr(istream& in, int& line, Value & retVal);
extern bool UnaryExpr(istream& in, int& line, Value & retVal);
extern bool PrimaryExpr(istream& in, int& line, int sign, Value & retVal);

extern int ErrCount();

#endif /* PARSE_H_ */
