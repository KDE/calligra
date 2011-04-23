/* This file is part of the KDE project
   Copyright (C) 2003-2007 Jarosław Staniek <staniek@kde.org>

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

#include "KexiStartup.h"

#include "kexiproject.h"
#include "kexiprojectdata.h"
#include "kexiprojectset.h"
#include "kexiguimsghandler.h"

#include <kexidb/utils.h>
#include <kexidb/driver.h>
#include <kexidb/drivermanager.h>
#include "KexiStartupDialog.h"
#include "KexiConnSelector.h"
#include "KexiProjectSelector.h"
#include <kexidbconnectionwidget.h>
#include <kexidbshortcutfile.h>

#include <kdebug.h>
#include <klocale.h>
#include <kmimetype.h>
#include <kmessagebox.h>
#include <kcmdlineargs.h>

#include <ktextedit.h>
#include <kuser.h>
#include <KProgressDialog>

#include <unistd.h>

#include <qapplication.h>
#include <qlayout.h>

//! @todo enable this when we need sqlite3-to-someting-newer migration
// #define KEXI_SQLITE_MIGRATION

#ifdef KEXI_SQLITE_MIGRATION
# include "KexiStartup_p.h"
#endif

namespace Kexi
{
K_GLOBAL_STATIC(KexiStartupHandler, _startupHandler)

KexiStartupHandler& startupHandler()
{
    return *_startupHandler;
}
}

//---------------------------------

//! @internal
class KexiStartupHandler::Private
{
public:
    Private()
            : passwordDialog(0)//, showConnectionDetailsExecuted(false)
            , shortcutFile(0), connShortcutFile(0), connDialog(0), startupDialog(0) {
    }

    ~Private() {
        destroyGui();
    }
    void destroyGui() {
        delete passwordDialog;
        passwordDialog = 0;
        delete connDialog;
        connDialog = 0;
        delete startupDialog;
        startupDialog = 0;
    }

    KexiDBPasswordDialog* passwordDialog;
//  bool showConnectionDetailsExecuted;
    KexiDBShortcutFile *shortcutFile;
    KexiDBConnShortcutFile *connShortcutFile;
    KexiDBConnectionDialog *connDialog;
    QString shortcutFileGroupKey;
    KexiStartupDialog *startupDialog;
};

//---------------------------------

static bool stripQuotes(const QString &item, QString &name)
{
    if (item.left(1) == "\"" && item.right(1) == "\"") {
        name = item.mid(1, item.length() - 2);
        return true;
    }
    name = item;
    return false;
}

void updateProgressBar(KProgressDialog *pd, char *buffer, int buflen)
{
    char *p = buffer;
    QByteArray line;
    line.reserve(80);
    for (int i = 0; i < buflen; i++, p++) {
        if ((i == 0 || buffer[i-1] == '\n') && buffer[i] == '%') {
            bool ok;
            int j = 0;
//   char *q=++p;
            ++i;
            line.clear();
            for (;i<buflen && *p >= '0' && *p <= '9'; j++, i++, p++)
                line += *p;
            --i; --p;
            const int percent = line.toInt(&ok);
            if (ok && percent >= 0 && percent <= 100 && pd->progressBar()->value() < percent) {
//    kDebug() << percent;
                pd->progressBar()->setValue(percent);
                qApp->processEvents(QEventLoop::AllEvents, 100);
            }
        }
    }
}

//---------------------------------

KexiDBPasswordDialog::KexiDBPasswordDialog(QWidget *parent, KexiDB::ConnectionData& cdata, bool showDetailsButton)
        : KPasswordDialog(parent, KPasswordDialog::NoFlags,
                          showDetailsButton ? KDialog::User1 : KDialog::None)
        , m_cdata(&cdata)
        , m_showConnectionDetailsRequested(false)
{
    QString msg = "<H2>" + i18n("Opening database") + "</H2><p>"
                  + i18n("Please enter the password.") + "</p>";
    /*  msg += cdata.userName.isEmpty() ?
          "<p>"+i18n("Please enter the password.")
          : "<p>"+i18n("Please enter the password for user.").arg("<b>"+cdata.userName+"</b>");*/

    QString srv = cdata.serverInfoString(false);
    if (srv.isEmpty() || srv.toLower() == "localhost")
        srv = i18n("local database server");

    msg += ("</p><p>" + i18n("Database server: %1", QString("<nobr>") + srv + "</nobr>") + "</p>");

    QString usr;
    if (cdata.userName.isEmpty())
        usr = i18nc("unspecified user", "(unspecified)");
    else
        usr = cdata.userName;

    msg += ("<p>" + i18n("Username: %1", usr) + "</p>");

    setPrompt(msg);
    if (showDetailsButton) {
        connect(this, SIGNAL(user1Clicked()),
                this, SLOT(slotShowConnectionDetails()));
        setButtonText(KDialog::User1, i18n("&Details") + " >>");
    }
    setButtonText(KDialog::Ok, i18n("&Open"));
    setButtonIcon(KDialog::Ok, KIcon("document-open"));
}

