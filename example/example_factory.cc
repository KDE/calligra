#include "example_factory.h"
#include "example_part.h"
#include <kaboutdata.h>
#include <kinstance.h>
#include <klocale.h>
#include <kdebug.h>

extern "C"
{
    void* init_libexample()
    {
	return new ExampleFactory;
    }
};

static const char* description=I18N_NOOP("Example KOffice Program");
static const char* version="0.1";

KInstance* ExampleFactory::s_global = 0;

ExampleFactory::ExampleFactory( QObject* parent, const char* name )
    : KoFactory( parent, name )
{
}

ExampleFactory::~ExampleFactory()
{
    if ( s_global )
    {
      delete s_global->aboutData();
      delete s_global;
    }
}

QObject* ExampleFactory::create( QObject* parent, const char* name, const char* classname, const QStringList & )
{
/*
    if ( parent && !parent->inherits("KoDocument") )
    {
	kdDebug(31000) << "ExampleFactory: parent does not inherit KoDocument" << endl;
	return 0;
    }
*/

    bool bWantKoDocument = ( strcmp( classname, "KoDocument" ) == 0 );

    ExamplePart *part = new ExamplePart( parent, name, !bWantKoDocument );

    if ( !bWantKoDocument )
      part->setReadWrite( false );

    emit objectCreated(part);
    return part;
}

KAboutData* ExampleFactory::aboutData()
{
    // Change this, of course
    KAboutData *aboutData = new KAboutData( "example", I18N_NOOP("Example"),
        version, description, KAboutData::License_GPL,
        "(c) 1998-2000, Torben Weis");
    aboutData->addAuthor("Torben Weis",0, "weis@kde.org");
    return aboutData;
}

KInstance* ExampleFactory::global()
{
    if ( !s_global )
    {
        s_global = new KInstance( aboutData() );
        // Add any application-specific resource directories here
    }

    return s_global;
}

#include "example_factory.moc"
