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
    delete s_global;
}

QObject* CSVFilterFactory::create( QObject* parent, const char* name, const char* )
{
    if ( parent && !parent->inherits("KoFilter") )
    {
	    qDebug("CSVFilterFactory: parent does not inherit KoFilter");
	    return 0L;
    }
    CSVFilter *f = new CSVFilter( (KoFilter*)parent, name );
    emit objectCreated(f);
    return f;
}

KInstance* CSVFilterFactory::global()
{
    return s_global;
}
