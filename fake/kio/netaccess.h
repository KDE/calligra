#ifndef FAKE_KIO_NETACCESS_H
#define FAKE_KIO_NETACCESS_H

#include <QMap>
#include <QStringList>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QObject>
#include <QDebug>

#include <kurl.h>
#include <kio/kjob.h>


#include "kofake_export.h"



#if 0
#include <kio/global.h>
#include <kio/udsentry.h>
#include <kio/jobclasses.h> // for KIO::JobFlags
#else
namespace KIO {
    class UDSEntry;
}
#endif

class QWidget;

class KFakeJob;
namespace KIO {

class KOFAKE_EXPORT NetAccess : public QObject
{
public:
    enum StatSide { SourceSide, DestinationSide };

    static bool download(const KUrl& src, QString & target, QWidget* window)
    {
        if (!src.isLocalFile()) {
            qWarning() << Q_FUNC_INFO << "Not a local file src=" << src;
            return false;
        }
        QString s = src.toLocalFile();
        if (target == s) {
            qWarning() << Q_FUNC_INFO << "Source and destionation are same" << s;
            return false;
        }
        if (!QFile(s).copy(target)) {
            qWarning() << Q_FUNC_INFO << "Failed to copy src=" << s << "target=" << target;
            return false;
        }
        return true;
    }

#if 0
    /**
     * Removes the specified file if and only if it was created
     * by KIO::NetAccess as a temporary file for a former download.
     *
     * Note: This means that if you created your temporary with KTempFile,
     * use KTempFile::unlink() or KTempFile::setAutoDelete() to have
     * it removed.
     *
     * @param name Path to temporary file to remove.  May not be
     *             empty.
     */
#endif

    static void removeTempFile(const QString& name)
    {
        qDebug() << Q_FUNC_INFO << "TODO";
        //QFile(name).remove();
    }

    static bool upload(const QString& src, const KUrl& target, QWidget* window)
    {
        if (!target.isLocalFile()) {
            qWarning() << Q_FUNC_INFO << "Not a local file target=" << target;
            return false;
        }
        QString t = target.toLocalFile();
        if (t == src) {
            qWarning() << Q_FUNC_INFO << "Source and destionation are same" << src;
            return false;
        }
        if (!QFile(src).copy(t)) {
            qWarning() << Q_FUNC_INFO << "Failed to copy src=" << src << "target=" << t;
            return false;
        }
        return true;
    }

    static bool file_copy( const KUrl& src, const KUrl& target, QWidget* window = 0 )
    {
        if (!src.isLocalFile()) {
            qWarning() << Q_FUNC_INFO << "Not a local file src=" << src;
            return false;
        }
        if (!target.isLocalFile()) {
            qWarning() << Q_FUNC_INFO << "Not a local file target=" << target;
            return false;
        }
        QString s = src.toLocalFile();
        QString t = target.toLocalFile();
        if (t == s) {
            qWarning() << Q_FUNC_INFO << "Source and destionation are same" << s;
            return false;
        }
        if (QFile(t).exists()) {
            qWarning() << Q_FUNC_INFO << "Failed to copy cause target exists already src=" << s << "target=" << t;
            return false;
        }
        if (!QFile(s).copy(t)) {
            qWarning() << Q_FUNC_INFO << "Failed to copy src=" << s << "target=" << t;
            return false;
        }
        return true;
    }

#if 0
    /**
     * Alternative method for copying over the network.
     *
     * This one takes two URLs and is a direct equivalent
     * of KIO::copy!.
     * This means that it can copy files and directories alike
     * (it should have been named copy()).
     *
     * This method will bring up a dialog if the destination already exists.
     *
     * @param src URL Referencing the file to upload.
     * @param target URL containing the final location of the
     *               file.
     * @param window main window associated with this job. This is used to
     *               automatically cache and discard authentication information
     *               as needed. If NULL, authentication information will be cached
     *               only for a short duration after which the user will again be
     *               prompted for passwords as needed.
     * @return true if successful, false for failure
     */
    static bool dircopy( const KUrl& src, const KUrl& target, QWidget* window ); // TODO deprecate in favor of KIO::copy + synchronousRun (or job->exec())

    /**
     * Overloaded method, which takes a list of source URLs
     */
    static bool dircopy( const KUrl::List& src, const KUrl& target, QWidget* window = 0L ); // TODO deprecate in favor of KIO::copy + synchronousRun (or job->exec())
#endif

