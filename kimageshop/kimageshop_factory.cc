#include "kimageshop_factory.h"
#include "kimageshop_doc.h"

#include <kinstance.h>
#include <kglobal.h>
#include <kstddirs.h>

extern "C"
{
    void* init_libkimageshop()
    {
	return new KImageShopFactory;
    }
};

KInstance* KImageShopFactory::s_global = 0;

KImageShopFactory::KImageShopFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
  s_global = new KInstance( "kimageshop" );
  
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
}

KImageShopFactory::~KImageShopFactory()
{
  delete s_global;
}

QObject* KImageShopFactory::create( QObject* parent, const char* name, const char* /*classname*/, const QStringList & )
{
    if ( parent && !parent->inherits("KoDocument") )
    {
	qDebug("KImageShopFactory: parent does not inherit KoDocument");
	return 0;
    }
    
    KImageShopDoc *doc = new KImageShopDoc( (KoDocument*)parent, name );
    emit objectCreated(doc);
    return doc;
}

KInstance* KImageShopFactory::global()
{
    return s_global;
}

#include "kimageshop_factory.moc"
