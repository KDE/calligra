/*
    This file is part of the KDE libraries
    SPDX-FileCopyrightText: 1997 Torben Weis (weis@kde.org)
    SPDX-FileCopyrightText: 1998 Matthias Ettrich (ettrich@kde.org)
    SPDX-FileCopyrightText: 1999 David Faure (faure@kde.org)

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoNetAccess.h"

#include <unistd.h>

#include <cstring>

#include <QApplication>
#include <QtCore/QDataStream>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QMetaClassInfo>
#include <QtCore/QTextStream>
#include <qtemporaryfile.h>

#include <KLocalizedString>
#include <kjobwidgets.h>

#include <KIO/CopyJob>
#include <KIO/DeleteJob>
#include <KIO/FileCopyJob>
#include <KIO/Job>
#include <KIO/MimetypeJob>
#include <KIO/MkdirJob>
#include <KIO/StatJob>

namespace KIO
{
class NetAccessPrivate
{
public:
    NetAccessPrivate()
        : m_metaData(nullptr)
        , bJobOK(true)
    {
    }
    UDSEntry m_entry;
    QString m_mimetype;
    QByteArray m_data;
    QUrl m_url;
    QMap<QString, QString> *m_metaData;

    /**
     * Whether the download succeeded or not
     */
    bool bJobOK;
};

} // namespace KIO

using namespace KIO;

/**
 * List of temporary files
 */
static QStringList *tmpfiles;

static QString *lastErrorMsg = nullptr;
static int lastErrorCode = 0;

NetAccess::NetAccess()
    : d(new NetAccessPrivate)
{
}

NetAccess::~NetAccess()
{
    delete d;
}

bool NetAccess::download(const QUrl &u, QString &target, QWidget *window)
{
    if (u.isLocalFile()) {
        // file protocol. We do not need the network
        target = u.toLocalFile();
        const bool readable = QFileInfo(target).isReadable();
        if (!readable) {
            if (!lastErrorMsg) {
                lastErrorMsg = new QString;
            }
            *lastErrorMsg = i18n("File '%1' is not readable", target);
            lastErrorCode = ERR_CANNOT_READ;
        }
        return readable;
    }

    if (target.isEmpty()) {
        QTemporaryFile tmpFile;
        tmpFile.setAutoRemove(false);
        tmpFile.open();
        target = tmpFile.fileName();
        if (!tmpfiles) {
            tmpfiles = new QStringList;
        }
        tmpfiles->append(target);
    }

    NetAccess kioNet;
    const QUrl dest = QUrl::fromLocalFile(target);
    return kioNet.filecopyInternal(u, dest, -1, KIO::Overwrite, window, false /*copy*/);
}

bool NetAccess::upload(const QString &src, const QUrl &target, QWidget *window)
{
    if (target.isEmpty()) {
        return false;
    }

    // If target is local... well, just copy. This can be useful
    // when the client code uses a temp file no matter what.
    // Let's make sure it's not the exact same file though
    if (target.isLocalFile() && target.toLocalFile() == src) {
        return true;
    }

    NetAccess kioNet;
    const QUrl srcUrl = QUrl::fromLocalFile(src);
    return kioNet.filecopyInternal(srcUrl, target, -1, KIO::Overwrite, window, false /*copy*/);
}

bool NetAccess::file_copy(const QUrl &src, const QUrl &target, QWidget *window)
{
    NetAccess kioNet;
    return kioNet.filecopyInternal(src, target, -1, KIO::DefaultFlags, window, false /*copy*/);
}

bool NetAccess::copy(const QUrl &src, const QUrl &target, QWidget *window)
{
    return file_copy(src, target, window);
}

bool NetAccess::dircopy(const QUrl &src, const QUrl &target, QWidget *window)
{
    QList<QUrl> srcList;
    srcList.append(src);
    return NetAccess::dircopy(srcList, target, window);
}

bool NetAccess::dircopy(const QList<QUrl> &srcList, const QUrl &target, QWidget *window)
{
    NetAccess kioNet;
    return kioNet.dircopyInternal(srcList, target, window, false /*copy*/);
}

bool NetAccess::move(const QUrl &src, const QUrl &target, QWidget *window)
{
    QList<QUrl> srcList;
    srcList.append(src);
    NetAccess kioNet;
    return kioNet.dircopyInternal(srcList, target, window, true /*move*/);
}

