#include "KIllustrator_factory.h"
#include "KIllustrator_doc.h"

#include <kinstance.h>
#include <kstddirs.h>

extern "C"
{
    void* init_libkillustrator()
    {
	return new KIllustratorFactory;
    }
};

KInstance* KIllustratorFactory::s_global = 0;

KIllustratorFactory::KIllustratorFactory( QObject* parent, const char* name )
    : KoFactory( parent, name )
{
}

KIllustratorFactory::~KIllustratorFactory()
{
  if ( s_global )
    delete s_global;
}

QObject* KIllustratorFactory::create( QObject* parent, const char* name, const char* classname, const QStringList & )
{
/*
   if ( parent && !parent->inherits("KoDocument") )
    {
	qDebug("KIllustratorFactory: parent does not inherit KoDocument");
	return 0;
    }
*/

    bool bWantKoDocument = ( strcmp( classname, "KoDocument" ) == 0 );

    KIllustratorDocument *doc = new KIllustratorDocument( parent, name, !bWantKoDocument );

    if ( !bWantKoDocument )
      doc->setReadWrite( false );

    emit objectCreated( doc );
    return doc;
}

KInstance* KIllustratorFactory::global()
{
    if ( !s_global )
      s_global = new KInstance( "killustrator" );
    return s_global;
}

#include "KIllustrator_factory.moc"
