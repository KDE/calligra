%{

#include "kscript_parsenode.h"
#include "kscript_parser.h"
#include <stdlib.h>

#include <qstring.h>

extern int yylex();

extern const char* idl_lexFile;
extern int idl_line_no;

void kscriptInitFlex( const char *_code, int extension );

void yyerror( const char *s )
{
  theParser->parse_error( idl_lexFile, s, idl_line_no );
}

%}


%union
{
  QString        *ident;
  KSParseNode    *node;
  KScript::Long   _int;
  QString        *_str;
  ushort          _char;
  KScript::Double _float;
}

%token <_str> T_FILE_OP
%token <_str> T_MATCH_LINE
%token T_LINE
%token T_INPUT
%token T_AMPERSAND
%token T_ASTERISK
%token T_CASE
%token <_char> T_CHARACTER_LITERAL
%token T_CIRCUMFLEX
%token T_COLON
%token T_COMMA
%token T_CONST
%token T_DEFAULT
%token T_ENUM
%token T_EQUAL
%token T_FALSE
%token <_float> T_FLOATING_PT_LITERAL
%token T_GREATER_THAN_SIGN
%token <ident> T_IDENTIFIER
%token T_IN
%token T_INOUT
%token <_int> T_INTEGER_LITERAL
%token T_LEFT_CURLY_BRACKET
%token T_LEFT_PARANTHESIS
%token T_LEFT_SQUARE_BRACKET
%token T_LESS_THAN_SIGN
%token T_MINUS_SIGN
%token T_OUT
%token T_PERCENT_SIGN
%token T_PLUS_SIGN
%token T_RIGHT_CURLY_BRACKET
%token T_RIGHT_PARANTHESIS
%token T_RIGHT_SQUARE_BRACKET
%token T_SCOPE
%token T_SEMICOLON
%token T_SHIFTLEFT
%token T_SHIFTRIGHT
%token T_SOLIDUS
%token <_str> T_STRING_LITERAL
%token <_str> T_PRAGMA
%token T_STRUCT
%token T_SWITCH
%token T_TILDE
%token T_TRUE
%token T_VERTICAL_LINE
%token T_CLASS
%token T_LESS_OR_EQUAL
%token T_GREATER_OR_EQUAL
%token T_ASSIGN
%token T_NOTEQUAL
%token T_MEMBER
%token T_DELETE
%token T_WHILE
%token T_IF
%token T_ELSE
%token T_FOR
%token T_DO
%token T_INCR
%token T_DECR
%token T_MAIN
%token T_FOREACH
%token <ident> T_SUBST
%token <_str> T_MATCH
%token T_NOT
%token T_RETURN
%token T_SIGNAL
%token T_EMIT
%token T_IMPORT
%token T_VAR
%token T_UNKNOWN
%token T_CATCH
%token T_TRY
%token T_RAISE
%token <_str> T_RANGE
%token <_str> T_CELL
%token T_FROM
%token T_PLUS_ASSIGN
%token T_MINUS_ASSIGN
%token T_AND
%token T_OR
%token T_DOLLAR

%type <node>   definitions
%type <node>   definition
%type <node>   const_dcl
%type <node>   or_expr
%type <node>   xor_expr
%type <node>   and_expr
%type <node>   shift_expr
%type <node>   add_expr
%type <node>   mult_expr
%type <node>   unary_expr
%type <node>   primary_expr
%type <node>   literal
%type <node>   scoped_name
%type <node>   inheritance_spec
%type <node>   class_dcl
%type <node>   class_header
%type <node>   class_body
%type <node>   class_exports
%type <node>   class_export
%type <node>   func_dcl
%type <node>   func_body
%type <node>   func_lines
%type <node>   func_line
%type <node>   func_param
%type <node>   func_params
%type <node>   assign_expr
%type <node>   equal_expr
%type <node>   index_expr
%type <node>   member_expr
%type <node>   array_elements
%type <node>   dict_elements
%type <node>   func_call_params
%type <node>   loops
%type <node>   incr_expr
%type <node>   while
%type <node>   else
%type <node>   main
%type <node>   match_expr
%type <node>   signal_param
%type <node>   signal_params
%type <node>   struct_dcl
%type <node>   struct_export
%type <node>   struct_exports
%type <node>   struct_members
%type <node>   qualified_names
%type <node>   qualified_name
%type <node>   catches
%type <node>   single_catch
%type <node>   loop_body
%type <_str>   import_list
%type <node>   bool_or
%type <node>   bool_and

