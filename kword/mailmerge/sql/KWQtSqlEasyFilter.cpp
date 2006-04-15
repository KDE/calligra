/* This file is part of the KDE project
 * Copyright (C) 2001 Joseph Wenninger <jowenn@kde.org>
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
#include "KWQtSqlEasyFilter.h"
#include "KWQtSqlEasyFilter.moc"
#include <q3table.h>
#include <q3scrollview.h>
#include <klocale.h>
#include <qlayout.h>
#include <qcheckbox.h>

KWQtSqlEasyFilter::KWQtSqlEasyFilter( QWidget *parent)
        :KDialogBase( Swallow, i18n( "Mail Merge - Editor" ), Ok | Cancel, Ok, parent, "", true)
{
       m_fieldList << "" <<"one" << "two" << "three" << "four";
       m_sortingList << ""<<i18n("ascending")<<i18n("descending");
       m_operationList <<"="<<i18n("contains")<< "<" << ">";

	m_table=new Q3Table(6,3,this);
	setMainWidget(m_table);
		
	m_table->verticalHeader()->setLabel(0,i18n("Field"));
	m_table->verticalHeader()->setLabel(1,i18n("Sorting Order"));
	m_table->verticalHeader()->setLabel(2,i18n("Include"));
	m_table->verticalHeader()->setLabel(3,i18n("Operator"));
	m_table->verticalHeader()->setLabel(4,i18n("Condition"));
	m_table->verticalHeader()->setLabel(5,i18n("Value"));
	m_table->setSelectionMode(Q3Table::NoSelection);
        m_table->setColumnMovingEnabled(true);
	m_table->setSorting(false);

	for (int i=0; i<3; i++)
	{
		createColumn(i);	
	}

	int h=m_table->rowHeight(0);
	for (int i=0;i<6;i++) h+=m_table->rowHeight(i);
	h+=m_table->horizontalHeader()->sizeHint().height();
	m_table->setMinimumHeight(h);
	
	int w=0;
	for (int i=0;i<3;i++) w+=m_table->columnWidth(i);
	w+=m_table->verticalHeader()->headerWidth();
	m_table->setMinimumWidth(w);
	connect(m_table,SIGNAL(valueChanged ( int, int)),this,SLOT(slotValueChanged ( int, int)));
}

void KWQtSqlEasyFilter::createColumn(int i)
{
	Q3TableItem *it;
	m_table->setItem(0,i,it=new Q3ComboTableItem(m_table,m_fieldList,false));
	m_table->setItem(1,i,it=new Q3ComboTableItem(m_table,m_sortingList,false));
	it->setEnabled(false);
	m_table->setItem(2,i,it=new Q3CheckTableItem(m_table,i18n("Yes")));
	it->setEnabled(false);
	m_table->setItem(3,i,it=new Q3CheckTableItem(m_table,i18n("NOT")));
	it->setEnabled(false);
	m_table->setItem(4,i,it=new Q3ComboTableItem(m_table,m_operationList,false));		
	it->setEnabled(false);
	m_table->setItem(5,i,it=new Q3TableItem(m_table,Q3TableItem::WhenCurrent,""));		
	it->setEnabled(false);
	m_table->ensureCellVisible(0,i);
}


void KWQtSqlEasyFilter::slotValueChanged ( int row, int col )
{
	switch (row)
	{
		case 0:	
			if ( !m_table->item(row,col)->text().isEmpty() )
			{
				if (col==m_table->numCols()-1)
				{
					m_table->insertColumns(col+1,1);
					createColumn(col+1);
				}
				m_table->item(1,col)->setEnabled(true);
				m_table->item(2,col)->setEnabled(true);
				bool enableSearch=(static_cast<Q3CheckTableItem*>(m_table->item(2,col))->isChecked());
				m_table->item(3,col)->setEnabled(enableSearch);
				m_table->item(4,col)->setEnabled(enableSearch);
				m_table->item(5,col)->setEnabled(enableSearch);
			}
			else
			{
				for (int i=1;i<6;i++)	m_table->item(i,col)->setEnabled(false);
			}
			break;
		case 2:
			bool enSearch=static_cast<Q3CheckTableItem*>(m_table->item(row,col))->isChecked();
			m_table->item(3,col)->setEnabled(enSearch);
			m_table->item(4,col)->setEnabled(enSearch);
			m_table->item(5,col)->setEnabled(enSearch);
			break;
	}
}

KWQtSqlEasyFilter::~KWQtSqlEasyFilter(){;}

