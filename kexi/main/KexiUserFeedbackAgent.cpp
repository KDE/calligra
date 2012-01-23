/* This file is part of the KDE project
   Copyright (C) 2012 Jarosław Staniek <staniek@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KexiUserFeedbackAgent.h"

#include <kexi_version.h>
#include <KexiMainWindowIface.h>

#include <KIO/Job>
#include <KLocale>
#include <KDebug>
#include <KConfigGroup>

#include <QPair>
#include <QApplication>
#include <QDesktopWidget>
#include <QProcess>
#include <QUuid>

//! Version of feedback data format.
//! Changelog:
//! *1.0: initial version
//! *1.1: added JSON-compatible character escaping; added uid
static const char KexiUserFeedbackAgent_VERSION[] = "1.1";

static const char KexiUserFeedbackAgent_URL[] = "http://www.kexi-project.org/feedback/send";

typedef QPair<QByteArray, QVariant> DataPair;

class KexiUserFeedbackAgent::Private
{
public:
    Private()
     : enabled(false)
    {
    }
    bool enabled;
    QList<DataPair> data;
    //! Unique user ID handy if used does not want to disclose username
    //! but agrees to be identified as unique user of the application.
    QUuid uid;
};

KexiUserFeedbackAgent::KexiUserFeedbackAgent(QObject* parent)
 : QObject(parent), d(new Private)
{
    KConfigGroup configGroup(KGlobal::config()->group("User Feedback"));
    
    // load or create uid
    QString uidString = configGroup.readEntry("uid", QString());
    d->uid = QUuid(uidString);
    if (d->uid.isNull()) {
        d->uid = QUuid::createUuid();
        configGroup.writeEntry("uid", d->uid.toString());
    }
    
    #define ADD(a, b) d->data.append(qMakePair(QByteArray(a), QVariant(b)))
    ADD("ver", KexiUserFeedbackAgent_VERSION);
    ADD("uid", d->uid.toString());
    ADD("app_ver", Kexi::versionString());
    ADD("app_ver_major", Kexi::versionMajor());
    ADD("app_ver_minor", Kexi::versionMinor());
    ADD("app_ver_release", Kexi::versionRelease());

    ADD("kde_ver", KDE::versionString());
    ADD("kde_ver_major", KDE::versionMajor());
    ADD("kde_ver_minor", KDE::versionMinor());
    ADD("kde_ver_release", KDE::versionRelease());

#ifdef Q_OS_LINUX
    {
        ADD("os", "linux");
        QProcess p;
        p.start("lsb_release", QStringList() << "-i" << "-r" << "-d");
        if (p.waitForFinished()) {
            QString info = p.readLine().replace("Distributor ID:", "").trimmed();
            if (!info.isEmpty()) {
                ADD("linux_id", info);
            }
            info = p.readLine().replace("Description:", "").trimmed();
            if (!info.isEmpty()) {
                ADD("linux_desc", info);
            }
            info = p.readLine().replace("Release:", "").trimmed();
            if (!info.isEmpty()) {
                ADD("linux_rel", info);
            }
        }
        p.close();
    }
#elif defined(Q_WS_MAC)
    ADD("os", "mac");
#elif defined(Q_WS_WIN)
    ADD("os", "windows");
#else
//! @todo BSD?
    ADD("os", "other");
#endif

    QSize screen(QApplication::desktop()->screenGeometry(
                           KexiMainWindowIface::global()->thisWidget()).size());
    ADD("screen_width", screen.width());
    ADD("screen_height", screen.height());

    ADD("country", KGlobal::locale()->country());
    ADD("language", KGlobal::locale()->language());
    ADD("date_format", KGlobal::locale()->dateFormat());
    ADD("short_date_format", KGlobal::locale()->dateFormatShort());
    ADD("time_format", KGlobal::locale()->timeFormat());
    ADD("right_to_left", QApplication::isRightToLeft());

    sendData();
}

KexiUserFeedbackAgent::~KexiUserFeedbackAgent()
{
    delete d;
}

void KexiUserFeedbackAgent::setEnabled(bool enabled)
{
    d->enabled = enabled;
}

//! Escapes string for json format (see http://json.org/string.gif).
inline QString escapeJson(const QString& s)
{
    QString res;
    for (int i=0; i<s.length(); i++) {
        switch (s[i].toLatin1()) {
        case '\\': res += QLatin1String("\\\\"); break;
        case '/': res += QLatin1String("\\/"); break;
        case '"': res += QLatin1String("\\\""); break;
        case '\b': res += QLatin1String("\\b"); break;
        case '\f': res += QLatin1String("\\f"); break;
        case '\n': res += QLatin1String("\\n"); break;
        case '\r': res += QLatin1String("\\r"); break;
        case '\t': res += QLatin1String("\\t"); break;
        default: res += s[i];
        }
    }
    return res;
}

void KexiUserFeedbackAgent::sendData()
{
    QByteArray postData;
    foreach (const DataPair &item, d->data) {
        if (!postData.isEmpty()) {
            postData += ',';
        }
        postData += (QByteArray("\"") + item.first + "\":\""
                     + escapeJson(item.second.toString()).toUtf8() + '"');
    }
    kDebug() << postData;
    
    KIO::Job* sendJob = KIO::storedHttpPost(
        postData, KUrl(KexiUserFeedbackAgent_URL), KIO::HideProgressInfo);
    connect(sendJob, SIGNAL(finished(KJob*)), this, SLOT(sendJobFinished(KJob*)));
    sendJob->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded");
}

void KexiUserFeedbackAgent::sendJobFinished(KJob* job)
{
    if (job->error()) {
        //! @todo error...
        return;
    }
    KIO::StoredTransferJob* sendJob = qobject_cast<KIO::StoredTransferJob*>(job);
    kDebug() << sendJob->data();
}
