#include "olefilter_factory.h"
#include "olefilter_factory.moc"
#include "olefilter.h"

#include <klibglobal.h>

extern "C"
{
    void* init_libolefilter()
    {
        return new OLEFilterFactory;
    }
};

KLibGlobal* OLEFilterFactory::s_global = 0;

OLEFilterFactory::OLEFilterFactory( QObject* parent, const char* name )
    : Factory( parent, name )
{
    s_global = new KLibGlobal( "olefilter" );
}

OLEFilterFactory::~OLEFilterFactory()
{
}

QObject* OLEFilterFactory::create( QObject* parent, const char* name )
{
    if ( parent && !parent->inherits("KoFilter") )
    {
	    qDebug("OLEFilterFactory: parent does not inherit KoFilter");
	    return 0L;
    }
    return new OLEFilter( (KoFilter*)parent, name );
}

KLibGlobal* OLEFilterFactory::global()
{
    return s_global;
}


