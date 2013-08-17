#ifndef FLOWFACTORYINIT_H
#define FLOWFACTORYINIT_H

#include <FlowFactory.h>

class FlowFactoryInit : public FlowFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID KPluginFactory_iid FILE "flowpart.json")
    Q_INTERFACES(KPluginFactory)

public:
    explicit FlowFactoryInit(QObject *parent = 0) : FlowFactory(parent) {}
    virtual ~FlowFactoryInit() {}

};

#endif // FLOWFACTORYINIT_H
