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
* License along with this library; if not, write to the Free Software
* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/

#include "kexisourceselector.h"

#include <QLabel>
#include <KLocale>
#include <KDebug>
#include <QDomElement>

KexiSourceSelector::KexiSourceSelector ( QWidget* parent, KexiDB::Connection *conn ) : QWidget(parent) {

    m_conn = conn;
    m_kdbd = 0;
    m_kmd = 0;
    
    m_layout = new QVBoxLayout(this);
    m_sourceType = new QComboBox(this);
    m_internalSource = new QComboBox(this);
    m_externalSource = new KLineEdit(this);
    m_setData = new KPushButton(i18n("Set Data"));

    connect(m_setData, SIGNAL(clicked()), this, SLOT(setDataClicked()));
    
    m_sourceType->addItem(i18n("Internal"), QVariant("internal"));
    m_sourceType->addItem(i18n("External"), QVariant("external"));

    m_internalSource->addItems(queryList());
    
    m_layout->addWidget(new QLabel("Source Type:", this));
    m_layout->addWidget(m_sourceType);
    m_layout->addSpacing(10);
    m_layout->addWidget(new QLabel("Internal Source:", this));
    m_layout->addWidget(m_internalSource);
    m_layout->addSpacing(10);
    m_layout->addWidget(new QLabel("External Source:", this));
    m_layout->addWidget(m_externalSource);
    m_layout->addSpacing(20);
    m_layout->addWidget(m_setData);
    m_layout->addStretch();
    setLayout(m_layout);
}

KexiSourceSelector::~KexiSourceSelector() {

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
    m_sourceType->setEditText(c.attribute("type"));
    
    if (c.attribute("type") == "internal" ) {
       m_internalSource->setCurrentIndex(m_sourceType->findText(c.attribute("source")));
    }
    
    if (c.attribute("type") == "external" ) {
        m_externalSource->setText(c.attribute("source"));
    }

    emit(setData(sourceData()));
    
}

QDomElement KexiSourceSelector::connectionData()
{
    kDebug();
    QDomDocument d;
    QDomElement conndata = d.createElement("connection");

    conndata.setAttribute("type", m_sourceType->itemData(m_sourceType->currentIndex()).toString());
    
    if (m_sourceType->itemData(m_sourceType->currentIndex()).toString() == "internal") {
        conndata.setAttribute("source", m_internalSource->currentText());
    }
    else {
        conndata.setAttribute("source", m_externalSource->text());
    }
    return conndata;
}

KoReportData* KexiSourceSelector::sourceData()
{
    if (m_kdbd) {
        delete m_kdbd;
        m_kdbd = 0;
    }

    if (m_kmd) {
        delete m_kmd;
        m_kmd = 0;
    }

    if (m_sourceType->itemData(m_sourceType->currentIndex()).toString() == "internal" ) {
        m_kdbd = new KexiDBReportData(m_internalSource->currentText(), m_conn);
        return m_kdbd;
    }
    if (m_sourceType->itemData(m_sourceType->currentIndex()).toString() == "external" ) {
        m_kmd = new KexiMigrateReportData(m_externalSource->text());
        return m_kmd;
    }

    return 0;
}

void KexiSourceSelector::setDataClicked() {
    emit(setData(sourceData()));
}
