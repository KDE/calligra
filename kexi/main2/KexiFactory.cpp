#include "KexiFactory.h"
#include "KexiDocument.h"
#include "KexiAboutData.h"

#include <kdebug.h>
#include <kinstance.h>
#include <kstandarddirs.h>
#include <kiconloader.h>

KInstance* KexiFactory::s_global = 0;
KAboutData* KexiFactory::s_aboutData = 0;

KexiFactory::KexiFactory(QObject* parent, const char* name)
    : KoFactory(parent, name)
{
    (void)global();
}

KexiFactory::~KexiFactory()
{
    delete s_aboutData; s_aboutData=0;
    delete s_global; s_global = 0;
}

KParts::Part* KexiFactory::createPartObject( QWidget *parentWidget, QObject* parent, const char* classname, const QStringList & )
{
    bool bWantKoDocument = ( strcmp( classname, "KoDocument" ) == 0 );
    KexiDocument *doc = new KexiDocument(parentWidget, parent, ! bWantKoDocument);
    if( ! bWantKoDocument )
        doc->setReadWrite( false );
    return doc;
}

KAboutData* KexiFactory::aboutData()
{
    if( !s_aboutData )
        s_aboutData = newAboutData();
    return s_aboutData;
}

KInstance* KexiFactory::global()
{
    if ( !s_global ) {
        s_global = new KInstance(aboutData());
        //s_global->dirs()->addResourceType( "kspread_template", KStandardDirs::kde_default("data") + "kspread/templates/");
        //s_global->dirs()->addResourceType( "toolbar", KStandardDirs::kde_default("data") + "koffice/toolbar/");
        //s_global->dirs()->addResourceType( "functions", KStandardDirs::kde_default("data") + "kspread/functions/");
        //s_global->dirs()->addResourceType( "sheet-styles", KStandardDirs::kde_default("data") + "kspread/sheetstyles/");
        // Tell the iconloader about share/apps/koffice/icons
        s_global->iconLoader()->addAppDir("koffice");
    }
    return s_global;
}

#include "KexiFactory.moc"
