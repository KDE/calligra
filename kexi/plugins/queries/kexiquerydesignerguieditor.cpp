/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

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

#include <qlayout.h>
#include <qcombobox.h>
#include <qpushbutton.h>

#include <klocale.h>
#include <kdebug.h>

#include "kexiDB/kexidb.h"
#include "kexiDB/kexidbrecord.h"

#include "kexitableview.h"
#include "kexitableitem.h"
#include "kexiquerydesignerguieditor.h"
//#include "kexirelationdialog.h"
#include "kexiproject.h"
#include "kexiview.h"

KexiQueryDesignerGuiEditor::KexiQueryDesignerGuiEditor(KexiView *view,QWidget *parent, KexiQueryDesigner *myparent, const char *name)
 : QWidget(parent, name)
{
	m_db = view->project()->db();
	m_parent = myparent;

//	m_tables = new KexiRelationDialog(view,this, "querytables", true);

	m_designTable = new KexiTableView(this);
	m_designTable->m_editOnDubleClick = true;
	connect(m_designTable, SIGNAL(dropped(QDropEvent *)), this, SLOT(slotDropped(QDropEvent *)));
	connect(m_designTable, SIGNAL(itemChanged(KexiTableItem *, int)), this, SLOT(slotItemChanged(KexiTableItem *, int)));

	m_designTable->viewport()->setAcceptDrops(true);
	m_designTable->addDropFilter("kexi/field");

	m_sourceList = view->project()->db()->tables();
	m_sourceList.prepend(i18n("[no table]"));

	m_designTable->addColumn(i18n("source"), QVariant::StringList, true, QVariant(m_sourceList));
	m_designTable->addColumn(i18n("field"), QVariant::String, true);
	m_designTable->addColumn(i18n("shown"), QVariant::Bool, true);
	m_designTable->addColumn(i18n("AND condition"), QVariant::String, true);
	m_designTable->addColumn(i18n("OR condition"), QVariant::String, true);


	m_insertItem = new KexiTableItem(m_designTable);
	m_insertItem->setValue(0, 0);
	m_insertItem->setValue(2, true);
	m_insertItem->setInsertItem(true);

	QGridLayout *g = new QGridLayout(this);
//	g->addWidget(m_tables,		0,	0);
	g->addWidget(m_designTable,	1,	0);
}

void
KexiQueryDesignerGuiEditor::slotDropped(QDropEvent *ev)
{
#if 0
	kdDebug() << "KexiQueryDesignerGuiEditor::slotDropped()" << endl;

	KexiRelationViewTable *sourceTable = static_cast<KexiRelationViewTable *>(ev->source());

	QString srcTable = sourceTable->table();
	QString srcField(ev->encodedData("kexi/field"));

	uint i=0;
	for(QStringList::Iterator it = m_sourceList.begin(); it != m_sourceList.end(); it++)
	{
		kdDebug() << "KexiQueryDesignerGuiEditor::slotDropped()" << srcTable << ":" << (*it) << endl;
		if(srcTable == (*it))
		{
			break;
		}
		i++;
	}

	m_insertItem->setValue(0, i);
	m_insertItem->setValue(1, srcField);
	m_insertItem->setValue(2, true);
	m_insertItem->setInsertItem(false);

	KexiTableItem *newInsert = new KexiTableItem(m_designTable);
	newInsert->setValue(0, 0);
	newInsert->setValue(2, true);
	newInsert->setInsertItem(true);
	m_insertItem = newInsert;
#endif
}

void
KexiQueryDesignerGuiEditor::slotItemChanged(KexiTableItem *item, int col)
{
	if(item->isInsertItem())
	{
		item->setInsertItem(false);

		KexiTableItem *newInsert = new KexiTableItem(m_designTable);
		newInsert->setValue(0, 0);
		newInsert->setValue(2, true);
		newInsert->setInsertItem(true);
		m_insertItem = newInsert;
	}
}

