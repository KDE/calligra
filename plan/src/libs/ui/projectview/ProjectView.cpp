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

#include "ProjectView.h"

#include "kptdebug.h"

#include <QTableView>
#include <QHeaderView>
#include <QSqlTableModel>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QDebug>

namespace KPlato
{



ProjectView::ProjectView(KoPart *part, KoDocument *doc, QWidget *parent)
    : ViewBase(part, doc, parent)
{
    debugPlan<<"----------------- Create ProjectView ----------------------";
    QVBoxLayout * l = new QVBoxLayout(this);
    l->setMargin(0);
    m_view = new QTableView(this);
    l->addWidget(m_view);
    if (createConnection()) {
        QSqlTableModel *model = new QSqlTableModel(m_view, QSqlDatabase::database("projects"));
        model->setTable("projects");
        model->setEditStrategy(QSqlTableModel::OnManualSubmit);
        model->select();
        
        model->setHeaderData(0, Qt::Horizontal, i18n("ID"));
        model->setHeaderData(1, Qt::Horizontal, i18n("Name"));
        model->setHeaderData(2, Qt::Horizontal, i18n("Manager"));
        model->setHeaderData(3, Qt::Horizontal, i18n("File"));
        m_view->setModel(model);
    }
    m_view->horizontalHeader()->setSectionHidden(0, true);
}

bool ProjectView::createConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "projects");
    db.setDatabaseName("projects.sqlite");
    if (!db.open()) {
        QMessageBox::critical(0, qApp->tr("Cannot open database"),
                              qApp->tr("Unable to establish a database connection.\n"
                              "This example needs SQLite support. Please read "
                              "the Qt SQL driver documentation for information how "
                              "to build it.\n\n"
                              "Click Cancel to exit."), QMessageBox::Cancel);
        return false;
    }
    
    QSqlQuery query(db);
    QString q = "create table if not exists projects (id integer primary key, "
    "name text, manager text, file text)";
    
    if (!query.exec(q)) {
        debugPlan<<"Table creation failed:"<<query.lastError();
        return false;
    }
    q = "SELECT tbl_name FROM sqlite_master";
    if (query.exec(q)) {
        debugPlan<<"tables:";
        while (query.next()) {
            qDebug() << "\t"<<query.value(0).toString();
        }
    } else {
        debugPlan<<"Table query failed:"<<query.lastError();
    }
    q = "insert into projects (name, manager, file) values ('P1', 'Meg', 'file://jens.plan)";
    query.exec(q);
    return true;
}

void ProjectView::setGuiActive(bool activate)
{
    debugPlan<<activate;
    updateActionsEnabled(true);
    ViewBase::setGuiActive(activate);
}

void ProjectView::slotCurrentChanged(const QModelIndex &, const QModelIndex &)
{
    slotEnableActions();
}

void ProjectView::slotSelectionChanged()
{
    slotEnableActions();
}

QModelIndexList ProjectView::selectedRows() const
{
    return m_view->selectionModel()->selectedRows();
}

int ProjectView::selectedRowCount() const
{
    return selectedRows().count();
}

void ProjectView::slotContextMenuRequested(const QPoint& pos)
{
    debugPlan;
    emit requestPopupMenu("reportsgeneratorview_popup", m_view->mapToGlobal(pos));
}

void ProjectView::slotEnableActions()
{
    updateActionsEnabled(isReadWrite());
}

void ProjectView::updateActionsEnabled(bool on)
{
}

void ProjectView::setupGui()
{
    // Umpff, adding a specific list name for this view in calligraplan.rc does not work!
    // But reusing an already existing name works, so...
    // Not important atm, the whole setup should be refactored anyway.
    QString name = "workpackage_list";

//     KActionCollection *coll = actionCollection();
// 
//     actionAddReport = new QAction(koIcon("list-add"), i18n("Add Report"), this);
//     coll->addAction("add_report", actionAddReport);
//     coll->setDefaultShortcut(actionAddReport, Qt::CTRL + Qt::Key_I);
//     connect(actionAddReport, SIGNAL(triggered(bool)), SLOT(slotAddReport()));
//     addAction(name, actionAddReport);
//     addContextAction(actionAddReport);
// 
//     actionRemoveReport = new QAction(koIcon("list-remove"), i18n("Remove Report"), this);
//     coll->addAction("remove_report", actionRemoveReport);
//     coll->setDefaultShortcut(actionRemoveReport, Qt::CTRL + Qt::Key_D);
//     connect(actionRemoveReport, SIGNAL(triggered(bool)), SLOT(slotRemoveReport()));
//     addAction(name, actionRemoveReport);
//     addContextAction(actionRemoveReport);
// 
//     actionGenerateReport = new QAction(koIcon("document-export"), i18n("Generate Report"), this);
//     coll->addAction("generate_report", actionGenerateReport);
//     coll->setDefaultShortcut(actionGenerateReport, Qt::CTRL + Qt::Key_G);
//     connect(actionGenerateReport, SIGNAL(triggered(bool)), SLOT(slotGenerateReport()));
//     addAction(name, actionGenerateReport);
//     addContextAction(actionGenerateReport);

//     createOptionAction();
}


void ProjectView::slotOptions()
{
    debugPlan;
//     SplitItemViewSettupDialog *dlg = new SplitItemViewSettupDialog(this, m_view, this);
//     dlg->addPrintingOptions();
//     connect(dlg, SIGNAL(finished(int)), SLOT(slotOptionsFinished(int)));
//     dlg->show();
//     dlg->raise();
//     dlg->activateWindow();
}


bool ProjectView::loadContext(const KoXmlElement &context)
{
    debugPlan;
    return true;
}

void ProjectView::saveContext(QDomElement &context) const
{
    debugPlan;
    ViewBase::saveContext(context);
}


} // namespace KPlato
