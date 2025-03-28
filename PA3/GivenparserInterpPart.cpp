
#include "parserInterp.h"
#include <unordered_set>
#include <stack>

map<string, bool> defVar;
map<string, LexItem> SymTable;
map<string, LexItem> VarTable;
map<string, Value> TempsResults; // Container of temporary locations of Value objects for results of expressions, variables values and constants
queue<Value> *ValQue;			 // declare a pointer variable to a queue of Value objects
std::stack<bool> interpretStack;
bool storeExprResult = true;

namespace Parser
{
	bool pushed_back = false;
	LexItem pushed_token;

	static LexItem GetNextToken(istream &in, int &line)
	{
		if (pushed_back)
		{
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem &t)
	{
		if (pushed_back)
		{
			abort();
		}
		pushed_back = true;
		pushed_token = t;
	}

}

static int error_count = 0;

int ErrCount()
{
	return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

bool IdentList(istream &in, int &line);

bool AreCompatibleTypes(Value &varVal, LexItem &varTypeToken)
{
	if (varTypeToken.GetToken() == INT)
	{
		if (varVal.IsInt() || varVal.IsReal() || varVal.IsChar())
		{
			return true;
		}
	}

	if (varTypeToken.GetToken() == FLOAT)
	{
		if (varVal.IsInt() || varVal.IsReal() || varVal.IsChar())
		{
			return true;
		}
	}

	if (varTypeToken.GetToken() == BOOL)
	{
		if (varVal.IsBool())
		{
			return true;
		}
	}

	if (varTypeToken.GetToken() == STRING)
	{
		if (varVal.IsString())
		{
			return true;
		}
	}

	if (varTypeToken.GetToken() == CHAR)
	{
		if (varVal.IsInt() || varVal.IsReal() || varVal.IsChar())
		{
			return true;
		}
	}

	return false;
}

bool AreCompatibleTypes(Value &val1, Value &val2)
{
	if (val1.IsInt())
	{
		if (val2.IsInt() || val2.IsReal() || val2.IsChar())
		{
			return true;
		}
	}

	if (val1.IsReal())
	{
		if (val2.IsInt() || val2.IsReal() || val2.IsChar())
		{
			return true;
		}
	}

	if (val1.IsChar())
	{
		if (val2.IsInt() || val2.IsReal() || val2.IsChar() || val2.IsString())
		{
			return true;
		}
	}

	if (val1.IsBool() && val2.IsBool())
	{
		return true;
	}

	if (val1.IsString())
	{
		if (val2.IsChar() || val2.IsString())
		{
			return true;
		}
	}

	return false;
}

// Program is: Prog = PROGRAM IDENT CompStmt
bool Prog(istream &in, int &line)
{
	bool sl = false;
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok.GetToken() == PROGRAM)
	{
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == IDENT)
		{
			sl = CompStmt(in, line);
			if (!sl)
			{
				ParseError(line, "Invalid Program");
				return false;
			}
			cout << "(DONE)" << endl;
			return true;
		}
		else
		{
			ParseError(line, "Missing Program name");
			return false;
		}
	}
	else if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}

	else
	{
		ParseError(line, "Missing Program keyword");
		return false;
	}
} // End of Prog

// StmtList ::= Stmt { Stmt }
bool StmtList(istream &in, int &line)
{
	bool status;
	LexItem tok;

	status = Stmt(in, line);
	tok = Parser::GetNextToken(in, line);
	while (status && tok != RBRACE)
	{
		Parser::PushBackToken(tok);

		status = Stmt(in, line);
		tok = Parser::GetNextToken(in, line);
	}
	if (!status)
	{
		ParseError(line, "Syntactic error in statement list.");

		return false;
	}
	Parser::PushBackToken(tok);
	return true;
} // End of StmtList

// DeclStmt ::= ( INT | FLOAT | BOOL | CHAR | STRING) VarList
bool DeclStmt(istream &in, int &line)
{
	bool status = false;
	LexItem tok;

	LexItem t = Parser::GetNextToken(in, line);

	if (t == INT || t == FLOAT || t == CHAR || t == BOOL || t == STRING)
	{

		status = VarList(in, line, t);

		if (!status)
		{
			ParseError(line, "Incorrect variable in Declaration Statement.");
			return status;
		}
		return true;
	}

	return true;
} // End of Decl