QString
KexiQueryDesignerGuiEditor::getQuery()
{
#if 0
#warning fixme
	//yo, let's get ugly :)

	if(m_designTable->rows() == 1)
		return "";

	QString	 mainTable;

	QString query = "SELECT ";

	QMap<QString, QString> involvedFields;

	int mI = 0;
	for(int i=0; i < m_designTable->rows(); i++)
	{
		KexiTableItem *cItem = m_designTable->itemAt(i);
		if(!cItem->isInsertItem())
		{
			//retriving table & field
			int tableID = cItem->getValue(0).toInt();
			QString field = cItem->getValue(1).toString();
			QString table = (*m_sourceList.at(tableID));

			query += table + "." + field;


			int iCount;
			if(m_involvedTables.contains(table))
			{
				iCount = m_involvedTables[table].involveCount;
			}
			else
			{
				iCount = 0;
			}

			InvolvedTable ivTable;
			ivTable.involveCount = iCount + 1;
			m_involvedTables.insert(table, ivTable, true);


			involvedFields.insert(table, field);

			Condition condition;
			condition.field = field;
			condition.andCondition = cItem->getValue(3).toString();
			condition.orCondition = cItem->getValue(4).toString();

			m_conditions.append(condition);

			if(mI != m_designTable->rows() - 2)
				query += ", ";

			mI++;
		}
	}

	JoinFields joinFields;

	RelationList relations = m_tables->view()->getConnections();

	QString maxTable;
	int maxCount = 0;
	bool isSrcTable = false;
	for(InvolvedTables::Iterator it = m_involvedTables.begin(); it != m_involvedTables.end(); it++)
	{
		for(RelationList::Iterator itRel = relations.begin(); itRel != relations.end(); itRel++)
		{
			if((*itRel).srcTable == it.key())
			{
				kdDebug() << "KexiQueryDesignerGuiEditor::getQuery(): " << it.key() << " inherits" << endl;
				for(QMap<QString,QString>::Iterator itS = involvedFields.begin(); itS != involvedFields.end(); itS++)
				{
					if(itS.key() == it.key())
					{
						isSrcTable = true;

						maxTable = it.key();
						maxCount = it.data().involveCount;
					}
				}
			}
			else if((*itRel).rcvTable == it.key())
			{
				kdDebug() << "KexiQueryDesignerGuiEditor::getQuery(): " << it.key() << " is inherited" << endl;
				for(QMap<QString,QString>::Iterator itS = involvedFields.begin(); itS != involvedFields.end(); itS++)
				{
					if(itS.key() == it.key())
					{
						isSrcTable = false;

						JoinField jf;
						jf.sourceField = (*itRel).rcvTable;
						jf.eqLeft = (*itRel).srcTable + "." + (*itRel).srcField;
						jf.eqRight = (*itRel).rcvTable + "." + (*itRel).rcvField;
						joinFields.append(jf);
					}

					if(maxCount < 0)
					{
						maxTable = it.key();
						maxCount = it.data().involveCount;
					}
				}
			}
		}
	}

	//get "forign" tables

	query += " FROM ";
	query += maxTable;

	for(JoinFields::Iterator itJ = joinFields.begin(); itJ != joinFields.end(); itJ++)
	{
		QStringList leftList = QStringList::split(".", (*itJ).eqLeft);
		kdDebug() << "KexiQueryDesignerGuiEditor::getQuery(): left: " << (*itJ).eqLeft << endl;
		kdDebug() << "KexiQueryDesignerGuiEditor::getQuery(): current master: " << leftList.first() << endl;


		if(leftList.first() == maxTable)
		{
			query += " LEFT JOIN ";
			query += (*itJ).sourceField;
			query += " ON ";
			query += (*itJ).eqLeft;
			query += " = ";
			query += (*itJ).eqRight;
		}
	}

	int conditionCount = 0;
	for(ConditionList::Iterator itC = m_conditions.begin(); itC != m_conditions.end(); itC++)
	{
		if((*itC).orCondition == "" && (*itC).andCondition == "")
		{
			conditionCount--;
		}
		else if((*itC).andCondition != "")
		{
			if(conditionCount != 0)
				query += " AND ";
			else
				query += " WHERE ";

			query += (*itC).field + " " + (*itC).andCondition;
		}
		if((*itC).orCondition != "")
		{
			query += " OR " + (*itC).field + " " + (*itC).orCondition;
		}

		conditionCount++;
	}

	//ok, we are trying to get the conditions


	kdDebug() << "KexiQueryDesignerGuiEditor::getQuery() query: " << query << endl;

	return query;
#endif
}

KexiQueryDesignerGuiEditor::~KexiQueryDesignerGuiEditor()
{
}


#include "kexiquerydesignerguieditor.moc"
