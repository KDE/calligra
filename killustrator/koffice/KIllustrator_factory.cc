#include <KIllustrator_factory.h>
#include <KIllustrator_doc.h>

#include <kinstance.h>
#include <kiconloader.h>
#include <kstddirs.h>
#include <kaboutdata.h>
#include <klocale.h>

static const char* description=I18N_NOOP("KOffice Illustration Tool");
static const char* version="0.2";

extern "C"
{
    void* init_libkillustratorpart()
    {
        return new KIllustratorFactory;
    }
};

KInstance* KIllustratorFactory::s_global = 0;
KAboutData* KIllustratorFactory::s_aboutData = 0;

KIllustratorFactory::KIllustratorFactory( QObject* parent, const char* name )
    : KoFactory( parent, name )
{
    global();
}

KIllustratorFactory::~KIllustratorFactory()
{
  if( s_aboutData )
  {
    delete s_aboutData;
    s_aboutData=0;
  }
  if ( s_global )
  {
    delete s_global;
    s_global = 0L;
  }
}

KAboutData* KIllustratorFactory::aboutData()
{
  if( !s_aboutData )
  {
    s_aboutData= new KAboutData ("killustrator",
                                 I18N_NOOP("KIllustrator"),
                                 version, description,
                                 KAboutData::License_GPL,
                                 "(c) 1998-2000, Kai-Uwe Sattler");
    s_aboutData->addAuthor("Kai-Uwe Sattler", 0, "kus@iti.cs.uni-magdeburg.de");
  }
  return s_aboutData;
}

KParts::Part* KIllustratorFactory::createPart( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, const char* classname, const QStringList & )
{
    bool bWantKoDocument = ( strcmp( classname, "KoDocument" ) == 0 );

    KIllustratorDocument *doc = new KIllustratorDocument( parentWidget, widgetName, parent, name, !bWantKoDocument );

    if ( !bWantKoDocument )
      doc->setReadWrite( false );

    emit objectCreated( doc );
    return doc;
}

KInstance* KIllustratorFactory::global()
{
    if ( !s_global )
      s_global = new KInstance(aboutData ());
    s_global->dirs ()->addResourceType ("killustrator_template",
                                        KStandardDirs::kde_default("data") +
                                        "killustrator/templates/");
    s_global->dirs ()->addResourceType ("killustrator_palettes",
                                        KStandardDirs::kde_default("data") +
                                        "killustrator/palettes/");
    // Tell the iconloader about share/apps/koffice/icons
    s_global->iconLoader()->addAppDir("koffice");
    return s_global;
}

#include <KIllustrator_factory.moc>
