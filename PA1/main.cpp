#include <fstream>
#include <vector>
#include "lex.h"
using namespace std;

vector<LexItem> format(vector<LexItem> tokens)
{
    vector<LexItem> fixedTokens = tokens;

    for (int i = 0; i < fixedTokens.size(); i++)
    {
        int count = 0;
        for (int j = 0; j < fixedTokens.size(); j++)
        {
            if (fixedTokens[i].GetToken() == ICONST || fixedTokens[i].GetToken() == RCONST)
            {
                if (fixedTokens[j].GetToken() == ICONST || fixedTokens[j].GetToken() == RCONST)
                {
                    if (stof(fixedTokens[i].GetLexeme()) == stof(fixedTokens[j].GetLexeme()))
                    {
                        if (count > 0)
                        {
                            fixedTokens.erase(fixedTokens.begin() + j);
                            j--;
                        }
                        else
                        {
                            count++;
                        }
                    }
                }
            }
            else if (fixedTokens[i].GetLexeme() == fixedTokens[j].GetLexeme())
            {
                if (count > 0)
                {
                    fixedTokens.erase(fixedTokens.begin() + j);
                    j--;
                }
                else
                {
                    count++;
                }
            }
        }
    }

    for (int i = 0; i < fixedTokens.size(); i++)
    {
        for (int j = i + 1; j < fixedTokens.size(); j++)
        {
            if ((fixedTokens[i].GetToken() == ICONST || fixedTokens[i].GetToken() == RCONST) && (fixedTokens[j].GetToken() == ICONST || fixedTokens[j].GetToken() == RCONST))
            {
                if (std::stof(fixedTokens[j].GetLexeme()) < std::stof(fixedTokens[i].GetLexeme()))
                {
                    LexItem temp = fixedTokens[i];
                    fixedTokens[i] = fixedTokens[j];
                    fixedTokens[j] = temp;
                }
            }
            else if (fixedTokens[j].GetLexeme() < fixedTokens[i].GetLexeme())
            {
                LexItem temp = fixedTokens[i];
                fixedTokens[i] = fixedTokens[j];
                fixedTokens[j] = temp;
            }
        }
    }
    return fixedTokens;
}

