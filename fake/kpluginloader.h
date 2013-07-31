#ifndef FAKE_KPLUGINLOADER_H
#define FAKE_KPLUGINLOADER_H

#include <QPluginLoader>
#include <QObject>
#include <QDebug>
#include <klibloader.h>
#include <kpluginfactory.h>

class KPluginLoader : public QPluginLoader
{
public:
    KPluginLoader(const QString &plugin, const KComponentData &componentdata = KGlobal::mainComponent(), QObject *parent = 0) : QPluginLoader() {
        Q_UNUSED(plugin);
        Q_UNUSED(componentdata);
        Q_UNUSED(parent);
        qDebug() << Q_FUNC_INFO;
    }
    KPluginLoader(const KService &service, const KComponentData &componentdata = KGlobal::mainComponent(), QObject *parent = 0) : QPluginLoader() {
        Q_UNUSED(service);
        Q_UNUSED(componentdata);
        Q_UNUSED(parent);
        qDebug() << Q_FUNC_INFO;
    }
    ~KPluginLoader() {
        qDebug() << Q_FUNC_INFO;
    }
    KPluginFactory *factory() {
        qDebug() << Q_FUNC_INFO << "TODO";
        return 0;
    }
    QString pluginName() const {
        return QString();
    }
    quint32 pluginVersion() const {
        return 1;
    }
    QString errorString() const {
        return QString();
    }
    bool isLoaded() const {
        return true;
    }
};

#endif
