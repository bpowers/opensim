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
    : whitespace
      kind_decls
      model_decls
    ;

whitespace
    : NEWLINE?
    ;

kind_decls
    : kind_decl*
    ;

kind_decl
    : 'kind' ID aka_list? (specializes conversion?)? NEWLINE
    ;

specializes
    : 'specializes' ID
    ;

conversion
    : '(' expr ID ')'
    ;

aka_list
    : '(' 'aka' id_list ')'
    ;

id_list
    : ID (','^ ID)*
    ;

model_decls
    : model_decl
    ;

model_decl
    : 'model' ID of_type? specializes? defs
    ;

of_type
    : '(' 'of' ID ')'
    ;

defs
    : '{' whitespace statements '}'
    ;

statements
    : statement*
    ;

statement
    : declaration (ASSIGN^ assignment)? NEWLINE
    ;

declaration
    : TYPE? ID type_decl?
    ;

assignment
    : typed_expr (',' typed_expr)?
    | '[' pair (',' pair)* ']'
    ;

pair
    : '(' FLOAT ',' FLOAT ')'
    ;

typed_expr
    : expr expr?
    ;

type_decl
    : '(' expr ')'
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
    : (ID '(') => call
    | term
    ;

call
    : ID '(' (expr (',' expr)*)? ')'
    ;

term
    : FLOAT
    | ID ('[' expr ']')?
    | '(' expr ')'
    ;

TYPE
    : 'stock'
    | 'table'
    | 'flow'
    | 'aux'
    ;

ASSIGN
    : ':='
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
    : ('\r'? '\n')*
    ;

// skip whitespace
WS
    : (' '|'\t')+ {skip();}
//    : (' '|'\t'|'\n'|'\r')+ {SKIP();}
    ;