%%

/*1*/
specification
	: /*empty*/
	  {
	    theParser->setRootNode( NULL );
	  }
	| T_ASSIGN equal_expr
	  {
	    theParser->setRootNode( $2 );
	  }
	| definitions
          {
	    theParser->setRootNode( new KSParseNode( definitions, $1 ) );
	  }
	;
	
definitions
	: definition
          {
	    $$ = new KSParseNode( definitions, $1 );
	  }
	| definition definitions
          {
	    $$ = new KSParseNode( definitions, $1 );
	    $$->setBranch( 2, $2 );
	  }
	;

/*2*/
definition
	: const_dcl T_SEMICOLON
          {
	    $$ = $1;
	  }
	| struct_dcl T_SEMICOLON
	  {
	    $$ = $1;
	  }
	| class_dcl T_SEMICOLON
	  {
	    $$ = $1;
	  }
	| func_dcl T_RIGHT_CURLY_BRACKET
	  {
	    $$ = $1;
	  }
	| T_IMPORT T_IDENTIFIER T_SEMICOLON
	  {
	    $$ = new KSParseNode( import );
	    $$->setIdent( $2 );
	  }
	| T_FROM T_IDENTIFIER T_IMPORT T_ASTERISK T_SEMICOLON
	  {
	    $$ = new KSParseNode( from );
	    $$->setIdent( $2 );
	    $$->setStringLiteral( QString( "" ) );
	  }
	| T_FROM T_IDENTIFIER T_IMPORT import_list T_SEMICOLON
	  {
	    $$ = new KSParseNode( from );
	    $$->setIdent( $2 );
	    $$->setStringLiteral( $4 );
	  }
        | T_PRAGMA      /*New*/
          {
	    $$ = new KSParseNode( t_pragma );
	    $$->setIdent( $1 );
	  }
	| main
	  {
	    $$ = $1;
	  }
	;

import_list
	: T_IDENTIFIER
	  {
		$$ = $1;
	  }
	| T_IDENTIFIER T_COMMA import_list
	  {
		(*$1) += "/";
		(*$1) += (*$3);
		$$ = $1;
	  }
	;

main
	: T_MAIN T_LEFT_CURLY_BRACKET func_body T_RIGHT_CURLY_BRACKET
	  {
	    $$ = new KSParseNode( func_dcl );
	    $$->setBranch( 2, $3 );
	    $$->setIdent( "main" );
	  }
	| T_MAIN T_LEFT_PARANTHESIS func_params T_RIGHT_PARANTHESIS T_LEFT_CURLY_BRACKET func_body T_RIGHT_CURLY_BRACKET
	  {
	    $$ = new KSParseNode( func_dcl, $3, $6 );
	    $$->setIdent( "main" );
	  }

/*10*/
inheritance_spec
	: T_COLON qualified_names
          {
	    $$ = $2;
	  }
	;

/* A list of qualified names like "KOM.Base, OpenParts.MainWindow, Qt.QWidget" */
qualified_names
	: qualified_name
          {
	    $$ = new KSParseNode( t_qualified_names, $1 );
	  }
	| qualified_name T_COMMA qualified_names
          {
	    $$ = new KSParseNode( t_qualified_names, $1 );
	    $$->setBranch( 2, $3 );
	  }
	;

/* A name like "QWidget" or "Qt.QWidget" or "Qt.Extension.PNG" */
qualified_name
	: scoped_name
	  {
	    $$ = $1;
	  }
	| qualified_name T_MEMBER T_IDENTIFIER
	  {
	    $$ = new KSParseNode( member_expr, $1 );
	    $$->setIdent( $3 );
	  }

/* A usual identifier that may start with "::". For example "MyClass" or "::MyModule" */
scoped_name
	: T_IDENTIFIER
          {
	    $$ = new KSParseNode( scoped_name );
	    $$->setIdent( $1 );
	  }
        | T_SCOPE T_IDENTIFIER
          {
	    $$ = new KSParseNode( scoped_name );
	    QString name = "::";
	    name += *($2);
	    delete $2;
	    $$->setIdent( name );
	  }
	;

