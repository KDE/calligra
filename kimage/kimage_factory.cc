#include <kinstance.h>
#include <kglobal.h>
#include <kstddirs.h>

#include "kimage_factory.h"
#include "kimage_doc.h"

extern "C"
{
    void* init_libkimage()
    {
	return new KImageFactory;
    }
};

KInstance* KImageFactory::s_global = 0;

KImageFactory::KImageFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
  s_global = new KInstance( "kimage" );
}

KImageFactory::~KImageFactory()
{
  delete s_global;
}

QObject* KImageFactory::create( QObject* parent, const char* name, const char* /* classname */, const QStringList & )
{
    if ( parent && !parent->inherits("KoDocument") )
    {
	qDebug("KImageFactory: parent does not inherit KoDocument");
	return 0;
    }
    
    KImageDocument *doc = new KImageDocument( (KoDocument*)parent, name );
    emit objectCreated(doc);
    return doc;
}

KInstance* KImageFactory::global()
{
    return s_global;
}

#include "kimage_factory.moc"
