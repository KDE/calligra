
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
#include <qdialog.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qsplitter.h>
#include <qregexp.h>

#include <klistview.h>
#include <klocale.h>
#include <kdebug.h>
#include <kpushbutton.h>
#include <klineeditdlg.h>

#include "kexiDB/kexidb.h"
#include "kexiDB/kexidbrecord.h"

#include "kexitableview.h"
#include "kexitableitem.h"
#include "kexiprojecthandler.h"
#include "kexiquerydesignerguieditor.h"
#include "kexiparameterlisteditor.h"
#include "kexidragobjects.h"
#include "kexiproject.h"
#include "kexiview.h"
#include "kexiaddparamdialog.h"
#include "kexirelation.h"

KexiQueryDesignerGuiEditor::KexiQueryDesignerGuiEditor(KexiView *view,QWidget *parent, KexiQueryDesigner *myparent, const char *name)
 : QWidget(parent, name)
{
	m_db = view->project()->db();
	m_view = view;
	m_parent = myparent;

	QSplitter *hSplitter = new QSplitter(Vertical, this);
	m_tables = view->project()->handlerForMime("kexi/relation")->embeddReadOnly(hSplitter, view);
	QSplitter *vSplitter = new QSplitter(Horizontal, hSplitter);
//	m_tables = new KexiRelationDialog(view,this, "querytables", true);
	m_designTable = new KexiTableView(vSplitter);
	m_designTable->m_editOnDubleClick = true;
	connect(m_designTable, SIGNAL(dropped(QDropEvent *)), this, SLOT(slotDropped(QDropEvent *)));
	connect(m_designTable, SIGNAL(itemChanged(KexiTableItem *, int)), this, SLOT(slotItemChanged(KexiTableItem *, int)));
	m_paramList=new KexiParameterListEditor(vSplitter);
	connect(m_paramList->addParameter,SIGNAL(clicked()),this,SLOT(slotAddParameter()));

	m_designTable->viewport()->setAcceptDrops(true);
	m_designTable->addDropFilter("kexi/field");

	m_sourceList = view->project()->db()->tables();
	m_sourceList.prepend(i18n("[no table]"));

	m_designTable->addColumn(i18n("source"), QVariant::StringList, true, QVariant(m_sourceList));
	m_designTable->addColumn(i18n("field"), QVariant::String, true);
	m_designTable->addColumn(i18n("shown"), QVariant::Bool, true);
	m_designTable->addColumn(i18n("AND condition"), QVariant::String, true);
	m_designTable->addColumn(i18n("OR condition"), QVariant::String, true);

	clear();
/*
	m_insertItem = new KexiTableItem(m_designTable);
	m_insertItem->setValue(0, 0);
	m_insertItem->setValue(2, true);
	m_insertItem->setInsertItem(true);
*/
	QGridLayout *g = new QGridLayout(this);
	g->addWidget(hSplitter, 0, 0);
//	g->addMultiCellWidget(m_tables,		0,	0,	0,	1);
//	g->addWidget(m_designTable,		1,	0);
//	g->addWidget(m_paramList,		1,	1);
}

void KexiQueryDesignerGuiEditor::clear()
{
	m_designTable->clear();

        m_insertItem = new KexiTableItem(m_designTable);
        m_insertItem->setValue(0, 0);
        m_insertItem->setValue(2, true);
        m_insertItem->setInsertItem(true);
}

void KexiQueryDesignerGuiEditor::appendLine(const QString &source, const QString &field, bool show,
	const QString &andC, const QString &orC)
{
	uint i=0;
	uint tid=0;
        for(QStringList::Iterator it = m_sourceList.begin(); it != m_sourceList.end(); it++)
        {
                if(source == (*it))
                {
			tid=i;
                        break;
                }
                i++;
        }

	m_insertItem->setValue(0,tid);
	m_insertItem->setValue(1,field);
	m_insertItem->setValue(2,show);
	m_insertItem->setValue(3,andC);
	m_insertItem->setValue(4,orC);
	m_insertItem->setInsertItem(false);

        KexiTableItem *newInsert = new KexiTableItem(m_designTable);
        newInsert->setValue(0, 0);
        newInsert->setValue(2, true);
        newInsert->setInsertItem(true);
        m_insertItem = newInsert;
}

