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

#include <klocale.h>

#include "objecttree.h"
#include "form.h"
#include "container.h"

#include "objecttreeview.h"

namespace KFormDesigner
{

// ObjectTreeViewItem

ObjectTreeViewItem::ObjectTreeViewItem(ObjectTreeViewItem *parent, ObjectTreeItem *item)
 : KListViewItem(parent, item->name(), item->className())
{
	m_item = item;
}

ObjectTreeViewItem::ObjectTreeViewItem(KListView *list)
 : KListViewItem(list)
{
	m_item = 0;
}

const QString
ObjectTreeViewItem::name()
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
		if(depth()==0)
			return;
		KListViewItem::paintCell(p, cg, column, width, align);
	}
	else
	{
		if(depth()==0)
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
		p->drawText(QRect(margin,0,width, height()-1), Qt::AlignVCenter, text(0));
		p->restore();
		
		p->setPen( QColor(200,200,200) ); //like in t.v.
		p->drawLine(width-1, 0, width-1, height()-1);
	}
	
	p->setPen( QColor(200,200,200) ); //like in t.v.
	p->drawLine(-50, height()-1, width, height()-1 );
}

void
ObjectTreeViewItem::paintBranches(QPainter *p, const QColorGroup &cg, int w, int y, int h)
{
	p->eraseRect(0,0,w,h);
	KListViewItem *item = (KListViewItem*)firstChild();
	if(!item)
		return;
	
	p->save();
	p->translate(0,y);
	while(item)
	{
		p->fillRect(0,0,w, item->height(), QBrush(item->backgroundColor()));
		p->fillRect(-50,0,50, item->height(), QBrush(item->backgroundColor()));
		p->save();
		p->setPen( QColor(200,200,200) ); //like in t.v.
		p->drawLine(-50, item->height()-1, w, item->height()-1 );
		p->restore();
		
		if(item->isSelected())
		{
			p->fillRect(0,0,w, item->height(), QBrush(cg.highlight()));
			p->fillRect(-50,0,50, item->height(), QBrush(cg.highlight()));
		}
		if(item->firstChild())
		{
		p->drawRect(2, item->height()/2 -4, 9, 9);
		p->drawLine(4, item->height()/2, 8, item->height()/2);
		if(!item->isOpen())
			p->drawLine(6, item->height()/2 - 2, 6, item->height()/2 +2);
		}
		
		p->translate(0, item->totalHeight());
		item = (KListViewItem*)item->nextSibling();
	}
	p->restore();
}

void
ObjectTreeViewItem::setup()
{
	KListViewItem::setup();
	if(depth()==0)
		setHeight(0);
}


/*QPixmap
ObjectTreeViewItem::iconByName(const QString &classname)
{

}*/

// ObjectTreeView itself

ObjectTreeView::ObjectTreeView(QWidget *parent, const char *name)
 : KListView(parent, name)
{
	addColumn(i18n("Name"), 130);
	addColumn(i18n("Class"), 100);

	connect((QObject*)header(), SIGNAL(sectionHandleDoubleClicked(int)), this, SLOT(slotColumnSizeChanged(int)));
	connect(this, SIGNAL(selectionChanged(QListViewItem*)), this, SLOT(emitSelChanged(QListViewItem*))); 

	setFullWidth(true);
	setAllColumnsShowFocus(true);
	setItemMargin(3);
}

QSize
ObjectTreeView::sizeHint() const
{
	return QSize( QFontMetrics(font()).width(columnText(0)+columnText(1)+"   "),
		KListView::sizeHint().height());
}

void
ObjectTreeView::slotColumnSizeChanged(int column)
{
	setColumnWidth(1, viewport()->width() - columnWidth(0));
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
ObjectTreeView::setSelWidget(QWidget *w)
{
	if(!w)
		return;

	QString name = w->name();

	ObjectTreeViewItem *objIt = static_cast<ObjectTreeViewItem*>(selectedItem());
	if(!objIt) return;
	if((name == objIt->name()) || (name == QString::null))
		return;

	setSelected((QListViewItem*) findItem(name), true);
}

void
ObjectTreeView::emitSelChanged(QListViewItem *item)
{
	ObjectTreeViewItem *it = static_cast<ObjectTreeViewItem*>(item);
	emit selectionChanged(it->objectTree()->widget());
}

void
ObjectTreeView::addItem(ObjectTreeItem *item)
{
	ObjectTreeViewItem *parent=0;

	parent = findItem(item->parent()->name());
	if(!parent)
		return;

	ObjectTreeViewItem *listitem = loadTree(item, parent);
}

void
ObjectTreeView::removeItem(ObjectTreeItem *item)
{
	ObjectTreeViewItem *it = findItem(item->name());
	delete it;
}

void
ObjectTreeView::setForm(Form *form)
{
	
	if(m_form == form)
		return;
	m_form = form;
	clear();
	m_topItem = new ObjectTreeViewItem(this); // Creates the hidden top Item 
	ObjectTree *tree = m_form->objectTree();
	loadTree(tree, m_topItem);
	m_topItem->setOpen(true);
	if(form->selectedWidget())
		setSelWidget(form->selectedWidget());
	else
		setSelWidget(form->toplevelContainer()->widget());
}

ObjectTreeViewItem*
ObjectTreeView::loadTree(ObjectTreeItem *item, ObjectTreeViewItem *parent)
{
	if(!item)
		return 0;
	ObjectTreeViewItem *treeItem = new ObjectTreeViewItem(parent, item);
	treeItem->setOpen(true);
	ObjectTreeC *list = item->children();

	for(ObjectTreeItem *it = list->first(); it; it = list->next())
		loadTree(it, treeItem);

	return treeItem;
}

}

#include "objecttreeview.moc"
