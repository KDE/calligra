/* This file is part of the KDE project
   Copyright (C) 2003-2005 Jaroslaw Staniek <js@iidea.pl>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kexifieldlistview.h"

#include <qheader.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qcursor.h>
#include <qpoint.h>
#include <qapplication.h>
#include <qbitmap.h>
#include <qstyle.h>

#include <kdebug.h>
#include <kiconloader.h>
#include <kdeversion.h>
#include <kconfig.h>
#include <kglobalsettings.h>
#include <klocale.h>

#include <kexidb/tableschema.h>
#include <kexidb/queryschema.h>
#include <kexidb/utils.h>
#include <kexidragobjects.h>

KexiFieldListView::KexiFieldListView(QWidget *parent, const char *name, int options)
 : KListView(parent, name)
 , m_schema(0)
 , m_options(options)
 , m_allColumnsItem(0)
{
	m_keyIcon = SmallIcon("key");
	m_noIcon = QPixmap(m_keyIcon.size());
	QBitmap bmp(m_noIcon.size());
	bmp.fill(Qt::color0);
	m_noIcon.setMask(bmp);

	setAcceptDrops(true);
	viewport()->setAcceptDrops(true);
	setDropVisualizer(false);
	setDropHighlighter(true);
	setAllColumnsShowFocus(true);
	addColumn(i18n("Field Name"));
	if (m_options & ShowDataTypes)
		addColumn(i18n("Data Type"));
	if (m_options & AllowMultiSelection)
		setSelectionMode(QListView::Extended);
	setResizeMode(QListView::LastColumn);
//	header()->hide();
	setSorting(-1, true); // disable sorting
	setDragEnabled(true);
	
	connect(this, SIGNAL(doubleClicked(QListViewItem*, const QPoint &, int)), 
		this, SLOT(slotDoubleClicked(QListViewItem*)));
}

KexiFieldListView::~KexiFieldListView()
{
	delete m_schema;
}

void KexiFieldListView::setSchema(KexiDB::TableOrQuerySchema* schema)
{
	if (schema && m_schema == schema)
		return;
	m_allColumnsItem = 0;
	clear();
	delete m_schema;
	m_schema = schema;
	if (!m_schema)
		return;

	int order=0;
	bool hasPKeys = true; //t->hasPrimaryKeys();
	KListViewItem *item = 0;
	KexiDB::QueryColumnInfo::Vector columns = m_schema->columns(true /*unique*/);
	const int count = columns.count();
	for(int i=-1; i < count; i++)
	{
		KexiDB::QueryColumnInfo *colinfo = 0;
		if (i==-1) {
			if (! (m_options & ShowAsterisk))
				continue;
			item = new KListViewItem(this, item, i18n("* (All Columns)"));
			m_allColumnsItem = item;
		}
		else {
			colinfo = columns[i];
			item = new KListViewItem(this, item, colinfo->aliasOrName());
			if (m_options & ShowDataTypes)
				item->setText(1, colinfo->field->typeName());
		}
		if(colinfo && (colinfo->field->isPrimaryKey() || colinfo->field->isUniqueKey()))
			item->setPixmap(0, m_keyIcon);
		else if (hasPKeys) {
			item->setPixmap(0, m_noIcon);
		}
		order++;
	}

	setCurrentItem(firstChild());
}

#if 0
QSize KexiFieldListView::sizeHint()
{
	QFontMetrics fm(font());

	kdDebug() << m_table->name() << " cw=" << columnWidth(1) + fm.width("i") << ", " << fm.width(m_table->name()+"  ") << endl; 

	QSize s( 
		QMAX( columnWidth(1) + fm.width("i"), fm.width(m_table->name()+"  ")), 
		childCount()*firstChild()->totalHeight() + 4 );
//	QSize s( columnWidth(1), childCount()*firstChild()->totalHeight() + 3*firstChild()->totalHeight()/10);
	return s;
}

void KexiFieldListView::setReadOnly(bool b)
{
	setAcceptDrops(!b);
	viewport()->setAcceptDrops(!b);
}
#endif

QDragObject* KexiFieldListView::dragObject()
{
	if (!schema())
		return 0;
	const QStringList selectedFields( selectedFieldNames() );
	return new KexiFieldDrag(m_schema->table() ? "kexi/table" : "kexi/query", 
		m_schema->name(), selectedFields, this, "KexiFieldDrag");
/*	if (selectedItem()) {
		KexiFieldDrag *drag = new KexiFieldDrag("kexi/table", m_schema->name(), 
			selectedItem()->text(1), this, "KexiFieldDrag");
			return drag;
	}*/
}

QStringList KexiFieldListView::selectedFieldNames() const
{
	if (!schema())
		return QStringList();
	QStringList selectedFields;
	for (QListViewItem *item = firstChild(); item; item = item->nextSibling()) {
		if (item->isSelected()) {
//! @todo what about query fields/aliases? it.current()->text(0) can be not enough
			if (item == m_allColumnsItem && m_allColumnsItem)
				selectedFields.append("*");
			else
				selectedFields.append(item->text(0));
		}
	}
	return selectedFields;
}

void KexiFieldListView::slotDoubleClicked(QListViewItem* item)
{
	if (schema() && item) {
	//! @todo what about query fields/aliases? it.current()->text(0) can be not enough
		emit fieldDoubleClicked(schema()->table() ? "kexi/table" : "kexi/query",
			schema()->name(), item->text(0));
	}
}

#include "kexifieldlistview.moc"
