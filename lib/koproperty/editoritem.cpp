/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004  Alexander Dymo <cloudtemple@mskat.net>

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

#include "editoritem.h"
#include "editor.h"
#include "property.h"
#include "widget.h"
#include "factory.h"

#include <qpainter.h>
#include <qpixmap.h>
#include <qheader.h>

#ifdef QT_ONLY
#else
#include <kdebug.h>
#include <kiconloader.h>
#endif

#define BRANCHBOX_SIZE 9

namespace KOProperty {

class EditorItemPrivate
{
	public:
		EditorItemPrivate()
		: property(0) {}
		~EditorItemPrivate() {}

		Property  *property;
		Editor  *editor;
		int  order;
};

EditorItem::EditorItem(Editor *editor, EditorItem *parent, Property *property, QListViewItem *after)
 : KListViewItem(parent, after, property->caption().isEmpty() ? property->name() : property->caption())
{
	d = new EditorItemPrivate();
	d->property = property;
	d->editor = editor;
	d->order = parent->childCount();

	setMultiLinesEnabled(true);
	//setHeight(static_cast<Editor*>(listView())->baseRowHeight()*3);
}

EditorItem::EditorItem(KListView *parent)
 : KListViewItem(parent)
{
	d = new EditorItemPrivate();
	d->property = 0;
	d->editor = 0;
	d->order = parent->childCount();
}

EditorItem::EditorItem(EditorItem *parent, const QString &text)
 : KListViewItem(parent, text)
{
	d = new EditorItemPrivate();
	d->property = 0;
	d->editor = 0;
	d->order = parent->childCount();
}

EditorItem::~EditorItem()
{
	delete d;
}

Property*
EditorItem::property()
{
	return d->property;
}

void
EditorItem::paintCell(QPainter *p, const QColorGroup & cg, int column, int width, int align)
{
	//int margin = static_cast<Editor*>(listView())->itemMargin();
	if(!d->property)
			return;

	if(column == 0)
	{
		QFont font = listView()->font();
		if(d->property->isModified())
			font.setBold(true);
		p->setFont(font);
		p->setBrush(cg.highlight());
		p->setPen(cg.highlightedText());
#ifdef QT_ONLY
		QListViewItem::paintCell(p, cg, column, width, align);
#else
		KListViewItem::paintCell(p, cg, column, width, align);
#endif

		p->setPen( KPROPEDITOR_ITEM_BORDER_COLOR );
		p->drawLine(width-1, 0, width-1, height()-1);
        }
	else if(column == 1)
	{
		QColorGroup icg(cg);
#ifdef QT_ONLY
		icg.setColor(QColorGroup::Background, white);
#else
		icg.setColor(QColorGroup::Background, backgroundColor());
		p->setBackgroundColor(backgroundColor());
#endif
		Widget *widget = d->editor->createWidgetForProperty(d->property, false /*don't change Widget::property() */);
		if(widget) {
			QRect r(0, 0, d->editor->header()->sectionSize(1), height()-(widget->hasBorders()?0:1));
			widget->drawViewer(p, icg, r, d->property->value());
		}
	}

	p->setPen( KPROPEDITOR_ITEM_BORDER_COLOR ); //! \todo custom color?
	p->drawLine(-250, height()-1, width, height()-1 );
}

void
EditorItem::paintBranches(QPainter *p, const QColorGroup &cg, int w, int y, int h)
{
	p->eraseRect(0,0,w,h);
#ifdef QT_ONLY
	QListViewItem *item = firstChild();
#else
	KListViewItem *item = (KListViewItem*)firstChild();
#endif
	if(!item)
		return;

	QColor backgroundColor;
#ifdef QT_ONLY
	backgroundColor = cg.backgroundColor();
#endif
	p->save();
	p->translate(0,y);
	while(item)
	{
#ifndef QT_ONLY
		backgroundColor = item->backgroundColor();
#endif
		p->fillRect(0,0,w, item->height(), QBrush(backgroundColor));
		p->fillRect(-50,0,50, item->height(), QBrush(backgroundColor));
		p->save();
		p->setPen( KPROPEDITOR_ITEM_BORDER_COLOR );
		p->drawLine(-50, item->height()-1, w, item->height()-1 );
		p->restore();

		if(item->isSelected())  {
			p->fillRect(0,0,w, item->height(), QBrush(cg.highlight()));
			p->fillRect(-50,0,50, item->height(), QBrush(cg.highlight()));
		}

		if(item->firstChild())  {
			//! \todo make BRANCHBOX_SIZE configurable?
			const int marg = (item->height() - BRANCHBOX_SIZE) / 2;
			p->setPen( KPROPEDITOR_ITEM_BORDER_COLOR );
			p->drawRect(2, marg, BRANCHBOX_SIZE, BRANCHBOX_SIZE);
			p->fillRect(2+1, marg + 1, BRANCHBOX_SIZE-2, BRANCHBOX_SIZE-2, backgroundColor);
			p->drawLine(2+2, marg+BRANCHBOX_SIZE/2, BRANCHBOX_SIZE-1, marg + BRANCHBOX_SIZE / 2);
			if(!item->isOpen())
				p->drawLine(2+BRANCHBOX_SIZE/2, marg+2,
					2+BRANCHBOX_SIZE/2, marg+BRANCHBOX_SIZE-3);

			//! \todo use QStyle instead
		}

		// draw icon (if there is one)
		EditorItem *editorItem = static_cast<EditorItem*>(item);
		if (editorItem && editorItem->property() && !editorItem->property()->icon().isEmpty()) {
			//int margin = listView()->itemMargin();
			QPixmap pix = SmallIcon(editorItem->property()->icon());
			p->drawPixmap(2, (item->height() - pix.height()) / 2, pix);
		}

		p->translate(0, item->totalHeight());
#ifdef QT_ONLY
		item = item->nextSibling();
#else
		item = (KListViewItem*)item->nextSibling();
#endif
	}
	p->restore();
}

void
EditorItem::paintFocus(QPainter *, const QColorGroup &, const QRect & )
{}

int
EditorItem::compare( QListViewItem *i, int col, bool ascending ) const
{
	if (!ascending)
		return -QListViewItem::key( col, ascending ).localeAwareCompare( i->key( col, ascending ) );

	return d->order - static_cast<EditorItem*>(i)->d->order;
}

//////////////////////////////////////////////////////

EditorGroupItem::EditorGroupItem(EditorItem *parent, const QString &text)
 : EditorItem(parent, text)
{
	setOpen(true);
	setSelectable(false);
}

EditorGroupItem::~EditorGroupItem()
{}

void
EditorGroupItem::paintCell(QPainter *p, const QColorGroup & cg, int column, int width, int align)
{
	//if(column == 1)
	//	return;

	QFont font = listView()->font();
	font.setBold(true);
	p->setFont(font);
	p->setBrush(cg.highlight());
	p->setPen(cg.highlightedText());
#ifdef QT_ONLY
		QListViewItem::paintCell(p, cg, column, width, align);
#else
		KListViewItem::paintCell(p, cg, column, width, align);
#endif
	p->setPen( KPROPEDITOR_ITEM_BORDER_COLOR ); //! \todo custom color?
	p->drawLine(-50, height()-1, width, height()-1 );
}

void
EditorGroupItem::setup()
{
	KListViewItem::setup();
	setHeight( int(height()*1.4) );
}

////////////////////////////////////////////////////////

EditorDummyItem::EditorDummyItem(KListView *listview)
 : EditorItem(listview)
{
	setSelectable(false);
	setOpen(true);
}

EditorDummyItem::~EditorDummyItem()
{}

void
EditorDummyItem::setup()
{
	setHeight(0);
}

}