bool NetAccess::move(const QList<QUrl> &srcList, const QUrl &target, QWidget *window)
{
    NetAccess kioNet;
    return kioNet.dircopyInternal(srcList, target, window, true /*move*/);
}

bool NetAccess::exists(const QUrl &url, bool source, QWidget *window)
{
    if (url.isLocalFile()) {
        return QFile::exists(url.toLocalFile());
    }
    NetAccess kioNet;
    return kioNet.statInternal(url, KIO::StatDetail::StatNoDetails, source ? SourceSide : DestinationSide, window);
}

bool NetAccess::exists(const QUrl &url, StatSide side, QWidget *window)
{
    if (url.isLocalFile()) {
        return QFile::exists(url.toLocalFile());
    }
    NetAccess kioNet;
    return kioNet.statInternal(url, KIO::StatDetail::StatNoDetails, side, window);
}

bool NetAccess::stat(const QUrl &url, KIO::UDSEntry &entry, QWidget *window)
{
    NetAccess kioNet;
    bool ret = kioNet.statInternal(url, KIO::StatDetail::StatDefaultDetails, SourceSide, window);
    if (ret) {
        entry = kioNet.d->m_entry;
    }
    return ret;
}

QUrl NetAccess::mostLocalUrl(const QUrl &url, QWidget *window)
{
    if (url.isLocalFile()) {
        return url;
    }

    KIO::UDSEntry entry;
    if (!stat(url, entry, window)) {
        return url;
    }

    const QString path = entry.stringValue(KIO::UDSEntry::UDS_LOCAL_PATH);
    if (!path.isEmpty()) {
        QUrl new_url = QUrl::fromLocalFile(path);
        return new_url;
    }

    return url;
}

bool NetAccess::del(const QUrl &url, QWidget *window)
{
    NetAccess kioNet;
    return kioNet.delInternal(url, window);
}
bool NetAccess::mkdir(const QUrl &url, QWidget *window, int permissions)
{
    NetAccess kioNet;
    return kioNet.mkdirInternal(url, permissions, window);
}

QString NetAccess::fish_execute(const QUrl &url, const QString &command, QWidget *window)
{
    NetAccess kioNet;
    return kioNet.fish_executeInternal(url, command, window);
}

QString NetAccess::mimetype(const QUrl &url, QWidget *window)
{
    NetAccess kioNet;
    return kioNet.mimetypeInternal(url, window);
}

QString NetAccess::lastErrorString()
{
    return lastErrorMsg ? *lastErrorMsg : QString();
}

int NetAccess::lastError()
{
    return lastErrorCode;
}

void NetAccess::removeTempFile(const QString &name)
{
    if (!tmpfiles) {
        return;
    }
    if (tmpfiles->contains(name)) {
        QFile::remove(name);
        tmpfiles->removeAll(name);
    }
}

bool NetAccess::filecopyInternal(const QUrl &src, const QUrl &target, int permissions, KIO::JobFlags flags, QWidget *window, bool move)
{
    d->bJobOK = true; // success unless further error occurs

    KIO::Job *job = move ? KIO::file_move(src, target, permissions, flags) : KIO::file_copy(src, target, permissions, flags);
    KJobWidgets::setWindow(job, window);
    connect(job, &KJob::result, this, &NetAccess::slotResult);

    enter_loop();
    return d->bJobOK;
}

bool NetAccess::dircopyInternal(const QList<QUrl> &src, const QUrl &target, QWidget *window, bool move)
{
    d->bJobOK = true; // success unless further error occurs

    KIO::Job *job = move ? KIO::move(src, target) : KIO::copy(src, target);
    KJobWidgets::setWindow(job, window);
    connect(job, &KJob::result, this, &NetAccess::slotResult);

    enter_loop();
    return d->bJobOK;
}

bool NetAccess::statInternal(const QUrl &url, KIO::StatDetail details, StatSide side, QWidget *window)
{
    d->bJobOK = true; // success unless further error occurs
    KIO::JobFlags flags = url.isLocalFile() ? KIO::HideProgressInfo : KIO::DefaultFlags;
    KIO::StatJob *job = KIO::stat(url, flags);
    KJobWidgets::setWindow(job, window);
    job->setDetails(details);
    job->setSide(side == SourceSide ? StatJob::SourceSide : StatJob::DestinationSide);
    connect(job, &KJob::result, this, &NetAccess::slotResult);
    enter_loop();
    return d->bJobOK;
}

