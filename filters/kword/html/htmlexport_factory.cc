#include "htmlexport_factory.h"
#include "htmlexport_factory.moc"
#include "htmlexport.h"

#include <kinstance.h>

extern "C"
{
    void* init_libhtmlexport()
    {
        return new HTMLExportFactory;
    }
};

KInstance* HTMLExportFactory::s_global = 0;

HTMLExportFactory::HTMLExportFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
    s_global = new KInstance( "htmlexport" );
}

HTMLExportFactory::~HTMLExportFactory()
{
}

QObject* HTMLExportFactory::create( QObject* parent, const char* name, const char* )
{
    if ( parent && !parent->inherits("KoFilter") )
    {
	    qDebug("HTMLExportFactory: parent does not inherit KoFilter");
	    return 0L;
    }
    return new HTMLExport( (KoFilter*)parent, name );
}

KInstance* HTMLExportFactory::global()
{
    return s_global;
}