    /**
     * Full-fledged equivalent of KIO::move.
     * Moves or renames one file or directory.
     * @deprecated use KIO::move and then KIO::NetAccess::synchronousRun (or job->exec())
     */
// #ifndef KDE_NO_DEPRECATED
    static /*KDE_DEPRECATED */ bool move( const KUrl& src, const KUrl& target, QWidget* window = 0L )
    {
        if (!src.isLocalFile()) {
            qWarning() << Q_FUNC_INFO << "Not a local file src=" << src;
            return false;
        }
        if (!target.isLocalFile()) {
            qWarning() << Q_FUNC_INFO << "Not a local file target=" << target;
            return false;
        }
        QString s = src.toLocalFile();
        QString t = target.toLocalFile();
        if (t == s) {
            qWarning() << Q_FUNC_INFO << "Source and destionation are same" << s;
            return false;
        }
        if (!QFile(s).copy(t)) {
            qWarning() << Q_FUNC_INFO << "Failed to copy src=" << s << "target=" << t;
            return false;
        }
        if (!QFile(s).remove()) {
            qWarning() << Q_FUNC_INFO << "Failed to delete src=" << s;
            return false;
        }
        return true;
    }
// #endif

#if 0
    /**
     * Full-fledged equivalent of KIO::move.
     * Moves or renames a list of files or directories.
     * @deprecated use KIO::move and then KIO::NetAccess::synchronousRun (or job->exec())
     */
#ifndef KDE_NO_DEPRECATED
    static KDE_DEPRECATED bool move( const KUrl::List& src, const KUrl& target, QWidget* window = 0L );
#endif

    /**
     * Tests whether a URL exists.
     *
     * @param url the URL we are testing
     * @param source if true, we want to read from that URL.
     *               If false, we want to write to it.
     * IMPORTANT: see documentation for KIO::stat for more details about this.
     * @param window main window associated with this job. This is used to
     *               automatically cache and discard authentication information
     *               as needed. If NULL, authentication information will be
     *               cached only for a short duration after which the user will
     *               again be prompted for passwords as needed.
     * @return true if the URL exists and we can do the operation specified by
     *              @p source, false otherwise
     *
     * @deprecated use the StatSide enum instead of the bool source
     */
#ifndef KDE_NO_DEPRECATED
    static KDE_DEPRECATED bool exists(const KUrl& url, bool source, QWidget* window);
#endif

#endif

    static bool exists(const KUrl& url, StatSide statSide, QWidget* window)
    {
        if (!url.isLocalFile()) {
            qWarning() << Q_FUNC_INFO << "Not a local file=" << url;
            return false;
        }
        return QFile(url.toLocalFile()).exists();
    }

#if 0
    /**
     * Tests whether a URL exists and return information on it.
     *
     * This is a convenience function for KIO::stat
     * (it saves creating a slot and testing for the job result).
     *
     * @param url The URL we are testing.
     * @param entry The result of the stat. Iterate over the list
     * of atoms to get hold of name, type, size, etc., or use KFileItem.
     * @param window main window associated with this job. This is used to
     *               automatically cache and discard authentication information
     *               as needed. If NULL, authentication information will be
     *               cached only for a short duration after which the user will
     *               again be prompted for passwords as needed.
     * @return true if successful, false for failure
     */
#endif

    static bool stat(const KUrl& url, KIO::UDSEntry & entry, QWidget* window)
    {
        if (!url.isLocalFile()) {
            qWarning() << Q_FUNC_INFO << "Not a local file=" << url;
            return false;
        }
        qDebug() << Q_FUNC_INFO << "TODO";
        //QFile(url.toLocalFile())
        return true;
    }

#if 0
    /**
     * Tries to map a local URL for the given URL.
     *
     * This is a convenience function for KIO::stat + parsing the
     * resulting UDSEntry.
     *
     * @param url The URL we are testing.
     * @param window main window associated with this job. This is used to
     *               automatically cache and discard authentication information
     *               as needed. If NULL, authentication information will be
     *               cached only for a short duration after which the user will
     *               again be prompted for passwords as needed.
     * @return a local URL corresponding to the same resource than the
     *         original URL, or the original URL if no local URL can be mapped
     */
    static KUrl mostLocalUrl(const KUrl& url, QWidget* window);

    /**
     * Deletes a file or a directory in a synchronous way.
     *
     * This is a convenience function for KIO::del
     * (it saves creating a slot and testing for the job result).
     *
     * @param url The file or directory to delete.
     * @param window main window associated with this job. This is used to
     *               automatically cache and discard authentication information
     *               as needed. If NULL, authentication information will be
     *               cached only for a short duration after which the user will
     *               again be prompted for passwords as needed.
     * @return true on success, false on failure.
     */
#endif

