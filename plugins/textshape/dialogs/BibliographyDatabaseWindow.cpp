/* This file is part of the KDE project
 * Copyright (C) 2011 Smit Patel <smitpatel24@gmail.com>
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
#include "BibliographyDb.h"
#include "InsertCitationDialog.h"

#include <QTableView>
#include <QHeaderView>
#include <QSqlTableModel>
#include <QDir>
#include <QMessageBox>
#include <QVariant>
#include <QDebug>

#include <klocale.h>
#include <KFileDialog>
#include <KUrl>

BibliographyDatabaseWindow::BibliographyDatabaseWindow(QWidget *parent) :
    QMainWindow(parent),
    m_table(0)
{
    ui.setupUi(this);
    setupActions();

    m_bibTableView = new QTableView(ui.centralwidget);
    m_bibTableView->setCornerButtonEnabled(true);
    m_bibTableView->verticalHeader()->setDefaultSectionSize(20);
    m_bibTableView->setEditTriggers(QTableView::AllEditTriggers);

    ui.centralwidget->layout()->addWidget(m_bibTableView);

    if (!tableDir.exists()) {
        if (!tableDir.mkpath(tableDir.absolutePath())) {
            QMessageBox::warning(this, i18n("Error"), QString(i18n("Error creating directory ")).append(tableDir.absolutePath()));
            emit close();
        }
    } else if (loadBibliographyDbs() == 0) {
        ui.tableList->addItem("bibliography.sqlite");
        ui.tableList->setItemData(0, QVariant::fromValue<QString>(tableDir.absolutePath()));

        tableChanged("bibliography.sqlite");
    }

    connect(ui.tableList, SIGNAL(currentIndexChanged(QString)), this, SLOT(tableChanged(QString)));
    ui.tableList->setCurrentIndex(ui.tableList->count() - 1);
}

BibliographyDatabaseWindow::~BibliographyDatabaseWindow()
{
    delete m_table;
}

int BibliographyDatabaseWindow::loadBibliographyDbs()
{
    tableDir = QDir(QDir::home().path().append(QDir::separator()).append(".calligra"),
                                              QString(), QDir::Name, QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    QFileInfoList tableFiles = tableDir.entryInfoList();
    for( int i = 0; i < tableFiles.size(); i++) {
        ui.tableList->addItem(tableFiles.at(i).fileName());
        ui.tableList->setItemData(i, QVariant::fromValue<QString>(tableFiles.at(i).dir().absolutePath()));
    }

    return tableFiles.size();
}

void BibliographyDatabaseWindow::tableChanged(QString newTable)
{
    if (m_table) {
        delete m_table;
    }

    QString dbPath = ui.tableList->itemData(ui.tableList->currentIndex()).value<QString>();
    if (dbPath.isEmpty()) {
        return;
    }

    m_table = new BibliographyDb(this, dbPath, newTable);
    m_bibTableView->setModel(m_table->tableModel());
    m_bibTableView->hideColumn(0);      // hide ID column
    m_bibTableView->resizeColumnsToContents();
    m_bibTableView->horizontalHeader()->setSortIndicatorShown(true);
    m_bibTableView->setSortingEnabled(true);
    m_bibTableView->show();

    this->setWindowTitle(QString("Bibliography Database - ").append(newTable));
    //We add extra row to insert new citation record
    //m_bibTableView->model()->insertRow(m_bibTableView->model()->rowCount());
}

void BibliographyDatabaseWindow::insertBlankRow()
{
    //m_bibTableView->model()->insertRow(m_bibTableView->model()->rowCount());
}

void BibliographyDatabaseWindow::setupActions()
{
    ui.actionNew->setStatusTip(i18n("New citation record"));
    connect(ui.actionNew, SIGNAL(triggered()), this, SLOT(newRecord()));

    ui.actionOpen->setStatusTip(i18n("Open citation database from file"));
    connect(ui.actionOpen, SIGNAL(triggered()), this, SLOT(openFile()));

    ui.actionClose->setStatusTip(i18n("Close bibliography database"));
    connect(ui.actionClose, SIGNAL(triggered()), this, SLOT(close()));
}

void BibliographyDatabaseWindow::newRecord()
{
    new InsertCitationDialog(m_table, this);
}

void BibliographyDatabaseWindow::openFile()
{
    QString tableFile = KFileDialog::getOpenFileName(KUrl(tableDir.absolutePath()), ".sqlite", this, "Open Bibliography database table from file");

    if (!tableFile.isEmpty()) {
        ui.tableList->addItem(tableFile.section(QDir::separator(), -1));           //set list entry as tableFile.section(QDir::separator(), -1)
        ui.tableList->setItemData(ui.tableList->count(), QVariant::fromValue<QString>(tableFile));

        ui.tableList->setCurrentIndex(ui.tableList->count() - 1);
    }
}
