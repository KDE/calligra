#include "htmlimport_factory.h"
#include "htmlimport_factory.moc"
#include "htmlimport.h"

#include <kinstance.h>

extern "C"
{
    void* init_libhtmlimport()
    {
        return new HTMLImportFactory;
    }
};

KInstance* HTMLImportFactory::s_global = 0;

HTMLImportFactory::HTMLImportFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
    s_global = new KInstance( "htmlimport" );
}

HTMLImportFactory::~HTMLImportFactory()
{
    delete s_global;
}

QObject* HTMLImportFactory::create( QObject* parent, const char* name, const char* )
{
    if ( parent && !parent->inherits("KoFilter") )
    {
	    qDebug("HTMLImportFactory: parent does not inherit KoFilter");
	    return 0L;
    }
    HTMLImport *f = new HTMLImport( (KoFilter*)parent, name );
    emit objectCreated(f);
    return f;
}

KInstance* HTMLImportFactory::global()
{
    return s_global;
}