// Stmt ::= DeclStmt ; | ControlStmt ; | CompStmt
bool Stmt(istream &in, int &line)
{
	bool status = true;

	LexItem t = Parser::GetNextToken(in, line);

	switch (t.GetToken())
	{

	case PRINT:
	case IF:
	case IDENT:
		Parser::PushBackToken(t);
		status = ControlStmt(in, line);

		if (!status)
		{
			ParseError(line, "Invalid control statement.");
			return false;
		}

		break;
	case INT:
	case FLOAT:
	case CHAR:
	case BOOL:
	case STRING:
		Parser::PushBackToken(t);
		status = DeclStmt(in, line);
		if (!status)
		{
			ParseError(line, "Invalid declaration statement.");
			return false;
		}
		t = Parser::GetNextToken(in, line);
		if (t == SEMICOL)
		{
			return true;
		}
		else
		{
			ParseError(line, "Missing semicolon at end of Statement.");
			return false;
		}
		break;
	case LBRACE:
		Parser::PushBackToken(t);
		status = CompStmt(in, line);
		if (!status)
		{
			ParseError(line, "Invalid compound statement.");
			return false;
		}

		break;
	default:
		Parser::PushBackToken(t);

		return false;
	}

	return status;
} // End of Stmt

bool CompStmt(istream &in, int &line)
{
	bool status = false;

	LexItem t = Parser::GetNextToken(in, line);
	if (t == LBRACE)
	{
		status = StmtList(in, line);
		if (!status)
		{
			ParseError(line, "Incorrect statement list");
			return false;
		}
		LexItem t = Parser::GetNextToken(in, line);
		if (t == RBRACE)
		{
			return true;
		}
		else if (t == DONE)
		{
			ParseError(line, "Missing end of program right brace.");
			return false;
		}
		else
		{
			ParseError(line, "Missing right brace.");
			return false;
		}
	}
	return status;
} // End of CompStmt

bool ControlStmt(istream &in, int &line)
{
	bool status;

	LexItem t = Parser::GetNextToken(in, line);

	switch (t.GetToken())
	{

	case PRINT:
		status = PrintStmt(in, line);

		if (!status)
		{
			ParseError(line, "Incorrect Print Statement");
			return false;
		}
		t = Parser::GetNextToken(in, line);

		if (t == SEMICOL)
		{
			return true;
		}
		else
		{
			ParseError(line, "Missing semicolon at end of Statement.");
			return false;
		}

		break;

	case IDENT:
		Parser::PushBackToken(t);
		status = AssignStmt(in, line);
		if (!status)
		{
			ParseError(line, "Incorrect Assignment Statement");
			return false;
		}
		t = Parser::GetNextToken(in, line);

		if (t == SEMICOL)
		{
			return true;
		}
		else
		{
			ParseError(line, "Missing semicolon at end of Statement.");
			return false;
		}

		break;
	case IF:
		status = IfStmt(in, line);
		storeExprResult = true;
		if (!status)
		{
			ParseError(line, "Incorrect IF Statement");
			return false;
		}

		break;

	default:
		Parser::PushBackToken(t);
		return true;
	}

	return status;
} // End of ControlStmt

// VarList ::= Var [= Expr] {, Var [= Expr]}
bool VarList(istream &in, int &line, LexItem &idtok)
{
	bool status = false, exprstatus = false;
	string identstr;

	LexItem tok = Parser::GetNextToken(in, line);
	if (tok == IDENT)
	{
		identstr = tok.GetLexeme();
		if (!(defVar.find(identstr)->second))
		{
			defVar[identstr] = true;
			SymTable[identstr] = idtok;
			VarTable[identstr] = tok;
		}
		else
		{
			ParseError(line, "Variable Redefinition");
			return false;
		}
	}
	else
	{

		ParseError(line, "Missing Variable Name");
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok == ASSOP)
	{
		Value retVal;
		exprstatus = Expr(in, line, retVal);
		if (!exprstatus)
		{
			ParseError(line, "Incorrect initialization for a variable.");
			return false;
		}
		else
		{
			// CHECK idenststr type. If it is FLOAT and retVal is int, convert the retVal to double.
			LexItem lex = SymTable[identstr];
			if (lex.GetToken() == FLOAT && retVal.IsInt())
			{
				retVal = Value((double)retVal.GetInt());
			}

			TempsResults[identstr] = retVal;
		}

		// COMMENTED:  cout<< "Initialization of the variable " << identstr <<" in the declaration statement at line " << line << endl;
		tok = Parser::GetNextToken(in, line);

		if (tok == COMMA)
		{
			status = VarList(in, line, idtok);
		}
		else
		{
			Parser::PushBackToken(tok);
			return true;
		}
	}
	else if (tok == COMMA)
	{
		status = VarList(in, line, idtok);
	}
	else if (tok == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");

		return false;
	}
	else
	{

		Parser::PushBackToken(tok);
		return true;
	}

	return status;

} // End of VarList

