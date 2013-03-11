#ifndef FAKE_KSERVICETYPETRADER_H
#define FAKE_KSERVICETYPETRADER_H

#include <QObject>
#include <QDebug>

#include <kurl.h>
#include <kservice.h>

// const KService::List offers = KServiceTypeTrader::self()->query(serviceType, query);
// KService::List plugins;

class KServiceTypeTrader : public QObject
{
public:
    static KServiceTypeTrader* self()
    {
        static KServiceTypeTrader* s_instance = 0;
        if (!s_instance)
            s_instance = new KServiceTypeTrader();
        return s_instance;
    }
    
#if 0
    /**
     * The main function in the KServiceTypeTrader class.
     *
     * It will return a list of services that match your
     * specifications.  The only required parameter is the service
     * type.  This is something like 'text/plain' or 'text/html'.  The
     * constraint parameter is used to limit the possible choices
     * returned based on the constraints you give it.
     *
     * The @p constraint language is rather full.  The most common
     * keywords are AND, OR, NOT, IN, and EXIST, all used in an
     * almost spoken-word form.  An example is:
     * \code
     * (Type == 'Service') and (('KParts/ReadOnlyPart' in ServiceTypes) or (exist Exec))
     * \endcode
     *
     * The keys used in the query (Type, ServiceType, Exec) are all
     * fields found in the .desktop files.
     *
     * @param servicetype A service type like 'KMyApp/Plugin' or 'KFilePlugin'.
     * @param constraint  A constraint to limit the choices returned, QString() to
     *                    get all services of the given @p servicetype
     *
     * @return A list of services that satisfy the query
     * @see http://techbase.kde.org/Development/Tutorials/Services/Traders#The_KTrader_Query_Language
     */
#endif

    KService::List query( const QString& servicetype, const QString& constraint = QString() ) const
    {
        qWarning() << Q_FUNC_INFO << "TOFO";
        return KService::List();
    }

#if 0
    /**
     * Returns all offers associated with a given servicetype, IGNORING the
     * user preference. The sorting will be the one coming from the InitialPreference
     * in the .desktop files, and services disabled by the user will still be listed here.
     * This is used for "Revert to defaults" buttons in GUIs.
     */
    KService::List defaultOffers( const QString& serviceType,
                                  const QString& constraint = QString() ) const;
    /**
     * Returns the preferred service for @p serviceType.
     *
     * @param serviceType the service type (e.g. "KMyApp/Plugin")
     * @return the preferred service, or 0 if no service is available
     */
    KService::Ptr preferredService( const QString & serviceType ) const;

    /**
     * This is a static pointer to the KServiceTypeTrader singleton.
     *
     * You will need to use this to access the KServiceTypeTrader functionality since the
     * constructors are protected.
     *
     * @return Static KServiceTypeTrader instance
     */
    static KServiceTypeTrader* self();

    /**
     * Get a plugin from a trader query
     *
     * Example:
     * \code
     * KMyAppPlugin* plugin = KServiceTypeTrader::createInstanceFromQuery<KMyAppPlugin>( serviceType, QString(), parentObject );
     * if ( plugin ) {
     *     ....
     * }
     * \endcode
     *
     * @param serviceType the type of service for which to find a plugin
     * @param constraint an optional constraint to pass to the trader (see KTrader)
     * @param parent the parent object for the part itself
     * @param args A list of arguments passed to the service component
     * @param error The string passed here will contain an error description.
     * @return A pointer to the newly created object or a null pointer if the
     *         factory was unable to create an object of the given type.
     */
    template <class T>
    static T *createInstanceFromQuery(const QString &serviceType,
            const QString &constraint = QString(), QObject *parent = 0,
            const QVariantList &args = QVariantList(), QString *error = 0)
    {
        return createInstanceFromQuery<T>(serviceType, 0, parent, constraint, args, error);
    }

    /**
     * Get a plugin from a trader query
     *
     * This method works like
     * createInstanceFromQuery(const QString&, const QString&, QObject*, const QVariantList&, QString*),
     * but you can specify an additional parent widget.  This is important for
     * a KPart, for example.
     *
     * @param serviceType the type of service for which to find a plugin
     * @param parentWidget the parent widget for the plugin
     * @param parent the parent object for the part itself
     * @param constraint an optional constraint to pass to the trader (see KTrader)
     * @param args A list of arguments passed to the service component
     * @param error The string passed here will contain an error description.
     * @return A pointer to the newly created object or a null pointer if the
     *         factory was unable to create an object of the given type.
     */
    template <class T>
    static T *createInstanceFromQuery(const QString &serviceType,
            QWidget *parentWidget, QObject *parent, const QString &constraint = QString(),
            const QVariantList &args = QVariantList(), QString *error = 0)
    {
        const KService::List offers = self()->query(serviceType, constraint);
        Q_FOREACH (const KService::Ptr &ptr, offers) {
            T *component = ptr->template createInstance<T>(parentWidget, parent, args, error);
            if (component) {
                if (error)
                    error->clear();
                return component;
            }
        }
        if (error)
            *error = i18n("No service matching the requirements was found");
        return 0;
    }

    /**
     * @deprecated Use
     * createInstanceFromQuery(const QString&, const QString&, QObject*, const QVariantList&, QString*)
     * instead
     */
#ifndef KDE_NO_DEPRECATED
    template <class T>
    static KDE_DEPRECATED T *createInstanceFromQuery(const QString &serviceType, const QString &constraint,
            QObject *parent, const QStringList &args, int *error = 0)
    {
        const KService::List offers = KServiceTypeTrader::self()->query(serviceType, constraint);
        if (offers.isEmpty()) {
            if (error) {
                *error = KLibLoader::ErrNoServiceFound;
            }
            return 0;
        }

        return KService::createInstance<T>(offers.begin(), offers.end(), parent, args, error);
    }
#endif


    /**
     * @internal  (public for KMimeTypeTrader)
     */
    static void applyConstraints( KService::List& lst,
                                  const QString& constraint );


#endif
};

#endif
