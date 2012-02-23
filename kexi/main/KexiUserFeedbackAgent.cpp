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

#include <QApplication>
#include <QDesktopWidget>
#include <QProcess>
#include <QUuid>

#if defined HAVE_UNAME || defined Q_WS_WIN
# include <sys/utsname.h>
#endif

/*! Version of the feedback data format.
 Changelog:
 1.0: initial version
 1.1: added JSON-compatible character escaping; added uid
 1.2: added JSON-compatible character escaping; added os_release, os_machine, screen_count
*/
static const char KexiUserFeedbackAgent_VERSION[] = "1.2";

class KexiUserFeedbackAgent::Private
{
public:
    Private()
     : configGroup(KConfigGroup(KGlobal::config()->group("User Feedback")))
     , areas(KexiUserFeedbackAgent::NoAreas)
     , sentDataInThisSession(KexiUserFeedbackAgent::NoAreas)
     , url(QLatin1String("http://www.kexi-project.org/feedback"))
     , redirectChecked(false)
    {
    }
    
    void updateData();
    
    KConfigGroup configGroup;
    KexiUserFeedbackAgent::Areas areas;
    KexiUserFeedbackAgent::Areas sentDataInThisSession;
    QList<QByteArray> keys;
    QMap<QByteArray, QVariant> data;
    QMap<QByteArray, KexiUserFeedbackAgent::Area> areasForKeys;
    //! Unique user ID handy if used does not want to disclose username
    //! but agrees to be identified as unique user of the application.
    QUuid uid;
    QString url;
    bool redirectChecked;
};

void KexiUserFeedbackAgent::Private::updateData()
{
    kDebug();
    keys.clear();
    data.clear();
    areasForKeys.clear();
    #define ADD(key, val, area) { \
        keys.append(QByteArray(key)); data.insert(QByteArray(key), QVariant(val)); \
        areasForKeys.insert(key, KexiUserFeedbackAgent::area); \
    }
    ADD("ver", KexiUserFeedbackAgent_VERSION, BasicArea);
    ADD("uid", uid.toString(), AnonymousIdentificationArea);
    ADD("app_ver", Kexi::versionString(), BasicArea);
    ADD("app_ver_major", Kexi::versionMajor(), BasicArea);
    ADD("app_ver_minor", Kexi::versionMinor(), BasicArea);
    ADD("app_ver_release", Kexi::versionRelease(), BasicArea);

    ADD("kde_ver", KDE::versionString(), BasicArea);
    ADD("kde_ver_major", KDE::versionMajor(), BasicArea);
    ADD("kde_ver_minor", KDE::versionMinor(), BasicArea);
    ADD("kde_ver_release", KDE::versionRelease(), BasicArea);
#ifdef Q_OS_LINUX
    {
        ADD("os", "linux", SystemInfoArea);
        QProcess p;
        p.start("lsb_release", QStringList() << "-i" << "-r" << "-d");
        if (p.waitForFinished()) {
            QString info = p.readLine().replace("Distributor ID:", "").trimmed();
            if (!info.isEmpty()) {
                ADD("linux_id", info, SystemInfoArea);
            }
            info = p.readLine().replace("Description:", "").trimmed();
            if (!info.isEmpty()) {
                ADD("linux_desc", info, SystemInfoArea);
            }
            info = p.readLine().replace("Release:", "").trimmed();
            if (!info.isEmpty()) {
                ADD("linux_rel", info, SystemInfoArea);
            }
        }
        p.close();
    }
#elif defined(Q_WS_MAC)
    ADD("os", "mac", SystemInfoArea);
#elif defined(Q_WS_WIN)
    ADD("os", "windows", SystemInfoArea);
#else
//! @todo BSD?
    ADD("os", "other", SystemInfoArea);
#endif

#if defined HAVE_UNAME || defined Q_WS_WIN
    struct utsname buf;
    if (uname(&buf) == 0) {
        ADD("os_release", buf.release, SystemInfoArea);
        ADD("os_machine", buf.machine, SystemInfoArea);
    }
#endif

    QSize screen(QApplication::desktop()->screenGeometry(
                        KexiMainWindowIface::global()->thisWidget()).size());
    ADD("screen_width", screen.width(), ScreenInfoArea);
    ADD("screen_height", screen.height(), ScreenInfoArea);
    ADD("screen_count", QApplication::desktop()->screenCount(), ScreenInfoArea);

    ADD("country", KGlobal::locale()->country(), RegionalSettingsArea);
    ADD("language", KGlobal::locale()->language(), RegionalSettingsArea);
    ADD("date_format", KGlobal::locale()->dateFormat(), RegionalSettingsArea);
    ADD("short_date_format", KGlobal::locale()->dateFormatShort(), RegionalSettingsArea);
    ADD("time_format", KGlobal::locale()->timeFormat(), RegionalSettingsArea);
    ADD("right_to_left", QApplication::isRightToLeft(), RegionalSettingsArea);
#undef ADD
}

// ---

