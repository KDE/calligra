/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <kdebug.h>

#include <qpainter.h>

#include <kiconloader.h>
#include <klocale.h>

#include "objecttree.h"
#include "form.h"
#include "container.h"
#include "formmanager.h"
#include "widgetlibrary.h"

#include "objecttreeview.h"

namespace KFormDesigner
{

// ObjectTreeViewItem

ObjectTreeViewItem::ObjectTreeViewItem(ObjectTreeViewItem *parent, ObjectTreeItem *item)
 : KListViewItem(parent, item->name(), item->className())
{
	m_item = item;
}

ObjectTreeViewItem::ObjectTreeViewItem(KListView *list, ObjectTreeItem *item)
 : KListViewItem(list, item ? item->name() : QString::null, item ? item->className() : QString::null)
{
	m_item = item;
}

const QString
ObjectTreeViewItem::name() const
{
	if(m_item)
		return m_item->name();
	else
		return QString::null;
}

void
ObjectTreeViewItem::paintCell(QPainter *p, const QColorGroup & cg, int column, int width, int align)
{
	int margin = listView()->itemMargin();
	if(column == 1)
	{
		//if(depth()==0)
		if(!m_item)
			return;
		KListViewItem::paintCell(p, cg, column, width, align);
	}
	else
	{
		//if(depth()==0)
		if(!m_item)
			return;

		p->fillRect(0,0,width, height(), QBrush(backgroundColor()));

		if(isSelected())
		{
			p->fillRect(0,0,width, height(), QBrush(cg.highlight()));
			p->setPen(cg.highlightedText());
		}

		QFont f = listView()->font();
		p->save();
		if(isSelected())
			f.setBold(true);
		p->setFont(f);
		if(depth() == 0) // for edit tab order dialog
		{
			QString iconName = ((ObjectTreeView*)listView())->pixmapForClass(m_item->widget()->className());
			p->drawPixmap(margin, (height() - IconSize(KIcon::Small))/2 , SmallIcon(iconName));
			p->drawText(QRect(2*margin + IconSize(KIcon::Small),0,width, height()-1), Qt::AlignVCenter, m_item->name());
		}
		else
			p->drawText(QRect(margin,0,width, height()-1), Qt::AlignVCenter, m_item->name());
		p->restore();

		p->setPen( QColor(200,200,200) ); //like in t.v.
		p->drawLine(width-1, 0, width-1, height()-1);
	}

	p->setPen( QColor(200,200,200) ); //like in t.v.
	p->drawLine(-150, height()-1, width, height()-1 );
}

void
ObjectTreeViewItem::paintBranches(QPainter *p, const QColorGroup &cg, int w, int y, int h)
{
	p->eraseRect(0,0,w,h);
	ObjectTreeViewItem *item = (ObjectTreeViewItem*)firstChild();
	if(!item || !item->m_item || !item->m_item->widget())
		return;

	p->save();
	p->translate(0,y);
	while(item)
	{
		p->fillRect(0,0,w, item->height(), QBrush(item->backgroundColor()));
		p->fillRect(-150,0,150, item->height(), QBrush(item->backgroundColor()));
		p->save();
		p->setPen( QColor(200,200,200) ); //like in t.v.
		p->drawLine(-150, item->height()-1, w, item->height()-1 );
		p->restore();

		if(item->isSelected())
		{
			p->fillRect(0,0,w, item->height(), QBrush(cg.highlight()));
			p->fillRect(-150,0,150, item->height(), QBrush(cg.highlight()));
		}
		/*if(item->firstChild())
		{
		p->drawRect(2, item->height()/2 -4, 9, 9);
		p->drawLine(4, item->height()/2, 8, item->height()/2);
		if(!item->isOpen())
			p->drawLine(6, item->height()/2 - 2, 6, item->height()/2 +2);
		}*/
		QString iconName;
		if(depth() == 0)
			iconName = "form";
		else
			iconName = ((ObjectTreeView*)listView())->pixmapForClass(item->m_item->widget()->className());
		p->drawPixmap((w - IconSize(KIcon::Small))/2, (item->height() - IconSize(KIcon::Small))/2 , SmallIcon(iconName));

		p->translate(0, item->totalHeight());
		item = (ObjectTreeViewItem*)item->nextSibling();
	}
	p->restore();
}

void
ObjectTreeViewItem::setup()
{
	KListViewItem::setup();
	//if(depth()==0)
	if(!m_item)
		setHeight(0);
}

// ObjectTreeView itself

ObjectTreeView::ObjectTreeView(QWidget *parent, const char *name, bool tabStop)
 : KListView(parent, name)
{
	m_form = 0;
	addColumn(i18n("Name"), 130);
	addColumn(i18n("Class"), 100);

	connect((QObject*)header(), SIGNAL(sectionHandleDoubleClicked(int)), this, SLOT(slotColumnSizeChanged(int)));
	if(!tabStop)
	{
		setSelectionModeExt(Extended);
		connect(this, SIGNAL(selectionChanged()), this, SLOT(slotSelectionChanged()));
		connect(this, SIGNAL(contextMenu(KListView *, QListViewItem *, const QPoint&)), this, SLOT(displayContextMenu(KListView*, QListViewItem*, const QPoint&)));
	}

	setFullWidth(true);
	setAllColumnsShowFocus(true);
	setItemMargin(3);
	setSorting(-1);
}

QSize
ObjectTreeView::sizeHint() const
{
	return QSize( QFontMetrics(font()).width(columnText(0)+columnText(1)+"   "),
		KListView::sizeHint().height());
}

QString
ObjectTreeView::pixmapForClass(const QString &classname)
{
	return m_form->manager()->lib()->icon(classname);
}

void
ObjectTreeView::slotColumnSizeChanged(int)
{
	setColumnWidth(1, viewport()->width() - columnWidth(0));
}

void
ObjectTreeView::displayContextMenu(KListView *list, QListViewItem *item, const QPoint &)
{
	if((list!= this) || !m_form)
		return;

	QWidget *w = ((ObjectTreeViewItem*)item)->m_item->widget();
	if(!w)  return;

	bool enable = true;
	if((w->isA("QWidget")) || (m_form->manager()->isTopLevel(w)))
		enable = false;

	m_form->manager()->createContextMenu(w, m_form->activeContainer(), enable);
}

ObjectTreeViewItem*
ObjectTreeView::findItem(const QString &name)
{
	QListViewItemIterator it(this);
        while(it.current())
	{
		ObjectTreeViewItem *item = static_cast<ObjectTreeViewItem*>(it.current());
		if(item->name() == name)
		{
			return item;
		}
		it++;
	}
	return 0;
}

void
ObjectTreeView::setSelectedWidget(QWidget *w, bool add)
{
	blockSignals(true); // to avoid recursion

	if(!w)
	{
		clearSelection();
		blockSignals(false);
		return;
	}

	if(selectedItems().count() == 0)
		add = false;

	if(!add)
		clearSelection();


	QListViewItem *item = (QListViewItem*) findItem(w->name());
	if(!add)
	{
		setCurrentItem(item);
		setSelectionAnchor(item);
		setSelected(item, true);
	}
	else
		setSelected(item, true);

	blockSignals(false);
}

void
ObjectTreeView::slotSelectionChanged()
{
	QPtrList<QListViewItem> list = selectedItems();

	m_form->resetSelection();
	for(QListViewItem *item = list.first(); item; item = list.next())
	{
		ObjectTreeViewItem *it = static_cast<ObjectTreeViewItem*>(item);
		QWidget *w = it->objectTree()->widget();
		if(w && (m_form->selectedWidgets()->findRef(w) == -1))
			m_form->setSelectedWidget(w, true);
	}
}

void
ObjectTreeView::addItem(ObjectTreeItem *item)
{
	ObjectTreeViewItem *parent=0;

	parent = findItem(item->parent()->name());
	if(!parent)
		return;

	loadTree(item, parent);
}

void
ObjectTreeView::removeItem(ObjectTreeItem *item)
{
	if(!item)
		return;
	ObjectTreeViewItem *it = findItem(item->name());
	delete it;
}

void
ObjectTreeView::renameItem(const QString &oldname, const QString &newname)
{
	if(findItem(newname))
		return;
	ObjectTreeViewItem *item = findItem(oldname);
	if(!item)
		return;
	item->setText(0, newname);
}

void
ObjectTreeView::setForm(Form *form)
{
	//if(m_form == form)
	//	return;
	m_form = form;
	clear();

	if(!form)
		return;

	// Creates the hidden top Item
	m_topItem = new ObjectTreeViewItem(this);
	m_topItem->setSelectable(false);
	m_topItem->setOpen(true);

	ObjectTree *tree = m_form->objectTree();
	loadTree(tree, m_topItem);

	if(!form->selectedWidgets()->isEmpty())
		setSelectedWidget(form->selectedWidgets()->first());
	else
		setSelectedWidget(form->toplevelContainer()->widget());
}

ObjectTreeViewItem*
ObjectTreeView::loadTree(ObjectTreeItem *item, ObjectTreeViewItem *parent)
{
	if(!item)
		return 0;
	ObjectTreeViewItem *treeItem = new ObjectTreeViewItem(parent, item);
	treeItem->setOpen(true);

	// The item is inserted by default at the beginning, but we want it to be at the end, so we move it
	QListViewItem *last = parent->firstChild();
	while(last->nextSibling())
		last = last->nextSibling();
	treeItem->moveItem(last);

	ObjectTreeC *list = item->children();
	for(ObjectTreeItem *it = list->first(); it; it = list->next())
		loadTree(it, treeItem);

	return treeItem;
}

}

#include "objecttreeview.moc"
