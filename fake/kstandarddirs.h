#ifndef FAKE_KSTANDARDDIRS_H
#define FAKE_KSTANDARDDIRS_H

#include <kglobal.h>
#include <kcomponentdata.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDebug>

class KStandardDirs
{
public:

    enum SearchOption { NoSearchOptions = 0,
                        Recursive = 1,
                        NoDuplicates = 2,
                        IgnoreExecBit = 4 };
    Q_DECLARE_FLAGS( SearchOptions, SearchOption )

    static QList<QStandardPaths::StandardLocation> locations(const QByteArray &type) {
        QList<QStandardPaths::StandardLocation> r;
        if (type == "data") {
            r << QStandardPaths::DataLocation;
            r << QStandardPaths::GenericDataLocation;
        } else if (type == "apps"|| type == "xdgdata-apps") {
            r << QStandardPaths::ApplicationsLocation;
        } else if (type == "config") {
            r << QStandardPaths::ConfigLocation;
        } else if (type == "pixmap" || type == "xdgdata-pixmap" || type == "xdgdata-icon") {
            r << QStandardPaths::PicturesLocation;
        } else if (type == "data") {
            r << QStandardPaths::DataLocation;
        }
        return r;
    }

    KStandardDirs() {}

    void addPrefix( const QString& dir ) {}

#if 0
    /**
     * Adds another search dir to front of the @c XDG_CONFIG_XXX list
     * of prefixes.
     * This prefix is only used for resources that start with @c "xdgconf-"
     *
     * @param dir The directory to append relative paths to.
     */
    void addXdgConfigPrefix( const QString& dir );

    /**
     * Adds another search dir to front of the @c XDG_DATA_XXX list
     * of prefixes.
     * This prefix is only used for resources that start with @c "xdgdata-"
     *
     * @param dir The directory to append relative paths to.
     */
    void addXdgDataPrefix( const QString& dir );

    /**
     * Adds suffixes for types.
     *
     * You may add as many as you need, but it is advised that there
     * is exactly one to make writing definite.
     * All basic types are added by addKDEDefaults(),
     * but for those you can add more relative paths as well.
     *
     * The later a suffix is added, the higher its priority. Note, that the
     * suffix should end with / but doesn't have to start with one (as prefixes
     * should end with one). So adding a suffix for app_pics would look
     * like KGlobal::dirs()->addResourceType("app_pics", "data" ,"app/pics");
     *
     * @param type Specifies a short descriptive string to access
     * files of this type.
     * @param relativename Specifies a directory relative to the root
     * of the KFSSTND.
     * @param priority if true, the directory is added before any other,
     * otherwise after
     * @return true if successful, false otherwise.
     *
     * @deprecated, use addResourceType(type, 0, relativename, priority)
     */
    //KDE_DEPRECATED bool addResourceType( const char *type,
    //                                    const QString& relativename, bool priority = true );

    /**
     * Adds suffixes for types.
     *
     * You may add as many as you need, but it is advised that there
     * is exactly one to make writing definite.
     * All basic types are added by addKDEDefaults(),
     * but for those you can add more relative paths as well.
     *
     * The later a suffix is added, the higher its priority. Note, that the
     * suffix should end with / but doesn't have to start with one (as prefixes
     * should end with one). So adding a suffix for app_pics would look
     * like KGlobal::dirs()->addResourceType("app_pics", "data", "app/pics");
     *
     * @param type Specifies a short descriptive string to access
     * files of this type.
     * @param basetype Specifies an already known type, or 0 if none
     * @param relativename Specifies a directory relative to the basetype
     * @param priority if true, the directory is added before any other,
     * otherwise after
     * @return true if successful, false otherwise.
     */
#endif

    bool addResourceType( const char *type, const char *basetype,
                          const QString& relativename, bool priority = true )
    {
        return true;
    }