KexiUserFeedbackAgent::KexiUserFeedbackAgent(QObject* parent)
 : QObject(parent), d(new Private)
{
    if (d->configGroup.readEntry("BasicInfo", false)) {
        d->areas |= BasicArea;
    }
    if (d->configGroup.readEntry("AnonymousIdentification", false)) {
        d->areas |= AnonymousIdentificationArea;
    }
    if (d->configGroup.readEntry("SystemInfo", false)) {
        d->areas |= SystemInfoArea;
    }
    if (d->configGroup.readEntry("ScreenInfo", false)) {
        d->areas |= ScreenInfoArea;
    }
    if (d->configGroup.readEntry("RegionalSettings", false)) {
        d->areas |= RegionalSettingsArea;
    }
    
    // load or create uid
    QString uidString = d->configGroup.readEntry("Uid", QString());
    d->uid = QUuid(uidString);
    if (d->uid.isNull()) {
        d->uid = QUuid::createUuid();
        d->configGroup.writeEntry("Uid", d->uid.toString());
    }

    d->updateData();
    sendData();
}

KexiUserFeedbackAgent::~KexiUserFeedbackAgent()
{
    delete d;
}

void KexiUserFeedbackAgent::setEnabledAreas(Areas areas)
{
    if (areas != NoAreas && areas != AllAreas && !(areas & BasicArea)) {
        areas |= BasicArea; // fix, Basic Info is required
    }
    if (d->areas == areas) {
        return;
    }
    d->areas = areas;
    d->configGroup.writeEntry("BasicInfo", bool(d->areas & BasicArea));
    d->configGroup.writeEntry("AnonymousIdentification", bool(d->areas & AnonymousIdentificationArea));
    d->configGroup.writeEntry("SystemInfo", bool(d->areas & SystemInfoArea));
    d->configGroup.writeEntry("ScreenInfo", bool(d->areas & ScreenInfoArea));
    d->configGroup.writeEntry("RegionalSettings", bool(d->areas & RegionalSettingsArea));
    d->configGroup.sync();
    if ((d->sentDataInThisSession & d->areas) != d->areas) {
        d->updateData();
        sendData();
    }
}

KexiUserFeedbackAgent::Areas  KexiUserFeedbackAgent::enabledAreas() const
{
    return d->areas;
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
    kDebug();
    if (d->areas == NoAreas) {
        return;
    }
    
    if (!d->redirectChecked) {
        sendRedirectQuestion();
        return;
    }

    QByteArray postData;
    foreach (const QByteArray& key, d->keys) {
        Area area = d->areasForKeys.value(key);
        if (area != NoAreas && (d->areas & area)) {
            if (!postData.isEmpty()) {
                postData += ',';
            }
            postData += (QByteArray("\"") + key + "\":\""
                        + escapeJson(d->data.value(key).toString()).toUtf8() + '"');
        }
    }
    kDebug() << postData;
    
    KIO::Job* sendJob = KIO::storedHttpPost(postData, KUrl(d->url + "/send"), KIO::HideProgressInfo);
    connect(sendJob, SIGNAL(result(KJob*)), this, SLOT(sendDataFinished(KJob*)));
    sendJob->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded");
}

void KexiUserFeedbackAgent::sendDataFinished(KJob* job)
{
    if (job->error()) {
        //! @todo error...
        return;
    }
    KIO::StoredTransferJob* sendJob = qobject_cast<KIO::StoredTransferJob*>(job);
    QByteArray result = sendJob->data();
    result.chop(1); // remove \n
    kDebug() << result;
    if (result == "ok") {
        d->sentDataInThisSession = d->areas;
    }
}

QVariant KexiUserFeedbackAgent::value(const QString& key) const
{
    return d->data.value(key.toLatin1());
}

void KexiUserFeedbackAgent::sendRedirectQuestion()
{
    QByteArray postData = "get_url";
    kDebug() << postData;
    KIO::Job* sendJob = KIO::storedHttpPost(postData, KUrl(d->url + "/send"), KIO::HideProgressInfo);
    connect(sendJob, SIGNAL(result(KJob*)), this, SLOT(sendRedirectQuestionFinished(KJob*)));
    sendJob->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded");
}

void KexiUserFeedbackAgent::sendRedirectQuestionFinished(KJob* job)
{
    if (job->error()) {
        //! @todo error...
        kDebug() << "Error, no URL Redirect";
    }
    else {
        KIO::StoredTransferJob* sendJob = qobject_cast<KIO::StoredTransferJob*>(job);
        QByteArray result = sendJob->data();
        result.chop(1); // remove \n
        kDebug() << result;
        if (result.isEmpty()) {
            kDebug() << "No URL Redirect";
        }
        else {
            d->url = QString::fromUtf8(result);
            kDebug() << "URL Redirect to" << d->url;
        }
    }
    d->redirectChecked = true;
    emit redirectLoaded();
    sendData();
}

QString KexiUserFeedbackAgent::serviceUrl() const
{
    return d->url;
}

void KexiUserFeedbackAgent::waitForRedirect(QObject *receiver, const char* slot)
{
    if (!receiver) {
        return;
    }
    if (d->redirectChecked) {
        QMetaObject::invokeMethod(receiver, slot);
    }
    else {
        connect(this, SIGNAL(redirectLoaded()), receiver, slot, Qt::UniqueConnection);
        if (d->areas == NoAreas) {
            sendRedirectQuestion();
        }
    }
}
