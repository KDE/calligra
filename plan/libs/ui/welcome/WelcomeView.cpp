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

#include "kptmainprojectdialog.h"
#include "kptcommand.h"
#include "kptdebug.h"

#include <KoMainWindow.h>
#include <KoDocument.h>
#include <KoFileDialog.h>

#include <QStringList>
#include <QStringListModel>
#include <QItemSelectionModel>
#include <QItemSelection>
#include <QUrl>
#include <QIcon>

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
{
    widget.setupUi(this);
    widget.recentProjects->setBackgroundRole(QPalette::Midlight);

    m_model = new RecentFilesModel(this);
    widget.recentProjects->setModel(m_model);
    setupGui();

    connect(widget.newProjectBtn, SIGNAL(clicked(bool)), this, SLOT(slotNewProject()));
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

void WelcomeView::updateActionsEnabled( bool /*on */)
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
    Project *p = project();
    if (p) {
        MainProjectDialog dlg(*p);
        connect(&dlg, SIGNAL(sigLoadSharedResources(const QString&)), this, SLOT(slotLoadSharedResources(const QString&)));
        if (dlg.exec() == QDialog::Accepted) {
            MacroCommand *m = dlg.buildCommand();
            if (m) {
                koDocument()->addCommand(m);
            }
            emit selectDefaultView();
            emit finished();
        }
    }
}

void WelcomeView::slotOpenProject()
{
    Project *p = project();
    if (p) {
        KoFileDialog dialog(this, KoFileDialog::OpenFile, "OpenDocument");
        dialog.setCaption(i18n("Open Document"));
//         dialog.setDefaultDir(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
//         dialog.setMimeTypeFilters(koApp->mimeFilter(KoFilterManager::Import));
        dialog.setNameFilters(QStringList()<<"Plan files (*.plan)");
        dialog.setHideNameFilterDetailsOption();
        QUrl url = QUrl::fromUserInput(dialog.filename());
        if (!url.isEmpty() && mainWindow()->openDocument(url)) {
            emit finished();
        }
    }
}

void WelcomeView::slotLoadSharedResources(const QString &file)
{
    QUrl url(file);
    if (url.scheme().isEmpty()) {
        url.setScheme("file");
    }
    if (url.isValid()) {
        emit loadSharedResources(url);
    }
}

} // namespace KPlato
