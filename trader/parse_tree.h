#ifndef __parse_tree_h__
#define __parse_tree_h__

#include <string>

#include "trader.h"

class ParseTreeBase;

/**
 * @return 0  => Does not match
 *         1  => Does match
 *         <0 => Error
 */
int matchConstraint( ParseTreeBase *_tree, CosTrading::PropertySeq *_props );

enum PreferencesSortType { PST_RANDOM, PST_FIRST, PST_MIN, PST_MAX, PST_WITH, PST_ERROR };

class PreferencesReturn
{
public:
  enum Type { PRT_NUM, PRT_FLOAT, PRT_ERROR };

  PreferencesReturn()
  {
    type = PRT_ERROR;
  }
  
  PreferencesReturn( const PreferencesReturn& _r )
  {
    type = _r.type;
    i = _r.i;
    f = _r.f;
  }
  
  Type type;
  int i;
  float f;
};

/**
 * @param _return is filled with the return value.
 *                If the type of the expression is PST_WITH, then only PreferencesReturn::i is set to 0 or 1.
 *                For types PST_FIRST and PST_RANDOM PreferencesReturn::i is always set to 0.

 * @return 1 on success or <0 on Error
 */
int matchPreferences( ParseTreeBase *_tree, CosTrading::PropertySeq *_props, PreferencesReturn &_ret );

ParseTreeBase* parseConstraints( const char *_constr );
ParseTreeBase* parsePreferences( const char *_prefs, PreferencesSortType &type );

class ParseContext
{
public:
  ParseContext( ParseContext* _ctx ) { seq = _ctx->seq; }
  ParseContext( CosTrading::PropertySeq *_seq ) { seq = _seq; }

  enum Type { T_STRING = 1, T_FLOAT = 2, T_NUM = 3, T_BOOL = 4, T_NUM_SEQ = 5, T_STR_SEQ = 6, T_FLOAT_SEQ = 7 };
  
  string str;
  int i;
  float f;
  bool b;
  CosTrading::LongList numSeq;
  CosTrading::FloatList floatSeq;
  CosTrading::StringList strSeq;
  Type type;
  CosTrading::PropertySeq* seq;
};

class ParseTreeBase
{
public:
  ParseTreeBase() { }
  virtual ~ParseTreeBase() { };

  virtual bool isA( const char *_t ) { return ( strcmp( _t, "ParseTreeBase" ) == 0 ); }
  
  virtual bool eval( ParseContext *_context ) = 0;
};

class ParseTreeOR : public ParseTreeBase
{
public:
  ParseTreeOR( ParseTreeBase *_ptr1, ParseTreeBase *_ptr2 ) { m_pLeft = _ptr1; m_pRight = _ptr2; }
  ~ParseTreeOR() { delete m_pLeft; delete m_pRight; }
  
  bool eval( ParseContext *_context );
  
protected:
  ParseTreeBase *m_pLeft;
  ParseTreeBase *m_pRight;
};

class ParseTreeAND : public ParseTreeBase
{
public:
  ParseTreeAND( ParseTreeBase *_ptr1, ParseTreeBase *_ptr2 ) { m_pLeft = _ptr1; m_pRight = _ptr2; }
  ~ParseTreeAND() { delete m_pLeft; delete m_pRight; }
  
  bool eval( ParseContext *_context );
  
protected:
  ParseTreeBase *m_pLeft;
  ParseTreeBase *m_pRight;
};

class ParseTreeCMP : public ParseTreeBase
{
public:
  ParseTreeCMP( ParseTreeBase *_ptr1, ParseTreeBase *_ptr2, int _i ) { m_pLeft = _ptr1; m_pRight = _ptr2; m_cmd = _i; }
  ~ParseTreeCMP() { delete m_pLeft; delete m_pRight; }
  
  bool eval( ParseContext *_context );
  
protected:
  ParseTreeBase *m_pLeft;
  ParseTreeBase *m_pRight;
  int m_cmd;
};

class ParseTreeIN : public ParseTreeBase
{
public:
  ParseTreeIN( ParseTreeBase *_ptr1, ParseTreeBase *_ptr2 ) { m_pLeft = _ptr1; m_pRight = _ptr2; }
  ~ParseTreeIN() { delete m_pLeft; delete m_pRight; }
  
  bool eval( ParseContext *_context );
  
protected:
  ParseTreeBase *m_pLeft;
  ParseTreeBase *m_pRight;
};

class ParseTreeMATCH : public ParseTreeBase
{
public:
  ParseTreeMATCH( ParseTreeBase *_ptr1, ParseTreeBase *_ptr2 ) { m_pLeft = _ptr1; m_pRight = _ptr2; }
  ~ParseTreeMATCH() { delete m_pLeft; delete m_pRight; }
  
  bool eval( ParseContext *_context );
  
protected:
  ParseTreeBase *m_pLeft;
  ParseTreeBase *m_pRight;
};

class ParseTreeCALC : public ParseTreeBase
{
public:
  ParseTreeCALC( ParseTreeBase *_ptr1, ParseTreeBase *_ptr2, int _i ) { m_pLeft = _ptr1; m_pRight = _ptr2; m_cmd = _i; }
  ~ParseTreeCALC() { delete m_pLeft; delete m_pRight; }
  
