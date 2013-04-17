#ifndef FAKE_KCONFIG_H
#define FAKE_KCONFIG_H

#include <QVariant>
#include <QStringList>
#include <QSettings>
#include <QSharedPointer>
#include <QDebug>

#include <kconfigbase.h>
#include <ksharedptr.h>

#include "kofake_export.h"


class KConfig : public KConfigBase
{
public:

    enum OpenFlag {
        IncludeGlobals  = 0x01,
        CascadeConfig   = 0x02,
        SimpleConfig    = 0x00,
        NoCascade       = IncludeGlobals,
        NoGlobals       = CascadeConfig,
        FullConfig      = IncludeGlobals|CascadeConfig
    };
    Q_DECLARE_FLAGS(OpenFlags, OpenFlag)

    KConfig(const QString& file, OpenFlag = FullConfig, const char* resourceType = "config") : KConfigBase() { Q_UNUSED(file); Q_UNUSED(resourceType);}
    virtual ~KConfig() {}

#if 0
    /**
     * Creates a KConfig object to manipulate a configuration file for the
     * current application.
     *
     * If an absolute path is specified for @p file, that file will be used
     * as the store for the configuration settings.  If a non-absolute path
     * is provided, the file will be looked for in the standard directory
     * specified by resourceType.  If no path is provided, a default
     * configuration file will be used based on the name of the main
     * application component.
     *
     * @p mode determines whether the user or global settings will be allowed
     * to influence the values returned by this object.  See OpenFlags for
     * more details.
     *
     * @note You probably want to use KSharedConfig::openConfig instead.
     *
     * @param file         the name of the file. If an empty string is passed in
     *                     and SimpleConfig is passed in for the OpenFlags, then an in-memory
     *                     KConfig object is created which will not write out to file nor which
     *                     requires any file in the filesystem at all.
     * @param mode         how global settings should affect the configuration
     *                     options exposed by this KConfig object
     * @param resourceType The standard directory to look for the configuration
     *                     file in (see KStandardDirs)
     *
     * @sa KSharedConfig::openConfig(const QString&, OpenFlags, const char*)
     */
    explicit KConfig(const QString& file = QString(), OpenFlags mode = FullConfig,
                     const char* resourceType = "config");

    /**
     * Creates a KConfig object to manipulate the configuration for a specific
     * component.
     *
     * If an absolute path is specified for @p file, that file will be used
     * as the store for the configuration settings.  If a non-absolute path
     * is provided, the file will be looked for in the standard directory
     * specified by resourceType.  If no path is provided, a default
     * configuration file will be used based on the component's name.
     *
     * @p mode determines whether the user or global settings will be allowed
     * to influence the values returned by this object.  See KConfig::OpenFlags for
     * more details.
     *
     * @note You probably want to use KSharedConfig::openConfig instead.
     *
     * @param componentData the component that you wish to load a configuration
     *                      file for
     * @param file          overrides the configuration file name if not empty; if it is empty
     *                      and SimpleConfig is passed in for the OpenFlags, then an in-memory
     *                      KConfig object is created which will not write out to file nor which
     *                      requires any file in the filesystem at all.
     * @param mode          how global settings should affect the configuration
     *                      options exposed by this KConfig object.
     *                      See OpenFlags
     * @param resourceType  The standard directory to look for the configuration
     *                      file in
     *
     * @sa KSharedConfig::openConfig(const KComponentData&, const QString&, OpenFlags, const char*)
     */
    explicit KConfig(const KComponentData& componentData, const QString& file = QString(),
                     OpenFlags mode = FullConfig, const char* resourceType = "config");

    /**
     * @internal
     *
     * Creates a KConfig object using the specified backend. If the backend can not
     * be found or loaded, then the standard configuration parser is used as a fallback.
     *
     * @param file the file to be parsed
     * @param backend the backend to load
     * @param resourceType where to look for the file if an absolute path is not provided
     *
     * @since 4.1
     */
    KConfig(const QString& file, const QString& backend, const char* resourceType = "config");

    virtual ~KConfig();

    /**
     * Returns the component data this configuration is for.
     */
    const KComponentData &componentData() const; // krazy:exclude=constref

    /**
     * Returns the filename used to store the configuration.
     */
    QString name() const;

    /// @reimp
    void sync();

    /// @reimp
    void markAsClean();

    /// @{ configuration object state
    /// @reimp
    AccessMode accessMode() const;

    /**
     * Whether the configuration can be written to.
     *
     * If @p warnUser is true and the configuration cannot be
     * written to (ie: this method returns @c false), a warning
     * message box will be shown to the user telling them to
     * contact their system administrator to get the problem fixed.
     *
     * The most likely cause for this method returning @c false
     * is that the user does not have write permission for the
     * configuration file.
     *
     * @param warnUser whether to show a warning message to the user
     *                 if the configuration cannot be written to
     *
     * @returns true if the configuration can be written to, false
     *          if the configuration cannot be written to
     */
    bool isConfigWritable(bool warnUser);
    /// @}

