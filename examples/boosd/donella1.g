grammar donella1;

options {
    language = Python;
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

STRING
    : '\'' ( ESC_SEQ | ~('\\'|'\'') )* '\''
    ;

fragment
HEX_DIGIT
    : ('0'..'9'|'a'..'f'|'A'..'F')
    ;

fragment
ESC_SEQ
    : '\\' ('b'|'t'|'n'|'f'|'r'|'\"'|'\''|'\\')
    | UNICODE_ESC
    | OCTAL_ESC
    ;

fragment
OCTAL_ESC
    : '\\' ('0'..'3') ('0'..'7') ('0'..'7')
    | '\\' ('0'..'7') ('0'..'7')
    | '\\' ('0'..'7')
    ;

fragment
UNICODE_ESC
    : '\\' 'u' HEX_DIGIT HEX_DIGIT HEX_DIGIT HEX_DIGIT
    ;

NEWLINE
    : '\r'? '\n'
    ;

// skip whitespace
WS
    : (' '|'\t'|'\n'|'\r')+ {skip();}
    ;
