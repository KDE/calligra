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

#include <qdom.h>
#include <qstring.h>
#include <qvariant.h>

#include <kexiquerypartitem.h>
#include <kexiquerydesigner.h>
#include <kexiproject.h>
#include <kdebug.h>
#include <koStore.h>
#include <kexitablelist.h>
#include <kexitableitem.h>

KexiQueryPartItem::KexiQueryPartItem(KexiProjectHandler *parent,
		const QString& name, const QString& mime,
		const QString& identifier)
	:KexiProjectHandlerItem(parent,name,mime,identifier)
{
	m_designData = new KexiTableList();
	KexiTableItem *item = new KexiTableItem(6);

	item->setValue(0, 0);
	item->setValue(2, true);
	item->setInsertItem(true);

	m_designData->append(item);
}

void
KexiQueryPartItem::setParameterList(const KexiDataProvider::ParameterList& params)
{
	m_params=params;
}

const KexiDataProvider::ParameterList
KexiQueryPartItem::parameters()
{
	return m_params;
}


void KexiQueryPartItem::store(KoStore* store)
{

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
	sqlElement.setAttribute("statement", m_sql);
	docElement.appendChild(sqlElement);

	QDomElement itemsElement = domDoc.createElement("items");
	docElement.appendChild(itemsElement);

	KexiTableItem *it;
	for(it = m_designData->first(); it; it = m_designData->next())
	{
		if(!it->isInsertItem())
		{
			QDomElement item = domDoc.createElement("item");
			kdDebug() << "KexiQueryPartItem::store(): source=" << it->getValue(0).toString() << endl;
			item.setAttribute("source", it->getValue(0).toString());
			item.setAttribute("field", it->getValue(1).toString());
			item.setAttribute("shown", it->getValue(2).toBool()?"true":"false");
			item.setAttribute("orC", it->getValue(3).toString());
			item.setAttribute("andC", it->getValue(4).toString());

			itemsElement.appendChild(item);
		}
	}


	QDomElement paramsElement = domDoc.createElement("parameters");
	docElement.appendChild(paramsElement);

	for (KexiDataProvider::ParameterList::const_iterator it=m_params.begin();it!=m_params.end();++it)
	{
		QDomElement param = domDoc.createElement("parameter");
		param.setAttribute("name",(*it).name);
		param.setAttribute("type",(*it).type);
        	paramsElement.appendChild(param);
	}

	QByteArray data = domDoc.toCString();
	data.resize(data.size()-1);

	if(store)
	{
		store->open("/query/" + name() + ".query");
		store->write(data);
		store->close();
	}
}

void
KexiQueryPartItem::load(KoStore* store)
{
	delete m_designData;
	m_designData = new KexiTableList();

	kdDebug()<< "KexiQueryPartItem::load()" <<endl;

	if(store)
	{
		kdDebug() << "KexiQueryPartItem::load(): store!=0" << endl;

		store->open("/query/" + name() + ".query");
		QDomDocument doc;
		doc.setContent(store->device());
		store->close();
		QDomElement el=doc.documentElement();
		kdDebug()<<"document tag name: "<<el.tagName()<<endl;
		for (QDomElement readTag=el.firstChild().toElement();
		 !readTag.isNull();readTag=readTag.nextSibling().toElement())
		{
			kdDebug()<<"Looking for items tag"<<endl;
			// read the items
			if (readTag.tagName()=="items")
			{
				kdDebug()<<"Items tag found"<<endl;
				for (QDomElement itemTag=readTag.firstChild().toElement();
				!itemTag.isNull(); itemTag=itemTag.nextSibling().toElement())
				{
					KexiTableItem *tableItem = new KexiTableItem(5);
					tableItem->setValue(0, QVariant(itemTag.attribute("source")));
					tableItem->setValue(1, QVariant(itemTag.attribute("field")));
					tableItem->setValue(2, QVariant(true));
					tableItem->setValue(3, QVariant(itemTag.attribute("orC")));
					tableItem->setValue(4, QVariant(itemTag.attribute("andC")));

					m_designData->append(tableItem);
				}
			}
			else
			// read query parameters
			if (readTag.tagName()=="parameters")
			{
				kdDebug() << "Parameters tag found" << endl;
				for (QDomElement paramTag=readTag.firstChild().toElement();
				 !paramTag.isNull(); paramTag=paramTag.nextSibling().toElement())
				{
					m_params.append(KexiDataProvider::Parameter(
                                        paramTag.attribute("name"),
                                        paramTag.attribute("type").toInt()
                                        ));
				}
			}
		}
		m_sql = el.namedItem("sql").toElement().attribute("statement");
	}
}

KexiQueryPartItem::~KexiQueryPartItem()
{
}


#include "kexiquerypartitem.moc"

