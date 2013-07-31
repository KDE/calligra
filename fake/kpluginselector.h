#ifndef FAKE_KPluginSelector_H
#define FAKE_KPluginSelector_H

#include <QWidget>
#include <QList>
#include <ksharedconfig.h>
#include <kplugininfo.h>

class KPluginSelector : public QWidget
{
public:
    enum PluginLoadMethod { ReadConfigFile = 0, IgnoreConfigFile };
    KPluginSelector(QWidget *parent = 0) : QWidget(parent) {}
    void addPlugins(const QString &componentName, const QString &categoryName = QString(), const QString &categoryKey = QString(), KSharedConfig::Ptr config = KSharedConfig::Ptr()) {}
    void addPlugins(const KComponentData &instance, const QString &categoryName = QString(), const QString &categoryKey = QString(), const KSharedConfig::Ptr &config = KSharedConfig::Ptr()) {}
    void addPlugins(const QList<KPluginInfo> &pluginInfoList, PluginLoadMethod pluginLoadMethod = ReadConfigFile, const QString &categoryName = QString(), const QString &categoryKey = QString(), const KSharedConfig::Ptr &config = KSharedConfig::Ptr()) {}
    void load() {}
    void save() {}
    void defaults() {}
    bool isDefault() const { return false; }
    void updatePluginsState() {}
};

#endif
