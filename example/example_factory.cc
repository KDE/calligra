#include "example_factory.h"
#include "example_part.h"

#include <kinstance.h>

extern "C"
{
    void* init_libexample()
    {
	return new ExampleFactory;
    }
};

KInstance* ExampleFactory::s_global = 0;

ExampleFactory::ExampleFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
    s_global = new KInstance( "example" );
}

ExampleFactory::~ExampleFactory()
{
    delete s_global;
}

QObject* ExampleFactory::create( QObject* parent, const char* name, const char* /*classname*/ )
{
    if ( parent && !parent->inherits("KoDocument") )
    {
	qDebug("ExampleFactory: parent does not inherit KoDocument");
	return 0;
    }
    ExamplePart *part = new ExamplePart( (KoDocument*)parent, name );
    emit objectCreated(part);
    return part;
}

KInstance* ExampleFactory::global()
{
    return s_global;
}

#include "example_factory.moc"