    /**
     * Copies all entries from this config object to a new config
     * object that will save itself to @p file.
     *
     * The configuration will not actually be saved to @p file
     * until the returned object is destroyed, or sync() is called
     * on it.
     *
     * Do not forget to delete the returned KConfig object if
     * @p config was 0.
     *
     * @param file   the new config object will save itself to
     * @param config if not 0, copy to the given KConfig object rather
     *               than creating a new one
     *
     * @return @p config if it was set, otherwise a new KConfig object
     */
    KConfig* copyTo(const QString &file, KConfig *config = 0) const;

    /**
     * Ensures that the configuration file contains a certain update.
     *
     * If the configuration file does not contain the update @p id
     * as contained in @p updateFile, kconf_update is run to update
     * the configuration file.
     *
     * If you install config update files with critical fixes
     * you may wish to use this method to verify that a critical
     * update has indeed been performed to catch the case where
     * a user restores an old config file from backup that has
     * not been updated yet.
     *
     * @param id the update to check
     * @param updateFile the file containing the update
     */
    void checkUpdate(const QString &id, const QString &updateFile);
#endif

    /**
     * Updates the state of this object to match the persistent storage.
     */
    void reparseConfiguration() {}

#if 0
    /// @{ extra config files
    /**
     * Adds the list of configuration sources to the merge stack.
     *
     * Currently only files are accepted as configuration sources.
     *
     * The first entry in @p sources is treated as the most general and will
     * be overridden by the second entry.  The settings in the final entry
     * in @p sources will override all the other sources provided in the list.
     *
     * The settings in @p sources will also be overridden by the sources
     * provided by any previous calls to addConfigSources().
     *
     * The settings in the global configuration sources will be overridden by
     * the sources provided to this method (@see IncludeGlobals).
     * All the sources provided to any call to this method will be overridden
     * by any files that cascade from the source provided to the constructor
     * (@see CascadeConfig), which will in turn be
     * overridden by the source provided to the constructor (either explicitly
     * or implicity via a KComponentData).
     *
     * Note that only the most specific file, ie: the file provided to the
     * constructor, will be written to by this object.
     *
     * The state is automatically updated by this method, so there is no need to call
     * reparseConfiguration().
     *
     * @param sources A list of extra config sources.
     */
    void addConfigSources(const QStringList &sources);

    /// @}
    /// @{ locales
    /**
     * Returns the current locale.
     */
    QString locale() const;
    /**
     * Sets the locale to @p aLocale.
     *
     * The global locale is used by default.
     *
     * @note If set to the empty string, @b no locale will be matched. This effectively disables
     * reading translated entries.
     *
     * @return @c true if locale was changed, @c false if the call had no
     *         effect (eg: @p aLocale was already the current locale for this
     *         object)
     */
    bool setLocale(const QString& aLocale);
    /// @}

    /// @{ defaults
    /**
     * When set, all readEntry calls return the system-wide (default) values
     * instead of the user's settings.
     *
     * This is off by default.
     *
     * @param b whether to read the system-wide defaults instead of the
     *          user's settings
     */
    void setReadDefaults(bool b);
    /**
     * @returns @c true if the system-wide defaults will be read instead of the
     *          user's settings
     */
    bool readDefaults() const;
    /// @}

    /// @{ immutability
    /// @reimp
    bool isImmutable() const;
    /// @}

    /// @{ global
    /**
     * @deprecated
     *
     * Forces all following write-operations to be performed on @c kdeglobals,
     * independent of the @c Global flag in writeEntry().
     *
     * @param force true to force writing to kdeglobals
     * @see forceGlobal
     */
#ifndef KDE_NO_DEPRECATED
    KDE_DEPRECATED void setForceGlobal(bool force);
#endif
    /**
     * @deprecated
     *
     * Returns whether all entries are being written to @c kdeglobals.
     *
     * @return @c true if all entries are being written to @c kdeglobals
     * @see setForceGlobal
     * @deprecated
     */
#ifndef KDE_NO_DEPRECATED
    KDE_DEPRECATED bool forceGlobal() const;
#endif
    /// @}

    /// @reimp
    QStringList groupList() const;

    /**
     * Returns a map (tree) of entries in a particular group.
     *
     * The entries are all returned as strings.
     *
     * @param aGroup The group to get entries from.
     *
     * @return A map of entries in the group specified, indexed by key.
     *         The returned map may be empty if the group is empty, or not found.
     * @see   QMap
     */
    QMap<QString, QString> entryMap(const QString &aGroup=QString()) const;
#endif
    
};

#endif