bool NetAccess::delInternal(const QUrl &url, QWidget *window)
{
    d->bJobOK = true; // success unless further error occurs
    KIO::Job *job = KIO::del(url);
    KJobWidgets::setWindow(job, window);
    connect(job, &KJob::result, this, &NetAccess::slotResult);
    enter_loop();
    return d->bJobOK;
}

bool NetAccess::mkdirInternal(const QUrl &url, int permissions, QWidget *window)
{
    d->bJobOK = true; // success unless further error occurs
    KIO::Job *job = KIO::mkdir(url, permissions);
    KJobWidgets::setWindow(job, window);
    connect(job, &KJob::result, this, &NetAccess::slotResult);
    enter_loop();
    return d->bJobOK;
}

QString NetAccess::mimetypeInternal(const QUrl &url, QWidget *window)
{
    d->bJobOK = true; // success unless further error occurs
    d->m_mimetype = QStringLiteral("unknown");
    KIO::MimetypeJob *job = KIO::mimetype(url);
    KJobWidgets::setWindow(job, window);
    connect(job, &KJob::result, this, &NetAccess::slotResult);
    connect(job, &KIO::MimetypeJob::mimeTypeFound, this, &NetAccess::slotMimetype);
    enter_loop();
    return d->m_mimetype;
}

void NetAccess::slotMimetype(KIO::Job *, const QString &type)
{
    d->m_mimetype = type;
}

QString NetAccess::fish_executeInternal(const QUrl &url, const QString &command, QWidget *window)
{
    QString target, remoteTempFileName, resultData;
    QTemporaryFile tmpFile;
    tmpFile.open();

    if (url.scheme() == "fish") {
        // construct remote temp filename
        QUrl tempPathUrl = url;
        remoteTempFileName = tmpFile.fileName();
        // We only need the filename. The directory might not exist on the remote side.
        int pos = remoteTempFileName.lastIndexOf('/');
        remoteTempFileName = "/tmp/fishexec_" + remoteTempFileName.mid(pos + 1);
        tempPathUrl.setPath(remoteTempFileName);
        d->bJobOK = true; // success unless further error occurs
        QByteArray packedArgs;
        QDataStream stream(&packedArgs, QIODevice::WriteOnly);

        stream << int('X') << tempPathUrl << command;

        KIO::Job *job = KIO::special(tempPathUrl, packedArgs);
        KJobWidgets::setWindow(job, window);
        connect(job, &KJob::result, this, &NetAccess::slotResult);
        enter_loop();

        // since the KIO::special does not provide feedback we need to download the result
        if (NetAccess::download(tempPathUrl, target, window)) {
            QFile resultFile(target);

            if (resultFile.open(QIODevice::ReadOnly)) {
                QTextStream ts(&resultFile); // default encoding is Locale
                resultData = ts.readAll();
                resultFile.close();
                NetAccess::del(tempPathUrl, window);
            }
        }
    } else {
        resultData = i18n("ERROR: Unknown protocol '%1'", url.scheme());
    }
    return resultData;
}

void NetAccess::enter_loop()
{
    QEventLoop eventLoop;
    connect(this, &NetAccess::leaveModality, &eventLoop, &QEventLoop::quit);
    eventLoop.exec(QEventLoop::ExcludeUserInputEvents);
}

void NetAccess::slotResult(KJob *job)
{
    lastErrorCode = job->error();
    d->bJobOK = !job->error();
    if (!d->bJobOK) {
        if (!lastErrorMsg) {
            lastErrorMsg = new QString;
        }
        *lastErrorMsg = job->errorString();
    }
    KIO::StatJob *statJob = qobject_cast<KIO::StatJob *>(job);
    if (statJob) {
        d->m_entry = statJob->statResult();
    }

    KIO::Job *kioJob = qobject_cast<KIO::Job *>(job);
    if (kioJob && d->m_metaData) {
        *d->m_metaData = kioJob->metaData();
    }

    Q_EMIT leaveModality();
}

void NetAccess::slotData(KIO::Job *, const QByteArray &data)
{
    if (data.isEmpty()) {
        return;
    }

    unsigned offset = d->m_data.size();
    d->m_data.resize(offset + data.size());
    std::memcpy(d->m_data.data() + offset, data.data(), data.size());
}

void NetAccess::slotRedirection(KIO::Job *, const QUrl &url)
{
    d->m_url = url;
}
