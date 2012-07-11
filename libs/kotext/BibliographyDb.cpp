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

#include "BibliographyDb.h"

#include <KoOdfBibliographyConfiguration.h>
#include <klocale.h>
#include <KoInlineCite.h>

#include <QDir>
#include <QFile>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlTableModel>
#include <QSortFilterProxyModel>

const QList<QString> BibliographyDb::dbFields = QList<QString>() << "address" << "annote" << "author"
                                                                          << "bibliography_type" << "booktitle"
                                                                          << "chapter" << "custom1" << "custom2"
                                                                          << "custom3" << "custom4" << "custom5"
                                                                          << "edition" << "editor" << "howpublished"
                                                                          << "identifier" << "institution" << "isbn"
                                                                          << "issn" << "journal" << "month" << "note"
                                                                          << "number" << "organizations" << "pages"
                                                                          << "publisher" << "report_type" << "school"
                                                                          << "series" << "title" << "url" << "volume"
                                                                          << "year";

const QDir BibliographyDb::tableDir = QDir(QDir::home().path().append(QDir::separator()).append(".calligra"),
                                          QString(), QDir::Name, QDir::Files | QDir::Hidden | QDir::NoSymLinks);

BibliographyDb::BibliographyDb(QObject *parent, const QString &path, const QString &dbName) :
    QObject(parent),
    m_filterModel(new QSortFilterProxyModel(this)),
    m_db(QSqlDatabase::addDatabase("QSQLITE")),
    m_dbName(dbName),
    m_fullPath(path),
    m_valid(false)
{
#ifdef Q_OS_UNIX
    m_fullPath.append(QDir::separator()).append(m_dbName);
    m_fullPath = QDir::toNativeSeparators(m_fullPath);
    m_db.setDatabaseName(m_fullPath);
#else
    m_fullPath = m_dbName;
    m_db.setDatabaseName(m_dbName);
#endif
    if (!QFile::exists(m_fullPath)) {
        QFile dbFile(m_fullPath);
        dbFile.open(QIODevice::WriteOnly);
        dbFile.close();
    }
    if (openDb()) {
        m_model = new QSqlTableModel(this, m_db);
        m_model->setTable("bibref");
        m_model->setEditStrategy(QSqlTableModel::OnRowChange);
        m_model->select();

        m_filterModel->setSourceModel(m_model);
        m_filterModel->setDynamicSortFilter(true);
    }
}

bool BibliographyDb::openDb()
{
    if ( m_db.open() && !m_db.tables(QSql::Tables).contains("bibref")) {
        m_valid = false;
    } else {
        m_valid = true;
    }

    return m_db.isOpen();
}

bool BibliographyDb::deleteDb()
{
    m_db.close();
#ifdef Q_OS_LINUX
    return QFile::remove(m_fullPath);
#else
    return QFile::remove(m_dbName);
#endif
}

void BibliographyDb::closeDb()
{
    m_db.close();
}

QString BibliographyDb::lastError() const
{
    return m_model->lastError().text();
}

bool BibliographyDb::isLastErrorValid() const
{
    return m_model->lastError().isValid();
}
bool BibliographyDb::isValid() const
{
    return m_valid;
}

int BibliographyDb::rowCount() const
{
    return m_model->rowCount();
}

void BibliographyDb::removeRow(int index)
{
    m_model->removeRow(index);
}

void BibliographyDb::submitAll()
{
    m_model->submitAll();
}

bool BibliographyDb::createTable()
{
    bool ret = false;
    if (!m_db.isOpen()) {
        m_db.open();
    } else {
        QString query("CREATE TABLE bibref"
                      "(id INTEGER PRIMARY KEY,"
                      "identifier TEXT NOT NULL UNIQUE,"
                      "bibliography_type TEXT,"
                      "address TEXT,"
                      "annote TEXT,"
                      "author TEXT,"
                      "booktitle TEXT,"
                      "chapter TEXT,"
                      "edition TEXT,"
                      "editor TEXT,"
                      "howpublished TEXT,"
                      "institution TEXT,"
                      "journal TEXT,"
                      "month TEXT,"
                      "note TEXT,"
                      "number TEXT,"
                      "organizations TEXT,"
                      "pages TEXT,"
                      "publisher TEXT,"
                      "school TEXT,"
                      "series TEXT,"
                      "title TEXT,"
                      "report_type TEXT,"
                      "volume TEXT,"
                      "year TEXT,"
                      "url TEXT,"
                      "custom1 TEXT,"
                      "custom2 TEXT,"
                      "custom3 TEXT,"
                      "custom4 TEXT,"
                      "custom5 TEXT,"
                      "isbn TEXT,"
                      "issn TEXT)");
        m_db.exec(query);
    }
    return ret;
}

QSqlTableModel* BibliographyDb::tableModel()
{
    return m_model;
}

QSortFilterProxyModel* BibliographyDb::proxyModel()
{
    return m_filterModel;
}

void BibliographyDb::setSearchFilter(QRegExp expr)
{
    if (m_filterModel) {
        m_filterModel->setFilterRegExp(expr);
        m_filterModel->setFilterKeyColumn(-1);
    }
}

void BibliographyDb::setFilter(QString filter)
{
    m_model->setFilter(filter);

    if (m_model->lastError().isValid()) {
        qDebug() << m_model->lastError().text();
    }
}

bool BibliographyDb::insertCitation(KoInlineCite *cite)
{
    QSqlRecord record = BibliographyDb::sqlRecord(cite);
    return m_model->insertRecord(-1, record);
}

QMap<QString, KoInlineCite *> BibliographyDb::citationRecords()
{
    QMap<QString, KoInlineCite *> answers;
    int citeCount = m_model->rowCount();

    for (int i = 0; i < citeCount; i++) {
        QSqlRecord record = m_model->record(i);
        KoInlineCite *cite = new KoInlineCite(KoInlineCite::Citation);

        for (int i = 0; i < BibliographyDb::dbFields.size(); i++) {
            cite->setField(KoOdfBibliographyConfiguration::bibDataFields.at(i), record.value(BibliographyDb::dbFields.at(i)).toString());
        }
        answers.insert(cite->value("identifier"), cite);
    }
    return answers;
}

BibliographyDb::~BibliographyDb()
{
    closeDb();
    if (m_model) {
        m_model->submitAll();
        delete m_model;
    }
}

QSqlRecord BibliographyDb::sqlRecord(const KoInlineCite *cite)
{
    QSqlRecord record;

    for (int i = 0; i < BibliographyDb::dbFields.size(); i++) {
        QSqlField field(BibliographyDb::dbFields.at(i), QVariant::String);
        field.setValue(cite->value(KoOdfBibliographyConfiguration::bibDataFields.at(i)));
        record.append(field);
    }
    return record;
}
