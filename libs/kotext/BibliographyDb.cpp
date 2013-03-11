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
#include "BibliographyTableModel.h"

#include <KoOdfBibliographyConfiguration.h>
#include <klocale.h>
#include <KoInlineCite.h>
#include <KSharedPtr>

#include <db/drivermanager.h>
#include <db/cursor.h>
#include <db/preparedstatement.h>

#include <QDir>
#include <QFile>
#include <QSortFilterProxyModel>

const QList<QString> BibliographyDb::dbFields = QList<QString>() << "identifier"<< "bibliography_type"
                                                                 << "author" << "annote" << "address"
                                                                 << "booktitle" << "chapter"
                                                                 << "edition" << "editor" << "howpublished"
                                                                 << "institution" << "isbn"
                                                                 << "issn" << "journal" << "month" << "note"
                                                                 << "number" << "organizations" << "pages"
                                                                 << "publisher" << "report_type" << "school"
                                                                 << "series" << "title" << "url" << "volume"
                                                                 << "year" << "custom1" << "custom2"
                                                                 << "custom3" << "custom4" << "custom5";

const QDir BibliographyDb::tableDir = QDir(QDir::home().path().append(QDir::separator()).append(".calligra"),
                                          QString(), QDir::Name, QDir::Files | QDir::Hidden | QDir::NoSymLinks);

KexiDB::DriverManager BibliographyDb::manager;

BibliographyDb::BibliographyDb(QObject *parent, const QString &path, const QString &dbName) :
    QObject(parent),
    m_filterModel(new QSortFilterProxyModel(this)),
    m_dbName(dbName),
    m_fullPath(path),
    m_valid(true),
    m_driver(manager.driver("sqlite3"))
{
#ifdef Q_OS_UNIX
    m_fullPath.append(QDir::separator()).append(m_dbName);
    m_fullPath = QDir::toNativeSeparators(m_fullPath);
#else
    m_fullPath = m_dbName;
#endif
    m_cdata.setFileName(m_fullPath);

    init();
}

void BibliographyDb::init()
{
    if (!connect()) {
        qDebug() << "Unable to connect to database " << m_fullPath;
        return;
    }

    if (!m_conn->databaseExists(m_fullPath) && !m_conn->createDatabase(m_fullPath)) {
        qDebug() << "Unable to create database " << m_fullPath;
        m_valid = false;
        return;
    }
    if (!m_conn->useDatabase(m_fullPath)) {
        qDebug() << "Unable to read database " << m_fullPath;
        m_valid = false;
        return;
    }
    if (!m_conn->tableNames().contains("bibref") && !createTable()) {
        qDebug() << "Unable to create table bibref in database.";
        m_conn->debugError();
        m_valid = false;
        return;
    }

    m_model = new BibliographyTableModel(m_conn, this);
    m_filterModel->setSourceModel(m_model);
    m_filterModel->setDynamicSortFilter(true);
    m_schema = m_conn->tableSchema("bibref");

    qDebug() << (m_conn->isConnected()?"Connected":"Disconnected");
}

QString BibliographyDb::getDbPath() const
{
    return m_fullPath;
}

bool BibliographyDb::connect()
{
    m_conn = m_driver->createConnection(m_cdata);
    return m_conn->connect();
}

bool BibliographyDb::deleteDb()
{
    return m_conn->dropDatabase(m_fullPath);
}

void BibliographyDb::closeDb()
{
    m_conn->closeDatabase();
}

QString BibliographyDb::lastError() const
{
    return m_conn->errorMsg();
}

bool BibliographyDb::isLastErrorValid() const
{
    return m_conn->error();
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
//    m_model->submitAll();
}

bool BibliographyDb::createTable()
{
    m_schema = new KexiDB::TableSchema("bibref");

    KexiDB::Field *field = new KexiDB::Field("id", KexiDB::Field::Integer,
                                             KexiDB::Field::PrimaryKey | KexiDB::Field::AutoInc,
                                             KexiDB::Field::Unsigned);
    field->setCaption("ID");
    m_schema->addField(field);

    field = new KexiDB::Field("identifier", KexiDB::Field::Text, KexiDB::Field::NotNull | KexiDB::Field::Unique);
    field->setCaption("identifier");
    m_schema->addField(field);

    foreach(QString fieldName, BibliographyDb::dbFields) {
        if (fieldName != "identifier") {
            field = new KexiDB::Field(fieldName, KexiDB::Field::Text);
            field->setCaption(fieldName);
            m_schema->addField(field);
        }
    }

    if (!m_conn->isConnected()) {
        connect();
    }

    return m_conn->createTable(m_schema, true);
}

BibliographyTableModel* BibliographyDb::tableModel()
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

void BibliographyDb::setFilter(QList<BibDbFilter*> *filters)
{
    m_model->setFilter(filters);       //TODO:
}

bool BibliographyDb::insertCitation(KoInlineCite *cite)
{
    KexiDB::PreparedStatement::Ptr statement = m_conn->prepareStatement(KexiDB::PreparedStatement::InsertStatement
                                                                        , *m_schema);

    foreach(KexiDB::Field *f, *m_schema->fields()) {
        (*statement) << cite->value(f->name().replace('_', '-'));
    }
    return statement->execute();
}

QMap<QString, KoInlineCite *> BibliographyDb::citationRecords()
{
    QMap<QString, KoInlineCite *> answers;
    int citeCount = m_model->rowCount();

    KexiDB::Cursor *c = m_conn->executeQuery(*m_schema);
    c->moveFirst();

    for (int i = 0; i < citeCount; i++) {
        KoInlineCite *cite = new KoInlineCite(KoInlineCite::Citation);

        for (int i = 0; i < BibliographyDb::dbFields.size(); i++) {
            cite->setField(m_schema->fields()->at(i+1)->name().replace('_', '-'), c->value(i+1).toString());
        }
        answers.insert(cite->value("identifier"), cite);
        c->moveNext();
    }
    m_conn->deleteCursor(c);
    return answers;
}

BibliographyDb::~BibliographyDb()
{
    closeDb();
//    if (m_model) {
////        m_model->submitAll();
//        delete m_model;
//    }
}