/* This rule fits for assignments like "a = 100" */
assign_expr
	: bool_or T_ASSIGN assign_expr
	  {
	    $$ = new KSParseNode( assign_expr, $1, $3 );
	  }
	| bool_or T_PLUS_ASSIGN assign_expr
	  {
	    $$ = new KSParseNode( plus_assign, $1, $3 );
	  }
	| bool_or T_MINUS_ASSIGN assign_expr
	  {
	    $$ = new KSParseNode( minus_assign, $1, $3 );
	  }
	| bool_or
	  {
	    $$ = $1;
	  }
	;

bool_or
	: bool_and T_OR bool_or
	  {
	    $$ = new KSParseNode( bool_or, $1, $3 );
	  }
	| bool_and
	  {
	    $$ = $1;
	  }
	;

bool_and
	: equal_expr T_AND bool_and
	  {
	    $$ = new KSParseNode( bool_and, $1, $3 );
	  }
	| equal_expr
	  {
	    $$ = $1;
	  }
	;

equal_expr
	: or_expr T_EQUAL equal_expr
	  {
	    $$ = new KSParseNode( t_equal, $1, $3 );
	  }
	| or_expr T_NOTEQUAL equal_expr
	  {
	    $$ = new KSParseNode( t_notequal, $1, $3 );
	  }
	| or_expr T_LESS_OR_EQUAL equal_expr
	  {
	    $$ = new KSParseNode( t_less_or_equal, $1, $3 );
	  }
	| or_expr T_GREATER_OR_EQUAL equal_expr
	  {
	    $$ = new KSParseNode( t_greater_or_equal, $1, $3 );
	  }
	| or_expr T_LESS_THAN_SIGN equal_expr
	  {
	    $$ = new KSParseNode( t_less, $1, $3 );
	  }
	| or_expr T_GREATER_THAN_SIGN equal_expr
	  {
	    $$ = new KSParseNode( t_greater, $1, $3 );
	  }
	| or_expr
	  {
	    $$ = $1;
	  }
	;

/*15*/
or_expr
	: xor_expr
          {
	    $$ = $1;
	  }
	| or_expr T_VERTICAL_LINE xor_expr
          {
	    $$ = new KSParseNode( t_vertical_line, $1, $3 );
	  }
	;

/*16*/
xor_expr
	: and_expr
          {
	    $$ = $1;
	  }
	| xor_expr T_CIRCUMFLEX and_expr
          {
	    $$ = new KSParseNode( t_circumflex, $1, $3 );
	  }
	;

/*17*/
and_expr
	: shift_expr
          {
	    $$ = $1;
	  }
	| and_expr T_AMPERSAND shift_expr
          {
	    $$ = new KSParseNode( t_ampersand, $1, $3 );
	  }
	;

/*18*/
shift_expr
	: add_expr
          {
	    $$ = $1;
	  }
	| shift_expr T_SHIFTRIGHT add_expr
          {
	    $$ = new KSParseNode( t_shiftright, $1, $3 );
	  }
	| shift_expr T_SHIFTLEFT add_expr
          {
	    $$ = new KSParseNode( t_shiftleft, $1, $3 );
	  }
	;

/*19*/
add_expr
	: mult_expr
          {
	    $$ = $1;
	  }
	| add_expr T_PLUS_SIGN mult_expr
          {
	    $$ = new KSParseNode( t_plus_sign, $1, $3 );
	  }
	| add_expr T_MINUS_SIGN mult_expr
          {
	    $$ = new KSParseNode( t_minus_sign, $1, $3 );
	  }
	;

/*20*/
mult_expr
	: unary_expr
          {
	    $$ = $1;
	  }
	| mult_expr T_ASTERISK unary_expr
          {
	    $$ = new KSParseNode( t_asterik, $1, $3 );
	  }
	| mult_expr T_SOLIDUS unary_expr
          {
	    $$ = new KSParseNode( t_solidus, $1, $3 );
	  }
	| mult_expr T_PERCENT_SIGN unary_expr
          {
	    $$ = new KSParseNode( t_percent_sign, $1, $3 );
	  }
	;

/*22*/
unary_expr
	: T_MINUS_SIGN index_expr
          {
	    $$ = new KSParseNode( t_minus_sign, $2 );
	  }
	| T_PLUS_SIGN index_expr
          {
	    $$ = new KSParseNode( t_plus_sign, $2 );
	  }
	| T_TILDE index_expr
          {
	    $$ = new KSParseNode( t_tilde, $2 );
	  }
	| T_NOT index_expr
          {
	    $$ = new KSParseNode( t_not, $2 );
	  }
	| index_expr
          {
	    $$ = $1;
	  }
	;

