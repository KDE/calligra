/* This file is part of the KDE project
  Copyright (C) 2017 Dag Andersen <danders@get2net.dk>

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
  Boston, MA 02110-1301, USA.
*/

#include "WelcomeView.h"

#include "kptcommand.h"
#include "kptdebug.h"
#include "WhatsThis.h"

#include <KoApplication.h>
#include <KoMainWindow.h>
#include <KoDocument.h>
#include <KoFileDialog.h>

#include <QStringList>
#include <QStringListModel>
#include <QItemSelectionModel>
#include <QItemSelection>
#include <QUrl>
#include <QIcon>
#include <QStandardPaths>
#include <QEvent>

const QLoggingCategory &PLANWELCOME_LOG()
{
    static const QLoggingCategory category("calligra.plan.welcome");
    return category;
}

#define debugWelcome qCDebug(PLANWELCOME_LOG)
#define warnWelcome qCWarning(PLANWELCOME_LOG)
#define errorWelcome qCCritical(PLANWELCOME_LOG)

namespace KPlato
{

class RecentFilesModel : public QStringListModel
{
public:
    RecentFilesModel(QObject *parent = 0);
    Qt::ItemFlags flags(const QModelIndex &idx) const;
    QVariant data(const QModelIndex &idx, int role) const;
};

RecentFilesModel::RecentFilesModel(QObject *parent)
: QStringListModel(parent)
{
}

Qt::ItemFlags RecentFilesModel::flags(const QModelIndex &idx) const
{
    Qt::ItemFlags f = (QStringListModel::flags(idx) & ~Qt::ItemIsEditable);
    return f;
}

QVariant RecentFilesModel::data(const QModelIndex &idx, int role) const
{
    switch(role) {
        case Qt::DecorationRole:
            return QIcon::fromTheme(QStringLiteral("document-open"));
            break;
        case Qt::FontRole: break;
        default:
            break;
    }
    return QStringListModel::data(idx, role);
}

//-----------------------------------
WelcomeView::WelcomeView(KoPart *part, KoDocument *doc, QWidget *parent)
    : ViewBase(part, doc, parent)
    , m_projectdialog(0)
    , m_filedialog(0)
{
    widget.setupUi(this);
    widget.recentProjects->setBackgroundRole(QPalette::Midlight);

    Help::add(widget.newProjectBtn,
                   xi18nc("@info:whatsthis",
                          "<title>Create a new project</title>"
                          "<para>"
                          "Creates a new project with default values defined in"
                          " <emphasis>Settings</emphasis>."
                          "<nl/>Opens the <emphasis>project dialog</emphasis>"
                          " so you can define project specific properties like"
                          " <resource>Project Name</resource>,"
                          " <resource>Target Start</resource>"
                          " and <resource>- End</resource> times."
                          "<nl/><link url='%1'>More...</link>"
                          "</para>", Help::page("Howto/Create_New_Project")));

    Help::add(widget.createResourceFileBtn,
                   xi18nc("@info:whatsthis",
                          "<title>Shared resources</title>"
                          "<para>"
                          "Create a shared resources file."
                          "<nl/>This enables you to only create your resources once,"
                          " you just refer to your resources file when you create a new project."
                          "<nl/>These resources can then be shared between projects"
                          " to avoid overbooking resources across projects."
                          "<nl/>Shared resources must be defined in a separate file."
                          "<nl/><link url='%1'>More...</link>"
                          "</para>", Help::page("Howto/Create_Shared_Resources")));

    Help::add(widget.recentProjects,
                   xi18nc("@info:whatsthis",
                          "<title>Recent Projects</title>"
                          "<para>"
                          "A list of the 10 most recent project files opened."
                          "</para><para>"
                          "<nl/>This enables you to quickly open projects you have worked on recently."
                          "<nl/><link url='%1'>More...</link>"
                          "</para>", Help::page("Howto")));

    Help::add(widget.introductionBtn,
                   xi18nc("@info:whatsthis",
                          "<title>Introduction to <application>Plan</application></title>"
                          "<para>"
                          "These introductory pages gives you hints and tips on what"
                          " you can use <application>Plan</application> for, and how to use it."
                          "</para>"));

    Help::add(widget.contextHelp,
                   xi18nc("@info:whatsthis",
                          "<title>Context help</title>"
                          "<para>"
                          "Help is available many places using <emphasis>What's This</emphasis>."
                          "<nl/>It is activated using the menu entry <interface>Help->What's this?</interface>"
                          " or the keyboard shortcut <shortcut>Shift+F1</shortcut>."
                          "</para><para>"
                          "In dialogs it is available via the <interface>?</interface> in the dialog title bar."
                          "</para><para>"
                          "If you see <link url='%1'>More...</link> in the text,"
                          " pressing it will display more information from online resources in your browser."
                          "</para>", Help::page("Howto")));

    m_model = new RecentFilesModel(this);
    widget.recentProjects->setModel(m_model);
    setupGui();

    connect(widget.newProjectBtn, SIGNAL(clicked(bool)), this, SLOT(slotNewProject()));
    connect(widget.createResourceFileBtn, SIGNAL(clicked(bool)), this, SLOT(slotCreateResourceFile()));
    connect(widget.openProjectBtn, SIGNAL(clicked(bool)), this, SLOT(slotOpenProject()));
    connect(widget.introductionBtn, SIGNAL(clicked(bool)), this, SIGNAL(showIntroduction()));

    connect(widget.recentProjects->selectionModel(), SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)), this, SLOT(slotRecentFileSelected(const QItemSelection&)));
}

