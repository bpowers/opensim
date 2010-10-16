grammar boosd;

options {
    language = Java;
    //language = C;
    output = AST;
    // as suggested here:
    // http://www.antlr.org/wiki/display/ANTLR3/Using+the+ANTLR3+C+Target
    //ASTLabelType=pANTLR3_BASE_TREE;
}


compilation_unit
    : kind_decls?
    ;

kind_decls
    : kind_decl*
    ;

kind_decl
    : KIND^ ID ('(' AKA id_list ')')? (SPECIALIZES ID)? NEWLINE
    ;

id_list
    : ID ((',' ID)*)?
    ;

statements
    : statement*
    ;

statement
    : assignment
    ;

assignment
    : ID '='^ expr NEWLINE
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

AKA
    : 'aka'
    ;

SPECIALIZES
    : 'specializes'
    ;

KIND
    : 'kind'
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
    : (' '|'\t')+ {skip();}
//    : (' '|'\t'|'\n'|'\r')+ {SKIP();}
    ;