// PrintStmt:= PRINT (ExpreList)
bool PrintStmt(istream &in, int &line)
{
	LexItem t;
	ValQue = new queue<Value>;

	t = Parser::GetNextToken(in, line);
	if (t != LPAREN)
	{

		ParseError(line, "Missing Left Parenthesis");
		return false;
	}

	bool ex = ExprList(in, line);

	if (!ex)
	{
		ParseError(line, "Missing expression list after Print");
		while (!(*ValQue).empty())
		{
			ValQue->pop();
		}
		delete ValQue;
		return false;
	}

	t = Parser::GetNextToken(in, line);
	if (t != RPAREN)
	{
		ParseError(line, "Missing Right Parenthesis");
		while (!(*ValQue).empty())
		{
			ValQue->pop();
		}
		delete ValQue;
		return false;
	}
	// Evaluate: print out the list of expressions' values
	while (!(*ValQue).empty())
	{
		Value nextVal = (*ValQue).front();
		if (storeExprResult) cout << nextVal;
		ValQue->pop();
	}
	delete ValQue;
	if (storeExprResult) cout << endl;
	return true;
} // End of PrintStmt

// IfStmt ::= IF (Expr) Stmt [ ELSE Stmt ]
bool IfStmt(istream &in, int &line)
{
	bool ex = false, status;
	static int nestlevel = 0;
	LexItem t;

	t = Parser::GetNextToken(in, line);
	if (t != LPAREN)
	{

		ParseError(line, "Missing Left Parenthesis");
		return false;
	}

	Value retVal;
	ex = Expr(in, line, retVal);

	if (!ex)
	{
		ParseError(line, "Missing if statement condition");
		return false;
	}

	if (!retVal.IsBool())
	{
		ParseError(line, "Invalid type for If statement condition");
		return false;
	}

	t = Parser::GetNextToken(in, line);
	if (t != RPAREN)
	{
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}

	nestlevel += 1;
	// COMMENTED: cout << "in IfStmt then-clause at nesting level: " << nestlevel << endl;

	storeExprResult = interpretStack.empty() ? retVal.GetBool() : (interpretStack.top() && retVal.GetBool());
	interpretStack.push(storeExprResult);

	status = Stmt(in, line);
	if (!status)
	{
		ParseError(line, "If-Stmt Syntax Error");
		return false;
	}

	t = Parser::GetNextToken(in, line);
	if (t == ELSE)
	{
		// COMMENTED: cout << "in IfStmt else-clause at nesting level: " << nestlevel << endl;
		interpretStack.pop();
		storeExprResult = interpretStack.empty() ? !retVal.GetBool() : (interpretStack.top() && !retVal.GetBool());
		interpretStack.push(storeExprResult);

		status = Stmt(in, line);
		if (!status)
		{
			ParseError(line, "Missing Statement for If-Stmt Else-Part");
			return false;
		}

		nestlevel--;
		interpretStack.pop();
		return true;
	}

	nestlevel--;
	interpretStack.pop();
	Parser::PushBackToken(t);
	return true;
} // End of IfStmt function

// Var:= ident
bool Var(istream &in, int &line, LexItem &idtok)
{
	string identstr;

	LexItem tok = Parser::GetNextToken(in, line);

	if (tok == IDENT)
	{
		identstr = tok.GetLexeme();

		if (!(defVar.find(identstr)->second))
		{
			ParseError(line, "Undeclared Variable");
			return false;
		}
		idtok = VarTable[identstr];
		return true;
	}
	else if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	return false;
} // End of Var

