/* Qusai Hroub
 *
 * Grammar:
 * Program -> Header Declarations Block
 * Header -> program id(input, output);
 * Declarations-> VAR VariableDeclarations | <epsilon>
 * VariableDeclarations-> VariableDeclaration next_VariableDeclaration
 * next_VariableDeclaration-> VariableDeclaration next_VariableDeclaration | <epsilon>
 * VariableDeclaration-> IdentifierList : Type;
 * IdentifierList-> id next_id
 * next_id -> ,IdentifierList next_id | <epsilon>
 * Type -> integer | real | char | boolean
 * Block -> { Statements }
 * Statements -> Statement next_statement
 * next_statement -> ;Statement next_statement | <epsilon>
 * Statement -> id:=Expression | Block | if Expression then Statement ElseClause |
 * for id:=num to | downto num do Statement
 * ElseClause -> else Statement | <epsilon>
 * Expression -> SimpleExpression relop SimpleExpression | SimpleExpression
 * SimpleExpression -> Term | addop Term | SimpleExression addop Term
 * Term -> Factor next_Term
 * next_Term -> Term next_Term | <epsilon>
 * Factor -> id | num | (Expression) | not Factor
 */

#include <vector>
#include <string>
#include <iostream>
#include <cstring>
#include <stdio.h>

using std::vector;
using std::string;
using std::cout;
using std::cin;
using std::cerr;

const char endl = '\n';
const string SYNTAX_ERROR = "syntax error";

#define BSIZE 128
#define NONE -1
#define EOS '\0'

#define PROGRAM 256
#define INPUT 257
#define OUTPUT 258
#define VAR 259
#define INTEGER 260
#define REAL 261
#define CHAR 262
#define BOOLEAN 263
#define IF 264
#define THEN 265
#define FOR 266
#define TO 267
#define DOWNTO 268
#define DO 269
#define ELSE 270
#define ID 271
#define DIV 272
#define MOD 273
#define AND 274
#define OR 275
#define NUM 276
#define EQUAL 277
#define IS_EQUAL 278
#define GRETER_THAN 279
#define SMALLER_THAN 280
#define GRETER_THAN_OR_EQUAL 281
#define SMALLER_THAN_OR_EQUAL 282
#define NOT_EQUAL 283
#define IINC 284
#define IDEC 285
#define NOT 286

#define DONE 500
#define CPP_HEADER 501
#define CPP_MAIN_FN 502
#define STRMAX 9999
#define SYMMAX 1000

int lineno = 1;
int tokenval = NONE;
int lookahead;
int lastchar = - 1;
int lastentry = 0;

string idListStr;
bool isItParssDeclarations;
bool isProgramNameReaded;

struct entry {
    char *lexptr;
    int token;
};

struct entry keywords [] = {
    (char *)"PROGRAM", PROGRAM,
    (char *)"INPUT", INPUT,
    (char *)"OUTPUT", OUTPUT,
    (char *)"VAR", VAR,
    (char *)"INTEGER", INTEGER,
    (char *)"REAL", REAL,
    (char *)"CHAR", CHAR,
    (char *)"BOOLEAN", BOOLEAN,
    (char *)"IF", IF,
    (char *)"THEN", THEN,
    (char *)"FOR", FOR,
    (char *)"TO", TO,
    (char *)"DOWNTO", DOWNTO,
    (char *)"DO", DO,
    (char *)"ELSE", ELSE,
    (char *)":=", EQUAL,
    (char *)"=", IS_EQUAL,
    (char *)">", GRETER_THAN,
    (char *)"<", SMALLER_THAN,
    (char *)">=", GRETER_THAN_OR_EQUAL,
    (char *)"<=", SMALLER_THAN_OR_EQUAL,
    (char *)"<>", NOT_EQUAL,
    (char *)"NOT", NOT,
    0, 0
};

vector<entry> symtable(SYMMAX, entry());
char lexbuf[BSIZE];
char lexemes[STRMAX];

void header ();
void declarations ();
void mainBlock ();
void block ();

void varDeclaration ();
void varDeclarations ();
void idList ();
void type ();

