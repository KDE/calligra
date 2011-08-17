/*
    <one line to give the library's name and an idea of what it does.>
    Copyright (C) 2011  Adam Pigg <adam@piggz.co.uk>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include "InternalSourceSelector.h"
#include <kdebug.h>

InternalSourceSelector::InternalSourceSelector(QWidget *parent, KexiDB::Connection *conn) : QComboBox(parent), m_conn(conn)
{
    addItems(queryList());
}

InternalSourceSelector::~InternalSourceSelector()
{
    
}   

void InternalSourceSelector::mousePressEvent(QMouseEvent *e) {
    clear();
    addItems(queryList());
    QComboBox::mousePressEvent(e);
}
    
QStringList InternalSourceSelector::queryList(){
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
    