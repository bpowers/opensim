grammar donella1;

options {
    language="Cpp";
}

tokens {
	ADD = '+';
	SUB = '-';
	MULT = '*';
	DIV = '/';
	EXP = '^';
    LPAREN = '(';
    RPAREN = ')';
    COMMA = ','
}


FN_CALL	
    : ID  LPAREN! expr (COMMA! expr)* RPAREN!
	| expr
	;

expr
    : loose
    ;

loose
    : tight ((ADD|SUB) loose)?
	;

tight
	: unary ((DIV|MULT) tight)?
	;

unary
    : (ADD|SUB)? expon
    ;

expon
    : term (EXP expon)?
	;

term
    : ID
    | FLOAT
    | (LPAREN! expr RPAREN!)
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

/** Skip whitespace */
WS : (' ' | '\t' | '\r' | '\n') {skip();} ;