    /// @internal - just to avoid unwanted overload
    bool addResourceType( const char *type, const char *basetype,
                          const char* relativename, bool priority = true )
    {
        return addResourceType(type, basetype, QLatin1String(relativename), priority);
    }

    bool addResourceDir( const char *type, const QString& absdir, bool priority = true )
    {
        return true;
    }

#if 0
    /**
     * Tries to find a resource in the following order:
     * @li All PREFIX/\<relativename> paths (most recent first).
     * @li All absolute paths (most recent first).
     *
     * The filename should be a filename relative to the base dir
     * for resources. So is a way to get the path to libkdecore.la
     * to findResource("lib", "libkdecore.la"). KStandardDirs will
     * then look into the subdir lib of all elements of all prefixes
     * ($KDEDIRS) for a file libkdecore.la and return the path to
     * the first one it finds (e.g. /opt/kde/lib/libkdecore.la).
     * You can use the program kde4-config to list all resource types:
     * @code
     * $ kde4-config --types
     * @endcode
     *
     * Example:
     * @code
     * QString iconfilename=KGlobal::dirs()->findResource("icon",QString("oxygen/22x22/apps/ktip.png"));
     * @endcode
     *
     * @param type The type of the wanted resource
     * @param filename A relative filename of the resource.
     *
     * @return A full path to the filename specified in the second
     *         argument, or QString() if not found.
     */
#endif

    QString findResource( const char *type, const QString& filename ) const
    {
        QString r;
        Q_FOREACH(QStandardPaths::StandardLocation l, locations(type)) {
            Q_FOREACH(const QString &dir, QStandardPaths::standardLocations(l)) {
                QFileInfo fi(dir, filename);
                if (r.isEmpty() && fi.isFile() && fi.exists())
                    r = fi.absoluteFilePath();
            }
        }
        qDebug() << Q_FUNC_INFO << "type=" << type << "filename=" << filename << "result=" << r;
        return r;
    }

#if 0
    /**
     * Checks whether a resource is restricted as part of the KIOSK
     * framework. When a resource is restricted it means that user-
     * specific files in the resource are ignored.
     *
     * E.g. by restricting the @c "wallpaper" resource, only system-wide
     * installed wallpapers will be found by this class. Wallpapers
     * installed under the @c $KDEHOME directory will be ignored.
     *
     * @param type The type of the resource to check
     * @param relPath A relative path in the resource.
     *
     * @return True if the resource is restricted.
     */
    bool isRestrictedResource( const char *type,
                               const QString& relPath=QString() ) const;

    /**
     * Returns a number that identifies this version of the resource.
     * When a change is made to the resource this number will change.
     *
     * @param type The type of the wanted resource
     * @param filename A relative filename of the resource.
     * @param options If the flags includes Recursive,
     *                all resources are taken into account
     *                otherwise only the one returned by findResource().
     *
     * @return A number identifying the current version of the
     *          resource.
     */
    quint32 calcResourceHash( const char *type,
                              const QString& filename,
                              SearchOptions options = NoSearchOptions) const;
#endif

    /**
     * Tries to find all directories whose names consist of the
     * specified type and a relative path. So
     * findDirs("apps", "Settings") would return
     * @li /home/joe/.kde/share/applnk/Settings/
     * @li /opt/kde/share/applnk/Settings/
     *
     * (from the most local to the most global)
     *
     * Note that it appends @c / to the end of the directories,
     * so you can use this right away as directory names.
     *
     * @param type The type of the base directory.
     * @param reldir Relative directory.
     *
     * @return A list of matching directories, or an empty
     *         list if the resource specified is not found.
     */
    QStringList findDirs( const char *type, const QString& reldir = QString() ) const
    {
        QStringList r;
        Q_FOREACH(QStandardPaths::StandardLocation l, locations(type)) {
            Q_FOREACH(const QString &dir, QStandardPaths::standardLocations(l)) {
                QFileInfo fi(dir, reldir);
                if (fi.isDir())
                    r.append(fi.absolutePath());
            }
        }
        qDebug() << Q_FUNC_INFO << "type=" << type << "reldir=" << reldir << "result=" << r;
        return r;
    }

#if 0
    /**
     * Tries to find the directory the file is in.
     * It works the same as findResource(), but it doesn't
     * return the filename but the name of the directory.
     *
     * This way the application can access a couple of files
     * that have been installed into the same directory without
     * having to look for each file.
     *
     * findResourceDir("lib", "libkdecore.la") would return the
     * path of the subdir libkdecore.la is found first in
     * (e.g. /opt/kde/lib/)
     *
     * @param type The type of the wanted resource
     * @param filename A relative filename of the resource.
     * @return The directory where the file specified in the second
     *         argument is located, or QString() if the type
     *         of resource specified is unknown or the resource
     *         cannot be found.
     */
    QString findResourceDir( const char *type,
                             const QString& filename) const;
#endif

