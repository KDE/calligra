/**
 * $Id:$
 */

#include "KRTFTokenizer.h"
#include "KRTFToken.h"

#include <qfile.h>

int main( int argc, char* argv[] )
{
    if( argc != 3 ) {
	fprintf( stderr, "Usage: rtf2kword <infile> <outfile>" );
	exit( -127 );
    }
    
    QFile in( argv[1] );
    QFile out( argv[2] );

    if( !in.open( IO_ReadOnly ) ) {
	debug( "Cannot open %s", argv[1] );
	exit( -126 );
    }
    if( !out.open( IO_ReadOnly ) ) {
	debug( "Cannot open %s", argv[2] );
	exit( -125 );
    }
	
    KRTFTokenizer tokenizer( &in );
    KRTFToken* token;
    while( !in.atEnd() ) {
	token = tokenizer.nextToken();
	switch( token->_type ) {
	case ControlWord:
	    debug( "Control word found: %s, param: %s", token->_text.data(), token->_param.data() );
	    break;
	case ControlSymbol:
	    debug( "Control symbol found: %s", token->_text.data() );
	    break;
	case OpenGroup:
	    debug( "Open group found" );
	    break;
	case CloseGroup:
	    debug( "Close group found" );
	    break;
	case PlainText:
	    debug( "Plain text found: %s", token->_text.data() );
	}
	delete token;
    }


    in.close();
    out.close();
}
