/**
 * $Id$
 *
 * Kalle Dalheimer <kalle@kde.org>
 */

#ifndef KCHART_FACTORY_H
#define KCHART_FACTORY_H

#include <klibloader.h>

class KInstance;
class KAboutData;

class KChartFactory : public KLibFactory
{
    Q_OBJECT
public:
    KChartFactory( QObject* parent = 0, const char* name = 0 );
    virtual ~KChartFactory();

    virtual QObject* create( QObject* parent = 0, const char* name = 0, const char* classname = "QObject", const QStringList &args = QStringList() );

    static KInstance* global();
 
    // _Creates_ a KAboutData but doesn't keep ownership
    static KAboutData* aboutData();

private:
    static KInstance* s_global;
};

#endif
