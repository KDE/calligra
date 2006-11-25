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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include <kdebug.h>

#include <qpainter.h>
//Added by qt3to4:
#include <Q3CString>
#include <Q3PtrList>

#include <kiconloader.h>
#include <klocale.h>

#include "objecttree.h"
#include "form.h"
#include "container.h"
#include "formmanager.h"
#include "widgetlibrary.h"

#include "objecttreeview.h"

using namespace KFormDesigner;

ObjectTreeViewItem::ObjectTreeViewItem(ObjectTreeViewItem *parent, ObjectTreeItem *item)
 : K3ListViewItem(parent, item->name(), item->className())
{
	m_item = item;
}

ObjectTreeViewItem::ObjectTreeViewItem(K3ListView *list, ObjectTreeItem *item)
 : K3ListViewItem(list, item ? item->name() : QString::null, item ? item->className() : QString::null)
{
	m_item = item;
}

ObjectTreeViewItem::~ObjectTreeViewItem()
{
}

QString
ObjectTreeViewItem::name() const
{
	if(m_item)
		return m_item->name();
    else
        return QString();
}

void
ObjectTreeViewItem::paintCell(QPainter *p, const QColorGroup & cg, int column, int width, int align)
{
	int margin = listView()->itemMargin();
	if(column == 1)
	{
		if(!m_item)
			return;
		K3ListViewItem::paintCell(p, cg, column, width, align);
	}
	else
	{
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
			QString iconName 
				= ((ObjectTreeView*)listView())->iconNameForClass(m_item->widget()->className());
			p->drawPixmap(margin, (height() - IconSize(K3Icon::Small))/2 , SmallIcon(iconName));
			p->drawText(
				QRect(2*margin + IconSize(K3Icon::Small),0,width, height()-1), 
				Qt::AlignVCenter, m_item->name());
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

		QString iconName 
			= ((ObjectTreeView*)listView())->iconNameForClass(item->m_item->widget()->className());
		p->drawPixmap(
			(w - IconSize(K3Icon::Small))/2, (item->height() - IconSize(K3Icon::Small))/2 , 
			SmallIcon(iconName));

		p->translate(0, item->totalHeight());
		item = (ObjectTreeViewItem*)item->nextSibling();
	}
	p->restore();
}

void
ObjectTreeViewItem::setup()
{
	K3ListViewItem::setup();
	if(!m_item)
		setHeight(0);
}

void
ObjectTreeViewItem::setOpen( bool o )
{
	//don't allow to collapse the node, user may be tricked because we're not displaying [+] marks
	if (o)
		K3ListViewItem::setOpen(o);
}

// ObjectTreeView itself ----------------

ObjectTreeView::ObjectTreeView(QWidget *parent, const char *name, bool tabStop)
 : K3ListView(parent, name)
 , m_form(0)
{
	addColumn(i18n("Name"), 130);
	addColumn(i18n("Widget's type", "Type"), 100);

	installEventFilter(this);

	connect((QObject*)header(), SIGNAL(sectionHandleDoubleClicked(int)), this, SLOT(slotColumnSizeChanged(int)));
	if(!tabStop)
	{
		setSelectionModeExt(Extended);
		connect(this, SIGNAL(selectionChanged()), this, SLOT(slotSelectionChanged()));
		connect(this, SIGNAL(contextMenu(K3ListView *, Q3ListViewItem *, const QPoint&)), this, SLOT(displayContextMenu(K3ListView*, Q3ListViewItem*, const QPoint&)));
	}

	setFullWidth(true);
	setAllColumnsShowFocus(true);
	setItemMargin(3);
	setSorting(-1);
}

ObjectTreeView::~ObjectTreeView()
{
}

QSize
ObjectTreeView::sizeHint() const
{
	return QSize( QFontMetrics(font()).width(columnText(0)+columnText(1)+"   "),
		K3ListView::sizeHint().height());
}

QString
ObjectTreeView::iconNameForClass(const Q3CString &classname)
{
	return m_form->library()->iconName(classname);
}

void
ObjectTreeView::slotColumnSizeChanged(int)
{
	setColumnWidth(1, viewport()->width() - columnWidth(0));
}

void
ObjectTreeView::displayContextMenu(K3ListView *list, Q3ListViewItem *item, const QPoint &)
{
	if(list != this || !m_form || !item)
		return;

	QWidget *w = ((ObjectTreeViewItem*)item)->m_item->widget();
	if(!w)
		return;

	FormManager::self()->createContextMenu(w, m_form->activeContainer());
}

ObjectTreeViewItem*
ObjectTreeView::findItem(const QString &name)
{
	Q3ListViewItemIterator it(this);
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


	Q3ListViewItem *item = (Q3ListViewItem*) findItem(w->name());
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
	const bool hadFocus = hasFocus();
	Q3PtrList<Q3ListViewItem> list = selectedItems();
	m_form->selectFormWidget();
	for(Q3ListViewItem *item = list.first(); item; item = list.next())
	{
		ObjectTreeViewItem *it = static_cast<ObjectTreeViewItem*>(item);
		QWidget *w = it->objectTree()->widget();
		if(w && (m_form->selectedWidgets()->findRef(w) == -1))
			m_form->setSelectedWidget(w, true, true);
	}
	if (hadFocus)
		setFocus(); //restore focus
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
ObjectTreeView::renameItem(const Q3CString &oldname, const Q3CString &newname)
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
	if (m_form)
		disconnect(m_form, SIGNAL(destroying()), this, SLOT(slotBeforeFormDestroyed()));
	m_form = form;
	m_topItem = 0;
	clear();

	if(!m_form)
		return;

	connect(m_form, SIGNAL(destroying()), this, SLOT(slotBeforeFormDestroyed()));

	// Creates the hidden top Item
	m_topItem = new ObjectTreeViewItem(this);
	m_topItem->setSelectable(false);
	m_topItem->setOpen(true);

	ObjectTree *tree = m_form->objectTree();
	loadTree(tree, m_topItem);

	if(!form->selectedWidgets()->isEmpty())
		setSelectedWidget(form->selectedWidgets()->first());
	else
		setSelectedWidget(form->widget());
}

void
ObjectTreeView::slotBeforeFormDestroyed()
{
	setForm(0);
}

ObjectTreeViewItem*
ObjectTreeView::loadTree(ObjectTreeItem *item, ObjectTreeViewItem *parent)
{
	if(!item)
		return 0;
	ObjectTreeViewItem *treeItem = new ObjectTreeViewItem(parent, item);
	treeItem->setOpen(true);

	// The item is inserted by default at the beginning, but we want it to be at the end, so we move it
	Q3ListViewItem *last = parent->firstChild();
	while(last->nextSibling())
		last = last->nextSibling();
	treeItem->moveItem(last);

	ObjectTreeList *list = item->children();
	for(ObjectTreeItem *it = list->first(); it; it = list->next())
		loadTree(it, treeItem);

	return treeItem;
}

#include "objecttreeview.moc"
