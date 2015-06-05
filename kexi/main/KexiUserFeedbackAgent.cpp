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
#include <kconfiggroup.h>
#include <kaboutdata.h>
#include <kdeversion.h>
#include <KSharedConfig>
#include <KLocalizedString>

#include <QDebug>
#include <QApplication>
#include <QDesktopWidget>
#include <QProcess>
#include <QUuid>
#include <QLocale>

#if defined Q_OS_WIN
#include <windows.h>
#endif

#if defined HAVE_UNAME
# include <sys/utsname.h>
#endif

/*! Version of the feedback data format.
 Changelog:
 1.0: initial version
 1.1: added JSON-compatible character escaping; added uid
 1.2: added os_release, os_machine, screen_count
 1.3: added running_desktop, running_desktop_version
*/
static const char KexiUserFeedbackAgent_VERSION[] = "1.3";

class KexiUserFeedbackAgent::Private
{
public:
    Private()
     : configGroup(KConfigGroup(KSharedConfig::openConfig()->group("User Feedback")))
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
    qDebug();
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
        const QByteArray desktop = qgetenv("XDG_CURRENT_DESKTOP").trimmed().toLower();
        const QByteArray gdm = qgetenv("GDMSESSION").trimmed().toLower();
        const bool kdeSession = qgetenv("KDE_FULL_SESSION").trimmed().toLower() == "true";
        // detect running desktop
        // http://standards.freedesktop.org/menu-spec/latest/apb.html
        QString runningDesktop;
        QString runningDesktopVersion;
        //! @todo set runningDesktopVersion for other desktops
        if (desktop.contains("kde") || kdeSession || gdm.contains("kde")) {
            runningDesktop = "KDE Plasma";
            //! @todo run kde{4|5}-config --kde-version to know full version and save in runningDesktopVersion
            runningDesktopVersion = qgetenv("KDE_SESSION_VERSION").trimmed();
        }
        else if (desktop.contains("unity")) {
            runningDesktop = "Unity";
        }
        else if (desktop.contains("razor")) {
            runningDesktop = "Razor-qt";
        }
        else if (desktop.contains("rox")) {
            runningDesktop = "ROX";
        }
        else if (desktop.contains("tde")) {
            runningDesktop = "Trinity";
        }
        else if (desktop.contains("mate")) {
            runningDesktop = "MATE";
        }
        else if (desktop.contains("lxde") || gdm.contains("lubuntu")) {
            runningDesktop = "LXDE";
        }
        else if (desktop.contains("xfce") || gdm.contains("xfce")) {
            runningDesktop = "Xfce";
        }
        else if (desktop.contains("ede")) {
            runningDesktop = "EDE";
        }
        else if (desktop.contains("cinnamon")) {
            runningDesktop = "Cinnamon";
        }
        else if (desktop.contains("gnome") || gdm.contains("gnome")) {
            if (gdm.contains("cinnamon")) {
                runningDesktop = "Cinnamon";
            }
            else if (gdm.contains("classic")) {
                runningDesktop = "GNOME Classic";
            }
            else {
                runningDesktop = "GNOME";
            }
        }
        else {
            if (!desktop.isEmpty()) {
                runningDesktop = "Other: " + desktop;
            }
            else if (!gdm.isEmpty()) {
                runningDesktop = "Other: " + gdm;
            }
        }
        if (!runningDesktop.isEmpty()) {
            ADD("running_desktop", runningDesktop, SystemInfoArea);
        }
        if (!runningDesktopVersion.isEmpty()) {
            ADD("running_desktop_version", runningDesktopVersion, SystemInfoArea);
        }
        // retrieve distribution name and release
        QProcess p;
        p.start("lsb_release", QStringList() << "-i" << "-r" << "-d");
        if (p.waitForFinished()) {
            QString info = p.readLine().replace("Distributor ID:", "").trimmed();
            if (info.toLower() == "ubuntu") { // Ubuntu derivatives (http://askubuntu.com/a/227669/226642)
                if (runningDesktop == "KDE Plasma") {
                    info = "Kubuntu";
                }
                else if (runningDesktop ==  "LXDE") {
                    info = "Lubuntu";
                }
                else if (runningDesktop == "Xfce") {
                    info = "Xubuntu";
                }
                else if (runningDesktop.contains(QLatin1String("gnome"), Qt::CaseInsensitive)) {
                    info = "Ubuntu GNOME";
                }
            }
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
#elif defined(Q_OS_MAC)
    ADD("os", "mac", SystemInfoArea);
#elif defined(Q_OS_WIN)
    ADD("os", "windows", SystemInfoArea);
#else
//! @todo BSD?
    ADD("os", "other", SystemInfoArea);
#endif

#if defined HAVE_UNAME
    struct utsname buf;
    if (uname(&buf) == 0) {
        ADD("os_release", buf.release, SystemInfoArea);
        ADD("os_machine", buf.machine, SystemInfoArea);
    }
#elif defined(Q_OS_WIN)
    OSVERSIONINFO versionInfo;
    SYSTEM_INFO sysInfo;
    char* releaseStr;
    releaseStr = new char[6]; // "xx.xx\0"
    
    versionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&versionInfo);
    GetSystemInfo(&sysInfo);
    