WelcomeView::~WelcomeView()
{
    debugWelcome;
}

void WelcomeView::setRecentFiles(const QStringList &files)
{
    QStringList lst;
    for (const QString &s : files) {
        lst.prepend(s);
    }
    m_model->setStringList(lst);
    widget.recentProjects->resizeColumnToContents(0);
}

void WelcomeView::updateReadWrite(bool /*readwrite */)
{
}

void WelcomeView::setGuiActive(bool activate)
{
    debugPlan<<activate;
}

void WelcomeView::slotRecentFileSelected(const QItemSelection &selected)
{
    QModelIndex idx = selected.indexes().value(0);
    if (idx.isValid()) {
        QString file = idx.data().toString();
        int start = file.indexOf('[');
        int end = file.indexOf(']');
        file = file.left(end).mid(start+1);
        QUrl url = QUrl::fromUserInput(file);
        debugWelcome<<file<<url;
        if (url.isValid()) {
            emit recentProject(url);
            emit finished();
        }
    }
}

void WelcomeView::slotContextMenuRequested(const QModelIndex &/*index*/, const QPoint& /*pos */)
{
    //debugPlan<<index.row()<<","<<index.column()<<":"<<pos;
}

void WelcomeView::slotEnableActions(bool on)
{
    updateActionsEnabled(on);
}

void WelcomeView::updateActionsEnabled(bool /*on */)
{
}

void WelcomeView::setupGui()
{
    // Add the context menu actions for the view options
}

KoPrintJob *WelcomeView::createPrintJob()
{
    return 0;
}

void WelcomeView::slotNewProject()
{
    if (m_filedialog) {
        return;
    }
    Project *p = project();
    if (p) {
        if (!m_projectdialog) {
            m_projectdialog =  new MainProjectDialog(*p, this, false /*edit*/);
            connect(m_projectdialog, SIGNAL(dialogFinished(int)), SLOT(slotProjectEditFinished(int)));
            connect(m_projectdialog, SIGNAL(sigLoadSharedResources(const QString&, const QUrl&, bool)), this, SLOT(slotLoadSharedResources(const QString&, const QUrl&, bool)));
        }
        m_projectdialog->show();
        m_projectdialog->raise();
        m_projectdialog->activateWindow();
    }
}

void WelcomeView::slotProjectEditFinished(int result)
{
    qDebug()<<Q_FUNC_INFO;
    MainProjectDialog *dia = qobject_cast<MainProjectDialog*>(sender());
    if (dia == 0) {
        return;
    }
    if (result == QDialog::Accepted) {
        MacroCommand *cmd = dia->buildCommand();
        if (cmd) {
            cmd->execute();
            delete cmd;
            koDocument()->setModified(true);
        }
        emit projectCreated();
        emit selectDefaultView();
        emit finished();
    }
    dia->deleteLater();
}

void WelcomeView::slotCreateResourceFile()
{
    QString file = QStandardPaths::locate(QStandardPaths::AppDataLocation, "templates/.source/SharedResources.plant");
    emit openTemplate(QUrl::fromUserInput(file));
    emit finished();
}

void WelcomeView::slotOpenProject()
{
    if (m_projectdialog) {
        qWarning()<<Q_FUNC_INFO<<"Project dialog is open";
        return;
    }
    Project *p = project();
    if (p) {
        KoFileDialog filedialog(this, KoFileDialog::OpenFile, "OpenDocument");
        filedialog.setCaption(i18n("Open Document"));
        filedialog.setDefaultDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
        filedialog.setMimeTypeFilters(koApp->mimeFilter(KoFilterManager::Import));
        filedialog.setHideNameFilterDetailsOption();
        QUrl url = QUrl::fromUserInput(filedialog.filename());
        if (!url.isEmpty() && mainWindow()->openDocument(url)) {
            emit finished();
        }
    }
}

void WelcomeView::slotOpenFileFinished(int result)
{
    KoFileDialog *dia = qobject_cast<KoFileDialog*>(sender());
    if (dia == 0) {
        return;
    }
    if (result == QDialog::Accepted) {
        QUrl url = QUrl::fromUserInput(dia->filename());
        if (!url.isEmpty() && mainWindow()->openDocument(url)) {
            emit finished();
        }
    }
    dia->deleteLater();
}

void WelcomeView::slotLoadSharedResources(const QString &file, const QUrl &projects, bool loadProjectsAtStartup)
{
    QUrl url(file);
    if (url.scheme().isEmpty()) {
        url.setScheme("file");
    }
    if (url.isValid()) {
        emit loadSharedResources(url, loadProjectsAtStartup ? projects :QUrl());
    }
}

} // namespace KPlato
