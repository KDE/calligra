#ifndef FAKE_KPARTS_PLUGIN_H
#define FAKE_KPARTS_PLUGIN_H

#include <QObject>
#include <QDebug>
#include <kparts/part.h>
#include <kxmlguiclient.h>

namespace KParts {

    class Plugin : public QObject, virtual public KXMLGUIClient
    {
    public:
        Plugin(QObject* parent = 0) {}
        
        struct PluginInfo
        {
            QString m_relXMLFileName;
            QString m_absXMLFileName;
            QDomDocument m_document;
        };
#if 0
        virtual QString xmlFile() const;
        virtual QString localXMLFile() const;
#endif
        static void loadPlugins( QObject *parent, const KComponentData &instance ) {
            qDebug() << Q_FUNC_INFO;
        }
        static void loadPlugins( QObject *parent, const QList<PluginInfo> &pluginInfos ) {
            qDebug() << Q_FUNC_INFO;
        }
        static void loadPlugins( QObject *parent, const QList<PluginInfo> &pluginInfos, const KComponentData &instance ) {
            qDebug() << Q_FUNC_INFO;
        }
        static void loadPlugins(QObject *parent, KXMLGUIClient* parentGUIClient, const KComponentData &instance, bool enableNewPluginsByDefault = true, int interfaceVersionRequired = 0) {
            qDebug() << Q_FUNC_INFO;
        }
        static QList<Plugin *> pluginObjects( QObject *parent ) {
            qDebug() << Q_FUNC_INFO;
            return QList<Plugin *>();
        }

    protected:
        static QList<Plugin::PluginInfo> pluginInfos(const KComponentData &instance) {
            qDebug() << Q_FUNC_INFO;
            return QList<Plugin::PluginInfo>();
        }
        static Plugin* loadPlugin( QObject * parent, const QString &libname ) {
            qDebug() << Q_FUNC_INFO;
            return 0;
        }
        static Plugin* loadPlugin( QObject * parent, const QString &libname, const QString &keyword  ) {
            qDebug() << Q_FUNC_INFO;
            return 0;
        }
        virtual void setComponentData(const KComponentData &instance) {}
    };

}

#endif
 
