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

#include <qstringlist.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qheader.h>
#include <qevent.h>
//#include <qbytearray.h>

#include "kexirelationview.h"

KexiRelationView::KexiRelationView(QWidget *parent, const char *name)
 : QScrollView(parent, name)
{
	setFrameStyle(QFrame::WinPanel | QFrame::Sunken);

	m_tableCount = 0;
	
	viewport()->setPaletteBackgroundColor(colorGroup().mid());
}

void
KexiRelationView::addTable(QString table, QStringList columns)
{
//	QFrame *f = new QFrame(this);
//	f->setFrameStyle(QFrame::WinPanel | QFrame::Raised);
	KexiRelationViewTable *f = new KexiRelationViewTable(this, table, columns);
//	QVBoxLayout *l = new QVBoxLayout(f);
//	l->addWidget(t);
	

	f->setGeometry(0, 0, 155, 250);


	if(m_tableCount == 0)
	{
		addChild(f, 5, 5);
	}
	else
	{
		addChild(f, ((155 + 30) * m_tableCount) + 5, 5);
	}


	f->show();

	m_tables.append(f);
	m_tableCount++;
}

void
KexiRelationView::addConnection(Connection connection)
{

}

KexiRelationView::~KexiRelationView()
{
}


/// implementation of KexiRelationViewTable

KexiRelationViewTable::KexiRelationViewTable(QWidget *parent, QString table, QStringList fields, const char *name)
 : QListView(parent)
{
	m_fieldList = fields;

	addColumn(table);
	header()->hide();
	for(QStringList::Iterator it = m_fieldList.begin(); it != m_fieldList.end(); it++)
	{
		new QListViewItem(this, (*it));
	}
	
//	setDragEnabled(true);
//	setDragEnabled
	connect(this, SIGNAL(dropped(QDropEvent *)), this, SLOT(slotDroppend(QDropEvent *ev)));
}

QDragObject *
KexiRelationViewTable::dragObject()
{
	qDebug("KexiRelationViewTable::dragObject()");
}

void
KexiRelationViewTable::slotDropped(QDropEvent *ev)
{
	qDebug("KexiRelationViewTable::slotDropped()");
}

KexiRelationViewTable::~KexiRelationViewTable()
{
}


/// implementation of KexiFieldMetaDrag

KexiFieldMetaDrag::KexiFieldMetaDrag(uchar meta, QWidget *parent, const char *name)
 : QStoredDrag("kexi/field", parent, name)
{
	QByteArray data(1);
	data[0] = meta;
	setEncodedData(meta);
}


bool
KexiFieldMetaDrag::canDecode(QDragMoveEvent *e)
{
	return e->provides("kexi/field");
}

bool
KexiFieldMetaDrag::decode( QDropEvent* e, QString& str )
{
	QByteArray payload = e->data("secret/magic");
	if(payload.size())
	{
		e->accept();
		str = payload[0];
		return true;
	}
	return false;
}

#include "kexirelationview.moc"