    QStringList findAllResources( const char *type, const QString& filter = QString(), SearchOptions options = NoSearchOptions ) const
    {
        return findDirs(type);
    }

    QStringList findAllResources( const char *type, const QString& filter, int options ) const {
        return findDirs(type);
    }

    QStringList findAllResources( const char *type, const QString& filter, SearchOptions options, QStringList &relPaths) const
    {
        QStringList r;
        Q_FOREACH(const QString &relPath, relPaths)
            r << findDirs(type, relPath);
        return r;
    }

#if 0
    /**
     * Returns a QStringList list of pathnames in the system path.
     *
     * @param pstr  The path which will be searched. If this is
     *      null (default), the @c $PATH environment variable will
     *      be searched.
     *
     * @return a QStringList list of pathnames in the system path.
     */
    static QStringList systemPaths( const QString& pstr=QString() );

    /**
     * Finds the executable in the system path.
     *
     * A valid executable must
     * be a file and have its executable bit set.
     *
     * @param appname The name of the executable file for which to search.
     *                if this contains a path separator, it will be resolved
     *                according to the current working directory
     *                (shell-like behaviour).
     * @param pathstr The path which will be searched. If this is
     *                null (default), the @c $PATH environment variable will
     *                be searched.
     * @param options if the flags passed include IgnoreExecBit the path returned
     *                may not have the executable bit set.
     *
     * @return The path of the executable. If it was not found,
     *         it will return QString().
     * @see findAllExe()
     */
    static QString findExe( const QString& appname,
                            const QString& pathstr = QString(),
                            SearchOptions options = NoSearchOptions );

    /**
     * Finds all occurrences of an executable in the system path.
     *
     * @param list will be filled with the pathnames of all the
     *             executables found. Will be empty if the executable
     *             was not found.
     * @param appname the name of the executable for which to
     *                search.
     * @param pathstr the path list which will be searched. If this
     *                is 0 (default), the @c $PATH environment variable will
     *                be searched.
     * @param options if the flags passed include IgnoreExecBit the path returned
     *                may not have the executable bit set.
     *
     * @return The number of executables found, 0 if none were found.
     *
     * @see findExe()
     */
    static int findAllExe( QStringList& list, const QString& appname,
                           const QString& pathstr=QString(),
                           SearchOptions options = NoSearchOptions );

    /**
     * Reads customized entries out of the given config object and add
     * them via addResourceDirs().
     *
     * @param config The object the entries are read from. This should
     *        contain global config files
     * @return @c true if new config paths have been added
     * from @p config.
     **/
    bool addCustomized(KConfig *config);

    /**
     * This function is used internally by almost all other function as
     * it serves and fills the directories cache.
     *
     * @param type The type of resource
     * @return The list of possible directories for the specified @p type.
     * The function updates the cache if possible.  If the resource
     * type specified is unknown, it will return an empty list.
     * Note, that the directories are assured to exist beside the save
     * location, which may not exist, but is returned anyway.
     */
#endif

