/*
 *  Copyright (c) 2008-2009 Hyves (Startphone Ltd.)
 *  Copyright (c) 2014 Boudewijn Rempt <boud@valdyas.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 */
#include "mainwindow.h"

#include <QtCore>
#include <QtGui>
#include <QtNetwork>

#include <kglobal.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <kstandarddirs.h>
#include <kconfig.h>
#include <kconfiggroup.h>

#include <calligraversion.h>
#include <calligragitversion.h>

#include <cstdlib>

#include <KoConfig.h>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

//#ifdef Q_WS_WIN
//#include <cstring>
//#include <windows.h>
//#include <shellapi.h>
///**
// * Native Win32 method for starting a process. This is required in order to
// * launch the installer with User Account Control enabled.
// *
// * @param path Path to the process to start.
// * @param parameters Parameters for the process.
// * @return @c true if the process is started successfully, @c false otherwise.
// */
//bool startProcess(LPCWSTR path, char *parameters = 0) {

//    Q_ASSERT(path != 0);

//    SHELLEXECUTEINFO info;
//    memset(&info, '\0', sizeof(info));

//    info.cbSize = sizeof(info);
//    info.fMask = 0;
//    info.hwnd = 0;
//    info.lpVerb = TEXT("open");
//    info.lpFile = path;
//    info.lpParameters = (LPCWSTR)parameters;
//    info.lpDirectory = 0;
//    info.nShow = SW_SHOWNORMAL;
//    return ShellExecuteEx(&info);
//}

//void doRestart(bool resetConfig)
//{
//    if (!startProcess(QString("krita").toStdWString().data())) {
//        QMessageBox::warning(0, "Krita",
//                             i18n("Could not restart %1. Please try to restart %1 manually.").arg("krita"));
//    }
//}

//#else // Q_WS_WIN

class GfxCheckIntelInfo
{
public:
    GfxCheckIntelInfo(const QString& card, int v1, int v2, int v3, int v4)
    : cardString(card)
    {
        version[0] = v1;
        version[1] = v2;
        version[2] = v3;
        version[3] = v4;
    }
    QString cardString;
    int version[4];
};

void doResetConfig(const QString &applicationId)
{
    QString applicationExecutable = MainWindow::getApplicationExeFromId(applicationId);
    QString appRcFile = applicationExecutable + "rc";

    {
        QString appdata = qgetenv("APPDATA");
        QDir inputDir(appdata + "/krita/share/apps/krita/input/");
        foreach(QString entry, inputDir.entryList(QStringList("*.profile"))) {
            inputDir.remove(entry);
        }
        QDir configDir(appdata + "/krita/share/config/");
        configDir.remove(appRcFile);
    }
    {
        QString appdata = qgetenv("LOCALAPPDATA");
        QDir inputDir(appdata + "/krita/share/apps/krita/input/");
        foreach(QString entry, inputDir.entryList(QStringList("*.profile"))) {
            inputDir.remove(entry);
        }
        QDir configDir(appdata + "/krita/share/config/");
        configDir.remove(appRcFile);
    }
    {
        QDir inputDir(KGlobal::dirs()->saveLocation("appdata", "input/"));
        foreach(QString entry, inputDir.entryList(QStringList("*.profile"))) {
            inputDir.remove(entry);
        }
        QDir configDir(KGlobal::dirs()->saveLocation("config"));
        configDir.remove(appRcFile);
    }
}

void doRestart(MainWindow* mainWindow, const QString &applicationId, bool resetConfig)
{
    QString applicationExecutable = MainWindow::getApplicationExeFromId(applicationId);

    if (resetConfig) {
        doResetConfig(applicationId);
    }

    QString restartCommand;

#ifdef Q_WS_MAC
    QDir bundleDir(qApp->applicationDirPath());

    bundleDir.cdUp();
    bundleDir.cdUp();
    bundleDir.cdUp();

    restartCommand = QString("open \"") + QString(bundleDir.absolutePath() + "/" + applicationExecutable + ".app\"");
#endif

#ifdef Q_WS_WIN
    restartCommand = qApp->applicationDirPath().replace(' ', "\\ ") + "/" + applicationExecutable + ".exe \"";
#endif

#ifdef Q_WS_X11
    restartCommand = "sh -c \"" + qApp->applicationDirPath().replace(' ', "\\ ") + "/" + applicationExecutable + "\"";
#endif

    qDebug() << "restartCommand" << restartCommand;
    QProcess restartProcess;
    if (!restartProcess.startDetached(restartCommand)) {
        QMessageBox::warning(mainWindow, "krita",
                             i18n("Could not restart Krita. Please try to restart manually."));
    }
}