KexiDBPasswordDialog::~KexiDBPasswordDialog()
{
}

void KexiDBPasswordDialog::done(int r)
{
    if (r == QDialog::Accepted) {
        m_cdata->password = password();
    }
// if (d->showConnectionDetailsExecuted || ret == QDialog::Accepted) {
    /*   } else {
            m_action = Exit;
            return true;
          }
        }*/
    KPasswordDialog::done(r);
}

void KexiDBPasswordDialog::slotShowConnectionDetails()
{
    m_showConnectionDetailsRequested = true;
    close();
}

//---------------------------------
KexiStartupHandler::KexiStartupHandler()
        : QObject(0)
        , KexiStartupData()
        , d(new Private())
{
    // K_GLOBAL_STATIC is cleaned up *after* QApplication is gone
    // but we have to cleanup before -> use qAddPostRoutine
    qAddPostRoutine(Kexi::_startupHandler.destroy);

    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(slotAboutToAppQuit()));
}

KexiStartupHandler::~KexiStartupHandler()
{
    qRemovePostRoutine(Kexi::_startupHandler.destroy); // post routine is installed!
    delete d;
}

void KexiStartupHandler::slotAboutToAppQuit()
{
    d->destroyGui();
}

bool KexiStartupHandler::getAutoopenObjects(KCmdLineArgs *args, const QByteArray &action_name)
{
    QStringList list = args->getOptionList(action_name);
    bool atLeastOneFound = false;
    foreach(QString option, list) {
        QString type_name, obj_name, item = option;
        int idx;
        bool name_required = true;
        if (action_name == "new") {
            obj_name.clear();
            stripQuotes(item, type_name);
            name_required = false;
        } else {//open, design, text...
            QString defaultType;
            if (action_name == "execute")
                defaultType = "macro";
            else
                defaultType = "table";

            //option with " " (set default type)
            if (stripQuotes(item, obj_name)) {
                type_name = defaultType;
            } else if ((idx = item.indexOf(':')) != -1) {
                //option with type name specified:
                type_name = item.left(idx).toLower();
                obj_name = item.mid(idx + 1);
                //optional: remove ""
                if (obj_name.left(1) == "\"" && obj_name.right(1) == "\"")
                    obj_name = obj_name.mid(1, obj_name.length() - 2);
            } else {
                //just obj. name: set default type name
                obj_name = item;
                type_name = defaultType;
            }
        }
        if (type_name.isEmpty())
            continue;
        if (name_required && obj_name.isEmpty())
            continue;

        atLeastOneFound = true;
        if (projectData()) {
            KexiProjectData::ObjectInfo* info = new KexiProjectData::ObjectInfo();
            info->insert("name", obj_name);
            info->insert("type", type_name);
            info->insert("action", action_name);
            //ok, now add info for this object
            projectData()->autoopenObjects.append(info);
        } else
            return true; //no need to find more because we do not have projectData() anyway
    } //for
    return atLeastOneFound;
}

