/* This file is part of the KDE project
   Copyright (C) 2003   Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2003   Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
 */

#include <qtimer.h>

#include <kmessagebox.h>
#include <klocale.h>
#include <kdebug.h>
#include <kaction.h>

#include <kexidb/connection.h>
#include <kexidb/cursor.h>

#include "kexitableheader.h"
#include "kexidatatableview.h"


KexiDataTableView::KexiDataTableView(QWidget *parent, const char *name)
 : KexiTableView(0, parent, name)
{
	init();
}

KexiDataTableView::KexiDataTableView(QWidget *parent, const char *name, KexiDB::Cursor *cursor)
 : KexiTableView(0, parent, name)
{
	init();
	setData(cursor);
}

KexiDataTableView::~KexiDataTableView()
{
}

void
KexiDataTableView::init()
{
	m_cursor = 0;

//	m_maxRecord = 0;
//	m_records = 0;
//	m_first = false;

//	connect(this, SIGNAL(contentsMoving(int, int)), this, SLOT(slotMoving(int)));
//	connect(verticalScrollBar(), SIGNAL(sliderMoved(int)), this, SLOT(slotMoving(int)));
}

/*void KexiDataTableView::initActions(KActionCollection *col)
{
	KexiTableView::initActions(col);
	new KAction(i18n("Filter"), "filter", 0, this, SLOT(filter()), col, "tablepart_filter");
}*/

bool KexiDataTableView::setData(KexiDB::Cursor *cursor)
{
//js	if (!m_first)
//js		clearColumns();
	if (!cursor) {
		clearColumns();
		m_cursor = 0;
		return true;
	}
	if (cursor!=m_cursor) {
		clearColumns();
	}
	m_cursor = cursor;

	if (!m_cursor->query()) {
		kdDebug() << "KexiDataTableView::setData(): WARNING: cursor should have query schema defined!\n--aborting setData()." << endl;
		m_cursor->debug();
		clearColumns();
		return false;
	}

	if (m_cursor->fieldCount()<1) {
		clearColumns();
		return true;
	}

	if (!m_cursor->isOpened() && !m_cursor->open()) {
		kdDebug() << "KexiDataTableView::setData(): WARNING: cannot open cursor\n--aborting setData()." << endl;
		m_cursor->debug();
		clearColumns();
		return false;
	}

	uint i = 0;
	KexiDB::Field::Vector vector = m_cursor->query()->fieldsExpanded();
	KexiTableViewData *tv_data = new KexiTableViewData(m_cursor);
/* moved to KexiTableViewData ctor
	KexiTableViewColumn* col;
	for (i=0;i<vector.count();i++) {
		KexiDB::Field *f = vector[i];// = list->first();
		col=new KexiTableViewColumn(*m_cursor->query(), *f);
		tv_data->addColumn( col );
	}*/

	QString caption = m_cursor->query()->caption();
	if (caption.isEmpty()) {
		caption = m_cursor->query()->name();
	}
	setCaption( caption );

	//PRIMITIVE!! data setting:
//	const char **cd = m_cursor->recordData();

	const uint fcount = m_cursor->fieldCount();
	m_cursor->moveFirst();
	while (!m_cursor->eof()) {
		KexiTableItem *item = new KexiTableItem(fcount);
		m_cursor->storeCurrentRow(*item);
/*		for (uint f=0; f<fcount; f++) {
			item->at(f) = m_cursor->value(f);
		}*/
		tv_data->append( item );
		m_cursor->moveNext();
	}
//	if (m_cursor->moveFirst() && m_cursor->moveLast()) {
//	}

	KexiTableView::setData(tv_data);


#if 0
	if(!m_first)
		clearAll();

	if(m_record)
	{
		disconnect(m_record,0,this,0);
		delete m_record;
	}

	m_record=rec;

	if(!m_record)
		return;

	connect(this, SIGNAL(itemChanged(KexiTableItem *, int,QVariant)), this, SLOT(slotItemChanged(KexiTableItem *, int,QVariant)));
	connect(m_record,SIGNAL(recordInserted(KexiDBUpdateRecord*)),this,SLOT(recordInsertFinished(KexiDBUpdateRecord*)));


	for(uint i = 0; i < m_record->fieldCount(); i++)
	{
		QVariant defaultval = QVariant("");
		addColumn(m_record->fieldName(i), m_record->type(i), !m_record->readOnly(),
		 defaultval, 100, m_record->fieldInfo(i)->auto_increment());
	}

	m_records = m_record->numRows();
	if(!m_record->readOnly())
		m_records++;

	resizeContents(contentsWidth(), m_records * rowHeight());

	m_first = false;

	int lastRow = rowAt(visibleHeight(), true) + 100;
	if(lastRow > m_records)
		lastRow = m_records;

	verticalHeader()->setUpdatesEnabled(false);
	while(m_maxRecord < lastRow)
	{
		insertNext();
	}
	verticalHeader()->setUpdatesEnabled(true);

	if(lastRow < m_records)
	{
	}
#endif
	return true;
}

bool
KexiDataTableView::readOnly()
{
//	return m_record ? m_record->readOnly() : true;
	return true;
}

#if 0
void
KexiDataTableView::insertNext()
{
	if((m_maxRecord + 1) == m_records && !m_record->readOnly()) {
		appendInsertItem();
		return;
	}else if(!m_record->next())
		return;

	KexiTableItem *it = new KexiTableItem(this);
	for(uint i = 0; i < m_record->fieldCount(); i++)
	{
		it->setValue(i, m_record->value(i));
	}
	it->setHint(QVariant(m_maxRecord));
	m_maxRecord++;
}
#endif

/*jsvoid
KexiDataTableView::appendInsertItem()
{
	KexiTableItem *it = new KexiTableItem(this);
	it->setInsertItem(true);
	m_maxRecord++;
}*/

#if 0
void
KexiDataTableView::slotItemChanged(KexiTableItem *i, int col,QVariant oldValue)
{
	kdDebug() << "KexiDataTableView::slotItemChanged()" << endl;
	if(i->isInsertItem())
	{
		i->setInsertItem(false);
		i->setHint("UPDATING");
//	 	KexiDBUpdateRecord *urec=m_record->insert(true);
		KexiDBUpdateRecord *urec=m_record->insert(true);
		urec->setValue(col,i->getValue(col));
		m_insertMapping.insert(urec,i);


		if ((!m_record->writeOut(urec)))
		{
			KMessageBox::detailedError(this, i18n("Error occurred while updating table."), m_record->latestError()->message(),
			 i18n("Database Error"));
			return;
		}

		KexiDBField *fi = m_record->fieldInfo(col);
		m_record->database()->watcher()->update(this, fi->table(), fi->name(), i->getHint().toUInt(),
		 i->getValue(col));

		KexiTableItem *newinsert = new KexiTableItem(this);
		newinsert->setHint(QVariant(i->getHint().toInt() + 1));
		newinsert->setInsertItem(true);

		takeInsertItem();
		recordMarker()->setInsertRow(rows());
		setInsertItem(newinsert);
	}
	else
	{


		QMap<QString,QVariant> fnvm;
		for (int c=0;c<cols();c++)
			fnvm[column(c)]=((c==col)?oldValue:i->getValue(c));
		KexiDBUpdateRecord *ur=m_record->update(fnvm);
		if (ur) {
			ur->setValue(col,i->getValue(col));
			m_record->writeOut();
		}
	}
}
#endif

/*QSize
KexiDataTableView::tableSize() const
{
	return QSize(KexiTableView::tableSize().width(), m_records * rowHeight());
}*/

#include "kexidatatableview.moc"
