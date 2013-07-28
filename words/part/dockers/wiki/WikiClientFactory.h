#ifndef WIKICLIENTFACTORY_H
#define WIKICLIENTFACTORY_H

// Base classes
#include <QDockWidget>

#include <KoDockFactoryBase.h>

class WikiClientFactory : public KoDockFactoryBase
{
public:
    WikiClientFactory();
    virtual QString id() const;
    virtual QDockWidget* createDockWidget();
    DockPosition defaultDockPosition() const {
        return DockMinimized;
    }
};

#endif // WIKICLIENTFACTORY_H
