/**
 * $Id$
 */

#include "KRTFTokenizer.h"
#include "KRTFToken.h"
#include "KRTFFileParser.h"

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

    KRTFFileParser parser( &tokenizer );
    if( !parser.parse() ) {
	qWarning( "Error in RTF file" );
	exit( -124 );
    }

    in.close();
    out.close();
}
