/***************************************************************************
                          kdbrecordviewitem.cpp  -  description                              
                             -------------------                                         
    begin                : Mon Jun 28 1999                                           
    copyright            : (C) 1999 by Ørn E. Hansen                         
    email                : hanseno@mail.bip.net                                     
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/

#include <iostream.h>

#include <qheader.h>

#include <kdbfieldset.h>
#include "kdbrecordview.h"
#include "kdbrecordviewitem.h"

kdbRecordViewItem::kdbRecordViewItem(int p_rec,kdbDataSet *p_set, QListView *p_parent)
 : QListViewItem(p_parent)
{
	_set = 0;
	_record = p_rec;
	init(p_set);
}

kdbRecordViewItem::kdbRecordViewItem(int p_rec,kdbDataSet *p_set, QListViewItem *p_parent)
 : QListViewItem(p_parent)
{
	_set = 0;
	_record = p_rec;
	init(p_set);
}

kdbRecordViewItem::kdbRecordViewItem(int p_rec,kdbDataSet *p_set, QListView *p_parent, QListViewItem *p_after)
 : QListViewItem(p_parent, p_after)
{
	_set = 0;
	_record = p_rec;
	init(p_set);
}

kdbRecordViewItem::kdbRecordViewItem(int p_rec,kdbDataSet *p_set, QListViewItem *p_parent, QListViewItem *p_after)
 : QListViewItem(p_parent, p_after)
{
	_set    = 0;
	_record = p_rec;
	init(p_set);
}

kdbRecordViewItem::~kdbRecordViewItem()
{
}

QString
kdbRecordViewItem::text(int p_col) const
{
	QString columnName;
	
	if ( !_set || p_col < 0 )
		return "";
	columnName = listView()->columnText( p_col );
	return (*_set)[ _record ][ columnName ].text();	
}

void
kdbRecordViewItem::setText(int p_col, const QString& p_text)
{
	QString columnName;
	
	if ( !_set || p_col < 0 )
		return;
	columnName = listView()->columnText( p_col );
	(*_set)[ _record ][ columnName ].setText( p_text );
	_set->update();
}

void
kdbRecordViewItem::init(kdbDataSet *p_set)
{
	if ( !p_set )
		return;
	_set = p_set;
}

void
kdbRecordViewItem::paintCell(QPainter *p_paint,const QColorGroup &p_cg,
			                        int p_col, int p_width, int p_align )
{
    // Change width() if you change this.

    if ( !p_paint )
        return;

    QListView *lv = listView();
    const QPixmap *icon = pixmap( p_col );

    p_paint->fillRect( 0,0,p_width,height(),p_cg.base() );

    int itMarg = lv ? lv->itemMargin() : 1;
    int lmarg  = itMarg;

    if ( isSelected() && (p_col==0 || listView()->allColumnsShowFocus()) ) {
	    p_paint->fillRect( 0,0,p_width,height(),p_cg.brush(QColorGroup::Highlight) );
	    if ( lv->isA("kdbRecordView") && p_col == ((kdbRecordView *)lv)->columnPos() ) {
				QPen pen;
				pen.setColor( p_cg.highlight() );
				pen.setWidth( 1 );
				p_paint->setPen( p_cg.highlight());
				p_paint->drawRect( 0,0,p_width,height() );
			}
	    p_paint->setPen( p_cg.highlightedText() );
    } else {
    	p_paint->setPen( DotLine );
			p_paint->drawRect( 0,0,p_width,height() );
      p_paint->setPen( p_cg.text() );
    }

    if ( icon ) {
      p_paint->drawPixmap( lmarg,(height()-icon->height())/2,*icon );
      lmarg += icon->width() + itMarg;
    }

    QString t = text( p_col );
    if ( !t.isEmpty() ) {
        // should do the ellipsis thing in drawText()
        p_paint->drawText( lmarg,0,p_width-lmarg-itMarg,height(),p_align|AlignVCenter,t );
    }
}








