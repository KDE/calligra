/*
* Kexi Report Plugin
* Copyright (C) 2007-2009 by Adam Pigg (adam@piggz.co.uk)
*
* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.
*
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "kexisourceselector.h"

#include <QLabel>
#include <KLocale>
#include <KDebug>
#include <QDomElement>

//#define NO_EXTERNAL_SOURCES

#ifdef NO_EXTERNAL_SOURCES
#ifdef __GNUC__
#warning enable external data sources for 2.3
#else
#pragma WARNING( enable external data sources for 2.3 )
#endif
#endif

KexiSourceSelector::KexiSourceSelector(QWidget* parent, KexiDB::Connection *conn) : QWidget(parent)
{

    m_conn = conn;
    m_kexiDBData = 0;
    m_kexiMigrateData = 0;

    m_layout = new QVBoxLayout(this);
    m_sourceType = new QComboBox(this);
    m_internalSource = new QComboBox(this);
    m_externalSource = new KLineEdit(this);
    m_setData = new KPushButton(i18n("Set Data"));

    connect(m_setData, SIGNAL(clicked()), this, SLOT(setDataClicked()));

    m_sourceType->addItem(i18n("Internal"), QVariant("internal"));
    m_sourceType->addItem(i18n("External"), QVariant("external"));

    m_internalSource->addItems(queryList());

#ifndef NO_EXTERNAL_SOURCES

//!@TODO enable when adding external data
    
    m_layout->addWidget(new QLabel(i18n("Source Type:"), this));
    m_layout->addWidget(m_sourceType);
    m_layout->addSpacing(10);
#else
    m_sourceType->setVisible(false);
    m_externalSource->setVisible(false);
#endif

    m_layout->addWidget(new QLabel(i18n("Internal Source:"), this));
    m_layout->addWidget(m_internalSource);
    m_layout->addSpacing(10);

#ifndef NO_EXTERNAL_SOURCES
    m_layout->addWidget(new QLabel(i18n("External Source:"), this));
    m_layout->addWidget(m_externalSource);
#endif
    m_layout->addSpacing(20);
    m_layout->addWidget(m_setData);
    m_layout->addStretch();
    setLayout(m_layout);
}

KexiSourceSelector::~KexiSourceSelector()
{

}

QStringList KexiSourceSelector::queryList()
{
    //Get the list of queries in the database
    QStringList qs;
    if (m_conn && m_conn->isConnected()) {
        QList<int> tids = m_conn->tableIds();
        qs << "";
        for (int i = 0; i < tids.size(); ++i) {
            KexiDB::TableSchema* tsc = m_conn->tableSchema(tids[i]);
            if (tsc)
                qs << tsc->name();
        }

        QList<int> qids = m_conn->queryIds();
        qs << "";
        for (int i = 0; i < qids.size(); ++i) {
            KexiDB::QuerySchema* qsc = m_conn->querySchema(qids[i]);
            if (qsc)
                qs << qsc->name();
        }
    }

    return qs;
}

void KexiSourceSelector::setConnectionData(QDomElement c)
{
    if (c.attribute("type") == "internal") {
	m_sourceType->setCurrentIndex(m_sourceType->findData("internal"));
        m_internalSource->setCurrentIndex(m_internalSource->findText(c.attribute("source")));
    }

    if (c.attribute("type") == "external") {
	m_sourceType->setCurrentIndex(m_sourceType->findText("external"));
        m_externalSource->setText(c.attribute("source"));
    }

    emit(setData(sourceData()));
}

QDomElement KexiSourceSelector::connectionData()
{
    kDebug();
    QDomDocument d;
    QDomElement conndata = d.createElement("connection");

#ifndef NO_EXTERNAL_SOURCES
//!@TODO Make a better gui for selecting external data source

    conndata.setAttribute("type", m_sourceType->itemData(m_sourceType->currentIndex()).toString());

    if (m_sourceType->itemData(m_sourceType->currentIndex()).toString() == "internal") {
        conndata.setAttribute("source", m_internalSource->currentText());
    } else {
        conndata.setAttribute("source", m_externalSource->text());
    }
#else
    conndata.setAttribute("type", "internal");
    conndata.setAttribute("source", m_internalSource->currentText());
#endif
    return conndata;
}

KoReportData* KexiSourceSelector::sourceData()
{
    if (m_kexiDBData) {
        delete m_kexiDBData;
        m_kexiDBData = 0;
    }

    if (m_kexiMigrateData) {
        delete m_kexiMigrateData;
        m_kexiMigrateData = 0;
    }

//!@TODO Fix when enable external data
#ifndef NO_EXTERNAL_SOURCES
    if (m_sourceType->itemData(m_sourceType->currentIndex()).toString() == "internal") {
        m_kexiDBData = new KexiDBReportData(m_internalSource->currentText(), m_conn);
        return m_kexiDBData;
    }
    if (m_sourceType->itemData(m_sourceType->currentIndex()).toString() == "external") {
        m_kexiMigrateData = new KexiMigrateReportData(m_externalSource->text());
        return m_kexiMigrateData;
    }
#else
    m_kexiDBData = new KexiDBReportData(m_internalSource->currentText(), m_conn);
    return m_kexiDBData;
#endif
    return 0;
}

void KexiSourceSelector::setDataClicked()
{
    emit(setData(sourceData()));
}
