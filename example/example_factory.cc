#include "example_factory.h"
#include "example_part.h"

#include <klibglobal.h>

extern "C"
{
    void* init_libexample()
    {
	return new ExampleFactory;
    }
};

KLibGlobal* ExampleFactory::s_global = 0;

ExampleFactory::ExampleFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
    s_global = new KLibGlobal( "example" );
}

ExampleFactory::~ExampleFactory()
{
}

QObject* ExampleFactory::create( QObject* parent, const char* name, const char* classname )
{
    if ( parent && !parent->inherits("KoDocument") )
    {
	qDebug("ExampleFactory: parent does not inherit KoDocument");
	return 0;
    }

    return new ExamplePart( (KoDocument*)parent, name );
}

KLibGlobal* ExampleFactory::global()
{
    return s_global;
}

#include "example_factory.moc"
