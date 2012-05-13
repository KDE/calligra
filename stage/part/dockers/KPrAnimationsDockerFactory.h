#ifndef KPRANIMATIONSDOCKERFACTORY_H
#define KPRANIMATIONSDOCKERFACTORY_H

#include <KoDockFactoryBase.h>

class KPrAnimationsDockerFactory: public KoDockFactoryBase
{
public:
    KPrAnimationsDockerFactory();

    QString id() const;
    QDockWidget* createDockWidget();
    DockPosition defaultDockPosition() const;
};

#endif // KPRANIMATIONSDOCKERFACTORY_H
