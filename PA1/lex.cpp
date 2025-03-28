#include <cctype>
#include "lex.h"

enum TokState {
    START,
    IN_IDENT,
    IN_ERR
};

LexItem delimiter(char& c, int linenum) {
    switch(c) {
        case ',':
            return LexItem(COMMA, ",", linenum);
        case ';':
            return LexItem(SEMICOL, ";", linenum);
        case '(':
            return LexItem(LPAREN, "(", linenum);
        case ')':
            return LexItem(RPAREN, ")", linenum);
        case '.':
            return LexItem(DOT, ".", linenum);
        case '{':
            return LexItem(LBRACE, "{", linenum);
        case '}':
            return LexItem(RBRACE, "}", linenum);
        default:
            return LexItem();
    }
}

ostream& operator<<(ostream& out, const LexItem& tok) {
    if(tok.GetToken() == IDENT) {
        out << "IDENT: <" << tok.GetLexeme() << ">";
    } else {
        out << tok.GetToken() << ": " << tok.GetLexeme();
    }

    return out;
}

LexItem id_or_kw(const string& lexeme, int linenum) {
    // Implement this, for now we assume everything is an identifier.
    return LexItem(IDENT, lexeme, linenum);
}

LexItem getNextToken(istream& in, int& linenum) {
    char c = 0;
    string lexeme = "";
    LexItem returnToken;
    TokState lexstate = TokState::START;

    // Skip whitespaces
    while(isspace(c = in.get())) {}

    // Check if delimiter char or EOF
    returnToken = delimiter(c, linenum);
    if(returnToken.GetToken() != ERR) {
        return returnToken;
    } else if (c == EOF) {
        return LexItem(DONE, "", linenum);
    } else {
        // Else put back into input stream
        in.putback(c);
    }

    // Main DFA Loop
    while(in.get(c)) {
        // Check if whitespace or delimiter, put back if it is.
        if(isspace(c) || delimiter(c, linenum).GetToken() != ERR) {
            in.putback(c);
            break;
        }

        // Add current char to the lexeme
        lexeme += c;

        // Tokenize based on derivation rules of terminal symbols.
        switch(lexstate) {
            case TokState::START:
                // Check if first char is letter or '_'
                if(isalpha(c) || c == '_') {
                    lexstate = TokState::IN_IDENT;
                } else {
                    // Whole bunch of other checks for the rest of the terminal symbols.
                    lexstate = TokState::IN_ERR;
                }
                break;
            case TokState::IN_IDENT:
                // Check if not a valid identifier
                if(!isalnum(c) && c != '_') {
                    // Check if whitespace or delimiter, put back if it is.
                    if(isspace(c) || delimiter(c, linenum).GetToken() != ERR) {
                        in.putback(c);
                    } else {
                        lexstate = TokState::IN_ERR;
                    }
                }
                break;
            case TokState::IN_ERR:
                // Demo: Read the rest of the chars until whitespace or EOF.
                c = in.get();
                while(!isspace(c) && c != EOF) {
                    lexeme += c;
                    c = in.get();
                }
                in.putback(c);
                break;
        }
    }

    // Check if identifier, then call id_or_kw
    if(lexstate == TokState::IN_IDENT) {
        return id_or_kw(lexeme, linenum);
    }

    // Default error token return
    return LexItem(ERR, lexeme, linenum);
}