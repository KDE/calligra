extern "C" 
{
#include "parse.h"

void mainParse( const char *_code );  
}

#include "parse_tree.h"

#include <assert.h>

ParseTreeBase *pConstraintsTree = 0L;
ParseTreeBase *pPreferencesTree = 0L;

ParseTreeBase* parseConstraints( const char *_constr )
{
  mainParse( _constr );
  return pConstraintsTree;
}

ParseTreeBase* parsePreferences( const char *_prefs, PreferencesSortType &_type )
{
  mainParse( _prefs );  

  if ( pPreferencesTree == 0L )
  {
    cerr << "No Preferences Parse Tree generated" << endl;
    _type = PST_ERROR;
  }
  else if ( pPreferencesTree->isA( "ParseTreeRandom" ) )
    _type = PST_RANDOM;
  else if ( pPreferencesTree->isA( "ParseTreeFirst" ) )
    _type = PST_FIRST;
  else if ( pPreferencesTree->isA( "ParseTreeMin" ) )
    _type = PST_MIN;
  else if ( pPreferencesTree->isA( "ParseTreeMax" ) )
    _type = PST_MAX;
  else if ( pPreferencesTree->isA( "ParseTreeWith" ) )
    _type = PST_WITH;
  else
    assert( 0 );

  return pPreferencesTree;
}

void setConstraintsParseTree( void *_ptr1 )
{
  pConstraintsTree = (ParseTreeBase*)_ptr1;
}

void setPreferencesParseTree( void *_ptr1 )
{
  pPreferencesTree = (ParseTreeBase*)_ptr1;
}

void* newOR( void *_ptr1, void *_ptr2 )
{
  return new ParseTreeOR( (ParseTreeBase*)_ptr1, (ParseTreeBase*)_ptr2 );
}

void* newAND( void *_ptr1, void *_ptr2 )
{
  return new ParseTreeAND( (ParseTreeBase*)_ptr1, (ParseTreeBase*)_ptr2 );
}

void* newCMP( void *_ptr1, void *_ptr2, int _i )
{
  return new ParseTreeCMP( (ParseTreeBase*)_ptr1, (ParseTreeBase*)_ptr2, _i );
}

void* newIN( void *_ptr1, void *_ptr2 )
{
  return new ParseTreeIN( (ParseTreeBase*)_ptr1, (ParseTreeBase*)_ptr2 );
}

void* newMATCH( void *_ptr1, void *_ptr2 )
{
  return new ParseTreeMATCH( (ParseTreeBase*)_ptr1, (ParseTreeBase*)_ptr2 );
}

void* newCALC( void *_ptr1, void *_ptr2, int _i )
{
  return new ParseTreeCALC( (ParseTreeBase*)_ptr1, (ParseTreeBase*)_ptr2, _i );
}

void* newBRACKETS( void *_ptr1 )
{
  return new ParseTreeBRACKETS( (ParseTreeBase*)_ptr1 );
}

void* newNOT( void *_ptr1 )
{
  return new ParseTreeNOT( (ParseTreeBase*)_ptr1 );
}

void* newEXIST( void *_ptr1 )
{
  return new ParseTreeEXIST( (char*)_ptr1 );
}

void* newID( char *_ptr1 )
{
  return new ParseTreeID( _ptr1 );
}

void* newSTRING( char *_ptr1 )
{
  return new ParseTreeSTRING( _ptr1 );
}

void* newNUM( int _i )
{
  return new ParseTreeNUM( _i );
}

void* newFLOAT( float _f )
{
  return new ParseTreeFLOAT( _f );
}

void* newBOOL( char _b )
{
  return new ParseTreeBOOL( (bool)_b );
}

void* newWITH( void *_ptr1 )
{
  return new ParseTreeWITH( (ParseTreeBase*)_ptr1 );
}

void* newMAX( void *_ptr1 )
{
  return new ParseTreeMAX( (ParseTreeBase*)_ptr1 );
}

void* newMIN( void *_ptr1 )
{
  return new ParseTreeMIN( (ParseTreeBase*)_ptr1 );
}

void* newFIRST()
{
  return new ParseTreeFIRST();
}

void* newRANDOM()
{
  return new ParseTreeRANDOM();
}

void* newMAX2( const char *_id )
{
  return new ParseTreeMAX2( _id );
}

void* newMIN2( const char *_id )
{
  return new ParseTreeMIN2( _id );
}
