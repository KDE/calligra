#include "kformula_factory.h"
#include "kformula_doc.h"

#include <kinstance.h>
#include <kiconloader.h>

#include <kfiledialog.h>
#include <kglobal.h>
#include <kstddirs.h>
//#include <kimgio.h>
#include <klocale.h>
#include <kaboutdata.h>

#include <qstringlist.h>

extern "C"
{
    void* init_libkformulapart()
    {
	return new KFormulaFactory;
    }
};

KInstance* KFormulaFactory::s_global = 0;

KAboutData* KFormulaFactory::aboutData()
{
      KAboutData *aboutData= new KAboutData( "kformula","KFormula",
        "0.2", "KDE formulaeditor", KAboutData::License_GPL,
        "(c) 1998-2000, Andrea Rizzi");
      aboutData->addAuthor("Andrea Rizzi",0, "rizzi@kde.org");
      return aboutData;
}


KFormulaFactory::KFormulaFactory( QObject* parent, const char* name )
    : KoFactory( parent, name )
{
    (void)global();
    /*s_global = new KInstance( "kformula" );
    // Tell the iconloader about share/apps/koffice/icons
    s_global->iconLoader()->addAppDir("koffice");*/
}

KFormulaFactory::~KFormulaFactory()
{
  if ( s_global )
  {
    delete s_global->aboutData();
    delete s_global;
  }
   // delete s_global;
}

KParts::Part* KFormulaFactory::createPart( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, const char* classname, const QStringList & )
{
  bool bWantKoDocument = ( strcmp( classname, "KoDocument" ) == 0 );

  KFormulaDoc *doc = new KFormulaDoc( parentWidget, widgetName, parent, name, !bWantKoDocument );

  if ( !bWantKoDocument )
    doc->setReadWrite( false );

  emit objectCreated( doc );
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
