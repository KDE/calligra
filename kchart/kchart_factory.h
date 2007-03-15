/**
 * $Id$
 *
 * Kalle Dalheimer <kalle@kde.org>
 */

#ifndef KCHART_FACTORY_H
#define KCHART_FACTORY_H


#include <KoFactory.h>
#include "kchart_export.h"

class KComponentData;
class KIconLoader;
class KAboutData;

namespace KChart
{

class KCHART_EXPORT KChartFactory : public KoFactory
{
    Q_OBJECT
public:
    explicit KChartFactory( QObject* parent = 0, const char* name = 0 );
    virtual ~KChartFactory();

    virtual KParts::Part  *createPartObject( QWidget* = 0,
					     QObject* parent = 0,
					     const char* classname = "KoDocument",
					     const QStringList &args = QStringList() );

    static const KComponentData &global();

    static KIconLoader* iconLoader();

    // _Creates_ a KAboutData but doesn't keep ownership
    static KAboutData* aboutData();

private:
    static KComponentData *s_global;
    static KIconLoader  *s_iconLoader;
    static KAboutData   *s_aboutData;
};

}  //namespace KChart

#endif
