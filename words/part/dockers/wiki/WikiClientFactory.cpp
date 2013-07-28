#include "WikiClientFactory.h"
#include "WikiClientDocker.h"

WikiClientFactory::WikiClientFactory()
{
}

QString WikiClientFactory::id() const
{
    return QString("WikiClientDocker");
}

QDockWidget* WikiClientFactory::createDockWidget()
{
    WikiClientDocker *wc = new WikiClientDocker();
    wc->setObjectName(id());
    return wc;
}
