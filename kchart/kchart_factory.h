/**
 * $Id$
 *
 * Kalle Dalheimer <kalle@kde.org>
 */

#ifndef KCHART_FACTORY_H
#define KCHART_FACTORY_H

#include <klibloader.h>

class KInstance;

class KChartFactory : public KLibFactory
{
    Q_OBJECT
public:
    KChartFactory( QObject* parent = 0, const char* name = 0 );
    virtual ~KChartFactory();

    virtual QObject* create( QObject* parent = 0, const char* name = 0, const char* classname = "QObject" );

    static KInstance* global();

private:
    static KInstance* s_global;
};

#endif
