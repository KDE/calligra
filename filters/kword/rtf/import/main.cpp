/**
 * $Id$
 */

#include "KRTFTokenizer.h"
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include "KRTFToken.h"
#include "KRTFFileParser.h"


#include <qfile.h>
#include <kapp.h>
#include <stdlib.h>

#include "koStore.h"

int main( int argc, char* argv[] )
{
    if( argc != 3 ) {
	fprintf( stderr, "Usage: rtf2kword <infile> <outfile>\n" );
	exit( -127 );
    }
    //KCmdLineArgs::init( 0,(char **) 0 ,(KAboutData *)0);
    KApplication app(argc,argv,"rtf2kwd");
    QFile in( argv[1] );
    KoStore *out = new KoStore(QString( argv[2]), KoStore::Write);

    if( !in.open( IO_ReadOnly ) ) {
	qDebug( "Cannot open %s", argv[1] );
	exit( -126 );
    }

    if( !out->open("root") ) {
	qDebug( "Cannot open %s", argv[2] );
	exit( -125 );
    }
	
    KRTFTokenizer tokenizer( &in );

    KRTFFileParser parser( &tokenizer, out );
    if( !parser.parse() ) {
	qWarning( "Error in RTF file" );
    }
      out->close();
      delete(out);

    in.close();
}