void statement ();
void statements ();
void simpleExpr ();

void If ();
void Else ();

void For ();
void ForB (const int);

int lexan ();

void parse ();
void expr ();
void term ();
void factor ();
void match (int);

void emit (const int, const int = NONE);
void put (const int, const int = NONE);

int lookup (const char *);
int insert (const char *, const int);

void init ();

void error (const string &);
void r_error (const string &);

int main (int argc, char * argv[]) {
    if (argc == 1) {
        r_error("Please enter the output file path as the first command line argument");
    }

    const string input = argv[1];
    const string output = argc == 3 ? argv[2] : string(input.begin(), input.begin() + input.rfind('.')) + ".cc";

    if (freopen (input.c_str(), "r", stdin) == nullptr) {
        r_error(input + " file doesn't exists");
    }

    freopen (output.c_str(), "w", stdout);
    init ();
    parse ();
    exit (0);
}

void init () {
    entry *p;

    for (p = keywords; p->token; ++p) {
        insert(p->lexptr, p->token);
    }
}

int insert (const char *s, const int tok) {
    const int len = strlen(s);

    if (lastentry + 1 >= SYMMAX) {
        error ("Symbol table full");
    }

    if (lastchar + len + 1 >= STRMAX) {
        error ("Lexemes array full");
    }

    lastentry = lastentry + 1;
    symtable[lastentry].token = tok;
    symtable[lastentry].lexptr = &lexemes[lastchar + 1];
    lastchar = lastchar + len + 1;
    strcpy(symtable[lastentry].lexptr, s);

    return lastentry;
}

int lookup (const char *s) {
    for (int p = lastentry; p > 0; --p) {
        if (strcmp(symtable[p].lexptr, s) == 0) {
            return p;
        }
    }

    return 0;
}

void parse () {
    lookahead = lexan ();
    isProgramNameReaded = false;
    header ();
    isProgramNameReaded = true;
    isItParssDeclarations = true;
    declarations ();
    isItParssDeclarations = false;
    mainBlock ();
}

void header () {
    switch (lookahead) {
        case PROGRAM:
            match (PROGRAM); match (ID);
            match ('('); match (INPUT);
            match (','); match (OUTPUT);
            match (')'); match (';');
            put(CPP_HEADER);
            break;
        default:
            error (SYNTAX_ERROR);
    }
}

void declarations () {
    if (lookahead == VAR) {
        match (VAR);
        varDeclarations ();
    }
}

void varDeclaration () {
    idList ();
    match (':'); type (); match (';');
    cout << idListStr;
    put (';');
}

void varDeclarations () {
    varDeclaration ();

    while (true) {
        switch ( lookahead ) {
            case ID:
                idListStr = "";
                varDeclaration ();
                continue;
            default:
                return;
        }
    }
}

void idList () {
    idListStr += symtable[tokenval].lexptr;
    match (ID);

    while (true) {
        switch ( lookahead ) {
            case ',':
                match(','); idListStr += ", "; idList ();
                continue;
            default:
                return;
        }
    }
}

void type () {
    switch ( lookahead ) {
        case INTEGER:
            match (INTEGER); emit (INTEGER); break;
        case REAL:
            match (REAL); emit (REAL); break;
        case CHAR:
            match (CHAR); emit (CHAR); break;
        case BOOLEAN:
            match (BOOLEAN); emit (BOOLEAN); break;
        default:
            error("Type must be one of INTEGER, REAL, CHAR and BOOLEAN");
    }
}

void mainBlock () {
    match ('{');
    put (CPP_MAIN_FN);
    statements ();
    match ('}');
    put ('}');
}

void block () {
    match ('{');
    put ('{');
    statements ();
    match ('}');
    put ('}');
}

void statements () {
    statement ();

    if (lookahead == ';') {
        match(lookahead);
        statements ();
    }
}

void statement () {
    switch ( lookahead ) {
        case ID:
            emit (ID, tokenval); match (ID); match (EQUAL); put (EQUAL); expr (); put (';'); break;
        case '{':
            block (); break;
        case IF:
            If (); break;
        case FOR:
            For (); break;
        default:
            error (SYNTAX_ERROR);
    }
}

