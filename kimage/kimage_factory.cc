#include <kinstance.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kimgio.h>

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
  KImageIO::registerFormats();

  s_global = new KInstance( "kimage" );

  /*
      FIXME: What do we need from this ?
  s_global->dirs()->addResourceType("kis",
				    KStandardDirs::kde_default("data") + "kimageshop/");
  s_global->dirs()->addResourceType("kis_images",
				    KStandardDirs::kde_default("data") + "kimageshop/images/");
  s_global->dirs()->addResourceType("kis_brushes",
				    KStandardDirs::kde_default("data") + "kimageshop/brushes/");
  s_global->dirs()->addResourceType("toolbar",
				    KStandardDirs::kde_default("data") + "koffice/toolbar/");
  s_global->dirs()->addResourceType("kis_pics",
				    KStandardDirs::kde_default("data") + "kimageshop/pics/");
  */
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
