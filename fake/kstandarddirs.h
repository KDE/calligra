#ifndef FAKE_KSTANDARDDIRS_H
#define FAKE_KSTANDARDDIRS_H

#include <kglobal.h>
#include <kcomponentdata.h>

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDebug>
#include <QCoreApplication>
#include <QMap>

class KStandardDirs
{
public:

    enum SearchOption { NoSearchOptions = 0,
                        Recursive = 1,
                        NoDuplicates = 2,
                        IgnoreExecBit = 4 };
    Q_DECLARE_FLAGS( SearchOptions, SearchOption )

    QMap<QString, QString> m_extraResourceDirs;

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

    void addPrefix(const QString& dir) {}

    bool addResourceType(const char *type, const char *basetype, const QString& relativename, bool priority = true)
    {
        Q_FOREACH(QStandardPaths::StandardLocation l, locations(basetype)) {
            Q_FOREACH(const QString &dirname, QStandardPaths::standardLocations(l)) {
                m_extraResourceDirs.insertMulti(type, dirname+'/'+relativename);
            }
        }
        return true;
    }

    /// @internal - just to avoid unwanted overload
    bool addResourceType(const char *type, const char *basetype, const char* relativename, bool priority = true)
    {
        return addResourceType(type, basetype, QLatin1String(relativename), priority);
    }

    bool addResourceDir(const char *type, const QString& absdir, bool priority = true)
    {
        return true;
    }

    QStringList resourceDirs(const char *type) const
    {
        QStringList r;

        // This hack is needed, as long as saveLocation does the same
        QDir dir = QDir::home();
        dir.cd(qApp->applicationName());
        r.append(dir.absolutePath());
        // end of hack

        Q_FOREACH(QStandardPaths::StandardLocation l, locations(type)) {
            r.append(QStandardPaths::standardLocations(l));
        }
        r.append(m_extraResourceDirs.values(type));
        return r;
    }

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
    QString findResource( const char *type, const QString& filename) const
    {
        Q_FOREACH(const QString &dir, resourceDirs(type)) {
            QFileInfo fi(dir, filename);
            if (fi.isFile() && fi.exists())
                return fi.absoluteFilePath();
        }
        return QString();
    }


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
    QStringList findDirs(const char *type, const QString& reldir = QString()) const
    {
        QStringList r;
        Q_FOREACH(const QString &dir, resourceDirs(type)) {
            QFileInfo fi(dir, reldir);
            if (fi.isDir())
                r.append(fi.absolutePath());
        }
        return r;
    }


    QStringList findAllResources(const char *type, const QString& filter = QString(), SearchOptions options = NoSearchOptions) const
    {
        QStringList r;
        Q_FOREACH(const QString &dir, resourceDirs(type)) {
            QFileInfo fi(dir, filter);
            if (fi.exists())
                r.append(fi.absoluteFilePath());
        }
        return r;
    }

    QStringList findAllResources(const char *type, const QString& filter, int options) const
    {
        return findAllResources(type, filter);
    }

    QStringList findAllResources(const char *type, const QString& filter, SearchOptions options, QStringList &relPaths) const
    {
        QStringList r;
        Q_FOREACH(const QString &relPath, relPaths)
            r << findDirs(type, relPath);
        return r;
    }

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

    static bool makeDir(const QString& directory, int mode = 0755)
    {
        QDir dir(directory);
        return dir.mkpath(dir.absolutePath());
    }

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
    static QString locate(const char *type, const QString& filename, const KComponentData &cData = KGlobal::mainComponent())
    {
        if (type == QString("data")) {
            return QCoreApplication::applicationDirPath()+QString("/../share/")+filename;
  //      } else if (type == "apps"|| type == "xdgdata-apps") {
        } else if (type == QString("config")|| type == QString("appdata")) {
            return QCoreApplication::applicationDirPath()+QString("/../share/")+cData.componentName()+QString("/")+filename;
  //      } else if (type == "pixmap" || type == "xdgdata-pixmap" || type == "xdgdata-icon") {
        }
        //Q_ASSERT(false);
        return QString()+filename;
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
    static QString locateLocal(const char *type, const QString& filename, const KComponentData &cData = KGlobal::mainComponent())
    {
        qWarning() << Q_FUNC_INFO << "TODO" << type << filename;
        return QString();
    }

    static QString locateLocal( const char *type, const QString& filename, bool createDir, const KComponentData &cData = KGlobal::mainComponent())
    {
        return locateLocal(type, filename, cData);
    }
};


#endif