// AssgnStmt ::= Var ( = | += | -= | *= | /= | %= ) Expr
bool AssignStmt(istream &in, int &line)
{
	bool varstatus = false, status = false;
	LexItem t, varToken;

	varstatus = Var(in, line, varToken);

	if (varstatus)
	{
		t = Parser::GetNextToken(in, line);

		if (t == ASSOP || t == ADDASSOP || t == SUBASSOP || t == MULASSOP || t == DIVASSOP || t == REMASSOP)
		{
			Value retVal;
			status = Expr(in, line, retVal);

			if (!status)
			{
				ParseError(line, "Missing Expression in Assignment Statement");
				return status;
			}

			string varLexeme = varToken.GetLexeme();
			LexItem varTypeToken = SymTable[varLexeme];
			if (t == ASSOP && AreCompatibleTypes(retVal, varTypeToken) == false)
			{
				ParseError(line, "Illegal mixed-mode assignment operation");
				return false;
			}

			if (t != ASSOP)
			{
				auto it = TempsResults.find(varLexeme);
				if (it == TempsResults.end())
				{
					ParseError(line, "Invalid Expression evaluation in Assignment Statement");
					return false;
				}

				Value currentVal = it->second;

				// Perform the arithmetic operation
				if (t == ADDASSOP)
				{
					retVal = currentVal + retVal;
				}
				else if (t == SUBASSOP)
				{
					retVal = currentVal - retVal;
				}
				else if (t == MULASSOP)
				{
					retVal = currentVal * retVal;
				}
				else if (t == DIVASSOP)
				{
					retVal = currentVal / retVal;
				}
				else if (t == REMASSOP)
				{
					if (!((currentVal.IsInt() || currentVal.IsChar()) && (retVal.IsInt() || retVal.IsChar())))
					{
						ParseError(line, "Invalid operand for the REM operator");
						return false;
					}
					retVal = currentVal % retVal;
				}
			}

			if (TempsResults.find(varLexeme) == TempsResults.end())
			{
				TempsResults[varLexeme] = retVal;
			}
			else
			{
				if (storeExprResult)
					TempsResults[varLexeme] = retVal;
			}
		}
		else if (t.GetToken() == ERR)
		{
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << t.GetLexeme() << ")" << endl;
			return false;
		}
		else
		{
			ParseError(line, "Missing Assignment Operator");
			return false;
		}
	}
	else
	{
		ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
		return false;
	}
	return status;
} // End of AssignStmt

// ExprList:= Expr {,Expr}
bool ExprList(istream &in, int &line)
{
	bool status = false;
	Value retVal;

	status = Expr(in, line, retVal);
	if (!status)
	{
		ParseError(line, "Missing Expression");
		return false;
	}
	ValQue->push(retVal);
	LexItem tok = Parser::GetNextToken(in, line);

	if (tok == COMMA)
	{

		status = ExprList(in, line);
	}
	else if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else
	{
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
} // End of ExprList

// Expr ::= LogANDExpr { || LogANDRxpr }
bool Expr(istream &in, int &line, Value &retVal)
{

	Value opVal1;
	LexItem tok;
	bool t1 = LogANDExpr(in, line, opVal1);
	retVal = opVal1;

	if (!t1)
	{
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while (tok == OR)
	{
		Value opVal2;
		t1 = LogANDExpr(in, line, opVal2);
		if (!t1)
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}

		retVal = opVal1 || opVal2;

		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == ERR)
		{
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
	}
	Parser::PushBackToken(tok);
	return true;
} // End of Expr/LogORExpr

// LogANDExpr ::= EqualExpr { && EqualExpr }
bool LogANDExpr(istream &in, int &line, Value &retVal)
{

	Value opVal1;
	LexItem tok;
	bool t1 = EqualExpr(in, line, opVal1);
	retVal = opVal1;

	if (!t1)
	{
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while (tok == AND)
	{
		Value opVal2;
		t1 = EqualExpr(in, line, opVal2);
		if (!t1)
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}

		if (!(opVal1.IsBool() && (opVal2.IsBool())))
		{
			ParseError(line, "Invalid opernads for logical AND operator");
			return false;
		}

		retVal = opVal1 && opVal2;

		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == ERR)
		{
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
	}

	Parser::PushBackToken(tok);
	return true;
} // End of LogANDExpr

// EqualExpr ::= RelExpr [ (== | !=) RelExpr ]
bool EqualExpr(istream &in, int &line, Value &retVal)
{

	Value opVal1;
	LexItem tok;
	bool t1 = RelExpr(in, line, opVal1);
	retVal = opVal1;

	if (!t1)
	{
		return false;
	}

	tok = Parser::GetNextToken(in, line);

	if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}

	if (tok == EQ || tok == NEQ)
	{
		Value opVal2;
		t1 = RelExpr(in, line, opVal2);

		if (!t1)
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}

		if (tok == EQ)
		{
			retVal = opVal1 == opVal2;
		}

		if (tok == NEQ)
		{
			retVal = opVal1 != opVal2;
		}

		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == ERR)
		{
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
	}
	Parser::PushBackToken(tok);

	return true;
} // End of EqualExpr

