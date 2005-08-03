/* This file is part of the KDE project
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>
   Copyright (C) 2004 Alexander Dymo <cloudtemple@mskat.net>
   Copyright (C) 2004-2005 Jaroslaw Staniek <js@iidea.pl>

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
#include <qstyle.h>

#ifdef QT_ONLY
#else
#include <kdebug.h>
#include <kiconloader.h>
#include <kstyle.h>
#endif

#define BRANCHBOX_SIZE 9

namespace KoProperty {
class EditorItemPrivate
{
	public:
		EditorItemPrivate()
		: property(0) {}
		~EditorItemPrivate() {}

		Property  *property;
		Editor  *editor;
};
}

using namespace KoProperty;

EditorItem::EditorItem(Editor *editor, EditorItem *parent, Property *property, QListViewItem *after)
 : KListViewItem(parent, after, property->caption().isEmpty() ? property->name() : property->caption())
{
	d = new EditorItemPrivate();
	d->property = property;
	d->editor = editor;

	setMultiLinesEnabled(true);
	//setHeight(static_cast<Editor*>(listView())->baseRowHeight()*3);
}

EditorItem::EditorItem(KListView *parent)
 : KListViewItem(parent)
{
	d = new EditorItemPrivate();
	d->property = 0;
	d->editor = 0;
}

EditorItem::EditorItem(EditorItem *parent, const QString &text)
 : KListViewItem(parent, text)
{
	d = new EditorItemPrivate();
	d->property = 0;
	d->editor = 0;
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
	if (static_cast<Editor*>(listView())->insideFill())
		return;

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
			QRect r(0, 0, d->editor->header()->sectionSize(1), height() - (widget->hasBorders() ? 1 : 2));
			p->setClipRect(r, QPainter::CoordPainter);
			p->setClipping(true);
			widget->drawViewer(p, icg, r, d->property->value());
			p->setClipping(false);
		}
	}

	p->setPen( KPROPEDITOR_ITEM_BORDER_COLOR ); //! \todo custom color?
//	p->drawLine(parent() ? 0 : 50, 0, width, 0 );
	p->drawLine(parent() ? 0 : 0, height()-1, width, height()-1 );
}

void
EditorItem::paintBranches(QPainter *p, const QColorGroup &cg, int w, int y, int h)
{
	if (static_cast<Editor*>(listView())->insideFill())
		return;

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
		p->fillRect(parent() ? 0 : 50, 0, w, item->height()-1, QBrush(backgroundColor));
//		p->fillRect(-50,0,50, item->height(), QBrush(backgroundColor));
		p->save();
		p->setPen( KPROPEDITOR_ITEM_BORDER_COLOR );
		p->drawLine(parent() ? 0 : 50, item->height()-1, w, item->height()-1 );
		p->drawLine(parent() ? -1 : 18, 0, parent() ? -1 : 18, item->height() );
		p->restore();

//	for (int i=0; i<10000000; i++)
//		;
		if(item->isSelected())  {
			p->fillRect(parent() ? 0 : 50, 0, w, item->height()-1, QBrush(cg.highlight()));
//			p->fillRect(-50,0,50, item->height(), QBrush(cg.highlight()));
		}

		if(item->firstChild())  {
			//! \todo make BRANCHBOX_SIZE configurable?
			const int marg = (item->height() - BRANCHBOX_SIZE) / 2;

#if 0
			p->setPen( KPROPEDITOR_ITEM_BORDER_COLOR );
			p->drawRect(2, marg, BRANCHBOX_SIZE, BRANCHBOX_SIZE);
			p->fillRect(2+1, marg + 1, BRANCHBOX_SIZE-2, BRANCHBOX_SIZE-2, item->listView()->paletteBackgroundColor());
			p->setPen( item->listView()->paletteForegroundColor() );
			p->drawLine(2+2, marg+BRANCHBOX_SIZE/2, BRANCHBOX_SIZE-1, marg + BRANCHBOX_SIZE / 2);
			if(!item->isOpen())
				p->drawLine(2+BRANCHBOX_SIZE/2, marg+2,
					2+BRANCHBOX_SIZE/2, marg+BRANCHBOX_SIZE-3);
#endif
			KStyle* kstyle = dynamic_cast<KStyle*>(&listView()->style());
			const int lh = item->height(); 
			kstyle->drawKStylePrimitive( 
				KStyle::KPE_ListViewExpander, p, listView(), 
				QRect( (lh - 9)/2, (lh - 9)/2, 9, 9 ), cg, item->isOpen() ? 0 : QStyle::Style_On,
					QStyleOption::Default);
		}

		// draw icon (if there is one)
		EditorItem *editorItem = dynamic_cast<EditorItem*>(item);
		if (editorItem && editorItem->property() && !editorItem->property()->icon().isEmpty()) {
			//int margin = listView()->itemMargin();
			QPixmap pix = SmallIcon(editorItem->property()->icon());
			p->drawPixmap(1, (item->height() - pix.height()) / 2, pix);
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

	if (d->property) {
//		kopropertydbg << d->property->name() << " " << d->property->sortingKey() << " | "
//			<< static_cast<EditorItem*>(i)->property()->name() << " "
//			<< static_cast<EditorItem*>(i)->property()->sortingKey() << endl;
		return d->property->sortingKey() 
			- (dynamic_cast<EditorItem*>(i) ? dynamic_cast<EditorItem*>(i)->property()->sortingKey() : 0);
	}

	return 0;
//	return d->order - static_cast<EditorItem*>(i)->d->order;
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
	p->setPen( KPROPEDITOR_ITEM_BORDER_COLOR ); //! \todo custom color?
	p->drawLine(0, height()-1, width-1, height()-1);
	if (column==0) {
		p->eraseRect(QRect(0,0,width+listView()->columnWidth(1),height()-1));
	}
	else {
		return;
	}

	QFont font = listView()->font();
	font.setBold(true);
	p->setFont(font);
	p->setBrush(cg.highlight());
	p->setPen(cg.highlightedText());
/*
#ifdef QT_ONLY
		QListViewItem::paintCell(p, cg, column, width, align);
#else
		KListViewItem::paintCell(p, cg, column, width, align);
#endif*/
	p->setPen(cg.text());
	p->drawText(QRect(0,0, width+listView()->columnWidth(1), height()), 
		Qt::AlignLeft | Qt::AlignVCenter | Qt::SingleLine, text(0)); 
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
