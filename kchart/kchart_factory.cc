/**
 * $Id$
 *
 * Kalle Dalheimer <kalle@kde.org>
 */

#include "kchart_factory.h"
#include "kchart_part.h"

#include <klibglobal.h>

extern "C"
{
    void* init_libkchart()
    {
	return new KChartFactory;
    }
};

KLibGlobal* KChartFactory::s_global = 0;

KChartFactory::KChartFactory( QObject* parent, const char* name )
    : KLibFactory( parent, name )
{
    s_global = new KLibGlobal( "kchart" );
}

KChartFactory::~KChartFactory()
{
}

QObject* KChartFactory::create( QObject* parent, const char* name, const char* classname )
{
    if ( parent && !parent->inherits("KoDocument") ) {
		qDebug("KChartFactory: parent does not inherit KoDocument");
		return 0;
    }

    return new KChartPart( (KoDocument*)parent, name );
}

KLibGlobal* KChartFactory::global()
{
    return s_global;
}

#include "kchart_factory.moc"