tristate KexiStartupHandler::init(int /*argc*/, char ** /*argv*/)
{
    m_action = DoNothing;
// d->showConnectionDetailsExecuted = false;
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs(0);
    if (!args)
        return true;

    KexiDB::ConnectionData cdata;

    const QString connectionShortcutFileName(args->getOption("connection"));
    if (!connectionShortcutFileName.isEmpty()) {
        KexiDBConnShortcutFile connectionShortcut(connectionShortcutFileName);
        if (!connectionShortcut.loadConnectionData(cdata)) {
//! @todo Show error message from KexiDBConnShortcutFile when there's one implemented.
//!       For we're displaying generic error msg.
            KMessageBox::sorry(0, "<qt>"
                               + i18n("Could not read connection information from connection shortcut "
                                      "file <nobr>\"%1\"</nobr>.<br><br>Check whether the file has valid contents.",
                                      QDir::convertSeparators(connectionShortcut.fileName())));
            return false;
        }
    }

    if (!args->getOption("dbdriver").isEmpty())
        cdata.driverName = args->getOption("dbdriver");

    QString fileType(args->getOption("type").toLower());
    if (args->count() > 0 && (!fileType.isEmpty() && fileType != "project" && fileType != "shortcut" && fileType != "connection")) {
        KMessageBox::sorry(0,
                           i18n("You have specified invalid argument (\"%1\") for \"type\" command-line option.",
                                fileType));
        return false;
    }

// if (cdata.driverName.isEmpty())
//  cdata.driverName = KexiDB::defaultFileBasedDriverName();
    if (!args->getOption("host").isEmpty())
        cdata.hostName = args->getOption("host");
    if (!args->getOption("local-socket").isEmpty())
        cdata.localSocketFileName = args->getOption("local-socket");
    if (!args->getOption("user").isEmpty())
        cdata.userName = args->getOption("user");
// cdata.password = args->getOption("password");
    bool fileDriverSelected;
    if (cdata.driverName.isEmpty())
        fileDriverSelected = true;
    else {
        KexiDB::DriverManager dm;
        KexiDB::Driver::Info dinfo = dm.driverInfo(cdata.driverName);
        if (dinfo.name.isEmpty()) {
            //driver name provided explicitly, but not found
            KMessageBox::sorry(0, dm.errorMsg());
            return false;
        }
        fileDriverSelected = dinfo.fileBased;
    }
    bool projectFileExists = false;

    //obfuscate the password, if present
//removed
    /*
      for (int i=1; i<(argc-1); i++) {
        if (qstrcmp("--password",argv[i])==0
          || qstrcmp("-password",argv[i])==0)
        {
          QCString pwd(argv[i+1]);
          if (!pwd.isEmpty()) {
            pwd.fill(' ');
            pwd[0]='x';
            qstrcpy(argv[i+1], (const char*)pwd);
          }
          break;
        }
      }
      */

    const QString portStr = args->getOption("port");
    if (!portStr.isEmpty()) {
        bool ok;
        const int p = portStr.toInt(&ok);
        if (ok && p > 0)
            cdata.port = p;
        else {
            KMessageBox::sorry(0,
                               i18n("You have specified invalid port number \"%1\".", portStr));
            return false;
        }
    }

    m_forcedUserMode = args->isSet("user-mode");
    m_forcedDesignMode = args->isSet("design-mode");
    m_isProjectNavigatorVisible = args->isSet("show-navigator");
    m_isMainMenuVisible = !args->isSet("hide-menu");
    bool createDB = args->isSet("createdb");
    const bool alsoOpenDB = args->isSet("create-opendb");
    if (alsoOpenDB)
        createDB = true;
    const bool dropDB = args->isSet("dropdb");
    const bool openExisting = !createDB && !dropDB;
    const bool readOnly = args->isSet("readonly");
    const QString couldnotMsg = QString::fromLatin1("\n")
                                + i18n("Could not start Kexi application this way.");

    if (createDB && dropDB) {
        KMessageBox::sorry(0, i18n(
                               "You have used both \"createdb\" and \"dropdb\" startup options.") + couldnotMsg);
        return false;
    };

    if (createDB || dropDB) {
        if (args->count() < 1) {
            KMessageBox::sorry(0, i18n("No project name specified."));
            return false;
        }
        m_action = Exit;
    }

//TODO: add option for non-gui; integrate with KWallet;
//      move to static KexiProject method
    if (!fileDriverSelected && !cdata.driverName.isEmpty() && cdata.password.isEmpty()) {

        if (cdata.password.isEmpty()) {
            delete d->passwordDialog;
            d->passwordDialog = new KexiDBPasswordDialog(0, cdata, true);
//   connect( d->passwordDialog, SIGNAL(user1Clicked()),
//    this, SLOT(slotShowConnectionDetails()) );
            const int ret = d->passwordDialog->exec();
            if (d->passwordDialog->showConnectionDetailsRequested() || ret == QDialog::Accepted) {
//    if ( ret == QDialog::Accepted ) {
                //  if (QDialog::Accepted == KPasswordDialog::getPassword(pwd, msg)) {
//moved    cdata.password = QString(pwd);
//    }
            } else {
                m_action = Exit;
                return true;
            }
        }
    }

    /* kDebug() << "ARGC==" << args->count();
      for (int i=0;i<args->count();i++) {
        kDebug() << "ARG" <<i<< "= " << args->arg(i);
      }*/

    if (m_forcedUserMode && m_forcedDesignMode) {
        KMessageBox::sorry(0, i18n(
                               "You have used both \"user-mode\" and \"design-mode\" startup options.") + couldnotMsg);
        return false;
    }

    //database filenames, shortcut filenames or db names on a server
    if (args->count() >= 1) {
        QString prjName;
        QString fileName;
        if (fileDriverSelected) {
            fileName = args->arg(0);
        } else {
            prjName = args->arg(0);
        }

        if (fileDriverSelected) {
            QFileInfo finfo(fileName);
            prjName = finfo.fileName(); //filename only, to avoid messy names like when Kexi is started with "../../db" arg
            cdata.setFileName(finfo.absoluteFilePath());
            projectFileExists = finfo.exists();

            if (dropDB && !projectFileExists) {
                KMessageBox::sorry(0,
                                   i18n("Could not remove project.\nThe file \"%1\" does not exist.",
                                        QDir::convertSeparators(cdata.dbFileName())));
                return 0;
            }
        }

        if (createDB) {
            if (cdata.driverName.isEmpty())
                cdata.driverName = KexiDB::defaultFileBasedDriverName();
            m_projectData = new KexiProjectData(cdata, prjName); //dummy
        } else {
            if (fileDriverSelected) {
                int detectOptions = 0;
                if (fileType == "project")
                    detectOptions |= ThisIsAProjectFile;
                else if (fileType == "shortcut")
                    detectOptions |= ThisIsAShortcutToAProjectFile;
                else if (fileType == "connection")
                    detectOptions |= ThisIsAShortcutToAConnectionData;

                if (dropDB)
                    detectOptions |= DontConvert;

                QString detectedDriverName;
                const tristate res = detectActionForFile(m_importActionData, detectedDriverName,
                                     cdata.driverName, cdata.fileName(), 0, detectOptions);
                if (true != res)
                    return res;

                if (m_importActionData) { //importing action
                    m_action = ImportProject;
                    return true;
                }

                //opening action
                cdata.driverName = detectedDriverName;
                if (cdata.driverName == "shortcut") {
                    //get information for a shortcut file
                    d->shortcutFile = new KexiDBShortcutFile(cdata.fileName());
                    m_projectData = new KexiProjectData();
                    if (!d->shortcutFile->loadProjectData(*m_projectData, &d->shortcutFileGroupKey)) {
                        KMessageBox::sorry(0, i18n("Could not open shortcut file\n\"%1\".",
                                                   QDir::convertSeparators(cdata.fileName())));
                        delete m_projectData;
                        m_projectData = 0;
                        delete d->shortcutFile;
                        d->shortcutFile = 0;
                        return false;
                    }
                    d->connDialog = new KexiDBConnectionDialog(0,
                            *m_projectData, d->shortcutFile->fileName());
                    connect(d->connDialog, SIGNAL(saveChanges()),
                            this, SLOT(slotSaveShortcutFileChanges()));
                    int res = d->connDialog->exec();
                    if (res == QDialog::Accepted) {
                        //get (possibly changed) prj data
                        *m_projectData = d->connDialog->currentProjectData();
                    }

                    delete d->connDialog;
                    d->connDialog = 0;
                    delete d->shortcutFile;
                    d->shortcutFile = 0;

                    if (res == QDialog::Rejected) {
                        delete m_projectData;
                        m_projectData = 0;
                        return cancelled;
                    }
                } else if (cdata.driverName == "connection") {
                    //get information for a connection file
                    d->connShortcutFile = new KexiDBConnShortcutFile(cdata.fileName());
                    if (!d->connShortcutFile->loadConnectionData(cdata, &d->shortcutFileGroupKey)) {
                        KMessageBox::sorry(0, i18n("Could not open connection data file\n\"%1\".",
                                                   QDir::convertSeparators(cdata.fileName())));
                        delete d->connShortcutFile;
                        d->connShortcutFile = 0;
                        return false;
                    }
                    bool cancel = false;
                    const bool showConnectionDialog = !args->isSet("skip-conn-dialog");
                    while (true) {
                        if (showConnectionDialog) {
                            //show connection dialog, so user can change parameters
                            if (!d->connDialog) {
                                d->connDialog = new KexiDBConnectionDialog(0,
                                        cdata, d->connShortcutFile->fileName());
                                connect(d->connDialog, SIGNAL(saveChanges()),
                                        this, SLOT(slotSaveShortcutFileChanges()));
                            }
                            const int res = d->connDialog->exec();
                            if (res == QDialog::Accepted) {
                                //get (possibly changed) prj data
                                cdata = *d->connDialog->currentProjectData().constConnectionData();
                            } else {
                                cancel = true;
                                break;
                            }
                        }
                        m_projectData = selectProject(&cdata, cancel);
                        if (m_projectData || cancel || !showConnectionDialog)
                            break;
                    }

                    delete d->connShortcutFile;
                    d->connShortcutFile = 0;
                    delete d->connDialog;
                    d->connDialog = 0;

                    if (cancel)
                        return cancelled;
                } else
                    m_projectData = new KexiProjectData(cdata, prjName);
            } else
                m_projectData = new KexiProjectData(cdata, prjName);

        }
//  if (!m_projectData)
//   return false;
    }
    if (args->count() > 1) {
        //TODO: KRun another Kexi instances
    }

    //let's show connection details, user asked for that in the "password dialog"
    if (d->passwordDialog && d->passwordDialog->showConnectionDetailsRequested()) {
        d->connDialog = new KexiDBConnectionDialog(0, *m_projectData);
//  connect(d->connDialog->tabWidget->mainWidget, SIGNAL(saveChanges()),
//   this, SLOT(slotSaveShortcutFileChanges()));
        int res = d->connDialog->exec();

        if (res == QDialog::Accepted) {
            //get (possibly changed) prj data
            *m_projectData = d->connDialog->currentProjectData();
        }

        delete d->connDialog;
        d->connDialog = 0;

        if (res == QDialog::Rejected) {
            delete m_projectData;
            m_projectData = 0;
            return cancelled;
        }
    }

    //---autoopen objects:
    const bool atLeastOneAOOFound = getAutoopenObjects(args, "open")
                                    || getAutoopenObjects(args, "design")
                                    || getAutoopenObjects(args, "edittext")
                                    || getAutoopenObjects(args, "execute")
                                    || getAutoopenObjects(args, "new")
                                    || getAutoopenObjects(args, "print")
                                    || getAutoopenObjects(args, "print-preview");

    if (atLeastOneAOOFound && !openExisting) {
        KMessageBox::information(0,
                                 i18n("You have specified a few database objects to be opened automatically, "
                                      "using startup options.\n"
                                      "These options will be ignored because it is not available while creating "
                                      "or dropping projects."));
    }

    if (createDB) {
        bool creationNancelled;
        KexiGUIMessageHandler gui;
        KexiProject *prj = KexiProject::createBlankProject(creationNancelled, projectData(), &gui);
        bool ok = prj != 0;
        delete prj;
        if (creationNancelled)
            return cancelled;
        if (!alsoOpenDB) {
            if (ok) {
                KMessageBox::information(0, i18n("Project \"%1\" created successfully.",
                                                 QDir::convertSeparators(projectData()->databaseName())));
            }
            return ok;
        }
    } else if (dropDB) {
        KexiGUIMessageHandler gui;
        tristate res = KexiProject::dropProject(projectData(), &gui, false/*ask*/);
        if (res == true)
            KMessageBox::information(0, i18n("Project \"%1\" dropped successfully.",
                                             QDir::convertSeparators(projectData()->databaseName())));
        return res != false;
    }

    //------

    /* if (m_forcedFinalMode || (m_projectData && projectData->finalMode())) {
        //TODO: maybe also auto allow to open objects...
        KexiMainWindow::initFinal(m_projectData);
        return;
      }*/

    if (!m_projectData) {
        cdata = KexiDB::ConnectionData(); //clear

        if (args->isSet("skip-startup-dialog") || !KexiStartupDialog::shouldBeShown())
            return true;

        if (!d->startupDialog) {
            //create d->startupDialog for reuse because it can be used again after conn err.
            d->startupDialog = new KexiStartupDialog(
                KexiStartupDialog::Everything, KexiStartupDialog::CheckBoxDoNotShowAgain,
                Kexi::connset(), Kexi::recentProjects(), 0);
        }
        if (d->startupDialog->exec() != QDialog::Accepted)
            return true;

        const int r = d->startupDialog->result();
        if (r == KexiStartupDialog::CreateBlankResult) {
            m_action = CreateBlankProject;
            return true;
        } else if (r == KexiStartupDialog::ImportResult) {
            m_action = ImportProject;
            return true;
        } else if (r == KexiStartupDialog::CreateFromTemplateResult) {
            const QString selFile(d->startupDialog->selectedFileName());
            cdata.setFileName(selFile);
            QString detectedDriverName;
            const tristate res = detectActionForFile(m_importActionData, detectedDriverName,
                                 cdata.driverName, selFile);
            if (true != res)
                return res;
            if (m_importActionData || detectedDriverName.isEmpty())
                return false;
            cdata.driverName = detectedDriverName;
            m_projectData = new KexiProjectData(cdata, selFile);
            m_projectData->autoopenObjects = d->startupDialog->autoopenObjects();
            m_action = CreateFromTemplate;
            return true;
        } else if (r == KexiStartupDialog::OpenExistingResult) {
//   kDebug() << "Existing project --------";
            const QString selFile(d->startupDialog->selectedFileName());
            if (!selFile.isEmpty()) {
                //file-based project
//    kDebug() << "Project File: " << selFile;
                cdata.setFileName(selFile);
                QString detectedDriverName;
                const tristate res = detectActionForFile(m_importActionData, detectedDriverName,
                                     cdata.driverName, selFile);
                if (true != res)
                    return res;
                if (m_importActionData) { //importing action
                    m_action = ImportProject;
                    return true;
                }

                if (detectedDriverName.isEmpty())
                    return false;
                cdata.driverName = detectedDriverName;
                m_projectData = new KexiProjectData(cdata, selFile);
            } else if (d->startupDialog->selectedExistingConnection()) {
//    kDebug() << "Existing connection: " <<
//     d->startupDialog->selectedExistingConnection()->serverInfoString();
                KexiDB::ConnectionData *cdata = d->startupDialog->selectedExistingConnection();
                //ok, now we will try to show projects for this connection to the user
                bool cancelled;
                m_projectData = selectProject(cdata, cancelled);
                if ((!m_projectData && !cancelled) || cancelled) {
                    //try again
                    return init(0, 0);
                }
                //not needed anymore
                delete d->startupDialog;
                d->startupDialog = 0;
            }
        } else if (r == KexiStartupDialog::OpenRecentResult) {
//   kDebug() << "Recent project --------";
            const KexiProjectData *data = d->startupDialog->selectedProjectData();
            if (data) {
//    kDebug() << "Selected project: database=" << data->databaseName()
//     << " connection=" << data->constConnectionData()->serverInfoString();
            }
//! @todo
            return data != 0;
        }

        if (!m_projectData)
            return true;
    }

    if (m_projectData && (openExisting || (createDB && alsoOpenDB))) {
        m_projectData->setReadOnly(readOnly);
        m_action = OpenProject;
    }
    //show if wasn't show yet
// importantInfo(true);

    return true;
}