    static bool del( const KUrl & url, QWidget* window )
    {
        if (!url.isLocalFile()) {
            qWarning() << Q_FUNC_INFO << "Not a local file=" << url;
            return false;
        }
        bool ok = false;
        QDir dir(url.toLocalFile());
        if (dir.exists() && dir.rmdir(url.toLocalFile()))
            ok = true;
        QFile file(url.toLocalFile());
        if (file.exists() && file.remove())
            ok = true;
        if (!ok)
            qWarning() << Q_FUNC_INFO << "Failed to delete file/dir=" << url.toLocalFile();
        return ok;
    }

#if 0
    /**
     * Creates a directory in a synchronous way.
     *
     * This is a convenience function for @p KIO::mkdir
     * (it saves creating a slot and testing for the job result).
     *
     * @param url The directory to create.
     * @param window main window associated with this job. This is used to
     *               automatically cache and discard authentication information
     *               as needed. If NULL, authentication information will be
     *               cached only for a short duration after which the user will
     *               again be prompted for passwords as needed.
     * @param permissions directory permissions.
     * @return true on success, false on failure.
     */
    static bool mkdir( const KUrl & url, QWidget* window, int permissions = -1 );

    /**
     * Executes a remote process via the fish ioslave in a synchronous way.
     *
     * @param url The remote machine where the command should be executed.
     *            e.g. fish://someuser\@somehost:sshport/
     *            some special cases exist.
     *            fish://someuser\@localhost/
     *            will use su instead of ssh to connect and execute the command.
     *            fish://someuser\@localhost:port/
     *            will use ssh to connect and execute the command.
     * @param command The command to be executed.
     * @param window main window associated with this job. This is used to
     *               automatically cache and discard authentication information
     *               as needed. If NULL, authentication information will be
     *               cached only for a short duration after which the user will
     *               again be prompted for passwords as needed.
     * @return The resulting output of the @p command that is executed.
     */
    static QString fish_execute( const KUrl & url, const QString &command, QWidget* window );

    /**
     * This function executes a job in a synchronous way.
     * If a job fetches some data, pass a QByteArray pointer as data parameter to this function
     * and after the function returns it will contain all the data fetched by this job.
     *
     * @code
     * KIO::Job *job = KIO::get( url );
     * QMap<QString, QString> metaData;
     * metaData.insert( "PropagateHttpHeader", "true" );
     * if ( NetAccess::synchronousRun( job, 0, &data, &url, &metaData ) ) {
     *   QString responseHeaders = metaData[ "HTTP-Headers" ];
     *   kDebug()<<"Response header = "<< responseHeaders;
     * }
     * @endcode
     *
     * @param job job which the function will run. Note that after this function
     *            finishes running, job is deleted and you can't access it anymore!
     * @param window main window associated with this job. This is used to
     *               automatically cache and discard authentication information
     *               as needed. If NULL, authentication information will be
     *               cached only for a short duration after which the user will
     *               again be prompted for passwords as needed.
     * @param data if passed and relevant to this job then it will contain the data
     *               that was fetched by the job
     * @param finalURL if passed will contain the final url of this job (it might differ
     *                 from the one it was created with if there was a redirection)
     * @param metaData you can pass a pointer to the map with meta data you wish to
     *                 set on the job. After the job finishes this map will hold all the
     *                 meta data from the job.
     *
     * @return true on success, false on failure.
     */
    static bool synchronousRun( Job* job, QWidget* window, QByteArray* data=0,
                                KUrl* finalURL=0, QMap<QString,QString>* metaData=0 );

    /**
     * Determines the mimetype of a given URL.
     *
     * This is a convenience function for KIO::mimetype.  You
     * should call this only when really necessary.
     * KMimeType::findByUrl can determine extension a lot faster, but
     * less reliably for remote files. Only when findByUrl() returns
     * unknown (application/octet-stream) then this one should be
     * used.
     *
     * @param url The URL whose mimetype we are interested in.
     * @param window main window associated with this job. This is used to
     *               automatically cache and discard authentication information
     *               as needed. If NULL, authentication information will be
     *               cached only for a short duration after which the user will
     *               again be prompted for passwords as needed.
     * @return The mimetype name.
     */
    static QString mimetype( const KUrl & url, QWidget* window );

#endif

    static QString lastErrorString()
    {
        static QString m_lastErrorString;
        return m_lastErrorString;
    }

    static int lastError()
    {
        return -1;
    }

    static bool synchronousRun(KIO::Job *job, QWidget *w = 0)
    {
        return move( job->m_args[0].value<KUrl>(), job->m_args[1].value<KUrl>() );
    }
};

static KIO::Job* move( const KUrl& src, const KUrl& target, QWidget* window = 0L )
{
    KIO::Job *job = new KIO::Job();
    job->m_args << src << target;
    return job;
}

}


#endif