void If () {
    switch (lookahead) {
        case IF:
            match (lookahead); put (IF); put ('('); expr (); put (')'); match (THEN); cout << endl; statement (); Else (); break;
        default:
            return;
    }
}

void Else () {
    switch (lookahead) {
        case ELSE:
            match(ELSE); put(ELSE); statement(); break;
        default:
            return;
    }
}

void For () {
    int t_tokenval;

    switch (lookahead) {
        case FOR:
            match(lookahead); put(FOR); put('('); match(ID); t_tokenval = tokenval;
            put(ID, tokenval); match(EQUAL); emit(EQUAL); emit(NUM, tokenval);
            match(NUM); emit(';'); ForB(t_tokenval); put(')'); match(DO); cout << endl;
            statement(); break;
        default:
            return;
    }
}

void ForB (const int t_tokenval) {
    switch (lookahead) {
        case TO:
            put(lookahead, t_tokenval); match(lookahead); emit(NUM, tokenval);
            match(NUM); emit(';'); put(ID, t_tokenval); put(IINC); break;
        case DOWNTO:
            put(lookahead, t_tokenval); match(lookahead); emit(NUM, tokenval);
            match(NUM); emit(';'); put(ID, t_tokenval); put(IDEC); break;
        default:
            error("Error");
    }
}

int lexan () {
    int t;

    while (true) {
        t = getchar();

        if (t == ' ' || t == '\t') {

        } else if (t == '\n') {
            lineno++;
        } else if (isdigit(t)) {
            ungetc (t, stdin);
            scanf ("%d", &tokenval);

            return NUM;
        } else if (isalpha(t)) {
            int p, b = 0;

            while (isalnum(t)) {
                lexbuf[b] = toupper(t);
                t = getchar ();
                b++;

                if (b >= BSIZE) {
                    error ("compiler error");
                }
            }

            lexbuf[b] = EOS;

            if (t == '.' && !strcmp (lexbuf, "end")) {
                lexbuf[b] = '.';
                lexbuf[b + 1] = EOS;
                t = getchar();
            }

            if (t != EOF) {
                ungetc(t, stdin);
            }

            p = lookup (lexbuf);
            if (p == ID && isItParssDeclarations) {
                cout << lexbuf << endl;
                error ("Duplicated ID");
            }

            if (p == 0 && (isItParssDeclarations || !isProgramNameReaded)) {
                p = insert (lexbuf, ID);
            }

            if (p == 0 && isProgramNameReaded) {
                char *err = (char *)calloc(BSIZE + 18, sizeof(char));
                strcpy (err, lexbuf);
                strcat (err, " sym is undefined");
                error (err);
            }

            tokenval = p;

            return symtable[p].token;
        } else if (t == ':') {
            t = getchar ();

            if (t == '=') {
                return EQUAL;
            }

            if (t != EOF) {
                ungetc(t, stdin);
            }

            return ':';
        } else if (t == '=') {
            return IS_EQUAL;
        } else if (t == '>') {
            t = getchar ();

            if (t == '=') {
                return GRETER_THAN_OR_EQUAL;
            }

            if (t != EOF) {
                ungetc (t, stdin);
            }

            return GRETER_THAN;
        } else if (t == '<') {
            t = getchar ();

            switch(t) {
                case '=':
                    return SMALLER_THAN_OR_EQUAL;
                case '>':
                    return NOT_EQUAL;
            }

            if (t != EOF) {
                ungetc (t, stdin);
            }

            return SMALLER_THAN;
        } else if ( t == EOF) {
            return DONE;
        } else {
            tokenval = NONE;

            return t;
        }
    }
}

void expr () {
    simpleExpr ();
    int t;

    while (true) {
        switch ( lookahead ) {
            case IS_EQUAL: case GRETER_THAN: case SMALLER_THAN:
            case GRETER_THAN_OR_EQUAL: case SMALLER_THAN_OR_EQUAL:
            case NOT_EQUAL:
                t = lookahead;
                match (lookahead); emit (t); simpleExpr ();
                continue;
            default:
                return;
        }
    }
}

