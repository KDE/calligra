#include "asciiimport_factory.h"
#include "asciiimport_factory.moc"
#include "asciiimport.h"

#include <klibglobal.h>

extern "C"
{
    void* init_libasciiimport()
    {
        return new ASCIIImportFactory;
    }
};

KLibGlobal* ASCIIImportFactory::s_global = 0;

ASCIIImportFactory::ASCIIImportFactory( QObject* parent, const char* name )
    : Factory( parent, name )
{
    s_global = new KLibGlobal( "asciiimport" );
}

ASCIIImportFactory::~ASCIIImportFactory()
{
}

QObject* ASCIIImportFactory::create( QObject* parent, const char* name )
{
    if ( parent && !parent->inherits("KoFilter") )
    {
	    qDebug("ASCIIImportFactory: parent does not inherit KoFilter");
	    return 0L;
    }
    return new ASCIIImport( (KoFilter*)parent, name );
}

KLibGlobal* ASCIIImportFactory::global()
{
    return s_global;
}


