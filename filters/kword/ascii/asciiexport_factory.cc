#include "asciiexport_factory.h"
#include "asciiexport_factory.moc"
#include "asciiexport.h"

#include <klibglobal.h>

extern "C"
{
    void* init_libasciiexport()
    {
        return new ASCIIExportFactory;
    }
};

KLibGlobal* ASCIIExportFactory::s_global = 0;

ASCIIExportFactory::ASCIIExportFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
    s_global = new KLibGlobal( "asciiexport" );
}

ASCIIExportFactory::~ASCIIExportFactory()
{
}

QObject* ASCIIExportFactory::create( QObject* parent, const char* name, const char* classname )
{
    if ( parent && !parent->inherits("KoFilter") )
    {
	    qDebug("ASCIIExportFactory: parent does not inherit KoFilter");
	    return 0L;
    }
    return new ASCIIExport( (KoFilter*)parent, name );
}

KLibGlobal* ASCIIExportFactory::global()
{
    return s_global;
}
