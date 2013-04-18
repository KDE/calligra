#ifndef FAKE_KIOJOB_H
#define FAKE_KIOJOB_H

#include <QVariant>
#include <QObject>
#include <QWidget>

#include <kurl.h>
#include <kio/jobuidelegate.h>
#include "kofake_export.h"

class KOFAKE_EXPORT KJob : public QObject
{
    Q_OBJECT
    Q_ENUMS( KillVerbosity Capability Unit )
    Q_FLAGS( Capabilities )
public:
#if 0
    enum Unit { Bytes, Files, Directories };

    enum Capability { NoCapabilities = 0x0000,
                      Killable       = 0x0001,
                      Suspendable    = 0x0002 };

    Q_DECLARE_FLAGS( Capabilities, Capability )
#endif
    KJob( QObject *parent = 0 ) :QObject(parent) {}

#if 0
    /**
     * Attach a UI delegate to this job.
     *
     * If the job had another UI delegate, it's automatically deleted. Once
     * attached to the job, the UI delegate will be deleted with the job.
     *
     * @param delegate the new UI delegate to use
     * @see KJobUiDelegate
     */
    void setUiDelegate( KJobUiDelegate *delegate );

    /**
     * Retrieves the delegate attached to this job.
     *
     * @return the delegate attached to this job, or 0 if there's no such delegate
     */
    KJobUiDelegate *uiDelegate() const;

    /**
     * Returns the capabilities of this job.
     *
     * @return the capabilities that this job supports
     * @see setCapabilities()
     */
    Capabilities capabilities() const;

    /**
     * Returns if the job was suspended with the suspend() call.
     *
     * @return if the job was suspended
     * @see suspend() resume()
     */
    bool isSuspended() const;
#endif

    virtual void start() {
        qDebug() << Q_FUNC_INFO << "TODO";
    }

    enum KillVerbosity { Quietly, EmitResult };

public Q_SLOTS:

    bool kill( KillVerbosity verbosity = Quietly ) {
        qDebug() << Q_FUNC_INFO << "TODO";
        return true;
    }

#if 0
    /**
     * Suspends this job.
     * The job should be kept in a state in which it is possible to resume it.
     *
     * @return true if the operation is supported and succeeded, false otherwise
     */
    bool suspend();

    /**
     * Resumes this job.
     *
     * @return true if the operation is supported and succeeded, false otherwise
     */
    bool resume();

public:
    /**
     * Executes the job synchronously.
     *
     * This will start a nested QEventLoop internally. Nested event loop can be dangerous and
     * can have unintended side effects, you should avoid calling exec() whenever you can and use the
     * asynchronous interface of KJob instead.
     *
     * Should you indeed call this method, you need to make sure that all callers are reentrant,
     * so that events delivered by the inner event loop don't cause non-reentrant functions to be
     * called, which usually wreaks havoc.
     *
     * Note that the event loop started by this method does not process user input events, which means
     * your user interface will effectivly be blocked. Other events like paint or network events are
     * still being processed. The advantage of not processing user input events is that the chance of
     * accidental reentrancy is greatly reduced. Still you should avoid calling this function.
     *
     * @return true if the job has been executed without error, false otherwise
     */
    bool exec();

    enum
    {
        /*** Indicates there is no error */
        NoError = 0,
        /*** Indicates the job was killed */
        KilledJobError = 1,
        /*** Subclasses should define error codes starting at this value */
        UserDefinedError = 100
    };


    /**
     * Returns the error code, if there has been an error.
     *
     * Only call this method from the slot connected to result().
     *
     * @return the error code for this job, 0 if no error.
     */
    int error() const;

    /**
     * Returns the error text if there has been an error.
     *
     * Only call if error is not 0.
     *
     * This is usually some extra data associated with the error,
     * such as a URL.  Use errorString() to get a human-readable,
     * translated message.
     *
     * @return a string to help understand the error
     */
    QString errorText() const;

    /**
     * A human-readable error message.
     *
     * This provides a translated, human-readable description of the
     * error.  Only call if error is not 0.
     *
     * Subclasses should implement this to create a translated
     * error message from the error code and error text.
     * For example:
     * \code
     * if (error() == ReadFailed)
     *   i18n( "Could not read \"%1\"", errorText() );
     * \endcode
     *
     * @return a translated error message, providing error() is 0
     */
    virtual QString errorString() const;


    /**
     * Returns the processed amount of a given unit for this job.
     *
     * @param unit the unit of the requested amount
     * @return the processed size
     */
    qulonglong processedAmount(Unit unit) const;

