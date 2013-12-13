#ifndef FAKE_KLIBRARY_H
#define FAKE_KLIBRARY_H

#include <QObject>
#include <QLibrary>

#include <kcomponentdata.h>

class KLibrary : public QLibrary
{
public:
    typedef void (*void_function_ptr) ();
    KLibrary(QObject *parent = 0) : QLibrary(parent) {}
    KLibrary(const QString &name, const KComponentData &cData = KGlobal::mainComponent(), QObject *parent = 0) : QLibrary(name, parent) {Q_UNUSED(cData);}
    KLibrary(const QString &name, int verNum, const KComponentData &cData = KGlobal::mainComponent(), QObject *parent = 0) : QLibrary(name, verNum, parent) {Q_UNUSED(cData);}
#if 0
    /**
     * Returns the factory of the library.
     * @param factoryname The postfix to the init_ symbol used to create the
     * factory object. It corresponds to the first parameter to
     * K_EXPORT_COMPONENT_FACTORY.
     * @return The factory of the library if there is any, otherwise 0
     * @deprecated use KPluginLoader::factory
     */
    KDE_DEPRECATED KPluginFactory* factory( const char* factoryname = 0 );
#endif

    void *resolveSymbol(const char */*name*/) { return 0; /*return resolve(name);*/ }

    void_function_ptr resolveFunction(const char *name) {
#if QT_VERSION < 0x050000
        void*
#else
        QFunctionPointer
#endif
            psym = resolve(name);
        if (!psym) return 0;
        ptrdiff_t tmp = reinterpret_cast<ptrdiff_t>(psym);
        return reinterpret_cast<void_function_ptr>(tmp);
    }

#if 0
    void setFileName(const QString &name, const KComponentData &data = KGlobal::mainComponent());
    bool unload() { return false; } //this is only temporary. i will remove it as soon as I have removed all dangerous users of it
#endif
};

class KLibFactory : public QObject
{
    Q_OBJECT
public:
    KLibFactory(QObject *parent = 0) : QObject(parent) {}
    virtual ~KLibFactory() {}
    template<typename T> QObject* create(QObject *parent = 0) {
        qDebug() << Q_FUNC_INFO << "TODO";
        return 0;
    }
};

#endif
