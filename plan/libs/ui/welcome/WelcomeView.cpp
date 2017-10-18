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

    WhatsThis::add(widget.newProjectBtn,
                   i18nc("whatsthis",
                         "<html><head/><body>"
                         "<p><span style='font-weight:600;'>Create a new project</span></p><p>"
                         "Creates a new project with default values defined in"
                         " <span style='font-style:italic;'>Settings</span>."
                         "<br>Opens the <span style='font-style:italic;'>project dialog</span>"
                         " so you can define project specific properties like"
                         " <span style='font-style:italic;whitespace:nobreak'>Project Name</span>,"
                         " <span style='font-style:italic;whitespace:nobreak'>Target Start</span>"
                         " and <span style='font-style:italic;whitespace:nobreak'>- End</span> times."
                         "<br><a href='https://userbase.kde.org/Plan/Howto/'>More...</a>"
                         "</p></body></html>"));

    WhatsThis::add(widget.createResourceFileBtn,
                   i18nc("whatsthis",
                         "<html><head/><body>"
                         "<p><span style='font-weight:600;'>Shared resources</span></p><p>"
                         "Create a shared resources file."
                         "<br>This enables you to only create your resources once,"
                         " you just refer to your resources file when you create a new project."
                         "<br>Resources can also be shared between projects"
                         " to avoid overbooking resources across projects."
                         "<br>Shared resources must be defined in a separate file."
                         "<br><a href='https://userbase.kde.org/Plan/Howto/Create_Shared_Resources'>More...</a>"
                         "</p></body></html>"));

    WhatsThis::add(widget.recentProjects,
                   i18nc("whatsthis",
                         "<html><head/><body>"
                         "<p><span style='font-weight:600;'>Recent Projects</span></p><p>"
                         "<p>A list of the 10 most recent project files opened.</p>"
                         "<br>This enables you to quickly open projects you have worked on recently."
                         "<br><a href='https://userbase.kde.org/Plan/Howto/'>More...</a>"
                         "</p></body></html>"));

    WhatsThis::add(widget.contextHelp,
                   i18nc("whatsthis",
                         "<html><head/><body>"
                         "<p><span style=' font-weight:600;'>Context help</span></p><p>"
                         "Help is available many places using <span style='font-style:italic;white-space:nowrap'>What's This</span>."
                         "<p>It is activated using the menu entry <span style='font-style:italic;white-space:nowrap'>Help->What's this?</span>"
                         " or the keybord shortcut <span style='font-style:italic;white-space:nowrap'>Shift+F1</span>.</p>"
                         "<p>In dialogs it is available via the <span style='font-style:bold;'>?</span> in the dialog title bar."
                         "<p>If you see <a href='https://userbase.kde.org/Plan/Howto'>More...</a> in the text,"
                         " pressing it will display more information from online resources in you browser."
                         "</p></body></html>"));

    WhatsThis::add(widget.otherResourcesLabel,
                   i18nc("whatsthis",
                         "<html><head/><body>"
                         "<p><span style='font-weight:600;'>Other resources</span></p><p>"
                         "Here you find links to online resources."
                         " Information will be opened in you browser."
                         "</p></body></html>"));

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
        if (!m_filedialog) {
            m_filedialog = new QFileDialog(this,i18n("Open Document"));
            m_filedialog->setFileMode(QFileDialog::ExistingFile);
            m_filedialog->setNameFilters(QStringList()<<"Plan files (*.plan)");
            m_filedialog->setOption(QFileDialog::HideNameFilterDetails, true);
            connect(m_filedialog, SIGNAL(finished(int)), this, SLOT(slotOpenFileFinished(int)));
        }
        m_filedialog->show();
        m_filedialog->raise();
        m_filedialog->activateWindow();
    }
}

void WelcomeView::slotOpenFileFinished(int result)
{
    QFileDialog *dia = qobject_cast<QFileDialog*>(sender());
    if (dia == 0) {
        return;
    }
    if (result == QDialog::Accepted) {
        QUrl url = dia->selectedUrls().value(0);
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
