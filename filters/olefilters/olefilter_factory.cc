#include "olefilter_factory.h"
#include "olefilter_factory.moc"
#include "olefilter.h"

#include <kinstance.h>

extern "C"
{
    void* init_libolefilter()
    {
        return new OLEFilterFactory;
    }
};

KInstance* OLEFilterFactory::s_global = 0;

OLEFilterFactory::OLEFilterFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
    s_global = new KInstance( "olefilter" );
}

OLEFilterFactory::~OLEFilterFactory()
{
    delete s_global;
}

QObject* OLEFilterFactory::create( QObject* parent, const char* name, const char* )
{
    if ( parent && !parent->inherits("KoFilter") )
    {
	    qDebug("OLEFilterFactory: parent does not inherit KoFilter");
	    return 0L;
    }
    OLEFilter *f = new OLEFilter( (KoFilter*)parent, name );
    emit objectCreated(f);
    return f;
}

KInstance* OLEFilterFactory::global()
{
    return s_global;
}