void simpleExpr () {
    int t;

    switch ( lookahead ) {
        case '+': case '-':
            emit (lookahead); match (lookahead); term (); break;
        default:
            term ();

            while (true) {
                switch ( lookahead ) {
                    case '+': case '-': case OR:
                        t = lookahead;
                        match (lookahead); emit (t); simpleExpr ();
                        continue;
                    default:
                        return;
                }
            }
    }
}

void term () {
    int t;
    factor ();

    while (true) {
        switch(lookahead) {
            case '*': case '/': case DIV: case MOD: case AND:
                t = lookahead;
                match (lookahead); emit (t, NONE); factor ();
                continue;
            default:
                return;
        }
    }
}

void factor () {
    switch(lookahead) {
        case '(':
            match ('('); put ('('); simpleExpr (); match (')'); put (')'); break;
        case NUM:
            emit (NUM, tokenval); match (NUM); break;
        case ID:
            put (ID, tokenval); match (ID); break;
        case NOT:
            put (NOT); match (NOT); factor (); break;
        default:
            error (SYNTAX_ERROR);
    }
}

void match (int t) {
    if (lookahead == t) {
        lookahead = lexan();
    } else {
        error(SYNTAX_ERROR);
    }
}

void emit (const int t, const int tval) {
    switch(t) {
        case '+': case '-': case '*': case '/':
            cout << ' ' << (char)t; break;
        case DIV:
            cout << "div "; break;
        case MOD:
            cout << "mod "; break;
        case OR:
            cout << "| "; break;
        case AND:
            cout << "& "; break;
        case IS_EQUAL:
            cout << ' ' << "=="; break;
        case GRETER_THAN:
            cout << ' ' << ">"; break;
        case SMALLER_THAN:
            cout << ' ' << "<"; break;
        case GRETER_THAN_OR_EQUAL:
            cout << ' ' << ">="; break;
        case SMALLER_THAN_OR_EQUAL:
            cout << ' ' << "<="; break;
        case NOT_EQUAL:
            cout << ' ' << "!="; break;
        case INTEGER:
            cout << "int "; break;
        case REAL:
            cout << "float "; break;
        case CHAR:
            cout << "char "; break;
        case BOOLEAN:
            cout << "bool "; break;
        case NUM:
            cout << ' ' << tval; break;
        case ID:
            cout << symtable[tval].lexptr << ' '; break;
        case ';':
            cout << ";"; break;
        case EQUAL:
            cout << " ="; break;
        default:
            cout << "Token " << t << " tokenval" << tval << endl;
    }
}

void put (const int t, const int tval) {
     switch(t) {
        case '+': case '-': case '*': case '/': case ';':
            cout << (char)t << endl; break;
        case ',':
            cout << (char)t; break;
        case '(': case ')':
            cout << ' ' << (char)t; break;
        case DIV:
            cout << "div" << endl; break;
        case MOD:
            cout << "mod" << endl; break;
        case NUM:
            cout << tval << endl; break;
        case CPP_HEADER:
            cout << "#include <iostream>\nusing namespace std;" << endl; break;
        case CPP_MAIN_FN:
            cout << "void main (void) {" << endl; break;
        case '{':
            cout << '{' << endl; break;
        case '}':
            cout << '}' << endl; break;
        case EQUAL:
            cout << "="; break;
        case ID:
            cout << ' ' << symtable[tval].lexptr; break;
        case IF:
            cout << "if"; break;
        case ELSE:
            cout << "else" << endl; break;
        case FOR:
            cout << "for"; break;
        case TO:
            cout << " " <<symtable[tval].lexptr << " <="; break;
        case DOWNTO:
            cout << " " << symtable[tval].lexptr << " >="; break;
        case IINC:
            cout << "++"; break;
        case IDEC:
            cout << "--"; break;
        case NOT:
            cout << " ~"; break;
        default:
            cout << "Token " << t << " tokenval" << tval << endl;
    }
}

void error (const string &m) {
    cerr << "Line " << lineno << ": " << m << endl;
    exit(1);
}

void r_error (const string &m) {
    cerr << m << endl;
    exit(-1);
}
