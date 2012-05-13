#include "KPrAnimationsDockerFactory.h"
#include "KPrAnimationsDocker.h"

KPrAnimationsDockerFactory::KPrAnimationsDockerFactory()
{
}

QString KPrAnimationsDockerFactory::id() const
{
    return QString("Shape Animations");
}

QDockWidget* KPrAnimationsDockerFactory::createDockWidget()
{
    KPrAnimationsDocker * widget = new KPrAnimationsDocker();
    widget->setObjectName( id() );
    return widget;
}

KoDockFactoryBase::DockPosition KPrAnimationsDockerFactory::defaultDockPosition() const
{
    return DockRight;
}
