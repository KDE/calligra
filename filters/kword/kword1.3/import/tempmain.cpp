#include <qapplication.h>
#include <qfile.h>
#include <qxml.h>

#include "kwordparser.h"
#include "kworddocument.h"

int main( int argc, char **argv )
{
    QApplication a( argc, argv );		// QApplication required!

    const char* orig = "maindoc.xml";

    if ( argc >= 2 )				// use argument as file name
	orig = argv[1];


    KWordDocument kwordDocument;
    
    KWordParser handler( &kwordDocument );

    //We arbitrarily decide that Qt can handle the encoding in which the file was written!!
    QXmlSimpleReader reader;
    reader.setContentHandler( &handler );
    //reader.setErrorHandler( &handler );

    QFile fileIn(orig);
    fileIn.open(IO_ReadOnly);
    QXmlInputSource source(&fileIn); // Read the file
    fileIn.close();

    qDebug("Parsing...");
    if (!reader.parse( source ))
    {
        qDebug("Parse Error");
        return 1;
    }

}