#ifdef Q_WS_MAC
QString platformToStringMac(QSysInfo::MacVersion version)
{
    switch(version) {
    case QSysInfo::MV_9:
        return "MacOS 9";
    case QSysInfo::MV_10_0:
        return "OSX 10.0 (cheetah)";
    case QSysInfo::MV_10_1:
        return "OSX 10.1 (puma)";
    case QSysInfo::MV_10_2:
        return "OSX 10.2 (jaguar)";
    case QSysInfo::MV_10_3:
        return "OSX 10.3 (panther)";
    case QSysInfo::MV_10_4:
        return "OSX 10.4 (tiger)";
    case QSysInfo::MV_10_5:
        return "OSX 10.5 (leopard)";
    case QSysInfo::MV_10_6:
        return "OSX 10.6 (snow leopard)";
    case QSysInfo::MV_10_7:
        return "OSX 10.6 (Lion)";
    case QSysInfo::MV_10_8:
        return "OSX 10.6 (Mountain Lion)";
    case QSysInfo::MV_Unknown:
    default:
        return "Unknown OSX version";
    };
}
#endif

#ifdef Q_WS_WIN
QString platformToStringWin(QSysInfo::WinVersion version)
{
    switch(version) {
    case QSysInfo::WV_32s:
        return "Windows 3.1 with win32s";
    case QSysInfo::WV_95:
        return "Windows 95";
    case QSysInfo::WV_98:
        return "Windows 98";
    case QSysInfo::WV_Me:
        return "Windows Me";
    case QSysInfo::WV_NT:
        return "Windows NT";
    case QSysInfo::WV_2000:
        return "Windows 2000";
    case QSysInfo::WV_XP:
        return "Windows XP";
    case QSysInfo::WV_2003:
        return "Windows 2003";
    case QSysInfo::WV_VISTA:
        return "Windows Vista";
    case QSysInfo::WV_WINDOWS7:
        return "Windows 7";
    case QSysInfo::WV_WINDOWS8:
        return "Windows 8";
#if QT_VERSION >= 0x040806
    case QSysInfo::WV_WINDOWS8_1:
        return "Windows 8.1";
#endif
    default:
        return "Unknown Windows version";
    };

}
#endif

struct MainWindow::Private {
    QString dumpPath;
    QString id;
    QString applicationId;
    QNetworkAccessManager *networkAccessManager;

    QString graphicsWarning;
    QString graphicsInformation;

    bool doRestart;
    bool uploadStarted;

    Private() :
        doRestart(true),
        uploadStarted(false) {
    }
};

typedef QPair<QByteArray, QByteArray> Field;

MainWindow::MainWindow(const QString &dumpPath, const QString &id, const QString &applicationId, QWidget *parent)
    : QWidget(parent)
    , m_d(new Private())
{
    setupUi(this);
    progressBar->hide();

    lblKiki->setPixmap(QPixmap(KGlobal::dirs()->findResource("data", "krita/pics/KikiNurse_sm.png")));

    setWindowFlags(Qt::WindowStaysOnTopHint | windowFlags());

    m_d->networkAccessManager = new QNetworkAccessManager(this);
    connect(m_d->networkAccessManager, SIGNAL(finished(QNetworkReply *)), SLOT(uploadDone(QNetworkReply *)));

    connect(chkAllowUpload, SIGNAL(stateChanged(int)), this, SLOT(onToggleAllowUpload(int)));
    connect(bnRestart, SIGNAL(clicked()), this, SLOT(restart()));
    connect(bnClose, SIGNAL(clicked()), this, SLOT(close()));

    m_d->dumpPath = dumpPath;
    m_d->id = id;
    m_d->applicationId = applicationId;

    if (checkForOpenglProblems()) {
        // driver issue found
        QPalette warning_palette;
        warning_palette.setColor(QPalette::WindowText, Qt::red);
        lblDriverWarning->setPalette(warning_palette);

        lblDriverWarning->setText(m_d->graphicsInformation);
    }
}

MainWindow::~MainWindow()
{
    delete m_d;
}

void MainWindow::restart()
{
    m_d->doRestart = true;
    if (chkAllowUpload->isChecked()) {
        startUpload();
    }
    else {
        doRestart(this, m_d->applicationId, chkRemoveSettings->isChecked());
        qApp->quit();
    }
}

void MainWindow::close()
{
    m_d->doRestart = false;
    if (!m_d->uploadStarted && chkAllowUpload->isChecked()) {
        startUpload();
    }
    else {
        if (chkRemoveSettings->isChecked())
        {
            doResetConfig(m_d->applicationId);
        }
        qApp->quit();
    }
}

