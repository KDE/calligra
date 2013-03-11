/* This file is part of the KDE project
 * Copyright (C) 2012 Smit Patel <smitpatel24@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "BibliographyDatabaseWindow.h"
#include "InsertCitationDialog.h"
#include "EditFiltersDialog.h"
#include "BibliographyTypeEntryDelegate.h"
#include "BibliographyTableModel.h"

#include <BibliographyDb.h>
#include <BibDbFilter.h>

#include <QTableView>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QDir>
#include <QMessageBox>
#include <QVariant>
#include <QAction>
#include <QActionGroup>
#include <QFileDialog>

#include <KoOdfBibliographyConfiguration.h>
#include <klocale.h>
#include <KFileDialog>
#include <KUrl>

BibliographyDatabaseWindow::BibliographyDatabaseWindow(QWidget *parent) :
    QMainWindow(parent),
    m_table(0),
    m_syntax(QRegExp::FixedString),
    m_filters(new QList<BibDbFilter*>)
{
    ui.setupUi(this);
    setupActions();

    m_bibTableView = new QTableView(ui.centralwidget);

    m_bibTableView->setCornerButtonEnabled(true);
    m_bibTableView->setSortingEnabled(true);

    m_bibTableView->horizontalHeader()->setSortIndicatorShown(true);
    m_bibTableView->verticalHeader()->setDefaultSectionSize(20);

    m_bibTableView->setEditTriggers(QTableView::AllEditTriggers);
    m_bibTableView->setSelectionBehavior(QTableView::SelectItems);
    m_bibTableView->resizeColumnsToContents();

    m_bibTableView->show();

    ui.centralwidget->layout()->addWidget(m_bibTableView);

    if (!BibliographyDb::tableDir.exists()) {
        if (!BibliographyDb::tableDir.mkpath(BibliographyDb::tableDir.absolutePath())) {
            QMessageBox::warning(this, i18n("Error"), QString(i18n("Error creating directory ")).append(BibliographyDb::tableDir.absolutePath()));
            emit close();
        }
    }

    connect(ui.tableList, SIGNAL(currentIndexChanged(int)), this, SLOT(tableChanged(int)));
    connect(ui.search, SIGNAL(textChanged(QString)), this, SLOT(searchQueryChanged(QString)));

    if (loadBibliographyDbs() == 0) {
        QFileInfo fileInfo(BibliographyDb::tableDir.absolutePath().append(QDir::separator()).append("biblio.kexi"));
        addTableEntry(fileInfo);
    }
}

BibliographyDatabaseWindow::~BibliographyDatabaseWindow()
{
    if (m_table) {
        m_table->submitAll();
        delete m_table;
        m_table = 0;
    }
}

int BibliographyDatabaseWindow::loadBibliographyDbs()
{
    QFileInfoList tableFiles = BibliographyDb::tableDir.entryInfoList();
    foreach(QFileInfo fileInfo, tableFiles) {
        addTableEntry(fileInfo);
    }

    return tableFiles.size();
}

void BibliographyDatabaseWindow::tableChanged(int index)
{
    if (m_table) {
        m_table->submitAll();
        delete m_table;
    }

    m_table = new BibliographyDb(this, m_tables.at(index).absoluteDir().absolutePath(), m_tables.at(index).fileName());
    m_bibTableView->setItemDelegateForColumn(2, new BibliographyTypeEntryDelegate);

    if (!m_table->isValid()) {
        int ret = QMessageBox::critical(this, i18n("Error opening bibliography database")
                                        , i18n("Do you want to remove %1 from database directory?", QDir::convertSeparators(m_table->getDbPath()))
                                        , QMessageBox::Yes, QMessageBox::No);
        if (ret == QMessageBox::Yes && QFile(m_table->getDbPath()).exists() && !QDir().remove(m_table->getDbPath())) {
            QMessageBox::critical(this, i18n("Could not remove file \"%1\".",QDir::convertSeparators(m_table->getDbPath()))
                                  , i18n("Check the file's permissions and whether it is already opened and locked by another application."));
        } else {
            removeTableEntry(index);
        }
    }

    m_bibTableView->setModel(m_table->tableModel());
    m_bibTableView->hideColumn(0);      // hide ID column


    this->setWindowTitle(QString("Bibliography Database - ").append(m_tables.at(index).fileName()));
    ui.search->clear();                 //clears search query before loading new table

    clearFilters();                     //clear filters
    //We add extra row to insert new citation record
    //m_bibTableView->model()->insertRow(m_bibTableView->model()->rowCount());
}

void BibliographyDatabaseWindow::searchQueryChanged(QString query)
{
    if (query.isEmpty()) {
        m_bibTableView->setModel(m_table->tableModel());
    } else {
        m_bibTableView->setModel(m_table->proxyModel());
    }

    QRegExp searchExp(ui.search->text(), Qt::CaseInsensitive, m_syntax);
    m_table->setSearchFilter(searchExp);
}

void BibliographyDatabaseWindow::insertBlankRow()
{
    //m_bibTableView->model()->insertRow(m_bibTableView->model()->rowCount());
}

void BibliographyDatabaseWindow::setupActions()
{
    ui.actionCitation_record->setStatusTip(i18n("New citation record"));
    connect(ui.actionCitation_record, SIGNAL(triggered()), this, SLOT(newRecord()));

    ui.actionDatabase->setStatusTip(i18n("New citation database"));
    connect(ui.actionDatabase, SIGNAL(triggered()), this, SLOT(newDatabase()));

    ui.actionOpen->setStatusTip(i18n("Open citation database from file"));
    connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));

    ui.actionClose->setStatusTip(i18n("Close bibliography database"));
    connect(ui.actionClose, SIGNAL(triggered()), this, SLOT(close()));

    ui.actionFilter->setStatusTip(i18n("Edit filters"));
    connect(ui.actionFilter, SIGNAL(triggered()), this, SLOT(showFilters()));

    ui.actionClear_filters->setStatusTip(i18n("Clear filters"));
    connect(ui.actionClear_filters, SIGNAL(triggered()), this, SLOT(clearFilters()));

    QActionGroup *searchActions = new QActionGroup(this);

    QAction *action = new QAction(i18n("Regular expression"), this);
    action->setCheckable(true);
    action->setData(QVariant::fromValue<QRegExp::PatternSyntax>(QRegExp::RegExp));
    ui.searchButton->addAction(action);
    searchActions->addAction(action);

    action = new QAction(i18n("Widcard"), this);
    action->setCheckable(true);
    action->setData(QVariant::fromValue<QRegExp::PatternSyntax>(QRegExp::Wildcard));
    ui.searchButton->addAction(action);
    searchActions->addAction(action);

    action = new QAction(i18n("Fixed string"), this);
    action->setCheckable(true);
    action->setData(QVariant::fromValue<QRegExp::PatternSyntax>(QRegExp::FixedString));
    ui.searchButton->addAction(action);
    searchActions->addAction(action)->setChecked(true);

    ui.menuSearch->addActions(searchActions->actions());
    connect(searchActions, SIGNAL(triggered(QAction*)), this, SLOT(searchTypeChanged(QAction*)));
}

void BibliographyDatabaseWindow::searchTypeChanged(QAction *action)
{
    m_syntax = action->data().value<QRegExp::PatternSyntax>();
}

void BibliographyDatabaseWindow::showFilters()
{
    if (m_filters->count() == 0) {
        m_filters->append(new BibDbFilter(false));
    }
    EditFiltersDialog *dialog = new EditFiltersDialog(m_filters, this);
    connect(dialog, SIGNAL(accepted()), this, SLOT(applyFilters()));
}

void BibliographyDatabaseWindow::clearFilters()
{
    m_table->setFilter(0);
    if (!m_filters->isEmpty()) {
        qDeleteAll(m_filters->begin(), m_filters->end());
        m_filters->clear();
        m_table->tableModel()->setFilter(0);
    }
}

void BibliographyDatabaseWindow::applyFilters()
{
    m_table->setFilter(m_filters);
}

void BibliographyDatabaseWindow::newRecord()
{
    if (m_table) {
        new InsertCitationDialog(m_table, this);
    } else {
        QMessageBox::critical(this, i18n("Error opening bibliography database table"), i18n("Select at least one table"), QMessageBox::Ok);
    }
}

void BibliographyDatabaseWindow::openFile()
{
    QString tableFile = KFileDialog::getOpenFileName(
                KUrl(BibliographyDb::tableDir.absolutePath()), i18n("*.kexi|Kexi citation database (*.kexi)"),
                this, i18n("Open Bibliography database table from file"));

    if (!tableFile.isEmpty()) {
        QFileInfo fileInfo(tableFile);
        addTableEntry(fileInfo);
    }
}

void BibliographyDatabaseWindow::newDatabase()
{
    QString fileName = QFileDialog::getSaveFileName(
                this, i18n("Save Bibliography database table to file"),
                BibliographyDb::tableDir.absolutePath(), i18n("Kexi citation database (*.kexi);;All files(*.*)"));

    if (fileName.isEmpty()) {
        return;
    }

    QFileInfo fileInfo(fileName);
    QFile dbFile(fileName);

    if (fileInfo.completeSuffix().isEmpty()) {
        fileInfo.setFile(fileName.append(".kexi"));
    }

    if (m_tables.contains(fileInfo)) {
        removeTableEntry(m_tables.indexOf(fileInfo));

        if (!dbFile.remove()) {
            QMessageBox::warning(this, i18n("Error creating citation database"), i18n("Error while overwriting citation database"));
            return;
        }
    }

    BibliographyDb *table = new BibliographyDb(this, fileInfo.dir().absolutePath(), fileInfo.fileName());
//    if (!table->isValid()) {
//        table->createTable();            //creates bibref table in database file
//    }

    //add to table list
    addTableEntry(fileInfo);
}

void BibliographyDatabaseWindow::addTableEntry(QFileInfo fileInfo)
{
    if (!m_tables.contains(fileInfo)) {
        m_tables.append(fileInfo);
        ui.tableList->addItem(fileInfo.fileName(), QVariant::fromValue<QString>(fileInfo.dir().absolutePath()));
        ui.tableList->setCurrentIndex(m_tables.indexOf(fileInfo));
    } else {
        ui.tableList->setCurrentIndex(m_tables.indexOf(fileInfo));
    }
}

void BibliographyDatabaseWindow::removeTableEntry(int index)
{
    ui.tableList->removeItem(index);
    m_tables.removeAt(index);
}
