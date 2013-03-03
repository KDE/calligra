#include "GrammarCheckPlugin.h"
#include "GrammarCheckFactory.h"

#include <KDebug>
#include <KPluginFactory>
#include <KPluginLoader>

#include <KoTextEditingRegistry.h>

K_PLUGIN_FACTORY(GrammarCheckPluginFactory, registerPlugin<GrammarCheckPlugin>();)
K_EXPORT_PLUGIN(GrammarCheckPluginFactory("GrammarCheckPlugin"))

GrammarCheckPlugin::GrammarCheckPlugin(QObject *parent, const QVariantList&)
    : QObject(parent)
{
    kDebug(31000) << "grammar check instance created";
    KoTextEditingRegistry::instance()->add(new GrammarCheckFactory());
}

#include <GrammarCheckPlugin.moc>