index_expr
	: index_expr T_LEFT_SQUARE_BRACKET assign_expr T_RIGHT_SQUARE_BRACKET
	  {
	    $$ = new KSParseNode( t_array, $1, $3 );
	  }
	| index_expr T_LEFT_CURLY_BRACKET assign_expr T_RIGHT_CURLY_BRACKET
	  {
	    $$ = new KSParseNode( t_dict, $1, $3 );
	  }
	| index_expr T_LEFT_PARANTHESIS T_RIGHT_PARANTHESIS
	  {
	    $$ = new KSParseNode( t_func_call, $1 );
	  }
	| index_expr T_LEFT_PARANTHESIS func_call_params T_RIGHT_PARANTHESIS
	  {
	    $$ = new KSParseNode( t_func_call, $1, $3 );
	  }
	| member_expr
	  {
	    $$ = $1;
	  }
	;

func_call_params
	: assign_expr T_COMMA func_call_params
	  {
	    $$ = new KSParseNode( func_call_params, $1, $3 );
	  }
	;
	| assign_expr
	  {
	    $$ = new KSParseNode( func_call_params, $1 );
	  }
	;
	
member_expr
	: index_expr T_MEMBER T_IDENTIFIER
	  {
	    $$ = new KSParseNode( member_expr, $1 );
	    $$->setIdent( $3 );
	  }
	| incr_expr
	  {
	    $$ = $1;
	  }
	;

incr_expr
	: T_INCR match_expr
	  {
	    $$ = new KSParseNode( t_incr, $2 );
	  }
	| T_DECR match_expr
	  {
	    $$ = new KSParseNode( t_decr, $2 );
	  }
	| match_expr T_INCR
	  {
	    /* Setting $1 twice indicates that this is a postfix operator */
	    $$ = new KSParseNode( t_incr, $1, $1 );
	  }
	| match_expr T_DECR
	  {
	    /* Setting $1 twice indicates that this is a postfix operator */
	    $$ = new KSParseNode( t_decr, $1, $1 );
	  }
	| match_expr
	  {
	    $$ = $1;
	  }
	;

match_expr
	: primary_expr T_MATCH
	  {
	    $$ = new KSParseNode( t_match, $1 );
	    $$->setIdent( $2 );
	  }
	| primary_expr T_SUBST
	  {
	    $$ = new KSParseNode( t_subst, $1 );
	    $$->setIdent( $2 );
	  }
	| primary_expr
	  {
	    $$ = $1;
	  }
	;

/*23*/
primary_expr
	: scoped_name
          {
	    $$ = $1;
	  }
	| literal
          {
	    $$ = $1;
	  }
	| T_INPUT
	  {
	    $$ = new KSParseNode( t_input );
	  }
	| T_MATCH_LINE
	  {
	    $$ = new KSParseNode( t_match_line );
	    $$->setIdent( $1 );
	  }
	| T_FILE_OP assign_expr T_RIGHT_PARANTHESIS
	  {
	    $$ = new KSParseNode( t_file_op, $2 );
	    $$->setIdent( $1 );
	  }
	| T_LESS_THAN_SIGN or_expr T_GREATER_THAN_SIGN { }
	| T_LEFT_PARANTHESIS assign_expr T_RIGHT_PARANTHESIS
          {
	    $$ = $2;
	  }
	;

