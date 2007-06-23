#include "PluginHelperAction.h"

#include "TextTool.h"

PluginHelperAction::PluginHelperAction(const QString &name, TextTool *tool, const QString &pluginId)
    : QAction(name, tool),
    m_tool(tool),
    m_pluginId(pluginId)
{
}

void PluginHelperAction::executed() {
    m_tool->startTextEditingPlugin(m_pluginId);
}

#include <PluginHelperAction.moc>
