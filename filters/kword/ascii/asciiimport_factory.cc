#include "asciiimport_factory.h"
#include "asciiimport_factory.moc"
#include "asciiimport.h"

#include <kinstance.h>

extern "C"
{
    void* init_libasciiimport()
    {
        return new ASCIIImportFactory;
    }
};

KInstance* ASCIIImportFactory::s_global = 0;

ASCIIImportFactory::ASCIIImportFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
    s_global = new KInstance( "asciiimport" );
}

ASCIIImportFactory::~ASCIIImportFactory()
{
    delete s_global;
}

QObject* ASCIIImportFactory::create( QObject* parent, const char* name, const char* )
{
    if ( parent && !parent->inherits("KoFilter") )
    {
	    qDebug("ASCIIImportFactory: parent does not inherit KoFilter");
	    return 0L;
    }
    ASCIIImport *f = new ASCIIImport( (KoFilter*)parent, name );
    emit objectCreated(f);
    return f;
}

KInstance* ASCIIImportFactory::global()
{
    return s_global;
}