int main(int argc, char **argv)
{
    string filename;
    int all = 0;
    int bconst = 0;
    int sconst = 0;
    int ident = 0;
    int cconst = 0;
    int nconst = 0;
    int kw = 0;
    LexItem currentToken;
    int linenum = 1;

    if (argc == 1)
    {
        cout << "No specified input file." << endl;
        return 1;
    }

    ifstream file(filename);

    if (!file)
    {
        cout << "CANNOT OPEN THE FILE " << argv[1] << endl;
        return 1;
    }

    if (file.peek() == ifstream::traits_type().eof())
    {
        cout << "Empty File." << endl;
        return 1;
    }

    for (int i = 1; i < argc; i++)
    {
        if (string(argv[i]) == "-bool")
        {
            bconst++;
        }
        else if (string(argv[i]) == "-str")
        {
            sconst++;
        }
        else if (string(argv[i]) == "-char")
        {
            cconst++;
        }
        else if (string(argv[i]) == "-num")
        {
            nconst++;
        }
        else if (string(argv[i]) == "-id")
        {
            ident++;
        }
        else if (string(argv[i]) == "-kw")
        {
            kw++;
        }
        else if (argv[i][0] == '-')
        {
            cout << "UNRECOGNIZED FLAG {" << argv[i] << "}" << endl;
            return 1;
        }
        else if (string(filename) == "")
        {
            filename = argv[i];
        }
        else
        {
            cout << "ONLY ONE FILE NAME IS ALLOWED." << endl;
            return 1;
        }
    }

    vector<LexItem> allTokens;
    vector<LexItem> bcon;
    vector<LexItem> scon;
    vector<LexItem> iden;
    vector<LexItem> ncon;
    vector<LexItem> ccon;
    vector<LexItem> kwvec;

    while (true)
    {
        currentToken = getNextToken(file, linenum);
        if (currentToken.GetToken() == DONE)
        {
            break;
        }
        allTokens.push_back(currentToken);
        if (currentToken.GetToken() == SCONST)
        {
            scon.push_back(currentToken);
        }
        else if (currentToken.GetToken() == BCONST)
        {
            bcon.push_back(currentToken);
        }
        else if (currentToken.GetToken() == IDENT)
        {
            iden.push_back(currentToken);
        }
        else if ((currentToken.GetToken() == RCONST || currentToken.GetToken() == ICONST))
        {
            ncon.push_back(currentToken);
        }
        else if (currentToken.GetToken() == ERR)
        {
            break;
        }
    }

    bcon = format(bcon);
    ncon = format(ncon);
    scon = format(scon);
    iden = format(iden);
    ccon = format(ccon); 
    kwvec = format(kwvec);

    // Work on the logic for lines 194-204

    if (all)
    {
        for (int i = 0; i < allTokens.size(); i++)
        {
            cout << allTokens[i] << "\n";
            if (allTokens[i].GetToken() == ERR)
            {
                return 1;
            }
        }
    }
    cout << "\nLines: " << (linenum - 1) << endl;
    cout << "Total Tokens: " << allTokens.size() << endl;
    cout << "Identifiers and Keywords: " << iden.size() << endl;
    cout << "Numerics: " << ncon.size() << endl;
    cout << "Booleans: " << bcon.size() << endl;
    cout << "Strings and Characters: " << scon.size() << endl;

    if (ident)
    {
        cout << "IDENTIFIERS:" << endl;

        for (int i = 0; i < iden.size(); i++)
        {
            cout << iden[i].GetLexeme();
            if (i < (iden.size() - 1))
            {
                cout << ", ";
            }
        }

        cout << "\n";
    }
    
    if (kw)
    {
        cout << "KEYWORDS:" << endl;

        for (int i = 0; i < kwvec.size(); i++)
        {
            cout << kwvec[i].GetLexeme();
            if (i < (kwvec.size() - 1))
            {
                cout << ", ";
            }
        }

        cout << "\n";
    }

    if (nconst)
    {
        cout << "NUMERIC CONSTANTS:" << endl;

        // Initialize a flag to check if it's the first element
        bool first = true;

        for (int i = 0; i < ncon.size(); i++)
        {
            string lexNumConst = ncon[i].GetLexeme();
            int length = lexNumConst.length();

            // Handle numbers ending with a dot (.)
            if (length > 0 && lexNumConst[length - 1] == '.')
            {
                // If the last character is a dot, print everything but the dot
                lexNumConst = lexNumConst.substr(0, length - 1);
            }

            // Print the numeric constant
            if (first)
            {
                cout << lexNumConst;     // Print the first element without a leading comma
                first = false; // Set the flag to false after the first element
            }
            else
            {
                cout << ", " << lexNumConst; // Print a comma and then the current numeric constant
            }
        }

        cout << "\n"; // Print a newline at the end of the list
    }

    if (bconst)
    {
        cout << "BOOLEAN CONSTANTS:" << endl;
        for (int i = 0; i < bcon.size(); i++)
        {
            cout << bcon[i].GetLexeme() << endl;
        }
    }

    if (sconst)
    {
        cout << "STRINGS:" << endl;
        for (int i = 0; i < scon.size(); i++)
        {
            cout << "\"" << scon[i].GetLexeme() << "\"" << endl;
        }
    }
    
    if (cconst)
    {
        cout << "CHARACTER CONSTANTS:" << endl;
        for (int i = 0; i < ccon.size(); i++)
        {
            cout << "\"" << ccon[i].GetLexeme() << "\"" << endl;
        }
    }
    // Tokenize
    // LexItem currentToken;
    // int currentLineNum = 1;
    // while((currentToken = getNextToken(file, currentLineNum)).GetToken() != DONE) {
    // cout << currentToken << endl;
    //}

    file.close();

    return 0;
}