tristate KexiStartupHandler::detectActionForFile(
    KexiStartupData::Import& detectedImportAction, QString& detectedDriverName,
    const QString& _suggestedDriverName, const QString &dbFileName, QWidget *parent, int options)
{
    detectedImportAction = KexiStartupData::Import(); //clear
    QString suggestedDriverName(_suggestedDriverName); //safe
    detectedDriverName.clear();
    QFileInfo finfo(dbFileName);
    if (dbFileName.isEmpty() || !finfo.isReadable()) {
        if (!(options & SkipMessages))
            KMessageBox::sorry(parent, i18n("<p>Could not open project.</p>")
                               + i18n("<p>The file <nobr>\"%1\"</nobr> does not exist or is not readable.</p>",
                                      QDir::convertSeparators(dbFileName))
                               + i18n("Check the file's permissions and whether it is already opened "
                                      "and locked by another application."));
        return false;
    }

    KMimeType::Ptr ptr;
    QString mimename;

    const bool thisIsShortcut = (options & ThisIsAShortcutToAProjectFile)
                                || (options & ThisIsAShortcutToAConnectionData);

    if ((options & ThisIsAProjectFile) || !thisIsShortcut) {
        //try this detection if "project file" mode is forced or no type is forced:
        ptr = KMimeType::findByFileContent(dbFileName);
        mimename = ptr.data() ? ptr.data()->name() : QString();
        kDebug() << "KexiStartupHandler::detectActionForFile(): found mime is: "
        << mimename;
        if (mimename.isEmpty() || mimename == "application/octet-stream" || mimename == "text/plain") {
            //try by URL:
            ptr = KMimeType::findByUrl(KUrl::fromPath(dbFileName));
            mimename = ptr.data()->name();
        }
    }
    if (mimename.isEmpty() || mimename == "application/octet-stream") {
        // perhaps the file is locked
        QFile f(dbFileName);
        if (!f.open(QIODevice::ReadOnly)) {
            // BTW: similar error msg is provided in SQLiteConnection::drv_useDatabase()
            if (!(options & SkipMessages))
                KMessageBox::sorry(parent, i18n("<p>Could not open project.</p>")
                                   + i18n("<p>The file <nobr>\"%1\"</nobr> is not readable.</p>",
                                          QDir::convertSeparators(dbFileName))
                                   + i18n("Check the file's permissions and whether it is already opened "
                                          "and locked by another application."));
            return false;
        }
    }
    if ((options & ThisIsAShortcutToAProjectFile) || mimename == "application/x-kexiproject-shortcut") {
        detectedDriverName = "shortcut";
        return true;
    }

    if ((options & ThisIsAShortcutToAConnectionData) || mimename == "application/x-kexi-connectiondata") {
        detectedDriverName = "connection";
        return true;
    }

    //! @todo rather check this using migration drivers'
    //! X-KexiSupportedMimeTypes [strlist] property
    if (ptr.data()) {
        if (mimename == "application/vnd.ms-access") {
            if ((options & SkipMessages) || KMessageBox::Yes != KMessageBox::questionYesNo(
                        parent, i18n("\"%1\" is an external file of type:\n\"%2\".\n"
                                     "Do you want to import the file as a Kexi project?",
                                     QDir::convertSeparators(dbFileName), ptr.data()->comment()),
                        i18n("Open External File"), KGuiItem(i18n("Import...")), KStandardGuiItem::cancel())) {
                return cancelled;
            }
            detectedImportAction.mimeType = mimename;
            detectedImportAction.fileName = dbFileName;
            return true;
        }
    }

    if (!finfo.isWritable()) {
        //! @todo if file is ro: change project mode (but do not care if we're jsut importing)
    }

    // "application/x-kexiproject-sqlite", etc.:
    QString tmpDriverName = Kexi::driverManager().lookupByMime(mimename).toLatin1();
//@todo What about trying to reuse KOFFICE FILTER CHAINS here?
    bool useDetectedDriver = suggestedDriverName.isEmpty() || suggestedDriverName.toLower() == detectedDriverName.toLower();
    if (!useDetectedDriver) {
        int res = KMessageBox::Yes;
        if (!(options & SkipMessages))
            res = KMessageBox::warningYesNoCancel(parent, i18n(
                                                      "The project file \"%1\" is recognized as compatible with \"%2\" database driver, "
                                                      "while you have asked for \"%3\" database driver to be used.\n"
                                                      "Do you want to use \"%4\" database driver?",
                                                      QDir::convertSeparators(dbFileName),
                                                      tmpDriverName, suggestedDriverName, tmpDriverName));
        if (KMessageBox::Yes == res)
            useDetectedDriver = true;
        else if (KMessageBox::Cancel == res)
            return cancelled;
    }
    if (useDetectedDriver) {
        detectedDriverName = tmpDriverName;
    } else {//use suggested driver
        detectedDriverName = suggestedDriverName;
    }
// kDebug() << "KexiStartupHandler::detectActionForFile(): driver name: " << detectedDriverName;
//hardcoded for convenience:
    const QString newFileFormat = "SQLite3";

#ifdef KEXI_SQLITE_MIGRATION
    if (!(options & DontConvert || options & SkipMessages)
            && detectedDriverName.toLower() == "sqlite2" && detectedDriverName.toLower() != suggestedDriverName.toLower()
            && KMessageBox::Yes == KMessageBox::questionYesNo(parent, i18n(
                        "Previous version of database file format (\"%1\") is detected in the \"%2\" "
                        "project file.\nDo you want to convert the project to a new \"%3\" format (recommended)?",
                        detectedDriverName, QDir::convertSeparators(dbFileName), newFileFormat))) {
        SQLite2ToSQLite3Migration migr(finfo.absoluteFilePath());
        tristate res = migr.run();
//  kDebug() << "--- migr.run() END ---";
        if (!res) {
            //TODO msg
            KMessageBox::sorry(parent, i18n(
                                   "Failed to convert project file \"%1\" to a new \"%2\" format.\n"
                                   "The file format remains unchanged.",
                                   QDir::convertSeparators(dbFileName), newFileFormat));
            //continue...
        }
        if (res == true)
            detectedDriverName = newFileFormat;
    }
#endif
// action.driverName = detectedDriverName;
    if (detectedDriverName.isEmpty()) {
        QString possibleProblemsInfoMsg(Kexi::driverManager().possibleProblemsInfoMsg());
        if (!possibleProblemsInfoMsg.isEmpty()) {
            possibleProblemsInfoMsg.prepend(QString::fromLatin1("<p>") + i18n("Possible problems:"));
            possibleProblemsInfoMsg += QString::fromLatin1("</p>");
        }
        if (!(options & SkipMessages))
            KMessageBox::detailedSorry(parent,
                                       i18n("The file \"%1\" is not recognized as being supported by Kexi.",
                                            QDir::convertSeparators(dbFileName)),
                                       QString::fromLatin1("<p>")
                                       + i18n("Database driver for this file type not found.\nDetected MIME type: %1",
                                              mimename)
                                       + (ptr.data()->comment().isEmpty()
                                          ? QString::fromLatin1(".") : QString::fromLatin1(" (%1).").arg(ptr.data()->comment()))
                                       + QString::fromLatin1("</p>")
                                       + possibleProblemsInfoMsg);
        return false;
    }
    return true;
}

