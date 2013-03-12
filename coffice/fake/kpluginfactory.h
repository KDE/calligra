#ifndef FAKE_KLUGINFACTORY_H
#define FAKE_KLUGINFACTORY_H

#include <QObject>
#include <QPluginLoader>
#include <qplugin.h>
#include <QDebug>
#include <kcomponentdata.h>
#include <kpluginfactory.h>
//#include <kexportplugin.h>
#include <kglobal.h>

namespace KParts { class Part; }

#define K_PLUGIN_FACTORY_WITH_BASEFACTORY(name, baseFactory, pluginRegistrations) \
    class name : public baseFactory { \
    public: \
        explicit name(const char *componentName = 0, const char *catalogName = 0, QObject *parent = 0) : baseFactory(componentName, catalogName, parent) { init(); } \
        explicit name(const KAboutData &aboutData, QObject *parent = 0) : baseFactory(aboutData, parent) { init(); } \
        static KComponentData componentData() { return KComponentData(); } \
    private: \
        void init() { pluginRegistrations } \
    };

#define K_PLUGIN_FACTORY(name, pluginRegistrations) K_PLUGIN_FACTORY_WITH_BASEFACTORY(name, KPluginFactory, pluginRegistrations)

#define K_EXPORT_PLUGIN(x) \
    namespace { \
        class RegisterPluginFactory { \
        public: \
            static QObject* createPluginInstance() { \
                QObject *factory = new x ; \
                return factory; \
            } \
            RegisterPluginFactory() { \
                qRegisterStaticPluginInstanceFunction( (QtPluginInstanceFunction) createPluginInstance ); \
            } \
        }; \
        RegisterPluginFactory registerPluginFactory; \
    }

#if 0
/**
 * \relates KPluginFactory
 * K_PLUGIN_FACTORY_DECLARATION declares the KPluginFactory subclass. This macro can be used in a
 * header file.
 *
 * \param name The name of the KPluginFactory derived class. This is the name you'll need for
 * K_EXPORT_PLUGIN
 *
 * \see K_PLUGIN_FACTORY
 * \see K_PLUGIN_FACTORY_DEFINITION
 */
#define K_PLUGIN_FACTORY_DECLARATION(name) K_PLUGIN_FACTORY_DECLARATION_WITH_BASEFACTORY(name, KPluginFactory)

/**
 * \relates KPluginFactory
 * K_PLUGIN_FACTORY_DEFINITION defines the KPluginFactory subclass. This macro can <b>not</b> be used in a
 * header file.
 *
 * \param name The name of the KPluginFactory derived class. This is the name you'll need for
 * K_EXPORT_PLUGIN
 *
 * \param pluginRegistrations This is code inserted into the constructors the class. You'll want to
 * call registerPlugin from there.
 *
 * \see K_PLUGIN_FACTORY
 * \see K_PLUGIN_FACTORY_DECLARATION
 */
#define K_PLUGIN_FACTORY_DEFINITION(name, pluginRegistrations) K_PLUGIN_FACTORY_DEFINITION_WITH_BASEFACTORY(name, KPluginFactory, pluginRegistrations)
#endif

class KPluginFactory : public QObject
{
    //Q_OBJECT
public:
    KPluginFactory(const char *componentName = 0, const char *catalogName = 0, QObject *parent = 0) : QObject(parent) {}
    KPluginFactory(const KAboutData &aboutData, QObject *parent = 0) : QObject(parent) {}

    KComponentData componentData() const { return m_componentData; }

    template<typename T>
    T *create(QObject *parent = 0, const QVariantList &args = QVariantList());

    template<typename T>
    T *create(const QString &keyword, QObject *parent = 0, const QVariantList &args = QVariantList());

    template<typename T>
    T *create(QWidget *parentWidget, QObject *parent, const QString &keyword = QString(), const QVariantList &args = QVariantList());

#if 0
Q_SIGNALS:
    void objectCreated(QObject *object);
#endif

protected:
    typedef QObject *(*CreateInstanceFunction)(QWidget *, QObject *, const QVariantList &);

private:
    class PluginIface {
    public:
        QString m_keyword;
        CreateInstanceFunction m_instanceFunction;
        PluginIface(const QString &keyword = QString(), CreateInstanceFunction instanceFunction = 0) : m_instanceFunction(instanceFunction) {}
        virtual QObject* create(const char *iface, QWidget *parentWidget, QObject *parent, const QVariantList &args, const QString &keyword) = 0;
    };

