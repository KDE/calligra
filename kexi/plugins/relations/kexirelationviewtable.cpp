/* This file is part of the KDE project
   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>

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

#include <qheader.h>

#include <kdebug.h>

#include "kexirelationviewtable.h"

#include <kexidragobjects.h>

KexiRelationViewTable::KexiRelationViewTable(KexiRelationView *parent, QString table, QStringList fields, const char *name)
 : KListView(parent)
{
	m_fieldList = fields;
	m_table = table;
	m_parent = parent;

	setAcceptDrops(true);
	viewport()->setAcceptDrops(true);
	setDropVisualizer(false);

	addColumn("", 0);
	addColumn("fields");
	setResizeMode(QListView::LastColumn);
//	setResizeMode(QListView::AllColumns);
	header()->hide();

	setSorting(0, true); // disable sorting

	int order=0;
	for(QStringList::Iterator it = m_fieldList.begin(); it != m_fieldList.end(); it++)
	{
		KListViewItem *i = new KListViewItem(this, QString::number(order), (*it));
		i->setDragEnabled(true);
		i->setDropEnabled(true);
		order++;
	}

//	setDragEnabled
	connect(this, SIGNAL(dropped(QDropEvent *, QListViewItem *)), this, SLOT(slotDropped(QDropEvent *)));
	connect(this, SIGNAL(contentsMoving(int, int)), this, SLOT(slotContentsMoving(int,int)));
}

void KexiRelationViewTable::setReadOnly(bool b)
{
	setAcceptDrops(!b);
	viewport()->setAcceptDrops(!b);
}

int
KexiRelationViewTable::globalY(const QString &item)
{
	QListViewItem *i = findItem(item, 1);
	if(i)
	{
		int y=itemRect(i).y() + (itemRect(i).height() / 2);
		return mapToGlobal(QPoint(0, y)).y();
	}
	return -1;
}

QDragObject *
KexiRelationViewTable::dragObject()
{
	qDebug("KexiRelationViewTable::dragObject()");
	if(selectedItem())
	{
		KexiFieldDrag *drag = new KexiFieldDrag(m_table, selectedItem()->text(1), this);
		return drag;
	}

	return 0;
}

bool
KexiRelationViewTable::acceptDrag(QDropEvent *ev) const
{
	kdDebug() << "KexiRelationViewTable::acceptDrag()" << endl;
	if(ev->provides("kexi/field") && ev->source() != this)
		return true;

	return false;
}

void
KexiRelationViewTable::slotDropped(QDropEvent *ev)
{
	qDebug("KexiRelationViewTable::slotDropped()");

	QListViewItem *recever = itemAt(ev->pos());
	if(recever)
	{
		KexiRelationViewTable *sourceTable = static_cast<KexiRelationViewTable *>(ev->source());

		QString srcTable = sourceTable->table();
		QString srcField(ev->encodedData("kexi/field"));
		kdDebug() << "KexiRelationViewTable::slotDropped() srcfield: " << srcField << endl;

		QString rcvField = recever->text(1);

		SourceConnection s;
		s.srcTable = srcTable;
		s.rcvTable = m_table;
		s.srcField = srcField;
		s.rcvField = rcvField;

		m_parent->addConnection(s);

		kdDebug() << "KexiRelationViewTable::slotDropped() " << srcTable << ":" << srcField << " " << m_table << ":" << rcvField << endl;
		ev->accept();
		return;
	}
	ev->ignore();

}

void
KexiRelationViewTable::slotContentsMoving(int,int)
{
	emit tableScrolling(table());
}

KexiRelationViewTable::~KexiRelationViewTable()
{
}

#include "kexirelationviewtable.moc"
