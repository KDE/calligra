/**
 * $Id$
 *
 * Kalle Dalheimer <kalle@kde.org>
 */

#include "kchart_factory.h"
#include "kchart_part.h"
#include <klocale.h>
#include <kinstance.h>
#include <kaboutdata.h>
#include <kdebug.h>
#include <kiconloader.h>

static const char* description=I18N_NOOP("KOffice Chart Generator");
static const char* version="0.1";

extern "C"
{
    void* init_libkchartpart()
    {
	return new KChartFactory;
    }
};

KInstance* KChartFactory::s_global = 0;

KChartFactory::KChartFactory( QObject* parent, const char* name )
    : KoFactory( parent, name )
{
    global();
}

KChartFactory::~KChartFactory()
{
    if ( s_global )
    {
      delete s_global->aboutData();
      delete s_global;
      s_global = 0L;
    }
}

KParts::Part* KChartFactory::createPart( QWidget *parentWidget, const char *widgetName, QObject* parent, const char* name, const char *classname, const QStringList & )
{
    bool bWantKoDocument = ( strcmp( classname, "KoDocument" ) == 0 );

    KChartPart *part = new KChartPart( parentWidget, widgetName, parent, name, !bWantKoDocument );

    if ( !bWantKoDocument )
      part->setReadWrite( false );

    emit objectCreated(part);
    return part;
}

KAboutData* KChartFactory::aboutData()
{
    KAboutData *aboutData = new KAboutData("kchart", I18N_NOOP("KChart"),
        version, description, KAboutData::License_GPL,
        "(c) 1998-2000, Kalle Dalheimer");
    aboutData->addAuthor("Kalle Dalheimer",0, "kalle@kde.org");
    return aboutData;
}

KInstance* KChartFactory::global()
{
    if ( !s_global )
    {
         s_global = new KInstance(aboutData());
	 // Tell the iconloader about share/apps/koffice/icons
	 s_global->iconLoader()->addAppDir("koffice");
    }
    return s_global;
}

#include "kchart_factory.moc"
