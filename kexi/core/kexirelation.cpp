/* This file is part of the KDE project
   Copyright (C) 2002, 2003 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <qlayout.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qdom.h>
#include <qhbox.h>

#include <klocale.h>
#include <kdebug.h>

#include <koStore.h>


#include "kexiproject.h"
//#include "kexirelationview.h"
#include "kexirelation.h"

KexiRelation::KexiRelation(KexiProject *parent,const char * name)
 : QObject(parent, name)
{

	m_parent=parent;
#warning fixme
#if 0
	m_relationList.clear();
	m_undoStack.clear();
#endif
	m_usageCount=0;
}


void KexiRelation::incUsageCount(){m_usageCount++;}
void KexiRelation::decUsageCount()
{
	m_usageCount--;
#warning fixme
#if 0

	if (!m_usageCount)
	{
		m_undoStack.clear();
	}
#endif
}

#warning fixme
#if 0
RelationList KexiRelation::projectRelations(){return m_relationList;}
void KexiRelation::updateRelationList(QObject *who,RelationList relationList)
{
	kdDebug() << "KexiRelation::updateRelationList" << endl;
	m_undoStack.push(m_relationList);
	m_relationList=relationList;
	emit relationListUpdated(who);
	m_parent->db()->setRelations(relationList);
}
#endif

void KexiRelation::storeRelations(KoStore *store)
{

#warning fixme
#if 0
    QDomDocument domDoc("Relations");
    domDoc.appendChild(domDoc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));


    QDomElement relationsElement = domDoc.createElement("Relations");
    domDoc.appendChild(relationsElement);

    for(RelationList::Iterator it = m_relationList.begin(); it != m_relationList.end(); it++)
    {
        QDomElement relationElement = domDoc.createElement("Relation");
        relationsElement.appendChild(relationElement);

        QDomElement sourceTable = domDoc.createElement("sourcetable");
        relationElement.appendChild(sourceTable);
        QDomText tSourceTable = domDoc.createTextNode((*it).srcTable);
        sourceTable.appendChild(tSourceTable);

        QDomElement sourceField = domDoc.createElement("sourcefield");
        relationElement.appendChild(sourceField);
        QDomText tSourceField = domDoc.createTextNode((*it).srcField);
        sourceField.appendChild(tSourceField);

        QDomElement targetTable = domDoc.createElement("targettable");
        relationElement.appendChild(targetTable);
        QDomText tTargetTable = domDoc.createTextNode((*it).rcvTable);
        targetTable.appendChild(tTargetTable);

        QDomElement targetField = domDoc.createElement("targetfield");
        relationElement.appendChild(targetField);
        QDomText tTargetField = domDoc.createTextNode((*it).rcvField);
        targetField.appendChild(tTargetField);

        kdDebug() << "KexiRelationView::storeRelatoins() srcTable: " << (*it).srcTable << endl;
        kdDebug() << "KexiRelationView::storeRelatoins() srcField: " << (*it).srcField << endl;
        kdDebug() << "KexiRelationView::storeRelatoins() rcvTable: " << (*it).rcvTable << endl;
        kdDebug() << "KexiRelationView::storeRelatoins() rcvField: " << (*it).rcvField << endl;
    }

    QByteArray data = domDoc.toCString();
    data.resize(data.size()-1);

    if(store)
    {
        store->open("relations.xml");
        store->write(data);
        store->close();
    }

	m_undoStack.clear();
	m_usageCount=0;
    return;

#endif
}

void KexiRelation::loadRelations(KoStore *store)
{
#warning fixme
#if 0
	m_relationList.clear();
    if(!store->open("relations.xml"))
        return;

    QDomDocument inBuf;

    //error reporting
    QString errorMsg;
    int errorLine;
    int errorCol;

    bool parsed = inBuf.setContent(store->device(), false, &errorMsg, &errorLine, &errorCol);
    store->close();

    if(!parsed)
    {
        kdDebug() << "coudn't parse:" << endl;
        kdDebug() << "error: " << errorMsg << " line: " << errorLine << " col: " << errorCol << endl;
        return;
    }

    QDomElement element = inBuf.namedItem("Relations").toElement();
    for(QDomNode n = element.firstChild(); !n.isNull(); n = n.nextSibling())
    {
        SourceConnection con;
        QDomElement sourceTable = n.namedItem("sourcetable").toElement();
        QDomElement sourceField = n.namedItem("sourcefield").toElement();
        QDomElement targetTable = n.namedItem("targettable").toElement();
        QDomElement targetField = n.namedItem("targetfield").toElement();

        con.srcTable = sourceTable.text();
        con.srcField = sourceField.text();
        con.rcvTable = targetTable.text();
        con.rcvField = targetField.text();
        m_relationList.append(con);
    }
	m_undoStack.clear();
	m_usageCount=0;
	m_parent->db()->setRelations(m_relationList);
#endif
}

KexiRelation::~KexiRelation()
{
}

#include "kexirelation.moc"
