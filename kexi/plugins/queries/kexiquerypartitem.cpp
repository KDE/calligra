/* This file is part of the KDE project
   Copyright (C) 2002 Joseph Wenninger <jowenn@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
 */

#include <kexiquerypartitem.h>
#include <kexiquerydesigner.h>
#include <qstring.h>
#include <kexiproject.h>
#include <qdom.h>
#include <kdebug.h>
#include <koStore.h>

KexiQueryPartItem::KexiQueryPartItem(KexiProjectHandler *parent,
		const QString& name, const QString& mime, 
		const QString& identifier)
	:KexiProjectHandlerItem(parent,name,mime,identifier)
{
}

KexiQueryPartItem::~KexiQueryPartItem() {
}

const KexiQueryPartItem::QueryEntryList &KexiQueryPartItem::getQueryData() {
	return m_queryEntryList;
}


void KexiQueryPartItem::setQueryData(const KexiQueryPartItem::QueryEntryList& newlist) {
	m_queryEntryList=newlist;
}

void KexiQueryPartItem::setParameterList(const KexiDataProvider::ParameterList& params) {
	m_params=params;
}

const KexiDataProvider::ParameterList KexiQueryPartItem::parameters() {
	return m_params;
}


void KexiQueryPartItem::store(KoStore* store) {
        kdDebug() << "KexiQueryPartItem::store(KoStore*)" << endl;

	QDomDocument domDoc("Query");
	domDoc.appendChild(domDoc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));

        QDomElement docElement = domDoc.createElement("query");
	domDoc.appendChild(docElement);
        QDomElement nameElement = domDoc.createElement("name");
	QDomText attrName = domDoc.createTextNode(identifier());
        nameElement.appendChild(attrName);
        docElement.appendChild(nameElement);

	QDomElement sqlElement = domDoc.createElement("sql");
	QDomText attrSql;

	attrSql = domDoc.createTextNode("");
	sqlElement.appendChild(attrSql);

	docElement.appendChild(sqlElement);

        QDomElement itemsElement = domDoc.createElement("items");
        docElement.appendChild(itemsElement);

	for (QueryEntryList::const_iterator it=m_queryEntryList.begin();it!=m_queryEntryList.end();++it) {
		QDomElement item = domDoc.createElement("item");
		item.setAttribute("source",(*it).source);
		item.setAttribute("field",(*it).field);
		item.setAttribute("shown",(*it).show?"true":"false");
		item.setAttribute("orC",(*it).orC);
		item.setAttribute("andC",(*it).andC);
                itemsElement.appendChild(item);
	}


        QDomElement paramsElement = domDoc.createElement("parameters");
        docElement.appendChild(paramsElement);

	for (KexiDataProvider::ParameterList::const_iterator it=m_params.begin();it!=m_params.end();++it) {
		QDomElement param = domDoc.createElement("parameter");
		param.setAttribute("name",(*it).name);
		param.setAttribute("type",(*it).type);
                paramsElement.appendChild(param);
	}


/*
                QDomElement preparsed = domDoc.createElement("preparsed");
                domDoc.appendChild(preparsed);
                QDomText tPreparsed = domDoc.createTextNode(m_editor->getQuery());
                preparsed.appendChild(tPreparsed); */

        QByteArray data = domDoc.toCString();
        data.resize(data.size()-1);

	if(store) {
        	store->open("/query/" + identifier() + ".query");
                store->write(data);
                store->close();
        }

}

void KexiQueryPartItem::load(KoStore* store) {
	kdDebug()<<"*********KexiQueryPartItem::load(KoStore* store)"<<endl;
	if(store) {
		kdDebug()<<"*********KexiQueryPartItem::load(KoStore* store): store!=0"<<endl;
		store->open("/query/"+identifier()+".query");
		QDomDocument doc;
		doc.setContent(store->device());
		store->close();
		QDomElement el=doc.documentElement();
		kdDebug()<<"document tag name: "<<el.tagName()<<endl;
		for (QDomElement readTag=el.firstChild().toElement();
			!readTag.isNull();readTag=readTag.nextSibling().toElement()) {
			kdDebug()<<"Looking for items tag"<<endl;
			// read the items
			if (readTag.tagName()=="items") {
				kdDebug()<<"Items tag found"<<endl;
				for (QDomElement itemTag=readTag.firstChild().toElement();
				!itemTag.isNull(); itemTag=itemTag.nextSibling().toElement()) {
					m_queryEntryList.append(QueryEntry(
						itemTag.attribute("source"),
						itemTag.attribute("field"),
						itemTag.attribute("shown")=="true",
						itemTag.attribute("orC"),
						itemTag.attribute("andC")
					));	
				}
			}
			else
			// read query parameters
			if (readTag.tagName()=="parameters") {
				kdDebug()<<"Parameters tag found"<<endl;
				for (QDomElement paramTag=readTag.firstChild().toElement();
				!paramTag.isNull(); paramTag=paramTag.nextSibling().toElement()) {
					m_params.append(KexiDataProvider::Parameter(
						paramTag.attribute("name"),
						paramTag.attribute("type").toInt()
					));	
				}

			}
		}
	}
}


#include "kexiquerypartitem.moc"

