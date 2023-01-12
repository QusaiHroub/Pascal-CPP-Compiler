# Pascal-CPP-Compiler

## Objective
This project is to implement a part of a compiler for the language specified below.   
The language is called Sub-Pascal, lacking some of Pascal’s features like arrays, records, constants, certain data types and procedures/functions for purposes of simplicity. Sub-Pascal is case insensitive. The project will include a scanner, a parser and a special code generator that generates a standard C++ equivalent program. 
## The specifications for the language's syntax are as follows
### Sub-Pascal Syntax
```
Program -> Header Declarations Block
Header -> program id(input,output) ;
Declarations -> VAR VariableDeclarations | <epsilon>
VariableDeclarations -> VariableDeclarations VariableDeclaration | VariableDeclaration
VariableDeclaration -> IdentifierList : Type ;
IdentifierList -> IdentifierList , id | id
Type -> integer | real | char | boolean
Block -> { Statements }
Statements -> Statements ; Statement | Statement
Statement -> id := Expression | Block 
                              | if Expression then Statement ElseClause 
                              | for id:=num to|downto num do Statement
                              | <epsilon>
ElseClause -> else Statement | <epsilon>
ExpressionList -> ExpressionList , Expression | Expression
Expression -> SimpleExpression relop SimpleExpression | SimpleExpression
SimpleExpression -> Term | addop Term
                         | SimpleExpression addop Term
Term -> Term mulop Factor | Factor
Factor -> id | num | ( Expression ) | not Factor
```
### Lexical Conventions
Blanks between tokens are optional, except reserved words must be surrounded by blanks, newlines, and the beginning of the program.
#### Token id matches a letter or a letter followed by letters and digits"
```
letter -> [a-zA-Z]
digit -> [0-9]
id -> letter (letter | digit)*
```
#### Token num matches unsigned integers
```
digits -> digit digit*
optional_fractional -> . digits | ->
optional_exponent -> ( E (+ | - | e ) digits) | ->
num -> digits optional_fraction optional_exponent
```
Keywords are reserved and appear in boldface in the grammar   
The relational operators (relops) are =, >, <, >=, <=, <>   
The ** addops ** are +, -, OR   
The mulops are *, /, DIV, MOD, AND   
The lexeme for token assignop is :=   
### Normalized Grammar:
```
Program -> Header Declarations Block
Header -> program id(input, output);
Declarations-> VAR VariableDeclarations | <epsilon>
VariableDeclarations-> VariableDeclaration next_VariableDeclaration
next_VariableDeclaration-> VariableDeclaration next_VariableDeclaration | <epsilon>
VariableDeclaration-> IdentifierList : Type;
IdentifierList-> id next_id
next_id -> ,IdentifierList next_id | <epsilon>
Type -> integer | real | char | boolean
Block -> { Statements }
Statements -> Statement next_statement
next_statement -> ;Statement next_statement | <epsilon>
Statement -> id:=Expression | Block | if Expression then Statement ElseClause |
for id:=num to | downto num do Statement
ElseClause -> else Statement | <epsilon>
Expression -> SimpleExpression relop SimpleExpression | SimpleExpression
SimpleExpression -> Term | addop Term | SimpleExression addop Term
Term -> Factor next_Term
next_Term -> Term next_Term | <epsilon>
Factor -> id | num | (Expression) | not Factor
```
Build
-----
```
mkdir build
cd build
meson ..
ninja
```