KexiProjectData*
KexiStartupHandler::selectProject(KexiDB::ConnectionData *cdata, bool& cancelled, QWidget *parent)
{
    clearStatus();
    cancelled = false;
    if (!cdata)
        return 0;
    if (!cdata->savePassword && cdata->password.isEmpty()) {
        if (!d->passwordDialog)
            d->passwordDialog = new KexiDBPasswordDialog(0, *cdata, false);
        const int ret = d->passwordDialog->exec();
        if (d->passwordDialog->showConnectionDetailsRequested() || ret == QDialog::Accepted) {

        } else {
            cancelled = true;
            return 0;
        }
    }
    KexiProjectData* projectData = 0;
    //dialog for selecting a project
    KexiProjectSelectorDialog prjdlg(parent, *cdata, true, false);
    if (!prjdlg.projectSet() || prjdlg.projectSet()->error()) {
        KexiGUIMessageHandler msgh;
        if (prjdlg.projectSet())
            msgh.showErrorMessage(prjdlg.projectSet(),
                                  i18n("Could not load list of available projects for <b>%1</b> database server.",
                                       cdata->serverInfoString(true)));
        else
            msgh.showErrorMessage(
                i18n("Could not load list of available projects for <b>%1</b> database server.",
                     cdata->serverInfoString(true)));
//  setStatus(i18n("Could not load list of available projects for database server \"%1\"")
//  .arg(cdata->serverInfoString(true)), prjdlg.projectSet()->errorMsg());
        return 0;
    }
    if (prjdlg.exec() != QDialog::Accepted) {
        cancelled = true;
        return 0;
    }
    if (prjdlg.selectedProjectData()) {
        //deep copy
        projectData = new KexiProjectData(*prjdlg.selectedProjectData());
    }
    return projectData;
}

void KexiStartupHandler::slotSaveShortcutFileChanges()
{
    bool ok = true;
    if (d->shortcutFile)
        ok = d->shortcutFile->saveProjectData(d->connDialog->currentProjectData(),
                                              d->connDialog->savePasswordOptionSelected(),
                                              &d->shortcutFileGroupKey);
    else if (d->connShortcutFile)
        ok = d->connShortcutFile->saveConnectionData(
                 *d->connDialog->currentProjectData().connectionData(),
                 d->connDialog->savePasswordOptionSelected(),
                 &d->shortcutFileGroupKey);

    if (!ok) {
        KMessageBox::sorry(0, i18n("Failed saving connection data to\n\"%1\" file.",
                                   QDir::convertSeparators(d->shortcutFile->fileName())));
    }
}

/*void KexiStartupHandler::slotShowConnectionDetails()
{
  d->passwordDialog->close();
  d->showConnectionDetailsExecuted = true;
}*/

#include "KexiStartup.moc"
