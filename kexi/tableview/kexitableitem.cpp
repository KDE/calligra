/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Daniel Molkentin <molkentin@kde.org>

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
 
   Original Author:  Till Busch <till@bux.at>
   Original Project: buX (www.bux.at)
*/

#include <qtimer.h>
//#include <stdarg.h>
#include "kexitableheader.h"
#include "kexitableview.h"
#include "kexitablerm.h"
#include "kexitableitem.h"

KexiTableItem::KexiTableItem(int numCols)
{
	m_columns.resize(numCols);
	m_insertItem = false;
	m_pTable = 0;
}

void KexiTableItem::attach(KexiTableView *tableView, bool sorted=false)
{
	if(!sorted)
	{
		qDebug("inserting somewhere");
		tableView->m_contents->append(this);
	}
	else
	{
		qDebug("inserting sorted");
		tableView->m_contents->inSort(this);
	}
	tableView->m_numRows++;
	tableView->triggerUpdate();

//	if(isInsertItem())
//		tableView->recordMarker()->setInsertRow(position);
}

void KexiTableItem::attach(KexiTableView *tableView, int position)
{
//	qDebug("inserting at position %i", position);
	m_position = position;
	tableView->m_contents->insert(position, this);
	tableView->m_numRows++;
	tableView->triggerUpdate();

//	if(isInsertItem())
//		tableView->recordMarker()->setInsertRow(position);
}

KexiTableItem::KexiTableItem(KexiTableView *tableView)
{
	int numCols=tableView->cols();
	m_columns.resize(numCols);

	m_insertItem = false;
//	m_columns.setAutoDelete(true);

//	m_columnsI.resize(numCols);
//	m_columnsF.resize(numCols);

//	for(int i=0; i < tableView->cols(); i++)
//		m_columns.insert(i, new QString);
//	for(int i=0; i < tableView->cols(); i++)
//		m_columns.insert(i, new QVariant);

	tableView->m_contents->append(this);
	tableView->inserted();

	tableView->m_numRows++;
	tableView->triggerUpdate();
	m_pTable = tableView;
//	tableView->m_pUpdateTimer->start(1,true);
//	tableView->columnWidthChanged(0,0,0);
}
/*
KexiTableItem::KexiTableItem(KexiTableView *tableView, bool sorted, ...)
{
	va_list ap;
	va_start(ap, sorted);

	int numCols=tableView->cols();
	m_columns.resize(numCols);
	m_columns.setAutoDelete(true);

//	m_columnsI.resize(numCols);
//	m_columnsF.resize(numCols);

//	for(int i=0; i < tableView->cols(); i++)
//		m_columns.insert(i, new QString);
	for(int i=0; i < tableView->cols(); i++)
		m_columns.insert(i, new QVariant(va_arg(ap, QVariant)));


    if(!sorted)
		tableView->m_contents.append(this);
	else
	{
		qDebug("inserting sorted");
		tableView->m_contents.inSort(this);
	}

	tableView->m_numRows++;
	tableView->m_pUpdateTimer->start(1,true);
//	tableView->columnWidthChanged(0,0,0);
	va_end(ap);
}
*/
void
KexiTableItem::setInsertItem(bool insertItem)
{
	m_insertItem = insertItem;

	if(!m_pTable)
		return;

//	int m_position = m_pTable->m_contents.find(this);
	if(insertItem)
	{
		m_pTable->recordMarker()->setInsertRow(getHint().toInt() + 1);
		m_pTable->setInsertItem(this);
		qDebug("inserting into %i", getHint().toInt());
	}
	else
	{
		if(m_pTable->insertItem() && m_pTable->insertItem() == this)
		{
			m_pTable->takeInsertItem();
		}
	}

}


KexiTableItem::~KexiTableItem()
{
//	qDebug("KexiTableItem::~KexiTableItem()");
}
