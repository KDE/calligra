#include "csvfilter_factory.h"
#include "csvfilter_factory.moc"
#include "csvfilter.h"

#include <kinstance.h>

extern "C"
{
    void* init_libcsvfilter()
    {
        return new CSVFilterFactory;
    }
};

KInstance* CSVFilterFactory::s_global = 0;

CSVFilterFactory::CSVFilterFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
    s_global = new KInstance( "csvfilter" );
}

CSVFilterFactory::~CSVFilterFactory()
{
}

QObject* CSVFilterFactory::create( QObject* parent, const char* name, const char* )
{
    if ( parent && !parent->inherits("KoFilter") )
    {
	    qDebug("CSVFilterFactory: parent does not inherit KoFilter");
	    return 0L;
    }
    return new CSVFilter( (KoFilter*)parent, name );
}

KInstance* CSVFilterFactory::global()
{
    return s_global;
}