/*24*/
/*25*/
literal
	: T_INTEGER_LITERAL
          {
	    $$ = new KSParseNode( t_integer_literal );
	    $$->setIntegerLiteral( $1 );
	  }
	| T_CELL
	  {
	    $$ = new KSParseNode( t_cell );
	    $$->setStringLiteral( $1 );
	  }
	| T_RANGE
	  {
	    $$ = new KSParseNode( t_range );
	    $$->setStringLiteral( $1 );
	  }
	| T_STRING_LITERAL
          {
	    $$ = new KSParseNode( t_string_literal );
	    $$->setStringLiteral( $1 );
	  }
	| T_CHARACTER_LITERAL
          {
	    $$ = new KSParseNode( t_character_literal );
	    $$->setCharacterLiteral( $1 );
	  }
	| T_FLOATING_PT_LITERAL
          {
	    $$ = new KSParseNode( t_floating_pt_literal );
	    $$->setFloatingPtLiteral( $1 );
	  }
	| T_TRUE  /*boolean_literal*/
          {
	    $$ = new KSParseNode( t_boolean_literal );
	    $$->setBooleanLiteral( true );
	  }
	| T_FALSE /*boolean_literal*/
          {
	    $$ = new KSParseNode( t_boolean_literal );
	    $$->setBooleanLiteral( false );
	  }
	| T_LEFT_SQUARE_BRACKET T_RIGHT_SQUARE_BRACKET
	  {
	    $$ = new KSParseNode( t_array_const );
	  }
	| T_LEFT_SQUARE_BRACKET array_elements T_RIGHT_SQUARE_BRACKET
	  {
	    $$ = new KSParseNode( t_array_const, $2 );
	  }
	| T_LEFT_CURLY_BRACKET T_RIGHT_CURLY_BRACKET
	  {
	    $$ = new KSParseNode( t_dict_const );
	  }
	| T_LEFT_CURLY_BRACKET dict_elements T_RIGHT_CURLY_BRACKET
	  {
	    $$ = new KSParseNode( t_dict_const, $2 );
	  }
	| T_LINE
	  {
	    $$ = new KSParseNode( t_line );
	  }
	| T_DOLLAR T_INTEGER_LITERAL
	  {
	    $$ = new KSParseNode( t_regexp_group );
	    $$->setIntegerLiteral( $2 );
	  }
	;

array_elements
	: assign_expr T_COMMA array_elements
	  {
	    $$ = new KSParseNode( t_array_element, $1, $3 );
	  }
	| assign_expr
	  {
	    $$ = new KSParseNode( t_array_element, $1  );
	  }
	;

dict_elements
	: T_LEFT_PARANTHESIS assign_expr T_COMMA equal_expr T_RIGHT_PARANTHESIS T_COMMA dict_elements
	  {
	    $$ = new KSParseNode( t_dict_element, $2, $4, $7 );
	  }
	| T_LEFT_PARANTHESIS assign_expr T_COMMA equal_expr T_RIGHT_PARANTHESIS
	  {
	    $$ = new KSParseNode( t_dict_element, $2, $4 );
	  }
	;


/*79*/

struct_dcl
	: T_STRUCT T_IDENTIFIER T_LEFT_CURLY_BRACKET struct_exports T_RIGHT_CURLY_BRACKET
	  {
	    $$ = new KSParseNode( t_struct, $4 );
	    $$->setIdent( $2 );
	  }
	;

struct_exports
	: struct_export
          {
	    $$ = new KSParseNode( exports, $1 );
	  }
	| struct_export struct_exports
          {
	    $$ = new KSParseNode( exports, $1 );
	    $$->setBranch( 2, $2 );
	  }
	;

struct_export
	: func_dcl T_RIGHT_CURLY_BRACKET
	  {
	    $$ = $1;
	  }	
	| T_VAR struct_members T_SEMICOLON
	  {
	    $$ = $2;
	  }
	| const_dcl T_SEMICOLON
	  {
	    $$ = $1;
	  }
	;

struct_members
	: T_IDENTIFIER
	  {
	    $$ = new KSParseNode( t_struct_members );
	    $$->setIdent( $1 );
	  }
	| T_IDENTIFIER T_COMMA struct_members
	  {
	    $$ = new KSParseNode( t_struct_members, $3 );
	    $$->setIdent( $1 );
	  }
	;

/*83*/
class_dcl
	: class_header T_LEFT_CURLY_BRACKET class_body T_RIGHT_CURLY_BRACKET
          {
	    $$ = new KSParseNode( class_dcl, $1, $3 );
	  }
	;

/*84*/
class_header
	: T_CLASS T_IDENTIFIER
          {
	    $$ = new KSParseNode( class_header );
	    $$->setIdent( $2 );
	  }
	| T_CLASS T_IDENTIFIER inheritance_spec
          {
	    $$ = new KSParseNode( class_header );
	    $$->setIdent( $2 );
	    $$->setBranch( 1, $3 );
	  }
	;

/*85*/
class_body
	: /*empty*/
          {
	    $$ = NULL;
	  }
	| class_exports
          {
	    $$ = $1;
	  }
	;

class_exports
	: class_export
          {
	    $$ = new KSParseNode( exports, $1 );
	  }
	| class_export class_exports
          {
	    $$ = new KSParseNode( exports, $1 );
	    $$->setBranch( 2, $2 );
	  }
	;