void
KexiQueryDesignerGuiEditor::slotDropped(QDropEvent *ev)
{
	kdDebug() << "KexiQueryDesignerGuiEditor::slotDropped()" << endl;

	QString srcTable;
	QString srcField;
        QString dummy;
        //better check later if the source is really a table
        KexiFieldDrag::decode(ev,dummy,srcTable,srcField);
        kdDebug() << "KexiRelationViewTable::slotDropped() srcfield: " << srcField << endl;


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

#warning fixme
	//yo, let's get ugly :)

	ConditionList conditions;
	InvolvedTables involvedTables;

	if(m_designTable->rows() == 1)
		return "";

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
			if(involvedTables.contains(table))
			{
				iCount = involvedTables[table].involveCount;
			}
			else
			{
				iCount = 0;
			}

			InvolvedTable ivTable;
			ivTable.involveCount = iCount + 1;
			involvedTables.insert(table, ivTable, true);


			involvedFields.insert(table, field);

			Condition condition;
			condition.field = table + "." + field;
			condition.andCondition = cItem->getValue(3).toString();
			condition.orCondition = cItem->getValue(4).toString();

			conditions.append(condition);

			if(mI != m_designTable->rows() - 2)
				query += ", ";

			mI++;
		}
	}

	JoinFields joinFields;

	RelationList relations = m_view->project()->relationManager()->projectRelations();

	QString maxTable = QString::null;
	int maxCount = 0;
	bool isSrcTable = false;
	for(InvolvedTables::Iterator it = involvedTables.begin(); it != involvedTables.end(); it++)
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

	if(maxTable == QString::null)
	{
		if(involvedTables.count() == 1)
		{
			maxTable = involvedTables.begin().key();
		}
	}

	query += " FROM ";
	query += maxTable;

	QStringList joined;
	for(JoinFields::Iterator itJ = joinFields.begin(); itJ != joinFields.end(); itJ++)
	{
		QStringList leftList = QStringList::split(".", (*itJ).eqLeft);
		kdDebug() << "KexiQueryDesignerGuiEditor::getQuery(): left: " << (*itJ).eqLeft << endl;
		kdDebug() << "KexiQueryDesignerGuiEditor::getQuery(): current master: " << leftList.first() << endl;


		if(leftList.first() == maxTable && joined.findIndex((*itJ).sourceField) == -1)
		{
			query += " LEFT JOIN ";
			query += (*itJ).sourceField;
			query += " ON ";
			query += (*itJ).eqLeft;
			query += " = ";
			query += (*itJ).eqRight;

			joined.append((*itJ).sourceField);
		}
	}

	int conditionCount = 0;
	for(ConditionList::Iterator itC = conditions.begin(); itC != conditions.end(); itC++)
	{
		if(!(*itC).andCondition.isEmpty())
		{
			if(conditionCount != 0)
				query += " AND ";
			else
				query += " WHERE ";

			QString ccondition = (*itC).andCondition;
			if(ccondition.contains("kexi_"))
			{
				QRegExp exp("kexi_[a-zA-Z0-9]*");
				exp.search(ccondition);
				ccondition.replace(exp, getParam(exp.cap(), true));
			}

			query += (*itC).field + " " + ccondition;
			conditionCount++;
		}
		if(!(*itC).orCondition.isEmpty())
		{
			query += " OR " + (*itC).field + " " + (*itC).orCondition;
			conditionCount++;
		}
	}

	//ok, we are trying to get the conditions


	kdDebug() << "KexiQueryDesignerGuiEditor::getQuery() query: " << query << endl;

	return query;
}

QString
KexiQueryDesignerGuiEditor::getParam(const QString &name, bool escape)
{
	kdDebug() << "KexiQueryDesignerGuiEditor::getParam()" << endl;
	if(m_paramList->list->findItem(name, 0))
	{
		bool ok;
		QString result = KLineEditDlg::getText(i18n("Query"), name, "", &ok, this);
		if(escape)
			return QString("\"" + result + "\"");
	}
	else
	{
		return name;
	}
}

KexiQueryDesignerGuiEditor::~KexiQueryDesignerGuiEditor() {
	m_parent->saveBack();
}

void
KexiQueryDesignerGuiEditor::slotAddParameter()
{
	KexiAddParamDialog d(this);
	if (d.exec()==QDialog::Accepted)
	{
		kdDebug() << "KexiQueryDesignerGuiEditor::slotAddParameter(): name=" << d.parameterName() << endl;
		new KListViewItem(m_paramList->list, QString("kexi_" + d.parameterName()), "type");
	}
}

void KexiQueryDesignerGuiEditor::slotRemoveParameter() {
}


#include "kexiquerydesignerguieditor.moc"
