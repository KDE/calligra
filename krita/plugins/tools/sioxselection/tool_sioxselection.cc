
#include <kpluginfactory.h>

#include <KoToolRegistry.h>

#include "tool_sioxselection.h"
#include "kis_tool_sioxselection.h"

K_PLUGIN_FACTORY(SioxSelectionPluginFactory, registerPlugin<SioxSelectionPlugin>();)
K_EXPORT_PLUGIN(SioxSelectionPluginFactory("krita"))

SioxSelectionPlugin::SioxSelectionPlugin(QObject *parent, const QVariantList &)
    : QObject(parent)
{
    KoToolRegistry * r = KoToolRegistry::instance();
    r->add(new KisToolSioxSelectionFactory(QStringList()));
}

SioxSelectionPlugin::~SioxSelectionPlugin()
{
}

#include "tool_sioxselection.moc"
