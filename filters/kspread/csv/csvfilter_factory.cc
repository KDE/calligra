#include "csvfilter_factory.h"
#include "csvfilter_factory.moc"
#include "csvfilter.h"

#include <klibglobal.h>

extern "C"
{
    void* init_libcsvfilter()
    {
        return new CSVFilterFactory;
    }
};

KLibGlobal* CSVFilterFactory::s_global = 0;

CSVFilterFactory::CSVFilterFactory( QObject* parent, const char* name )
    : Factory( parent, name )
{
    s_global = new KLibGlobal( "csvfilter" );
}

CSVFilterFactory::~CSVFilterFactory()
{
}

QObject* CSVFilterFactory::create( QObject* parent, const char* name )
{
    if ( parent && !parent->inherits("KoFilter") )
    {
	    qDebug("CSVFilterFactory: parent does not inherit KoFilter");
	    return 0L;
    }
    return new CSVFilter( (KoFilter*)parent, name );
}

KLibGlobal* CSVFilterFactory::global()
{
    return s_global;
}