class_export
	: func_dcl T_RIGHT_CURLY_BRACKET
	  {
	    $$ = $1;
	  }
	| struct_dcl T_SEMICOLON
	  {
	    $$ = $1;
	  }
	| T_SIGNAL T_IDENTIFIER T_LEFT_PARANTHESIS signal_params T_RIGHT_PARANTHESIS T_SEMICOLON
	  {
	    $$ = new KSParseNode( signal_dcl, $4 );
	    $$->setIdent( $2 );
	  }
	| T_SIGNAL T_IDENTIFIER T_LEFT_PARANTHESIS T_RIGHT_PARANTHESIS T_SEMICOLON
	  {
	    $$ = new KSParseNode( signal_dcl, 0 );
	    $$->setIdent( $2 );
	  }
	| T_DELETE T_LEFT_PARANTHESIS T_IN T_IDENTIFIER T_RIGHT_PARANTHESIS T_LEFT_CURLY_BRACKET func_body T_RIGHT_CURLY_BRACKET
	  {
	    $$ = new KSParseNode( destructor_dcl );
	    KSParseNode* n = new KSParseNode( func_param_in );
	    n->setIdent( $4 );
	    $$->setBranch( 1, n );
	    $$->setBranch( 2, $7 );
	    $$->setIdent( "delete" );
	  }
	| const_dcl T_SEMICOLON
	  {
	    $$ = $1;
	  }
	;

func_dcl
	: T_IDENTIFIER T_LEFT_PARANTHESIS T_RIGHT_PARANTHESIS T_LEFT_CURLY_BRACKET func_body
	  {
	    $$ = new KSParseNode( func_dcl );
	    $$->setBranch( 2, $5 );
	    $$->setIdent( $1 );
	  }
	| T_IDENTIFIER T_LEFT_PARANTHESIS func_params T_RIGHT_PARANTHESIS T_LEFT_CURLY_BRACKET func_body
	  {
	    $$ = new KSParseNode( func_dcl, $3, $6 );
	    $$->setIdent( $1 );
	  }
	;

func_params
	: func_param
	  {
	    $$ = new KSParseNode( func_params, $1 );
	  }
	| func_param T_COMMA func_params
	  {
	    $$ = new KSParseNode( func_params, $1, $3 );
	  }
	;

func_param
	: T_IN T_IDENTIFIER
	  {
	    $$ = new KSParseNode( func_param_in );
	    $$->setIdent( $2 );
	  }
	| T_IN T_IDENTIFIER T_ASSIGN assign_expr
	  {
	    $$ = new KSParseNode( func_param_in, $4 );
	    $$->setIdent( $2 );
	  }
	| T_OUT T_IDENTIFIER
	  {
	    $$ = new KSParseNode( func_param_out );
	    $$->setIdent( $2 );
	  }
	| T_INOUT T_IDENTIFIER
	  {
	    $$ = new KSParseNode( func_param_inout );
	    $$->setIdent( $2 );
	  }
	;

signal_params
	: signal_param
	  {
	    $$ = new KSParseNode( signal_params, $1 );
	  }
	| signal_param T_COMMA func_params
	  {
	    $$ = new KSParseNode( signal_params, $1, $3 );
	  }
	;

signal_param
	: T_IN T_IDENTIFIER
	  {
	    $$ = new KSParseNode( signal_param );
	    $$->setIdent( $2 );
	  }
	| T_IN T_IDENTIFIER T_ASSIGN assign_expr
	  {
	    $$ = new KSParseNode( signal_param, $4 );
	    $$->setIdent( $2 );
	  }
	;

func_body
	: /*empty*/
          {
	    $$ = NULL;
	  }
	| func_lines
          {
	    $$ = $1;
	  }
	;

func_lines
	: func_line
          {
	    $$ = new KSParseNode( func_lines, $1 );
	  }
	| func_line func_lines
          {
	    $$ = new KSParseNode( func_lines, $1 );
	    $$->setBranch( 2, $2 );
	  }
	;

