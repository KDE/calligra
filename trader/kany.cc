#include "kany.h"

#include <assert.h>

ostream& operator<<( ostream& out, CORBA::Any& any )
{
  printAny( out, any, 0L );
  
  return out;
}

bool printAny( ostream& out, CORBA::Any& _any, CORBA::TypeCode_ptr _tc )
{
  if ( CORBA::is_nil( _tc ) )
  { 
    _tc = _any.type();
    assert( !CORBA::is_nil( _tc ) );
  }
  
  switch( _tc->kind() )
  {
  case CORBA::tk_string:
    {
      char* text;
      if ( !( _any >>= CORBA::Any::to_string( text, 0 ) ) )
	return false;
      out << "'" << (const char*)text << "'";
      CORBA::string_free( text );
      return true;
    }
  case CORBA::tk_long:
    {
      CORBA::Long x;
      if ( !( _any >>= x ) )
	return false;
      cout << x;  
      return true;
    }
  case CORBA::tk_float:
    {
      CORBA::Float x;
      if ( !( _any >>= x ) )
	return false;
      cout << x;
      return true;
    }
  case CORBA::tk_boolean:
    {
      CORBA::Boolean x;
      if ( !( _any >>= CORBA::Any::to_boolean( x ) ) )
      {
	cerr << "ERROR: Could not extract boolean" << endl;
	return false;
      }

      if ( x )
	out << "true";
      else
	out << "false";
      return true;
    }
  case CORBA::tk_sequence:
    {
      CORBA::ULong len;
      assert( _any.seq_get_begin( len ) );
      CORBA::TypeCode_var ctc = _tc->content_type();
      // HACK
      cerr << "content type=" << ctc->id() << endl;
      out << "( ";
      for( CORBA::ULong i = 0; i < len; i++ )
      {
	if ( !printAny( out, _any, ctc ) )
	{
	  cerr << "Could not extract sequence item " << i << endl;
	  return 0L;
	}
	if ( i + 1 < len )
	  out << ", ";
      }
      assert( _any.seq_get_end() );
      out << " )";
      return true;
    }
  case CORBA::tk_alias:
    {
      const CORBA::TypeCode_var ntc = _tc->content_type();
      return printAny( out, _any, ntc );
    }
  default:
    cerr << "INTERNAL ERROR: Did not handle tk_XXXX=" << (int)_tc->kind() << endl;
    assert( 0 );
  }
}
