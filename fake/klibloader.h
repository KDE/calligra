#ifndef FAKE_KLIBLOADER_H
#define FAKE_KLIBLOADER_H

#include <QObject>
#include <QLibrary>
#include <QDebug>
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

    void_function_ptr resolveFunction(const char */*name*/) {
//        void *psym = resolve(name);
//        if (!psym) return 0;
//        ptrdiff_t tmp = reinterpret_cast<ptrdiff_t>(psym);
//        return reinterpret_cast<void_function_ptr>(tmp);
        return 0;
    }

#if 0
    void setFileName(const QString &name, const KComponentData &data = KGlobal::mainComponent());
    bool unload() { return false; } //this is only temporary. i will remove it as soon as I have removed all dangerous users of it
#endif
};

class KLibFactory
{
public:
    template<typename T> QObject* create(QObject *parent = 0) {
        qDebug() << Q_FUNC_INFO << "TODO";
        return 0;
    }
};

class KLibLoader : public QObject
{
public:
#if 0
    /**
     * Loads and initializes a library. Loading a library multiple times is
     * handled gracefully.
     *
     * This is a convenience function that returns the factory immediately
     * @param libname  This is the library name without extension. Usually that is something like
     *                 "libkspread". The function will then search for a file named
     *                 "libkspread.la" in the KDE library paths.
     *                 The *.la files are created by libtool and contain
     *                 important information especially about the libraries dependencies
     *                 on other shared libs. Loading a "libfoo.so" could not solve the
     *                 dependencies problem.
     *
     *                 You can, however, give a library name ending in ".so"
     *                 (or whatever is used on your platform), and the library
     *                 will be loaded without resolving dependencies. Use with caution.
     * @param loadHint provides more control over how the library is loaded
     * @return the KPluginFactory, or 0 if the library does not exist or it does
     *         not have a factory
     * @see library
     */
    KPluginFactory* factory( const QString &libname, QLibrary::LoadHints loadHint = 0);

    /**
     * Loads and initializes a library. Loading a library multiple times is
     * handled gracefully.
     *
     * @param libname  This is the library name without extension. Usually that is something like
     *                 "libkspread". The function will then search for a file named
     *                 "libkspread.la" in the KDE library paths.
     *                 The *.la files are created by libtool and contain
     *                 important information especially about the libraries dependencies
     *                 on other shared libs. Loading a "libfoo.so" could not solve the
     *                 dependencies problem.
     *
     *                 You can, however, give a library name ending in ".so"
     *                 (or whatever is used on your platform), and the library
     *                 will be loaded without resolving dependencies. Use with caution.
     * @param loadHint provides more control over how the library is loaded
     * @return KLibrary is invalid (0) when the library couldn't be dlopened. in such
     * a case you can retrieve the error message by calling KLibLoader::lastErrorMessage()
     *
     * @see factory
     */
    KLibrary* library( const QString &libname, QLibrary::LoadHints loadHint = 0 );

    /**
     * Returns an error message that can be useful to debug the problem.
     * Returns QString() if the last call to library() was successful.
     * You can call this function more than once. The error message is only
     * reset by a new call to library().
     * @return the last error message, or QString() if there was no error
     */
    QString lastErrorMessage() const;

    /**
     * Unloads the library with the given name.
     * @param libname  This is the library name without extension. Usually that is something like
     *                 "libkspread". The function will then search for a file named
     *                 "libkspread.la" in the KDE library paths.
     *                 The *.la files are created by libtool and contain
     *                 important information especially about the libraries dependencies
     *                 on other shared libs. Loading a "libfoo.so" could not solve the
     *                 dependencies problem.
     *
     *                 You can, however, give a library name ending in ".so"
     *                 (or whatever is used on your platform), and the library
     *                 will be loaded without resolving dependencies. Use with caution.
     */
    void unloadLibrary( const QString &libname );

    /**
     * Returns a pointer to the factory.
     *
     * Use this function to get an instance of KLibLoader.
     *
     * @return a pointer to the loader. If no loader exists until now
     *         then one is created.
     *
     * @deprecated use KPluginLoader instead
     */
    static KDE_DEPRECATED KLibLoader* self();

    /**
     * Helper method which looks for a library in the standard paths
     * ("module" and "lib" resources).
     * Made public for code that doesn't use KLibLoader itself, but still
     * wants to open modules.
     * @param libname of the library. If it is not a path, the function searches in
     *                the "module" and "lib" resources. If there is no extension,
     *                ".la" will be appended.
     * @param cData a KComponentData used to get the standard paths
     * @return the name of the library if it was found, an empty string otherwise
     */
    static QString findLibrary(const QString &libname, const KComponentData &cData = KGlobal::mainComponent());