  bool eval( ParseContext *_context );
  
protected:
  ParseTreeBase *m_pLeft;
  ParseTreeBase *m_pRight;
  int m_cmd;
};

class ParseTreeBRACKETS : public ParseTreeBase
{
public:
  ParseTreeBRACKETS( ParseTreeBase *_ptr ) { m_pLeft = _ptr; }
  ~ParseTreeBRACKETS() { delete m_pLeft; }
  
  bool eval( ParseContext *_context ) { return m_pLeft->eval( _context ); }
  
protected:
  ParseTreeBase *m_pLeft;
};

class ParseTreeNOT : public ParseTreeBase
{
public:
  ParseTreeNOT( ParseTreeBase *_ptr ) { m_pLeft = _ptr; }
  ~ParseTreeNOT() { delete m_pLeft; }
  
  bool eval( ParseContext *_context );
  
protected:
  ParseTreeBase *m_pLeft;
};

class ParseTreeEXIST : public ParseTreeBase
{
public:
  ParseTreeEXIST( char *_id ) { m_pId = _id; }
  ~ParseTreeEXIST() { free( m_pId ); }
  
  bool eval( ParseContext *_context );
  
protected:
  char *m_pId;
};

class ParseTreeID : public ParseTreeBase
{
public:
  ParseTreeID( char *arg ) { m_str = arg; }
  ~ParseTreeID() { }
  
  bool eval( ParseContext *_context );
  
protected:
  string m_str;
};

class ParseTreeSTRING : public ParseTreeBase
{
public:
  ParseTreeSTRING( char *arg ) { m_str = arg; }
  ~ParseTreeSTRING() { }
  
  bool eval( ParseContext *_context ) { _context->type = ParseContext::T_STRING; _context->str = m_str; return true; }
  
protected:
  string m_str;
};

class ParseTreeNUM : public ParseTreeBase
{
public:
  ParseTreeNUM( int arg ) { m_int = arg; }
  ~ParseTreeNUM() { }
  
  bool eval( ParseContext *_context ) { _context->type = ParseContext::T_NUM; _context->i = m_int; return true; }
  
protected:
  int m_int;
};

class ParseTreeFLOAT : public ParseTreeBase
{
public:
  ParseTreeFLOAT( float arg ) { m_float = arg; }
  ~ParseTreeFLOAT() { }
  
  bool eval( ParseContext *_context ) { _context->type = ParseContext::T_FLOAT; _context->f = m_float; return true; }
  
protected:
  float m_float;
};

class ParseTreeBOOL : public ParseTreeBase
{
public:
  ParseTreeBOOL( bool arg ) { m_bool = arg; }
  ~ParseTreeBOOL() { }
  
  bool eval( ParseContext *_context ) { _context->type = ParseContext::T_BOOL; _context->b = m_bool; return true; }
  
protected:
  bool m_bool;
};

class ParseTreeRANDOM : public ParseTreeBase
{
public:
  ParseTreeRANDOM() {}
  ~ParseTreeRANDOM() {}
  
  bool eval( ParseContext *_context );
  
  virtual bool isA( const char *_t ) { return ( strcmp( _t, "ParseTreeRandom" ) == 0 ); }
};

class ParseTreeFIRST : public ParseTreeBase
{
public:
  ParseTreeFIRST() {}
  ~ParseTreeFIRST() {}
  
  bool eval( ParseContext *_context );
  
  virtual bool isA( const char *_t ) { return ( strcmp( _t, "ParseTreeFirst" ) == 0 ); }
};

class ParseTreeMAX : public ParseTreeBase
{
public:
  ParseTreeMAX( ParseTreeBase *_ptr ) { m_pLeft = _ptr; }
  ~ParseTreeMAX() { delete m_pLeft; }
  
  bool eval( ParseContext *_context );

  virtual bool isA( const char *_t ) { return ( strcmp( _t, "ParseTreeMax" ) == 0 ); }
  
protected:
  ParseTreeBase *m_pLeft;
};

class ParseTreeMIN : public ParseTreeBase
{
public:
  ParseTreeMIN( ParseTreeBase *_ptr ) { m_pLeft = _ptr; }
  ~ParseTreeMIN() { delete m_pLeft; }
  
  bool eval( ParseContext *_context );
  
  virtual bool isA( const char *_t ) { return ( strcmp( _t, "ParseTreeMin" ) == 0 ); }  

protected:
  ParseTreeBase *m_pLeft;
};

class ParseTreeWITH : public ParseTreeBase
{
public:
  ParseTreeWITH( ParseTreeBase *_ptr ) { m_pLeft = _ptr; }
  ~ParseTreeWITH() { delete m_pLeft; }
  
  bool eval( ParseContext *_context );

  virtual bool isA( const char *_t ) { return ( strcmp( _t, "ParseTreeWith" ) == 0 ); }  
  
protected:
  ParseTreeBase *m_pLeft;
};

#endif


