#include "kspread_factory.h"
#include "kspread_doc.h"
#include <kaboutdata.h>
#include "KSpreadAppIface.h"

#include <klocale.h>
#include <kinstance.h>
#include <kstddirs.h>
#include <kdebug.h>
#include <kiconloader.h>

static const char* description=I18N_NOOP("KOffice Spreadsheet Application");
static const char* version="1.1";

extern "C"
{
    void* init_libkspreadpart()
    {
	return new KSpreadFactory;
    }
};

KInstance* KSpreadFactory::s_global = 0;
DCOPObject* KSpreadFactory::s_dcopObject = 0;
KAboutData* KSpreadFactory::s_aboutData = 0;

KSpreadFactory::KSpreadFactory( QObject* parent, const char* name )
    : KoFactory( parent, name )
{
    kdDebug(36001) << "KSpreadFactory::KSpreadFactory()" << endl;
    // Create our instance, so that it becomes KGlobal::instance if the
    // main app is KSpread.
    (void)global();
    (void)dcopObject();
}

KSpreadFactory::~KSpreadFactory()
{
  kdDebug(36001) << "KSpreadFactory::~KSpreadFactory()" << endl;
  delete s_aboutData;
  s_aboutData=0;
  delete s_global;
  s_global = 0L;
  delete s_dcopObject;
  s_dcopObject = 0L;
}

KParts::Part* KSpreadFactory::createPart( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, const char* classname, const QStringList & )
{
  bool bWantKoDocument = ( strcmp( classname, "KoDocument" ) == 0 );

  KSpreadDoc *doc = new KSpreadDoc( parentWidget, widgetName, parent, name, !bWantKoDocument );

  if ( !bWantKoDocument )
    doc->setReadWrite( false );

  emit objectCreated( doc );
  return doc;
}

KAboutData* KSpreadFactory::aboutData()
{
  if( !s_aboutData )
  {
    s_aboutData = new KAboutData( "kspread", I18N_NOOP("KSpread"),
                                  version, description, KAboutData::License_GPL,
                                  "(c) 1998-2000, Torben Weis");
    s_aboutData->addAuthor("Torben Weis",0, "weis@kde.org");
    s_aboutData->addAuthor("Laurent Montel",0, "lmontel@mandrakesoft.com");
    s_aboutData->addAuthor("David Faure",0, "faure@kde.org");
  }
  return s_aboutData;
}

KInstance* KSpreadFactory::global()
{
    if ( !s_global )
    {
      s_global = new KInstance(aboutData());
      s_global->dirs()->addResourceType( "toolbar",
				         KStandardDirs::kde_default("data") + "koffice/toolbar/");
      s_global->dirs()->addResourceType( "extensions", KStandardDirs::kde_default("data") + "kspread/extensions/");
      s_global->dirs()->addResourceType( "table-styles", KStandardDirs::kde_default("data") + "kspread/tablestyles/");
      // Tell the iconloader about share/apps/koffice/icons
      s_global->iconLoader()->addAppDir("koffice");
    }
    return s_global;
}

DCOPObject* KSpreadFactory::dcopObject()
{
    if ( !s_dcopObject )
        s_dcopObject = new KSpreadAppIface;
    return s_dcopObject;
}

#include "kspread_factory.moc"
