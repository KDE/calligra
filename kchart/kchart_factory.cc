/**
 * $Id$
 *
 * Kalle Dalheimer <kalle@kde.org>
 */

#include "kchart_factory.h"
#include "kchart_part.h"

#include <kinstance.h>

extern "C"
{
    void* init_libkchart()
    {
	return new KChartFactory;
    }
};

KInstance* KChartFactory::s_global = 0;

KChartFactory::KChartFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
    s_global = new KInstance( "kchart" );
}

KChartFactory::~KChartFactory()
{
    delete s_global;
}

QObject* KChartFactory::create( QObject* parent, const char* name, const char*, const QStringList & )
{
    if ( parent && !parent->inherits("KoDocument") ) {
		qDebug("KChartFactory: parent does not inherit KoDocument");
		return 0;
    }
    KChartPart *part = new KChartPart( (KoDocument*)parent, name );
    emit objectCreated(part);
    return part;
}

KInstance* KChartFactory::global()
{
    return s_global;
}

#include "kchart_factory.moc"
