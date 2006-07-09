#include "kformula_factory.h"
#include "kformula_doc.h"
#include "kformula_aboutdata.h"

#include <kinstance.h>
#include <kiconloader.h>

#include <kfiledialog.h>
#include <kglobal.h>
#include <kstandarddirs.h>
#include <klocale.h>

#include <qstringlist.h>

K_EXPORT_COMPONENT_FACTORY( libkformulapart, KFormulaFactory )

KInstance* KFormulaFactory::s_global = 0;
KAboutData* KFormulaFactory::s_aboutData = 0;

KAboutData* KFormulaFactory::aboutData()
{
    if( !s_aboutData )
        s_aboutData=newKFormulaAboutData();
    return s_aboutData;
}


KFormulaFactory::KFormulaFactory( QObject* parent, const char* name )
    : KoFactory( parent, name )
{
    // Create our instance, so that it becomes KGlobal::instance if the
    // main app is KFormula.
    (void)global();
}

KFormulaFactory::~KFormulaFactory()
{
  delete s_aboutData;
  s_aboutData=0;
  delete s_global;
  s_global = 0L;
}

KParts::Part* KFormulaFactory::createPartObject( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, const char* classname, const QStringList & )
{
  bool bWantKoDocument = ( strcmp( classname, "KoDocument" ) == 0 );

  KFormulaDoc *doc = new KFormulaDoc( parentWidget, widgetName, parent, name, !bWantKoDocument );

  if ( !bWantKoDocument )
    doc->setReadWrite( false );

  return doc;
}

KInstance* KFormulaFactory::global()
{
    if ( !s_global )
    {
      s_global = new KInstance(aboutData());
      s_global->dirs()->addResourceType( "toolbar",
				         KStandardDirs::kde_default("data") + "koffice/toolbar/");
      // Tell the iconloader about share/apps/koffice/icons*/
      s_global->iconLoader()->addAppDir("koffice");
    }

    return s_global;
}

#include "kformula_factory.moc"