void MainWindow::onToggleAllowUpload(int state)
{
    switch(state) {
    case Qt::Unchecked:
        bnClose->setText("&Close");
        bnRestart->setText("&Restart");
        break;
    case Qt::Checked:
        bnClose->setText("Send && &Close");
        bnRestart->setText("Send && R&estart &Krita");
        break;
    }
}

void MainWindow::startUpload()
{
    bnRestart->setEnabled(false);
    bnClose->setEnabled(false);
    progressBar->show();

    m_d->uploadStarted = true;

    // Upload minidump
    QNetworkRequest request(QUrl("http://krita-breakpad.kogmbh.net:1127/post"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "multipart/form-data; boundary=9876543210");

    QString boundary = "--9876543210";

    QList<Field> fields;

    QString calligraVersion(CALLIGRA_VERSION_STRING);
    QString version;


#ifdef CALLIGRA_GIT_SHA1_STRING
    QString gitVersion(CALLIGRA_GIT_SHA1_STRING);
    version = QString("%1-%2").arg(calligraVersion).arg(gitVersion).toLatin1();
#else
    version = calligraVersion;
#endif

    fields << Field("BuildID", CALLIGRA_GIT_SHA1_STRING)
           << Field("ProductName", m_d->applicationId.toLatin1())
           << Field("Version", version.toLatin1())
           << Field("Vendor", "KO GmbH")
           << Field("Email", txtEmail->text().toLatin1())
           << Field("timestamp", QByteArray::number(QDateTime::currentDateTime().toTime_t()));

#ifdef Q_WS_WIN

    QString platform = platformToStringWin(QSysInfo::WindowsVersion);

#ifdef ENV32BIT
    platform += "_x86";
#else
    platform += "_x64";
#endif

    fields << Field("Platform", platform.toLatin1());
#endif
#ifdef Q_WS_X11
    fields << Field("Platform", "Linux/X11");
#endif
#ifdef Q_WS_MAC
    fields << Field("Platform", platformToStringMac(QSysInfo::MacintoshVersion).toLatin1());
#endif

    addFileAsField(QDesktopServices::storageLocation(QDesktopServices::TempLocation) + "/krita-startup.txt",
                   "Startup",
                   &fields);

    addFileAsField(QDesktopServices::storageLocation(QDesktopServices::TempLocation) + "/krita-opengl.txt",
                   "OpenGL",
                   &fields);

    QString body;
    foreach(Field const field, fields) {
        body += boundary + "\r\n";
        body += "Content-Disposition: form-data; name=\"" + field.first + "\"\r\n\r\n";
        body += field.second + "\r\n";
    }
    body += boundary + "\r\n";

    // add minidump file
    QString dumpfile = m_d->dumpPath + "/" + m_d->id + ".dmp";
    qDebug() << "dumpfile" << dumpfile;
    body += "Content-Disposition: form-data; name=\"upload_file_minidump\"; filename=\""
            + QFileInfo(dumpfile).fileName().toLatin1() + "\"\r\n";
    body += "Content-Type: application/octet-stream\r\n\r\n";
    QFile file(dumpfile);
    if (file.exists()) {
        file.open(QFile::ReadOnly);
        QByteArray ba = file.readAll();
        body += ba.toBase64();
        file.remove();
    }
    body += "\r\n";

    // add description
    body += boundary + "\r\n";
    body += "Content-Disposition: form-data; name=\"description\"\r\n";
    body += "\r\n";
    body += txtDescription->toPlainText();

    body += "\r\n";
    body += boundary + "--" + "\r\n";

    QFile report(QDir::homePath() + "/krita-" + m_d->id + ".report");
    report.open(QFile::WriteOnly);
    report.write(body.toLatin1());
    report.close();

    QNetworkReply *reply = m_d->networkAccessManager->post(request, body.toLatin1());
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(uploadError(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(uploadProgress(qint64, qint64)), this, SLOT(uploadProgress(qint64,qint64)));
}


void MainWindow::addFileAsField(const QString &filename, const QString &fieldName, QList<QPair<QByteArray, QByteArray>> *fields)
{
    QFile f(filename);

    qDebug() << filename << f.exists();

    if (f.exists()) {
        f.open(QFile::ReadOnly);
        *fields << Field(fieldName.toUtf8(), f.readAll());
    }
}

void MainWindow::uploadDone(QNetworkReply *reply)
{
    qDebug() << "updloadDone";
    if (reply && reply->error() != QNetworkReply::NoError) {
        qCritical() << "uploadDone: Error uploading crash report: " << reply->errorString();
    }
    if (m_d->doRestart) {
        doRestart(this, m_d->applicationId, chkRemoveSettings->isChecked());
    } else if (chkRemoveSettings->isChecked()) {
        doResetConfig(m_d->applicationId);
    }
    qApp->quit();

}

void MainWindow::uploadProgress(qint64 received, qint64 total)
{
    qDebug() << "updloadProgress";
    progressBar->setMaximum(total);
    progressBar->setValue(received);
}

void MainWindow::uploadError(QNetworkReply::NetworkError error)
{
    qDebug() << "updloadError" << error;
    // Fake success...
    progressBar->setRange(0, 100);
    progressBar->setValue(100);
    qCritical() << "UploadError: Error uploading crash report: " << error;

    uploadDone(0);
}

QString MainWindow::getApplicationExeFromId(const QString &applicationId)
{
    QString applicationExecutable;

    if (applicationId == "") {
        applicationExecutable = "";
    } else if (applicationId == "kritaanimationsteam") {
        applicationExecutable = "kritaanimation";
    } else if (applicationId == "kritageministeam") {
        applicationExecutable = "kritagemini";
    } else if (applicationId == "kritasketchsteam") {
        applicationExecutable = "kritasketch";
    } else {
        applicationExecutable = applicationId;
    }

    return applicationExecutable;
}

/**
 * @brief Inspect the contents of the OpenGL info file and alert the user if they might be running an old version
 *
 */
bool MainWindow::checkForOpenglProblems()
{
    bool showWarning = false;
    QFile f(QDesktopServices::storageLocation(QDesktopServices::TempLocation) + "/krita-opengl.txt");
    QString openglString;

    const QString AMD_DRIVER_STRING = "ATI Technologies Inc.";
    const QString AMD_TAG_STRING = "AMD";

    const QString NVIDIA_DRIVER_STRING = "NVIDIA Corporation";
    const QString NVIDIA_TAG_STRING = "GeForce";

    const QString INTEL_DRIVER_STRING = "Intel";
    const QString INTEL_CARD_STRING = "HD Graphics";
    const QString INTEL_BUILD_STRING = "Build";


    if (f.exists()) {
        // (otherwise krita crashed before initialising the opengl canvas)
        if (f.open(QFile::ReadOnly)) {
            openglString = f.readAll();
            f.close();

            openglString = openglString.section("\r", 0, 0);

            // Check for AMD/ATI drivers
            if (openglString.contains(AMD_DRIVER_STRING)
                    || openglString.contains(AMD_TAG_STRING)) {

                showWarning = checkAmdDriver(openglString);
            } // end check for AMD/ATi drivers

            // Check nVidia drivers here
            if (openglString.contains(NVIDIA_DRIVER_STRING)
                || openglString.contains(NVIDIA_TAG_STRING)) {
                // The string from NVIDIA cards are less descriptive
                showWarning = checkNvidiaDriver(openglString);
            }

            // Check for intel drivers here
            if (openglString.contains(INTEL_DRIVER_STRING)
                || openglString.contains(INTEL_CARD_STRING)) {
                // The string from NVIDIA cards are less descriptive
                showWarning = checkIntelDriver(openglString);
            }
        }

        /*
        if (showWarning) {
            QMessageBox msg;
            msg.setWindowTitle("Graphics");
            msg.setText(m_d->graphicsWarning);
            msg.setInformativeText(m_d->graphicsInformation);

            msg.setIcon(QMessageBox::Critical);
            msg.exec();
        }
        */
    }

    return showWarning;
}

bool MainWindow::checkAmdDriver(const QString& openglString)
{
    const QString AMD_CONTEXT_STRING = "Compatibility Profile Context";

    bool showWarning = false;
    QString driverVersionString;
    QStringList stringParts = openglString.split(",");

    if (stringParts.size()>=3) {
        // Third part is of the form:  (opengl version) Compatibility Profile Context (driver version)
        // version is stored at the end of the third part
        int index = stringParts[2].indexOf(AMD_CONTEXT_STRING);
        if (index != -1) {
            index += AMD_CONTEXT_STRING.size() + 1;
            if (index < stringParts[2].size()) {
                driverVersionString = stringParts[2].mid(index).trimmed();
                QStringList driverVersionParts = driverVersionString.split(".");

                if (driverVersionParts.size()>=2) {
                    int mainVer = driverVersionParts[0].toInt();
                    int minorVer = driverVersionParts[1].toInt();
                    showWarning = ( mainVer < 14)
                                  || ((mainVer == 14) && (minorVer < 4));

                    if (showWarning) {
                        m_d->graphicsWarning = "Possible outdated AMD driver";
                        m_d->graphicsInformation = "It appears that you may be using AMD graphics (Radeon) with older drivers. These are known to cause problems with Krita. You should visit support.amd.com to download the latest driver.";                               }
                }
            }
        }
    }
    return showWarning;
}

/**
 * @brief Checks for potential issues with NVIDIA graphics
 *
 * Examines the opengl string which is the form: <GL_VENDOR>, <GL_RENDERER>, <GL_VERSION>
 * The issue is that GL_VERSION for the GeForce driver does not give any driver version
 * information, justt the OpenGL version supported
 * more driver info
 */
bool MainWindow::checkNvidiaDriver(const QString& openglString)
{
    bool showWarning = false;
    QString driverVersionString;
    QStringList stringParts = openglString.split(",");

    if (stringParts.size()>=3) {
        // Third part is of the form:  (opengl version)
        // We can only really test against a known working version 4.4.0
        driverVersionString = stringParts[2].trimmed();
        QStringList driverVersionParts = driverVersionString.split(".");

        if (driverVersionParts.size()>=2) {
            int mainVer = driverVersionParts[0].toInt();
            int minorVer = driverVersionParts[1].toInt();
            showWarning = ( mainVer < 4)
                          || ((mainVer == 4) && (minorVer < 4));

            if (showWarning) {
                m_d->graphicsWarning = "Possible outdated NVIDIA driver";
                m_d->graphicsInformation = "It appears that you may be using NVIDIA graphics (GeForce) with older drivers. These are known to cause problems with Krita. You should visit www.geforce.com/drivers to download the latest driver.";                               }
        }
    }

    return showWarning;
}

/**
 * @brief Checks for potential issues with NVIDIA graphics
 *
 * Examines the opengl string which is the form: <GL_VENDOR>, <GL_RENDERER>, <GL_VERSION>
 * The issue is that GL_VERSION for the GeForce driver does not give any driver version
 * information, justt the OpenGL version supported
 * more driver info
 */
bool MainWindow::checkIntelDriver(const QString& openglString)
{
    const QString INTEL_BUILD_STRING = "Build";
    bool showWarning = false;
    QString driverVersionString;
    QStringList stringParts = openglString.split(",");

    if (stringParts.size()>=3) {
        // Third part is of the form:  (opengl version) - Build (driver version)
        // version is stored at the end of the third part
        // Known working version is 9.17.10.3040 for HD Graphics 2500
        // Known working version is 10.18.10.3316 for HD Graphics 4400

        QList<GfxCheckIntelInfo> testStrings;
        testStrings.append(GfxCheckIntelInfo("HD Graphics 2500", 9,17,10,3040));
        testStrings.append(GfxCheckIntelInfo("HD Graphics 4400", 10,18,10,3316));
        testStrings.append(GfxCheckIntelInfo(" ", 9,17,10,3040)); // catch for other cards

        int index = stringParts[2].indexOf(INTEL_BUILD_STRING);
        if (index != -1) {
            index += INTEL_BUILD_STRING.size() + 1;
            if (index < stringParts[2].size()) {
                driverVersionString = stringParts[2].mid(index).trimmed();
                QStringList driverVersionParts = driverVersionString.split(".");

                if (driverVersionParts.size()>=4) {
                    int v1 = driverVersionParts[0].toInt();
                    int v2 = driverVersionParts[1].toInt();
                    int v3 = driverVersionParts[2].toInt();
                    int v4 = driverVersionParts[3].toInt();
                    int tv1, tv2, tv3, tv4;

                    for(int infoIndex=0; !showWarning && infoIndex<testStrings.size(); infoIndex++) {

                        if (stringParts[1].contains(testStrings[infoIndex].cardString)) {
                            tv1 = testStrings[infoIndex].version[0];
                            tv2 = testStrings[infoIndex].version[1];
                            tv3 = testStrings[infoIndex].version[2];
                            tv4 = testStrings[infoIndex].version[3];
                            showWarning = ( v1 < tv1)
                                          || ((v1 == tv1) && (v2 < tv2))
                                          || ((v1 == tv1) && (v2 == tv2) && (v3 < tv3))
                                          || ((v1 == tv1) && (v2 == tv2) && (v3 == tv3) && (v4 < tv4));
                        }
                    }

                    if (showWarning) {
                        m_d->graphicsWarning = "Possible outdated Intel driver";
                        m_d->graphicsInformation = "It appears that you may be using Intel graphics with older drivers. These are known to cause problems with Krita. You should visit downloadcenter.intel.com to download the latest driver.";
                    }
                }
            }
        }
    }
    return showWarning;
}
