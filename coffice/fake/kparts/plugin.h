#ifndef FAKE_KPARTS_PLUGIN_H
#define FAKE_KPARTS_PLUGIN_H

#include <QObject>
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
        static void loadPlugins( QObject *parent, const KComponentData &instance ) {}
        static void loadPlugins( QObject *parent, const QList<PluginInfo> &pluginInfos ) {}
        static void loadPlugins( QObject *parent, const QList<PluginInfo> &pluginInfos, const KComponentData &instance ) {}
        static void loadPlugins(QObject *parent, KXMLGUIClient* parentGUIClient, const KComponentData &instance, bool enableNewPluginsByDefault = true, int interfaceVersionRequired = 0) {}
        static QList<Plugin *> pluginObjects( QObject *parent ) { return QList<Plugin *>(); }

    protected:
        static QList<Plugin::PluginInfo> pluginInfos(const KComponentData &instance) { return QList<Plugin::PluginInfo>(); }
        static Plugin* loadPlugin( QObject * parent, const QString &libname ) { return 0; }
        static Plugin* loadPlugin( QObject * parent, const QString &libname, const QString &keyword  ) { return 0; }
        virtual void setComponentData(const KComponentData &instance) {}
    };

}

#endif
 
