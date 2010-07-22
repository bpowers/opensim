grammar donella1;

options {
    language = C;
    output = AST;
}


statements
    : statement+ EOF
    ;

statement
    : fn_call
    | assign
    ;

assign
    : ID '=' expr ';'
    ;

fn_call
    : ID  '('! expr (','! expr)* ')'!
	;

expr
    : loose
    ;

loose
    : tight (('+'|'-') tight)*
	;

tight
	: unary (('*'|'/') unary)*
	;

unary
    : ('+'|'-')^ expon
    ;

expon
    : term ('^' expon)?
	;

term
    : ID
    | FLOAT
    | ('('! expr ')'!)
    ;

ID 
    : ('a'..'z'|'A'..'Z'|'_') ('a'..'z'|'A'..'Z'|'0'..'9'|'_')*
    ;

FLOAT
    : ('0'..'9')+ '.' ('0'..'9')* EXPONENT?
    | '.' ('0'..'9')+ EXPONENT?
    | ('0'..'9')+ EXPONENT
    ;

COMMENT
    : '#' ~('\n'|'\r')* '\r'? '\n' {$channel=HIDDEN;}
    ;

STRING
    : '\'' ( ESC_SEQ | ~('\\'|'\'') )* '\''
    ;

fragment
EXPONENT
    : ('e'|'E') ('+'|'-')? ('0'..'9')+
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

/** Skip whitespace */
WS : (' ' | '\t' | '\r' | '\n') {SKIP();} ;
