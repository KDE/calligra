/* This file is part of the KDE project
   Copyright (C) 2002,2003 Joseph Wenninger <jowenn@kde.org>
   		 2003 Lucijan Busch    <lucijan@gmx.at>

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
#include <qregexp.h>

#include <kexiquerypartitem.h>
#include <kexiquerydesigner.h>
#include <kexidynamicqueryparameterdialog.h>
#include <kexiproject.h>
#include <kdebug.h>
#include <koStore.h>
#include <kexitablelist.h>
#include <kexitableitem.h>
#include <kmessagebox.h>
#include <kexidbrecord.h>

KexiQueryPartItem::KexiQueryPartItem(KexiProjectHandler *parent,
		const QString& name, const QString& mime,
		const QString& identifier)
 :KexiProjectHandlerItem(parent,name,mime,identifier)
{
	m_client = 0;

	m_designData = new KexiTableList();
	KexiTableItem *item = new KexiTableItem(5);

	item->setValue(0, 0);
	item->setValue(2, true);
	item->setInsertItem(true);

	m_designData->append(item);
}

void
KexiQueryPartItem::setParameters(const KexiDataProvider::ParameterList& params)
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

	if(m_client)
	{
        	kdDebug() << "KexiQueryPartItem::store(): client is registered..." << endl;
		kdDebug() << "KexiQueryPartItem::store(): " << m_params.count() << " items" << endl;
		m_client->saveBack();
	}

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

	QDomElement fieldCacheParent = domDoc.createElement("fieldcache");
	docElement.appendChild(fieldCacheParent);
	for (QStringList::const_iterator it=m_fields.begin();it!=m_fields.end();++it) {
		QDomElement fieldCacheElement=domDoc.createElement("field");
		fieldCacheElement.setAttribute("name",(*it));
		fieldCacheParent.appendChild(fieldCacheElement);
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
			if (readTag.tagName()=="fieldcache") {
				m_fields.clear();
				for (QDomElement fieldTag=readTag.firstChild().toElement(); !fieldTag.isNull();fieldTag=fieldTag.nextSibling().toElement())
				{
					m_fields<<fieldTag.attribute("name");
				}
			}
		}
		m_sql = el.namedItem("sql").toElement().attribute("statement");

		KexiTableItem *item = new KexiTableItem(5);
		item->setValue(0, 0);
		item->setValue(2, true);
		item->setInsertItem(true);
		m_designData->append(item);
	}
}


KexiDBRecord *KexiQueryPartItem::records(QWidget* dpar,KexiDataProvider::Parameters params) {
	if(m_sql.isEmpty()) return 0;

	QString query=m_sql;
	if (m_params.count())
	{
		for (KexiDataProvider::ParameterList::const_iterator
			it=m_params.begin();it!=m_params.end();++it)
		{
			if (!params.contains((*it).name)) {
				KexiDynamicQueryParameterDialog d(dpar,&params,m_params);
				if (d.exec()==QDialog::Accepted) {

					KexiDataProvider::Parameters::Iterator it;
        				for ( it = params.begin(); it != params.end();++it ) {
						kdDebug()<<"["<<it.key()<<"]="
						<<it.data()<<endl;
					}
				} else return 0;
				//show an input dialog
				//KMessageBox::sorry(0,"missing parameters");
				break;
			}
		}
#warning "HERE SHOULD BE A TRANLATION EG. OF DATE INPUTS TO DATABASSE SPECIFIC FORMATS"
		KexiDataProvider::Parameters::Iterator it;
		for ( it = params.begin(); it != params.end();++it ) {
			QRegExp exp1(QString("%1\\W").arg(it.key()),false);

			//not sure if that is needed will have to test that later
			QRegExp exp2(QString("%1$").arg(it.key()),false);

			query.replace(exp1,it.data());
			query.replace(exp2,it.data());
			kdDebug()<<"["<<it.key()<<"]="
			<<it.data()<<endl;
		}
	}

	kdDebug()<<"resulting query string: "<<query<<endl;
	KexiDBRecord *rec;

        rec = projectPart()->kexiProject()->db()->queryRecord(query, true);
        if (!rec)
        {
                kdDebug() << "KexiQueryPartItem(): db-error" << endl;
                projectPart()->kexiProject()->db()->latestError()->toUser(0);
                return 0;
        }

	m_fields.clear();
	for (int i=0;i<rec->fieldCount();i++)
	{
		kdDebug()<<"KexiQueryPartItem::records():adding "<<rec->fieldName(i)<<" to cache"<<endl;
		m_fields<<rec->fieldName(i);
	}
	return rec;
}


KexiQueryPartItem::~KexiQueryPartItem()
{
}


#include "kexiquerypartitem.moc"

