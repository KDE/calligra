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
}

ExampleFactory::~ExampleFactory()
{
    if ( s_global ) 
      delete s_global;
}

QObject* ExampleFactory::create( QObject* parent, const char* name, const char* classname, const QStringList & )
{
/*
    if ( parent && !parent->inherits("KoDocument") )
    {
	qDebug("ExampleFactory: parent does not inherit KoDocument");
	return 0;
    }
*/

    bool bWantKoDocument = ( strcmp( classname, "KofficeDocument" ) == 0 );

    ExamplePart *part = new ExamplePart( parent, name, !bWantKoDocument );

    if ( !bWantKoDocument )
      part->setReadWrite( false );

    emit objectCreated(part);
    return part;
}

KInstance* ExampleFactory::global()
{
    if ( !s_global )
      s_global = new KInstance( "example" );
    return s_global;
}

#include "example_factory.moc"