    template<class T>
    class PluginImpl : public PluginIface {
    public:
        PluginImpl(const QString &keyword = QString(), CreateInstanceFunction instanceFunction = 0)
            : PluginIface(keyword, instanceFunction) {}
        virtual QObject* create(const char *iface, QWidget *parentWidget, QObject *parent, const QVariantList &args, const QString &keyword) {
            if (m_instanceFunction)
                return m_instanceFunction(parentWidget, parent, args);
            return new T(parent, args);
        }
    };

    QHash<QString, PluginIface*> m_registeredPlugins;

protected:
    template<class T>
    void registerPlugin(const QString &keyword = QString(), CreateInstanceFunction instanceFunction = 0 ) {
        QString name = T::staticMetaObject.className();
        PluginIface *p = new PluginImpl<T>(keyword, instanceFunction);
        m_registeredPlugins.insert(name, p);
    }

    void setComponentData(const KComponentData &componentData) { m_componentData = componentData; }

    virtual void setupTranslations() {}

    virtual QObject *create(const char *iface, QWidget *parentWidget, QObject *parent, const QVariantList &args, const QString &keyword) {
        qDebug() << Q_FUNC_INFO << "iface=" << iface << "parentWidget=" << parentWidget << "parent=" << parent << "args=" << args << "keyword=" << keyword;
//         QHash<QString, RegisteredPlugin>::ConstIterator it = m_registeredPlugins.constFind(keyword);
//         if (it != m_registeredPlugins.constEnd()) {
//             if (it->instanceFunction)
//                 return it->instanceFunction(parentWidget, parent, args);
//             Q_ASSERT(it->metaObject);
//             it->metaObject->
//         }

        QHash<QString, PluginIface*>::ConstIterator it =  m_registeredPlugins.constFind(keyword);
        if (it != m_registeredPlugins.constEnd()) {
            return it.value()->create(iface, parentWidget, parent, args, keyword);
        }
        return 0;
    }

#if 0
    template<class impl, class ParentType>
    static QObject *createInstance(QWidget *parentWidget, QObject *parent, const QVariantList &args)
    {
        Q_UNUSED(parentWidget);
        ParentType *p = 0;
        if (parent) {
            p = qobject_cast<ParentType *>(parent);
            Q_ASSERT(p);
        }
        return new impl(p, args);
    }

    template<class impl>
    static QObject *createPartInstance(QWidget *parentWidget, QObject *parent, const QVariantList &args)
    {
        return new impl(parentWidget, parent, args);
    }

    template<class impl>
    struct InheritanceChecker
    {
        CreateInstanceFunction createInstanceFunction(KParts::Part *) { return &createPartInstance<impl>; }
        CreateInstanceFunction createInstanceFunction(QWidget *) { return &createInstance<impl, QWidget>; }
        CreateInstanceFunction createInstanceFunction(...) { return &createInstance<impl, QObject>; }
    };
#endif

private:
    KComponentData m_componentData;
};

typedef KPluginFactory KLibFactory;

template<typename T>
inline T *KPluginFactory::create(QObject *parent, const QVariantList &args)
{
    QObject *o = create(T::staticMetaObject.className(), parent && parent->isWidgetType() ? reinterpret_cast<QWidget *>(parent): 0, parent, args, QString());
    T *t = qobject_cast<T *>(o);
    if (!t) {
        delete o;
    }
    return t;
}

template<typename T>
inline T *KPluginFactory::create(const QString &keyword, QObject *parent, const QVariantList &args)
{
    QObject *o = create(T::staticMetaObject.className(), parent && parent->isWidgetType() ? reinterpret_cast<QWidget *>(parent): 0, parent, args, keyword);
    T *t = qobject_cast<T *>(o);
    if (!t) {
        delete o;
    }
    return t;
}

template<typename T>
inline T *KPluginFactory::create(QWidget *parentWidget, QObject *parent, const QString &keyword, const QVariantList &args)
{
    QObject *o = create(T::staticMetaObject.className(), parentWidget, parent, args, keyword);
    T *t = qobject_cast<T *>(o);
    if (!t) {
        delete o;
    }
    return t;
}

#endif
 