func_line
	: assign_expr T_SEMICOLON
	  {
	    $$ = $1;
	  }
	| T_RAISE assign_expr T_COMMA assign_expr T_SEMICOLON
	  {
	    $$ = new KSParseNode( t_raise, $2, $4 );
	  }
	| T_EMIT assign_expr T_SEMICOLON
	  {
	    $$ = new KSParseNode( t_emit, $2 );
	  }
	| T_RETURN T_SEMICOLON
	  {
	    $$ = new KSParseNode( t_return );
	  }
	| T_RETURN assign_expr T_SEMICOLON
	  {
	    $$ = new KSParseNode( t_return, $2 );
	  }
	| loops
	  {
	    $$ = $1;
	  }
	| T_TRY T_LEFT_CURLY_BRACKET func_lines T_RIGHT_CURLY_BRACKET catches
	  {
	    $$ = new KSParseNode( t_try, $3, $5 );
	  }
	| T_LEFT_CURLY_BRACKET func_lines T_RIGHT_CURLY_BRACKET
	  {
	    $$ = new KSParseNode( t_scope, $2 );
	  }
	;

catches
	: single_catch
	  {
	    $$ = $1;
	  }
	| single_catch catches
	  {
	    $$ = $1;
	    $$->setBranch( 4, $2 );
	  }
	;

single_catch
	: T_CATCH T_LEFT_PARANTHESIS assign_expr T_COMMA T_IDENTIFIER T_RIGHT_PARANTHESIS
          T_LEFT_CURLY_BRACKET func_lines T_RIGHT_CURLY_BRACKET
	  {
	    $$ = new KSParseNode( t_catch, $3, $8 );
	    $$->setIdent( $5 );
	  }
	| T_DEFAULT T_LEFT_PARANTHESIS T_IDENTIFIER T_COMMA T_IDENTIFIER T_RIGHT_PARANTHESIS
          T_LEFT_CURLY_BRACKET func_lines T_RIGHT_CURLY_BRACKET
	  {
	    KSParseNode* x = new KSParseNode( scoped_name );
	    x->setIdent( $3 );
	    $$ = new KSParseNode( t_catch_default, x, $8 );
	    $$->setIdent( $5 );
	  }
	;

const_dcl
	: T_CONST T_IDENTIFIER T_ASSIGN equal_expr
          {
	    $$ = new KSParseNode( const_dcl, $4 );
	    $$->setIdent( $2 );
	  }
	;

loops
	: while loop_body
	  {
	    $$ = new KSParseNode( t_while, $1, $2 );
	  }
	| T_FOR T_LEFT_PARANTHESIS assign_expr T_SEMICOLON assign_expr T_SEMICOLON assign_expr T_RIGHT_PARANTHESIS loop_body
	  {
	    $$ = new KSParseNode( t_for, $3, $5, $7, $9 );
	  }
	| T_DO loop_body while T_SEMICOLON
	  {
	    $$ = new KSParseNode( t_do, $2, $3 );
	  }
	| T_IF T_LEFT_PARANTHESIS assign_expr T_RIGHT_PARANTHESIS loop_body else
	  {
	    $$ = new KSParseNode( t_if, $3, $5, $6 );
	  }
	| T_FOREACH T_LEFT_PARANTHESIS T_IDENTIFIER T_COMMA assign_expr T_RIGHT_PARANTHESIS loop_body
	  {
	    $$ = new KSParseNode( t_foreach, $5, $7 );
	    $$->setIdent( $3 );
	  }
	| T_FOREACH T_LEFT_PARANTHESIS T_IDENTIFIER T_COMMA T_IDENTIFIER T_COMMA assign_expr T_RIGHT_PARANTHESIS loop_body
	  {
	    /* We set $9 twice to indicate thet this is the foreach for maps */
	    $$ = new KSParseNode( t_foreach, $7, $9, $9 );
	    $$->setIdent( $3 );
	    $$->setStringLiteral( $5 );
	  }
	;

else
	: /*empty*/
          {
	    $$ = NULL;
	  }
	| T_ELSE T_IF T_LEFT_PARANTHESIS assign_expr T_RIGHT_PARANTHESIS loop_body else
	  {
	    $$ = new KSParseNode( t_if, $4, $6, $7 );
	  }
	| T_ELSE loop_body
	  {
	    $$ = $2;
	  }
	;

while
	: T_WHILE T_LEFT_PARANTHESIS assign_expr T_RIGHT_PARANTHESIS
	  {
	    $$ = $3;
	  }
	;

loop_body
	: T_LEFT_CURLY_BRACKET func_body T_RIGHT_CURLY_BRACKET
	  {
	    $$ = new KSParseNode( t_scope, $2 );
	  }
	;
%%

void kscriptParse( const char *_code, int extension )
{
    kscriptInitFlex( _code, extension );
    yyparse();
}
