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

#include <QTableView>
#include <QSqlTableModel>
#include <QDir>
#include <QMessageBox>
#include <QDebug>

BibliographyDatabaseWindow::BibliographyDatabaseWindow(QWidget *parent) :
    QMainWindow(parent),
    m_table(0)
{
    ui.setupUi(this);

    m_bibTableView = new QTableView(ui.centralwidget);
    ui.centralwidget->layout()->addWidget(m_bibTableView);

    connect(ui.tableList, SIGNAL(currentIndexChanged(QString)), this, SLOT(tableChanged(QString)));

    //TODO: add blank row after row change
    //connect(m_bibTableView, SIGNAL(rowCountChanged(int,int)), this, SLOT(insertBlankRow()));

    if (!tableDir.exists()) {
        if (!tableDir.mkpath(tableDir.absolutePath())) {
            QMessageBox::warning(this, i18n("Error"), QString(i18n("Error creating directory ")).append(tableDir.absolutePath()));
            emit close();
        }
    } else {
        loadBibliographyDbs();
    }
}

BibliographyDatabaseWindow::~BibliographyDatabaseWindow()
{
    delete m_table;
}

void BibliographyDatabaseWindow::loadBibliographyDbs()
{
    tableDir = QDir(QDir::home().path().append(QDir::separator()).append(".calligra"),
                                              QString(), QDir::Name, QDir::Files | QDir::Hidden | QDir::NoSymLinks);
    foreach(QFileInfo tableInfo, tableDir.entryInfoList()) {
        ui.tableList->addItem(tableInfo.fileName());
    }
}

void BibliographyDatabaseWindow::tableChanged(QString newTable)
{
    if (m_table) {
        delete m_table;
    }

    if (newTable.isEmpty()) {
        newTable = "bibliography.sqlite";
    }

    m_table = new BibliographyDb(this, tableDir.absolutePath(), newTable);
    m_bibTableView->setModel(m_table->tableModel());
    m_bibTableView->hideColumn(0);      // hide ID column
    m_bibTableView->resizeColumnsToContents();
    m_bibTableView->horizontalHeader()->setSortIndicatorShown(true);
    m_bibTableView->setSortingEnabled(true);
    m_bibTableView->show();

    //We add extra row to insert new citation record
    m_bibTableView->model()->insertRow(m_bibTableView->model()->rowCount());
}

void BibliographyDatabaseWindow::insertBlankRow()
{
    qDebug() << "inserting blank row..\n";
    m_bibTableView->model()->insertRow(m_bibTableView->model()->rowCount());
}
