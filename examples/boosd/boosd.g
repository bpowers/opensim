grammar boosd;

options {
    language = Python;
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
    : model_decl*
    ;

model_decl
    : 'model' ID of_type? specializes? callable? defs whitespace
    ;

callable
    : 'callable' '(' id_list ')'
    ;

of_type
    : '(' ID ')'
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
    : ID? ID type_decl?
    ;

assignment
    : qualified_expr
    | initializer
    | STRING
    ;

initializer
    : '{' whitespace initializer_expr* '}'
    ;

initializer_expr
    : ID (ASSIGN^ qualified_expr) NEWLINE
    ;

pair
    : '(' FLOAT ',' FLOAT ')'
    ;

qualified_expr
    : call (expr_u)?
    ;

call
    : expr
    ;

type_decl
    : '(' expr_u ')'
    ;

expr
    : loose
    ;

loose
    : tight (('+'|'-') NEWLINE? tight)*
    ;

tight
    : expon (('*'|'/') NEWLINE? expon)*
    ;

expon
    : term ('^' term)*
    ;

expr_list
    : (call (',' NEWLINE? call)*)?
    ;

term
    : FLOAT
    | '(' expr ')'
    | ident (lookup|'(' expr_list ')')?
    ;



expr_u
    : loose_u
    ;

loose_u
    : tight_u (('+'|'-') tight_u)*
    ;

tight_u
    : expon_u (('*'|'/') expon_u)*
    ;

expon_u
    : term_u ('^' term_u)*
    ;

term_u
    : FLOAT
    | ID
    ;


ident
    :  ID
    | '[' pair (',' pair)* ']'
    ;

lookup
    : '[' expr ']'
    ;

ASSIGN
    : '='
    ;

ID 
    : ('a'..'z'|'A'..'Z'|'_') ('a'..'z'|'A'..'Z'|'0'..'9'|'_'|'.')*
    ;

FLOAT
    : DIGITS+ ('.' DIGITS*)? EXPONENT?
    | '.' DIGITS+ EXPONENT?
    ;

STRING
    : '"' ~'"'+ '"'
    ;

COMMENT
    : '#' ~('\n'|'\r')* '\r'? '\n' {$channel=HIDDEN;}
    ;

fragment
EXPONENT
    : ('e'|'E') ('+'|'-')? DIGITS+
    ;

fragment
DIGITS
    : ('0'..'9')
    ;

NEWLINE
    : ('\r'? '\n')*
    ;

// skip whitespace
WS
    : (' '|'\t')+ {skip();}
//    : (' '|'\t'|'\n'|'\r')+ {SKIP();}
    ;
