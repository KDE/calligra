#include "kpluginfactory.h"

KPluginFactory::KPluginFactory(const char *componentName, const char *catalogName, QObject *parent)
    : QObject(parent)
{
    setObjectName(componentName);
}

KPluginFactory::KPluginFactory(const KAboutData &aboutData, QObject *parent)
    : QObject(parent)
{
}

KComponentData KPluginFactory::componentData() const { return m_componentData; }

void KPluginFactory::setComponentData(const KComponentData &componentData) { m_componentData = componentData; }

void KPluginFactory::setupTranslations() {}

QObject *KPluginFactory::create(const char *iface, QWidget *parentWidget, QObject *parent, const QVariantList &args, const QString &keyword)
{
    qDebug() << Q_FUNC_INFO << "iface=" << iface << "parentWidget=" << parentWidget << "parent=" << parent << "args=" << args << "keyword=" << keyword;
    if (keyword.isEmpty()) {
        const bool hasOne = m_registeredPlugins.count() == 1;
        QObject *p = m_registeredPlugins.count() > 1 ? new QObject(parent) : 0;
        for(QHash<QString, PluginIface*>::ConstIterator it = m_registeredPlugins.constBegin(); it != m_registeredPlugins.constEnd(); ++it) {
            QObject *obj = it.value()->create(iface, parentWidget, p ? p : parent, args, keyword);
            Q_ASSERT(obj);
            if (!obj)
                continue;
            emit objectCreated(obj);
            if (hasOne)
                p = obj;
        }
        return p;
    } else {
        QHash<QString, PluginIface*>::ConstIterator it =  m_registeredPlugins.constFind(keyword);
        if (it != m_registeredPlugins.constEnd()) {
            QObject *obj = it.value()->create(iface, parentWidget, parent, args, keyword);
            if (obj)
                emit objectCreated(obj);
            return obj;
        }
        return 0;
    }
}
