grammar boosd;

options {
    language = C;
    output = AST;
}


statements
    : statement*
    ;

statement
    : assignment
    ;

assignment
    : ID '='^ expr ';' NEWLINE?
    ;

expr
    : loose
    ;

loose
    : tight (('+'|'-') tight)*
    ;

tight
    : expon (('*'|'/') expon)*
    ;

expon
    : fn_call ('^' fn_call)*
    ;

fn_call
    : ID '(' (expr (',' expr)*)? ')'
    | term
    ;

term
    : FLOAT
    | ID
    | '(' expr ')'
    ;

ID 
    : ('a'..'z'|'A'..'Z'|'_') ('a'..'z'|'A'..'Z'|'0'..'9'|'_')*
    ;

FLOAT
    : ('0'..'9')+ '.' ('0'..'9')* EXPONENT?
    | '.' ('0'..'9')+ EXPONENT?
    | ('0'..'9')+ EXPONENT?
    ;

COMMENT
    : '#' ~('\n'|'\r')* '\r'? '\n' {$channel=HIDDEN;}
    ;

fragment
EXPONENT
    : ('e'|'E') ('+'|'-')? ('0'..'9')+
    ;

NEWLINE
    : '\r'? '\n'
    ;

// skip whitespace
WS
    : (' '|'\t'|'\n'|'\r')+ {SKIP();}
    ;