    QStringList resourceDirs(const char *type) const
    {
        QDir dir = QDir::home();
        dir.cd(qApp->applicationName());
        qDebug() << Q_FUNC_INFO << "type=" << type << "result=" << dir.absolutePath();
        return QStringList() << dir.absolutePath();
    }

#if 0
    /**
     * This function will return a list of all the types that KStandardDirs
     * supports.
     *
     * @return All types that KDE supports
     */
    QStringList allTypes() const;
#endif

    QString saveLocation(const char *type, const QString& suffix = QString(), bool create = true) const
    {
        QDir dir = QDir::home();
        bool exists = dir.cd(qApp->applicationName());
        qDebug() << Q_FUNC_INFO << "type=" << type << "exists=" << exists << "result=" << dir.absolutePath();
        if (create && !exists)
            if (!dir.mkpath(dir.absolutePath()))
                qWarning() << Q_FUNC_INFO << "Failed to create directory=" << dir.absolutePath();
        return dir.absolutePath();
    }

#if 0
    /**
     * Converts an absolute path to a path relative to a certain
     * resource.
     *
     * If "abs = ::locate(resource, rel)"
     * then "rel = relativeLocation(resource, abs)" and vice versa.
     *
     * @param type The type of resource.
     *
     * @param absPath An absolute path to make relative.
     *
     * @return A relative path relative to resource @p type that
     * will find @p absPath. If no such relative path exists, @p absPath
     * will be returned unchanged.
     */
    QString relativeLocation(const char *type, const QString &absPath);
#endif

    static bool makeDir(const QString& directory, int mode = 0755)
    {
        QDir dir(directory);
        return dir.mkpath(dir.absolutePath());
    }

#if 0
    /**
     * This returns a default relative path for the standard KDE
     * resource types. Below is a list of them so you get an idea
     * of what this is all about.
     *
     * @li @c data - @c share/apps
     * @li @c html - @c share/doc/HTML
     * @li @c icon - @c share/icon
     * @li @c config - @c share/config
     * @li @c pixmap - @c share/pixmaps
     * @li @c apps - @c share/applnk
     * @li @c sound - @c share/sounds
     * @li @c locale - @c share/locale
     * @li @c services - @c share/kde4/services
     * @li @c servicetypes - @c share/kde4/servicetypes
     * @li @c mime - @c share/mimelnk
     * @li @c cgi - @c cgi-bin
     * @li @c wallpaper - @c share/wallpapers
     * @li @c templates - @c share/templates
     * @li @c exe - @c bin
     * @li @c lib - @c lib[suffix]
     * @li @c module - @c lib[suffix]/kde4
     * @li @c qtplugins - @c lib[suffix]/kde4/plugins
     * @li @c kcfg - @c share/config.kcfg
     * @li @c emoticons - @c share/emoticons
     * @li @c xdgdata-apps - @c applications
     * @li @c xdgdata-icon - @c icons
     * @li @c xdgdata-pixmap - @c pixmaps
     * @li @c xdgdata-dirs - @c desktop-directories
     * @li @c xdgdata-mime - @c mime
     * @li @c xdgconf-menu - @c menus
     *
     * @returns Static default for the specified resource.  You
     *          should probably be using locate() or locateLocal()
     *          instead.
     * @see locate()
     * @see locateLocal()
     */
#ifndef KDE_NO_DEPRECATED
    static KDE_DEPRECATED QString kde_default(const char *type);
#endif

    /**
     * @internal (for use by sycoca only)
     */
    QString kfsstnd_prefixes();

    /**
     * @internal (for use by sycoca only)
     */
    QString kfsstnd_xdg_conf_prefixes();

    /**
     * @internal (for use by sycoca only)
     */
    QString kfsstnd_xdg_data_prefixes();

    /**
     * Returns the toplevel directory in which KStandardDirs
     * will store things. Most likely <tt>$HOME/.kde</tt>.
     * Don't use this function if you can use locateLocal()
     * @return the toplevel directory
     */
    QString localkdedir() const;