// RelExpr ::= AddExpr [ ( < | > ) AddExpr ]
bool RelExpr(istream &in, int &line, Value &retVal)
{

	Value opVal1;
	LexItem tok;
	bool t1 = AddExpr(in, line, opVal1);
	retVal = opVal1;

	if (!t1)
	{
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	if (tok == LTHAN || tok == GTHAN)
	{
		Value opVal2;
		t1 = AddExpr(in, line, opVal2);
		if (!t1)
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}

		if (!AreCompatibleTypes(opVal1, opVal2))
		{
			ParseError(line, "Invalid opernads for relational operators");
			return false;
		}

		if (tok == LTHAN)
		{
			retVal = opVal1 < opVal2;
		}

		if (tok == GTHAN)
		{
			retVal = opVal1 > opVal2;
		}

		tok = Parser::GetNextToken(in, line);

		if (tok.GetToken() == ERR)
		{
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
	}
	Parser::PushBackToken(tok);
	return true;
} // End of RelExpr

// AddExpr :: MultExpr { ( + | - ) MultExpr }
bool AddExpr(istream &in, int &line, Value &retVal)
{
	Value opVal1;
	bool t1 = MultExpr(in, line, opVal1);
	retVal = opVal1;
	LexItem tok;

	if (!t1)
	{
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while (tok == PLUS || tok == MINUS)
	{
		Value opVal2;
		t1 = MultExpr(in, line, opVal2);
		if (!t1)
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}

		if (tok == PLUS)
		{
			retVal = opVal1 + opVal2;
			opVal1 = retVal;
		}

		if (tok == MINUS)
		{
			retVal = opVal1 - opVal2;
			opVal1 = retVal;
		}

		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == ERR)
		{
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
	}
	Parser::PushBackToken(tok);
	return true;
} // End of AddExpr

// MultExpr ::= UnaryExpr { ( * | / | %) UnaryExpr }
bool MultExpr(istream &in, int &line, Value &retVal)
{

	Value opVal1;
	bool t1 = UnaryExpr(in, line, opVal1);
	retVal = opVal1;
	LexItem tok;

	if (!t1)
	{
		return false;
	}

	tok = Parser::GetNextToken(in, line);
	if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while (tok == MULT || tok == DIV || tok == REM)
	{
		Value opVal2;
		t1 = UnaryExpr(in, line, opVal2);

		if (!t1)
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}

		if (tok == MULT)
		{
			if (!(opVal1.IsInt() || opVal1.IsReal()) && (opVal2.IsInt() || opVal2.IsReal()))
			{
				ParseError(line, "Illegal operand type for the operation.");
				return false;
			}
			retVal = opVal1 * opVal2;
			opVal1 = retVal;
		}

		if (tok == DIV)
		{
			if (!(opVal1.IsInt() || opVal1.IsReal()) && (opVal2.IsInt() || opVal2.IsReal()))
			{
				ParseError(line, "Illegal operand type for the operation.");
				return false;
			}

			if ((opVal1.IsInt() && opVal1.GetInt() == 0) || (opVal2.IsReal() && opVal2.GetReal() == 0.0))
			{
				ParseError(line, "Run-Time Error-Illegal division by Zero");
				return false;
			}

			retVal = opVal1 / opVal2;
			opVal1 = retVal;
		}

		if (tok == REM)
		{
			if (!(opVal1.IsInt() && opVal2.IsInt()))
			{
				ParseError(line, "Invalid operand for the REM operator");
				return false;
			}
			retVal = opVal1 % opVal2;
			opVal1 = retVal;
		}

		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == ERR)
		{
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
	}
	Parser::PushBackToken(tok);
	return true;
} // End of MultExpr

