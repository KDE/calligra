#include "kio/job.h"
#include "kio/netaccess.h"

using namespace KIO;

void KIO::StatJob::exec()
{
    qWarning() << Q_FUNC_INFO << "TODO";
    deleteLater();
}

void KIO::FileCopyJob::exec()
{
    KUrl src = m_args[0].value<KUrl>();
    KUrl dest = m_args[1].value<KUrl>();
    NetAccess::file_copy(src, dest);
    deleteLater();
}

void KIO::SimpleJob::exec()
{
    qWarning() << Q_FUNC_INFO << "TODO";
    deleteLater();
}

void KIO::ListJob::exec()
{
    qWarning() << Q_FUNC_INFO << "TODO";
    deleteLater();
}

void KIO::TransferJob::exec()
{
    qWarning() << Q_FUNC_INFO << "TODO";
    deleteLater();
}

void KIO::StoredTransferJob::exec()
{
    qWarning() << Q_FUNC_INFO << "TODO";
    deleteLater();
}

FileCopyJob *KIO::file_copy(const KUrl& src, const KUrl& dest, int permissions, JobFlags flags)
{
    FileCopyJob *job = new FileCopyJob();
    job->m_args << QVariant::fromValue<KUrl>(src) << QVariant::fromValue<KUrl>(dest);
    return job;
}

FileCopyJob *KIO::file_move( const KUrl& src, const KUrl& dest, int permissions, JobFlags flags )
{
    FileCopyJob *job = new FileCopyJob();
    job->m_args << QVariant::fromValue<KUrl>(src) << QVariant::fromValue<KUrl>(dest);
    return job;
}

SimpleJob *KIO::file_delete( const KUrl& src, JobFlags flags )
{
    SimpleJob *job = new SimpleJob();
    job->m_args << QVariant::fromValue<KUrl>(src);
    return job;
}

ListJob *KIO::listDir( const KUrl& url, JobFlags flags, bool includeHidden )
{
    ListJob *job = new ListJob();
    job->m_args << QVariant::fromValue<KUrl>(url);
    return job;
}

ListJob *KIO::listRecursive( const KUrl& url, JobFlags flags, bool includeHidden )
{
    ListJob *job = new ListJob();
    job->m_args << QVariant::fromValue<KUrl>(url);
    return job;
}

StoredTransferJob *KIO::storedGet( const KUrl& url, LoadType reload, JobFlags flags)
{
    StoredTransferJob *job = new StoredTransferJob();
    job->m_args << QVariant::fromValue<KUrl>(url);
    return job;
}

StoredTransferJob *KIO::storedPut( const QByteArray& arr, const KUrl& url, int permissions, JobFlags flags)
{
    StoredTransferJob *job = new StoredTransferJob();
    job->m_args << arr << QVariant::fromValue<KUrl>(url);
    return job;
}

StoredTransferJob *KIO::storedHttpPost( const QByteArray& arr, const KUrl& url, JobFlags flags)
{
    StoredTransferJob *job = new StoredTransferJob();
    job->m_args << arr << QVariant::fromValue<KUrl>(url);
    return job;
}

StoredTransferJob *KIO::storedHttpPost( QIODevice* device, const KUrl& url, qint64 size, JobFlags flags)
{
    StoredTransferJob *job = new StoredTransferJob();
    job->m_args << /* QVariant::fromValue<QIODevice*>(device) << */ QVariant::fromValue<KUrl>(url) << size;
    return job;
}
