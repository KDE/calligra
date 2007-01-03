/**
 * $Id$
 *
 * Kalle Dalheimer <kalle@kde.org>
 */


#include "kchart_factory.h"
#include "kchart_aboutdata.h"
#include "kchart_part.h"
#include <klocale.h>
#include <kinstance.h>
#include <kdebug.h>
#include <kiconloader.h>
#include <kstandarddirs.h>

namespace KChart
{

KInstance   *KChartFactory::s_global     = 0;
KIconLoader *KChartFactory::s_iconLoader = 0;
KAboutData  *KChartFactory::s_aboutData  = 0;


KChartFactory::KChartFactory( QObject* parent, const char* name )
    : KoFactory( parent, name )
{
    global();
}


KChartFactory::~KChartFactory()
{
    delete s_aboutData;
    s_aboutData = 0;
    delete s_global;
    s_global = 0;
    delete s_iconLoader;
    s_iconLoader = 0;
}


KParts::Part* KChartFactory::createPartObject( QWidget *parentWidget,
					       QObject* parent,
					       const char *classname,
					       const QStringList & )
{
    bool bWantKoDocument = ( strcmp( classname, "KoDocument" ) == 0 );

    KChartPart *part = new KChartPart( parentWidget, parent,
				       !bWantKoDocument );

    if ( !bWantKoDocument )
      part->setReadWrite( false );

    return part;
}


KAboutData* KChartFactory::aboutData()
{
    if( !s_aboutData )
        s_aboutData = newKChartAboutData();
    return s_aboutData;
}


KInstance* KChartFactory::global()
{
    if ( !s_global )
    {
         s_global = new KInstance(aboutData());
         s_global->dirs()->addResourceType( "kchart_template",
         KStandardDirs::kde_default("data") + "kchart/templates/");
    }
    return s_global;
}


KIconLoader* KChartFactory::iconLoader()
{
  if ( !s_iconLoader ) {
      // Tell the iconloader about share/apps/koffice/icons
      s_iconLoader = new KIconLoader(global()->instanceName(), global()->dirs());
      s_iconLoader->addAppDir("koffice");
  }
  
  return s_iconLoader;
}


}  //namespace KChart

#include <kchart_factory.moc>
