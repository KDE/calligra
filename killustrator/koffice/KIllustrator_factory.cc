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
    : KLibFactory( parent, name )
{
    s_global = new KInstance( "killustrator" );
}

KIllustratorFactory::~KIllustratorFactory()
{
}

QObject* KIllustratorFactory::create( QObject* parent, const char* name, const char* classname, const QStringList & )
{
    if ( parent && !parent->inherits("KoDocument") )
    {
	qDebug("KIllustratorFactory: parent does not inherit KoDocument");
	return 0;
    }

    return new KIllustratorDocument( parent, name );
}

KInstance* KIllustratorFactory::global()
{
    return s_global;
}

#include "KIllustrator_factory.moc"