// UnaryExpr ::= ( - | + | ! ) PrimaryExpr | PrimaryExpr
bool UnaryExpr(istream &in, int &line, Value &retVal)
{
	LexItem tok = Parser::GetNextToken(in, line);
	bool status;
	int sign = 0;
	if (tok == MINUS)
	{
		sign = -1;
	}
	else if (tok == PLUS)
	{
		sign = 1;
	}
	else if (tok == NOT)
	{
		sign = 2;
	}
	else
		Parser::PushBackToken(tok);

	status = PrimaryExpr(in, line, sign, retVal);
	return status;
} // End of UnaryExpr

// PrimaryExpr ::= IDENT | ICONST | RCONST | SCONST | BCONST | CCONST | ( Expr )
bool PrimaryExpr(istream &in, int &line, int sign, Value &retVal)
{
	LexItem tok = Parser::GetNextToken(in, line);

	// Handle identifiers
	if (tok == IDENT)
	{
		string lexeme = tok.GetLexeme();

		if (!(defVar.find(lexeme)->second))
		{
			ParseError(line, "Using Undeclared Variable");
			return false;
		}

		if (TempsResults.find(lexeme) == TempsResults.end())
		{
			ParseError(line, "Using uninitialized Variable");
			return false;
		}

		Value val = TempsResults[lexeme];

		// Apply unary operator to the value
		if (val.IsInt() || val.IsReal())
		{
			if (sign == 1)
			{
				retVal = val.IsInt() ? Value(+val.GetInt()) : Value(+val.GetReal());
			}
			else if (sign == -1)
			{
				retVal = val.IsInt() ? Value(-val.GetInt()) : Value(-val.GetReal());
			}
			else if (sign == 2)
			{
				ParseError(line, "llegal Operand Type for NOT Operator");
				return false;
			}
			else
			{
				retVal = val;
			}
		}
		else if (val.IsString())
		{
			if (sign != 0)
			{
				ParseError(line, "Unary operators are not valid for strings");
				return false;
			}
			retVal = val;
		}
		else if (val.IsChar())
		{
			if (sign != 0)
			{
				ParseError(line, "Unary operators are not valid for characters");
				return false;
			}
			retVal = val;
		}
		else if (val.IsBool())
		{
			if (sign == 2)
			{
				val.SetBool(!val.GetBool());
				retVal = val;
			}
			else if (sign == 1 || sign == -1)
			{
				ParseError(line, "Illegal Operand Type for Sign Operator");
				return false;
			}
			else
			{
				retVal = val;
			}
		}

		return true;
	}

	// Handle integer constants
	else if (tok == ICONST)
	{
		string lexeme = tok.GetLexeme();
		if (sign == 2)
		{
			ParseError(line, "llegal Operand Type for NOT Operator");
			return false;
		}
		retVal = Value((sign == -1 ? -1 : 1) * stoi(lexeme));
		return true;
	}

	// Handle real constants
	else if (tok == RCONST)
	{
		string lexeme = tok.GetLexeme();
		if (sign == 2)
		{
			ParseError(line, "llegal Operand Type for NOT Operator");
			return false;
		}
		retVal = Value((sign == -1 ? -1 : 1) * stof(lexeme));
		return true;
	}

	// Handle string constants
	else if (tok == SCONST)
	{
		string lexeme = tok.GetLexeme();
		if (sign != 0)
		{
			ParseError(line, "Unary operators are not valid for strings");
			return false;
		}
		retVal = Value(lexeme);
		return true;
	}

	// Handle boolean constants
	else if (tok == BCONST)
	{
		string lexeme = tok.GetLexeme();
		if (sign == 1 || sign == -1)
		{
			ParseError(line, "Arithmetic operators (+, -) are not valid for booleans");
			return false;
		}
		retVal = Value(lexeme == "true" ? (sign == 2 ? false : true) : (sign == 2 ? true : false));
		return true;
	}

	else if (tok == CCONST)
	{
		string lexeme = tok.GetLexeme();
		if (sign != 0)
		{
			ParseError(line, "Unary operators are not valid for strings");
			return false;
		}
		retVal = Value(lexeme[0]);
		return true;
	}

	// Handle parenthesized expressions
	else if (tok == LPAREN)
	{
		if (!Expr(in, line, retVal))
		{
			ParseError(line, "Missing expression after Left Parenthesis");
			return false;
		}
		if (Parser::GetNextToken(in, line) != RPAREN)
		{
			ParseError(line, "Missing right Parenthesis after expression");
			return false;
		}
		return true;
	}

	// Handle errors
	else if (tok.GetToken() == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}

	// Default case
	return false;
}
// End of PrimaryExpr
