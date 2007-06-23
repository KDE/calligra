#ifndef PLUGINHELPERACTION_H
#define PLUGINHELPERACTION_H

#include <QAction>

class TextTool;
class QString;

class PluginHelperAction : public QAction {
    Q_OBJECT
public:
    PluginHelperAction(const QString &name, TextTool *tool, const QString &pluginId);

private slots:
    void executed();

private:
    TextTool *m_tool;
    QString m_pluginId;
};

#endif
