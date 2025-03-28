/* Implementation of Recursive-Descent Parser
 * for Mini C-Like Language
 * Programming Assignment 2
 * Fall 2024
 */

#include "parser.h"
#include <unordered_set>
map<string, bool> defVar;
map<string, Token> SymTable;
unordered_set<string> declaredVariables;
extern bool CheckUndeclaredVar(istream &in, int &line);
extern bool ValidStmtToken(istream &in, int &line);

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

    static bool IsControlStructure(LexItem t)
    {
        return (t == IF || t == ELSE);
    }

}

static int error_count = 0;
static int ifLevel = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
    ++error_count;
    cout << line << ": " << msg << endl;
}

void Message(string msg, int line)
{
    cout << msg << line << endl;
}

void IfMessage(string msg, int level)
{
    cout << msg << ": " << level << endl;
}

bool IdentList(istream &in, int &line);

bool Prog(istream &in, int &line)
{
    LexItem t = Parser::GetNextToken(in, line);
    if (t != PROGRAM)
    {
        ParseError(line, "Missing PROGRAM keyword");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if (t.GetToken() != IDENT)
    {
        ParseError(line, "Missing Program name");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if (t != LBRACE)
    {
        ParseError(line, "Invalid Program");
        return false;
    }

    Parser::PushBackToken(t);
    if (!CompStmt(in, line))
    {
        ParseError(line, "Invalid Program");
        return false;
    }

    cout << "(DONE)" << endl;
    return true;
}

bool CompStmt(istream &in, int &line)
{
    LexItem t = Parser::GetNextToken(in, line);
    if (t != LBRACE)
    {
        ParseError(line, "Missing left brace in compound statement");
        return false;
    }

    if (!StmtList(in, line))
    {
        ParseError(line, "Incorrect statement list");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if (t != RBRACE)
    {
        ParseError(line, "Missing right brace in compound statement");
        return false;
    }

    return true;
}

bool StmtList(istream &in, int &line)
{
    if (!Stmt(in, line))
    {
        ParseError(line, "Syntactic error in statement list.");
        return false;
    }

    LexItem t = Parser::GetNextToken(in, line);
    while (t == INT || t == FLOAT || t == BOOL || t == CHAR || t == STRING ||
           t == LBRACE || t == IF || t == IDENT || t == PRINT)
    {
        // If a token matches the start of a statement, push it back to reparse in Stmt
        Parser::PushBackToken(t);

        // Parse the next Stmt
        if (!Stmt(in, line))
        {
            ParseError(line, "Syntactic error in statement list.");
            return false;
        }

        if (!ValidStmtToken(in, line))
        {
            ParseError(line, "Syntactic error in statement list.");
            return false;
        }

        // Get the next token to check if there's another Stmt
        t = Parser::GetNextToken(in, line);
    }

    // Push back the last token as it might be part of the next structure
    Parser::PushBackToken(t);
    return true;
}

bool Stmt(istream &in, int &line)
{
    LexItem t = Parser::GetNextToken(in, line);
    Parser::PushBackToken(t);

    if (t == LBRACE)
    {
        if (!CompStmt(in, line))
        {
            ParseError(line, "Invalid compound statement.");
            return false;
        }
    }
    else if (t == IF || t == IDENT || t == PRINT)
    {
        if (!ControlStmt(in, line))
        {
            ParseError(line, "Invalid control statement.");
            return false;
        }

    }
    else
    {
        if (!DeclStmt(in, line))
        {
            ParseError(line, "Invalid declaration statement.");
            return false;
        }

        LexItem semicolon = Parser::GetNextToken(in, line);
        if (semicolon != SEMICOL)
        {
            ParseError(line, "Missing semicolon at end of Statement.");
            return false;
        }
    }

    return true;
}

bool DeclStmt(istream &in, int &line)
{
    LexItem t = Parser::GetNextToken(in, line);
    if (t != INT && t != FLOAT && t != BOOL && t != CHAR && t != STRING)
    {
        ParseError(line, "Incorrect type name");
        return false;
    }

    if (!VarList(in, line))
    {
        ParseError(line, "Incorrect variable in Declaration Statement.");
        return false;
    }

    return true;
}

bool ControlStmt(istream &in, int &line)
{
    LexItem t = Parser::GetNextToken(in, line);

    // Check for assignment, if, or print statement
    if (t == IDENT)
    {
        Parser::PushBackToken(t);
        int prevLine = line;
        if (!AssignStmt(in, line))
        {
            ParseError(line, "Incorrect Assignment Statement");
            return false;
        }

        LexItem semicolon = Parser::GetNextToken(in, line);
        if (semicolon != SEMICOL)
        {
            if (line != prevLine)
                line--;
            ParseError(line, "Missing semicolon at end of Statement.");
            return false;
        }
    }
    else if (t == IF)
    {
        // Push back the token for IfStmt to handle
        if (!IfStmt(in, line))
        {
            ParseError(line, "Incorrect IF Statement");
            return false;
        }
    }
    else if (t == PRINT)
    {
        // Push back the token for PrintStmt to handle
        if (!PrintStmt(in, line))
        {
            ParseError(line, "Incorrect Print Statement");
            return false;
        }

        LexItem semicolon = Parser::GetNextToken(in, line);
        if (semicolon != SEMICOL)
        {
            ParseError(line, "Missing semicolon at end of Print Statement.");
            return false;
        }
    }

    return true;
}

bool AssignStmt(istream &in, int &line)
{
    // Check if the token is a valid identifier
    if (!CheckUndeclaredVar(in, line))
    {
        ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
        return false;
    }

    // Step 2: Parse the assignment operator
    LexItem op = Parser::GetNextToken(in, line);
    if (op != ASSOP && op != ADDASSOP && op != SUBASSOP &&
        op != MULASSOP && op != DIVASSOP && op != REMASSOP)
    {
        ParseError(line, "Missing or invalid assignment operator.");
        return false;
    }

    // Step 3: Parse the expression (right-hand side)
    if (!Expr(in, line))
    {
        ParseError(line, "Missing Expression in Assignment Statement");
        return false;
    }

    // LexItem semicolon = Parser::GetNextToken(in, line);
    // if (semicolon != SEMICOL)
    // {
    //     Parser::PushBackToken(semicolon);
    // }

    return true; // Successful parsing of assignment statement
}

bool CheckUndeclaredVar(istream &in, int &line)
{
    LexItem t = Parser::GetNextToken(in, line);

    // Check if the variable has been declared
    string varName = t.GetLexeme();
    if (declaredVariables.find(varName) == declaredVariables.end())
    {
        ParseError(line, "Undeclared Variable");
        return false;
    }

    return true;
}

bool CheckRedeclaredVar(istream &in, int &line)
{
    LexItem t = Parser::GetNextToken(in, line);

    // Check if the variable has been declared
    string varName = t.GetLexeme();
    if (declaredVariables.find(varName) != declaredVariables.end())
    {
        ParseError(line, "Variable Redefinition");
        return false;
    }

    return true;
}

bool ValidStmtToken(istream &in, int &line)
{
    LexItem t = Parser::GetNextToken(in, line);
    Parser::PushBackToken(t);

    if (t == INT || t == FLOAT || t == BOOL || t == CHAR || t == STRING ||
        t == LBRACE || t == IF || t == IDENT || t == PRINT || t == RBRACE)
    {
        return true;
    }

    return false;
}

bool IfStmt(istream &in, int &line)
{
    LexItem t = Parser::GetNextToken(in, line);
    if (t != LPAREN)
    {
        ParseError(line, "Missing left parenthesis in If statement");
        return false;
    }

    if (!Expr(in, line))
    {
        ParseError(line, "Missing if statement condition");
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if (t != RPAREN)
    {
        ParseError(line, "Missing right parenthesis in If statement");
        return false;
    }

    IfMessage("in IfStmt then-clause at nesting level", ++ifLevel);
    if (!Stmt(in, line))
    {
        return false;
    }

    // Handle optional else clause
    t = Parser::GetNextToken(in, line);
    if (t == ELSE)
    {
        IfMessage("in IfStmt else-clause at nesting level", ifLevel);
        if (!Stmt(in, line))
        {
            return false;
        }
        --ifLevel;
    }
    else
    {
        Parser::PushBackToken(t);
    }

    return true;
}

bool VarList(istream &in, int &line)
{
    LexItem t = Parser::GetNextToken(in, line);
    if (t != IDENT)
    {
        ParseError(line, "Missing variable name");
        return false;
    }

    Parser::PushBackToken(t);
    if (!CheckRedeclaredVar(in, line))
    {
        return false;
    }
    string varName = t.GetLexeme();
    declaredVariables.insert(varName);
    defVar[t.GetLexeme()] = false;

    // Check for initialization
    t = Parser::GetNextToken(in, line);
    if (t == ASSOP)
    {
        // Handle initialization expression
        if (!Expr(in, line))
        {
            return false;
        }
        Message("Initialization of the variable " + varName + " in the declaration statement at line ", line);
        // Mark variable as initialized
        defVar[t.GetLexeme()] = true;
    }
    else
    {
        Parser::PushBackToken(t); // Push back if no assignment
    }

    // Handle possible additional variables
    while (true)
    {
        t = Parser::GetNextToken(in, line);
        if (t != COMMA)
        {
            Parser::PushBackToken(t);
            break;
        }
        t = Parser::GetNextToken(in, line);
        if (t != IDENT)
        {
            ParseError(line, "Missing variable name after comma");
            return false;
        }

        Parser::PushBackToken(t);
        if (!CheckRedeclaredVar(in, line))
        {
            return false;
        }
        string varName = t.GetLexeme();
        declaredVariables.insert(varName);
        defVar[t.GetLexeme()] = false;

        // Check for initialization
        t = Parser::GetNextToken(in, line);
        if (t == ASSOP)
        {
            if (!Expr(in, line))
            {
                return false;
            }
            Message("Initialization of the variable " + varName + " in the declaration statement at line ", line);
            defVar[t.GetLexeme()] = true;
        }
        else
        {
            Parser::PushBackToken(t);
        }
    }

    return true;
}

bool Var(istream &in, int &line)
{
    LexItem t = Parser::GetNextToken(in, line);
    if (t != IDENT)
    {
        ParseError(line, "Expected variable name");
        return false;
    }
    // Possibly check if the variable is declared
    return true;
}

bool Expr(istream &in, int &line)
{
    if (!LogANDExpr(in, line))
    {
        return false;
    }

    LexItem t = Parser::GetNextToken(in, line);
    while (t == OR)
    {
        if (!LogANDExpr(in, line))
        {
            return false;
        }
        t = Parser::GetNextToken(in, line);
    }

    Parser::PushBackToken(t);
    return true;
}

bool LogANDExpr(istream &in, int &line)
{
    if (!EqualExpr(in, line))
    {
        return false;
    }

    LexItem t = Parser::GetNextToken(in, line);
    while (t == AND)
    {
        if (!EqualExpr(in, line))
        {
            return false;
        }
        t = Parser::GetNextToken(in, line);
    }

    Parser::PushBackToken(t);
    return true;
}

bool EqualExpr(istream &in, int &line)
{
    if (!RelExpr(in, line))
    {
        return false;
    }

    LexItem t = Parser::GetNextToken(in, line);
    if (t == EQ || t == NEQ)
    {
        if (!RelExpr(in, line))
        {
            return false;
        }

        LexItem nextToken = Parser::GetNextToken(in, line);
        if (nextToken == EQ || nextToken == NEQ)
        {
            ParseError(line, "Illegal Equality Expression.");
            return false;
        }
        else
        {
            Parser::PushBackToken(nextToken);
        }
    }
    else
    {
        Parser::PushBackToken(t);
    }

    return true;
}

bool RelExpr(istream &in, int &line)
{
    if (!AddExpr(in, line))
    {
        return false;
    }

    LexItem t = Parser::GetNextToken(in, line);
    if (t == LTHAN || t == GTHAN)
    {
        if (!AddExpr(in, line))
        {
            return false;
        }

        LexItem nextToken = Parser::GetNextToken(in, line);
        if (nextToken == LTHAN || nextToken == GTHAN)
        {
            ParseError(line, "Illegal Relational Expression.");
            return false;
        }
        else
        {
            Parser::PushBackToken(nextToken);
        }
    }
    else
    {
        Parser::PushBackToken(t);
    }

    return true;
}

bool AddExpr(istream &in, int &line)
{
    if (!MultExpr(in, line))
    {
        return false;
    }

    LexItem t = Parser::GetNextToken(in, line);
    while (t == PLUS || t == MINUS)
    {
        if (!MultExpr(in, line))
        {
            return false;
        }
        t = Parser::GetNextToken(in, line);
    }

    Parser::PushBackToken(t);
    return true;
}

bool MultExpr(istream &in, int &line)
{
    if (!UnaryExpr(in, line))
    {
        return false;
    }

    LexItem t = Parser::GetNextToken(in, line);
    while (t == MULT || t == DIV || t == REM)
    {
        if (!UnaryExpr(in, line))
        {
            return false;
        }
        t = Parser::GetNextToken(in, line);
    }

    Parser::PushBackToken(t);
    return true;
}

bool UnaryExpr(istream &in, int &line)
{
    LexItem t = Parser::GetNextToken(in, line);
    if (t == PLUS || t == MINUS || t == NOT)
    {
        return UnaryExpr(in, line); // Process unary recursively
    }
    else
    {
        Parser::PushBackToken(t);
        return PrimaryExpr(in, line, t.GetToken() == MINUS ? -1 : 1); // Pass sign
    }
}

bool PrimaryExpr(istream &in, int &line, int sign)
{
    LexItem t = Parser::GetNextToken(in, line);
    if (t == IDENT)
    {
        // Check for variable declaration
        return true;
    }
    else if (t == ICONST || t == RCONST || t == SCONST || t == BCONST || t == CCONST)
    {
        return true; // Constant types
    }
    else if (t == LPAREN)
    {
        if (!Expr(in, line))
        {
            ParseError(line, "Missing expression after Left Parenthesis");
            return false;
        }
        t = Parser::GetNextToken(in, line);
        if (t != RPAREN)
        {
            ParseError(line, "Missing right parenthesis");
            return false;
        }
        return true;
    }

    ParseError(line, "Invalid primary expression");
    return false;
}

// PrintStmt:= PRINT (ExpreList)
bool PrintStmt(istream &in, int &line)
{
    LexItem t;
    // cout << "in PrintStmt" << endl;

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
        return false;
    }

    t = Parser::GetNextToken(in, line);
    if (t != RPAREN)
    {

        ParseError(line, "Missing Right Parenthesis");
        return false;
    }
    // Evaluate: print out the list of expressions values

    return true;
} // End of PrintStmt

// ExprList:= Expr {,Expr}
bool ExprList(istream &in, int &line)
{
    bool status = false;
    // cout << "in ExprList and before calling Expr" << endl;
    status = Expr(in, line);
    if (!status)
    {
        ParseError(line, "Missing Expression");
        return false;
    }

    LexItem tok = Parser::GetNextToken(in, line);

    if (tok == COMMA)
    {
        // cout << "before calling ExprList" << endl;
        status = ExprList(in, line);
        // cout << "after calling ExprList" << endl;
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