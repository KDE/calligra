#include "asciiexport_factory.h"
#include "asciiexport_factory.moc"
#include "asciiexport.h"

#include <kinstance.h>

extern "C"
{
    void* init_libasciiexport()
    {
        return new ASCIIExportFactory;
    }
};

KInstance* ASCIIExportFactory::s_global = 0;

ASCIIExportFactory::ASCIIExportFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
    s_global = new KInstance( "asciiexport" );
}

ASCIIExportFactory::~ASCIIExportFactory()
{
    delete s_global;
}

QObject* ASCIIExportFactory::create( QObject* parent, const char* name, const char*, const QStringList & )
{
    if ( parent && !parent->inherits("KoFilter") )
    {
	    qDebug("ASCIIExportFactory: parent does not inherit KoFilter");
	    return 0L;
    }
    ASCIIExport *f = new ASCIIExport( (KoFilter*)parent, name );
    emit objectCreated(f);
    return f;
}

KInstance* ASCIIExportFactory::global()
{
    return s_global;
}