    /**
     * @return @c $XDG_DATA_HOME
     * See also http://www.freedesktop.org/standards/basedir/draft/basedir-spec/basedir-spec.html
     */
    QString localxdgdatadir() const;

    /**
     * @return @c $XDG_CONFIG_HOME
     * See also http://www.freedesktop.org/standards/basedir/draft/basedir-spec/basedir-spec.html
     */
    QString localxdgconfdir() const;

    /**
     * @return the path where type was installed to by kdelibs. This is an absolute path and only
     * one out of many search paths
     */
    static QString installPath(const char *type);

    /**
     * Checks for existence and accessability of a file or directory.
     * Faster than creating a QFileInfo first.
     * @param fullPath the path to check. IMPORTANT: must end with a slash if expected to be a directory
     *                 (and no slash for a file, obviously).
     * @return @c true if the directory exists, @c false otherwise
     */
    static bool exists(const QString &fullPath);

    /**
     * Expands all symbolic links and resolves references to
     * '/./', '/../' and extra  '/' characters in @p dirname
     * and returns the canonicalized absolute pathname.
     * The resulting path will have no symbolic link, '/./'
     * or '/../' components.
     */
    static QString realPath(const QString &dirname);

    /**
     * Expands all symbolic links and resolves references to
     * '/./', '/../' and extra  '/' characters in @p filename
     * and returns the canonicalized absolute pathname.
     * The resulting path will have no symbolic link, '/./'
     * or '/../' components.
     */
    static QString realFilePath(const QString &filename);
#endif

    /**
     * This function is just for convenience. It simply calls
     * instance->dirs()->\link KStandardDirs::findResource() findResource\endlink(type, filename).
     *
     * @param type   The type of the wanted resource, see KStandardDirs
     * @param filename   A relative filename of the resource
     * @param cData   The KComponentData object
     *
     * @return A full path to the filename specified in the second
     *         argument, or QString() if not found
     **/
    static QString locate( const char *type, const QString& filename, const KComponentData &cData = KGlobal::mainComponent() )
    {
        qDebug() << Q_FUNC_INFO << type << filename;
        //Q_ASSERT(false);
        return QString();
    }

    /**
     * This function is much like locate. However it returns a
     * filename suitable for writing to. No check is made if the
     * specified @p filename actually exists. Missing directories
     * are created. If @p filename is only a directory, without a
     * specific file, @p filename must have a trailing slash.
     *
     * @param type   The type of the wanted resource, see KStandardDirs
     * @param filename   A relative filename of the resource
     * @param cData   The KComponentData object
     *
     * @return A full path to the filename specified in the second
     *         argument, or QString() if not found
     **/
    static QString locateLocal( const char *type, const QString& filename, const KComponentData &cData = KGlobal::mainComponent() )
    {
        qWarning() << Q_FUNC_INFO << "TODO" << type << filename;
        return QString();
    }

    static QString locateLocal( const char *type, const QString& filename, bool createDir, const KComponentData &cData = KGlobal::mainComponent() )
    {
        return locateLocal(type, filename, cData);
    }

#if 0
    /**
     * Check, if a file may be accessed in a given mode.
     * This is a wrapper around the access() system call.
     * checkAccess() calls access() with the given parameters.
     * If this is OK, checkAccess() returns true. If not, and W_OK
     * is part of mode, it is checked if there is write access to
     * the directory. If yes, checkAccess() returns true.
     * In all other cases checkAccess() returns false.
     *
     * Other than access() this function EXPLICITLY ignores non-existent
     * files if checking for write access.
     *
     * @param pathname The full path of the file you want to test
     * @param mode     The access mode, as in the access() system call.
     * @return Whether the access is allowed, true = Access allowed
     */
    static bool checkAccess(const QString& pathname, int mode);
#endif
};

#if 0
Q_DECLARE_OPERATORS_FOR_FLAGS(KStandardDirs::SearchOptions)
#endif

#endif