    /**
     * This enum type defines the possible error cases that can happen
     * when loading a component.
     *
     * Use errorString() to convert the error code to a human-readable string
     */
    enum ComponentLoadingError {
        ErrNoLibrary = 1, /*< the specified library could not be loaded. Use KLibLoader::lastErrorMessage for details*/
        ErrNoFactory, /*< the library does not export a factory */
        ErrNoComponent, /*< the factory does not support creating components of the specified type */
        ErrServiceProvidesNoLibrary, /*< the specified service provides no shared library (when using KService) */
        ErrNoServiceFound /*< no service implementing the given servicetype and fullfilling the given constraint expression can be found (when using KServiceTypeTrader) */
    };

    /**
     * Converts a numerical error code into a human-readable error message
     *
     * @param componentLoadingError the error code, as set using the @p error
     *                              parameter of createInstance()
     * @return the translated error message describing the error represented
     *         by @p componentLoadingError
     *
     * @see ComponentLoadingError
     */
    static QString errorString( int componentLoadingError );


    /**
     * This template allows to load the specified library and ask the
     * factory to create an instance of the given template type.
     *
     * @param keyword the keyword for the plugin - see KPluginFactory::registerPlugin()
     * @param libname the library to open
     * @param parent the parent object (see QObject constructor)
     * @param args a list of string arguments, passed to the factory and possibly
     *             to the component (see KPluginFactory)
     * @param error if not null, the int will be set to 0 on success or one of the
     *              error codes defined by ComponentLoadingError if there was an error
     * @return a pointer to the newly created object or a null pointer if the
     *         factory was unable to create an object of the given type
     * @deprecated Use KService::createInstance() or KPluginLoader instead
     */
    template <typename T>
    static KDE_DEPRECATED T *createInstance(const QString &keyword, const QString &libname, QObject *parent = 0,
                              const QVariantList &args = QVariantList(),
                              int *error = 0 )
    {
        KLibrary *library = KLibLoader::self()->library( libname );
        if ( !library )
        {
            if ( error )
                *error = ErrNoLibrary;
            return 0;
        }
        KPluginFactory *factory = library->factory();
        if ( !factory )
        {
            library->unload();
            if ( error )
                *error = ErrNoFactory;
            return 0;
        }
        QObject *object = factory->template create<T>(keyword, parent, args);
        T *res = qobject_cast<T *>( object );
        if ( !res )
        {
            delete object;
            library->unload();
            if ( error )
                *error = ErrNoComponent;
        }
        return res;
    }

    /**
     * This template allows to load the specified library and ask the
     * factory to create an instance of the given template type.
     *
     * @param libname the library to open
     * @param parent the parent object (see QObject constructor)
     * @param args a list of string arguments, passed to the factory and possibly
     *             to the component (see KPluginFactory)
     * @param error if not null, the int will be set to 0 on success or one of the
     *              error codes defined by ComponentLoadingError if there was an error
     * @return a pointer to the newly created object or a null pointer if the
     *         factory was unable to create an object of the given type
     * @deprecated Use KService::createInstance() or KPluginLoader instead
     */
    template <typename T>
    static KDE_DEPRECATED T *createInstance( const QString &libname, QObject *parent = 0,
                              const QVariantList &args = QVariantList(),
                              int *error = 0 )
    {
        return createInstance<T>(QString(), libname, parent, args, error);
    }

    /**
     * @deprecated Use one of the other createInstance methods or
     *             KPluginLoader or KService::createInstance instead
     */
    template <typename T>
    static KDE_DEPRECATED T *createInstance( const QString &libname, QObject *parent,
                              const QStringList &args,
                              int *error = 0 )
    {
        KLibrary *library = KLibLoader::self()->library( libname );
        if ( !library )
        {
            if ( error )
                *error = ErrNoLibrary;
            return 0;
        }
        KPluginFactory *factory = library->factory();
        if ( !factory )
        {
            library->unload();
            if ( error )
                *error = ErrNoFactory;
            return 0;
        }
        QObject *object = factory->template create<T>(parent, args);
        T *res = qobject_cast<T *>( object );
        if ( !res )
        {
            delete object;
            library->unload();
            if ( error )
                *error = ErrNoComponent;
        }
        return res;
    }
#endif
};

#endif