    snprintf(releaseStr, 6, "%2d.%2d", versionInfo.dwMajorVersion, versionInfo.dwMinorVersion);
    ADD("os_release", releaseStr, SystemInfoArea);
    
    delete [6] releaseStr;
    
    switch(sysInfo.wProcessorArchitecture) {
    case PROCESSOR_ARCHITECTURE_AMD64:
        ADD("os_machine", "x86_64", SystemInfoArea);
        break;
    case PROCESSOR_ARCHITECTURE_IA64:
        ADD("os_machine", "ia64", SystemInfoArea);
        break;
    case PROCESSOR_ARCHITECTURE_INTEL:
        ADD("os_machine", "x86", SystemInfoArea);
        break;
    default:
        ADD("os_machine", "unknown", SystemInfoArea);
    }
#endif

    QSize screen(QApplication::desktop()->screenGeometry(
                        KexiMainWindowIface::global()->thisWidget()).size());
    ADD("screen_width", screen.width(), ScreenInfoArea);
    ADD("screen_height", screen.height(), ScreenInfoArea);
    ADD("screen_count", QApplication::desktop()->screenCount(), ScreenInfoArea);

    QLocale locale;
    ADD("country", QLocale::countryToString(locale.country()), RegionalSettingsArea);
    ADD("language", QLocale::languageToString(locale.language()), RegionalSettingsArea);
    ADD("date_format", locale.dateFormat(QLocale::LongFormat), RegionalSettingsArea);
    ADD("short_date_format", locale.dateFormat(QLocale::ShortFormat), RegionalSettingsArea);
    ADD("time_format", locale.timeFormat(QLocale::LongFormat), RegionalSettingsArea);
    //! @todo KEXI3 "short_time_format"
    ADD("right_to_left", QApplication::isRightToLeft(), RegionalSettingsArea);
#undef ADD
}

// ---

KexiUserFeedbackAgent::KexiUserFeedbackAgent(QObject* parent)
 : QObject(parent), d(new Private)
{
    if (d->configGroup.readEntry("BasicInfo", false)) {
        d->areas |= BasicArea | AnonymousIdentificationArea;
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
    qDebug();
    if (d->areas == NoAreas) {
        return;
    }
    if (KGlobal::mainComponent().aboutData()->programName() != i18n(KEXI_APP_NAME)) {
        // Do not send feedback if this is not really Kexi but a test app based on Kexi
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
    qDebug() << postData;
    
    KIO::Job* sendJob = KIO::storedHttpPost(postData, QUrl(d->url + "/send"), KIO::HideProgressInfo);
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
    qDebug() << result;
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
    qDebug() << postData;
    KIO::Job* sendJob = KIO::storedHttpPost(postData, QUrl(d->url + "/send"), KIO::HideProgressInfo);
    connect(sendJob, SIGNAL(result(KJob*)), this, SLOT(sendRedirectQuestionFinished(KJob*)));
    sendJob->addMetaData("content-type", "Content-Type: application/x-www-form-urlencoded");
}

void KexiUserFeedbackAgent::sendRedirectQuestionFinished(KJob* job)
{
    if (job->error()) {
        //! @todo error...
        qDebug() << "Error, no URL Redirect";
    }
    else {
        KIO::StoredTransferJob* sendJob = qobject_cast<KIO::StoredTransferJob*>(job);
        QByteArray result = sendJob->data();
        result.chop(1); // remove \n
        qDebug() << result;
        if (result.isEmpty()) {
            qDebug() << "No URL Redirect";
        }
        else {
            d->url = QString::fromUtf8(result);
            qDebug() << "URL Redirect to" << d->url;
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