    /**
     * Returns the total amount of a given unit for this job.
     *
     * @param unit the unit of the requested amount
     * @return the total size
     */
    qulonglong totalAmount(Unit unit) const;

    /**
     * Returns the overall progress of this job.
     *
     * @return the overall progress of this job
     */
    unsigned long percent() const;

    /**
     * set the auto-delete property of the job. If @p autodelete is
     * set to false the job will not delete itself once it is finished.
     *
     * The default for any KJob is to automatically delete itself.
     *
     * @param autodelete set to false to disable automatic deletion
     * of the job.
     */
    void setAutoDelete( bool autodelete );

    /**
     * Returns whether this job automatically deletes itself once
     * the job is finished.
     *
     * @return whether the job is deleted automatically after
     * finishing.
     */
    bool isAutoDelete() const;

Q_SIGNALS:
    void finished(KJob *job);
    void suspended(KJob *job);
    void resumed(KJob *job);
    void result(KJob *job);

Q_SIGNALS:
    void description(KJob *job, const QString &title,
                     const QPair<QString, QString> &field1 = qMakePair(QString(), QString()),
                     const QPair<QString, QString> &field2 = qMakePair(QString(), QString()));
    void infoMessage( KJob *job, const QString &plain, const QString &rich = QString() );
    void warning( KJob *job, const QString &plain, const QString &rich = QString() );

Q_SIGNALS:
    void totalAmount(KJob *job, KJob::Unit unit, qulonglong amount);
    void processedAmount(KJob *job, KJob::Unit unit, qulonglong amount);
    void totalSize(KJob *job, qulonglong size);
    void processedSize(KJob *job, qulonglong size);
    void percent( KJob *job, unsigned long percent );
    void speed(KJob *job, unsigned long speed);
#endif
};

#if 0
Q_DECLARE_OPERATORS_FOR_FLAGS( KJob::Capabilities )
#endif

namespace KIO {

enum LoadType { Reload, NoReload };

class UDSEntry
{
public:
};

class KOFAKE_EXPORT Job : public KJob
{
    Q_OBJECT
public:
    QVariantList m_args;
    virtual JobUiDelegate *ui() const { return 0; }
    virtual void exec() {
        deleteLater();
    }
};

class KOFAKE_EXPORT FileCopyJob : public Job
{
    Q_OBJECT
public:
    virtual void exec();
};

class KOFAKE_EXPORT SimpleJob : public Job
{
public:
    virtual void exec();
};

class KOFAKE_EXPORT ListJob : public Job
{
    Q_OBJECT
public:
    virtual void exec();
};

class KOFAKE_EXPORT TransferJob : public SimpleJob
{
public:
    virtual void exec();
};

class KOFAKE_EXPORT StoredTransferJob : public TransferJob
{
    Q_OBJECT
public:
    void setData( const QByteArray& arr ) {}
    QByteArray data() const { return QByteArray(); }
    virtual void exec();
};

enum JobFlag {
    DefaultFlags = 0,
    HideProgressInfo = 1,
    Resume = 2,
    Overwrite = 4
};
Q_DECLARE_FLAGS(JobFlags, JobFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(JobFlags)

KOFAKE_EXPORT FileCopyJob *file_copy(const KUrl& src, const KUrl& dest, int permissions=-1, JobFlags flags = DefaultFlags);
KOFAKE_EXPORT FileCopyJob *file_move( const KUrl& src, const KUrl& dest, int permissions=-1, JobFlags flags = DefaultFlags );
//FileCopyJob *file_move( const KUrl& src, const KUrl& dest, JobFlags flags ); // not implemented - on purpose.
KOFAKE_EXPORT SimpleJob *file_delete( const KUrl& src, JobFlags flags = DefaultFlags );
KOFAKE_EXPORT ListJob *listDir( const KUrl& url, JobFlags flags = DefaultFlags, bool includeHidden = true );
KOFAKE_EXPORT ListJob *listRecursive( const KUrl& url, JobFlags flags = DefaultFlags, bool includeHidden = true );
KOFAKE_EXPORT StoredTransferJob *storedGet( const KUrl& url, LoadType reload = NoReload, JobFlags flags = DefaultFlags );
KOFAKE_EXPORT StoredTransferJob *storedPut( const QByteArray& arr, const KUrl& url, int permissions, JobFlags flags = DefaultFlags );
KOFAKE_EXPORT StoredTransferJob *storedHttpPost( const QByteArray& arr, const KUrl& url, JobFlags flags = DefaultFlags );
KOFAKE_EXPORT StoredTransferJob *storedHttpPost( QIODevice* device, const KUrl& url, qint64 size = -1, JobFlags flags = DefaultFlags );

}

#endif
