#include <KIllustrator_factory.h>
#include <KIllustrator_doc.h>
#include <KIllustrator_aboutdata.h>

#include <kinstance.h>
#include <kiconloader.h>
#include <kstddirs.h>
#include <kaboutdata.h>
#include <klocale.h>

extern "C"
{
    void* init_libkontourpart()
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
    delete s_aboutData;
    s_aboutData=0;
    delete s_global;
    s_global = 0L;
}

KAboutData* KIllustratorFactory::aboutData()
{
  if( !s_aboutData )
      s_aboutData=newKIllustratorAboutData();
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
    s_global->dirs ()->addResourceType ("kontour_template",
                                        KStandardDirs::kde_default("data") +
                                        "kontour/templates/");
    s_global->dirs ()->addResourceType ("kontour_palettes",
                                        KStandardDirs::kde_default("data") +
                                        "kontour/palettes/");
    // Tell the iconloader about share/apps/koffice/icons
    s_global->iconLoader()->addAppDir("koffice");
    return s_global;
}

#include <KIllustrator_factory.moc>
