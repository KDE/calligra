/* This file is part of the KDE project
   Copyright (C) 2002 Lucijan Busch <lucijan@gmx.at>
   Copyright (C) 2002 Till Busch <till@bux.at>
   Copyright (C) 2002 Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2003-2006 Jaroslaw Staniek <js@iidea.pl>

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

#include "kexirecordmarker.h"

#include <qcolor.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qimage.h>
#include <qapplication.h>
#include <QPolygon>
#include <QPaintEvent>
#include <QStyle>
#include <QStyleOptionHeader>
 
#include <kdebug.h>
#include <kstaticdeleter.h>

#include <kexiutils/utils.h>

#include "../../pics/tableview_pen.xpm"
#include "../../pics/tableview_plus.xpm"

static KStaticDeleter<QImage> KexiRecordMarker_pen_deleter, KexiRecordMarker_plus_deleter;
QImage* KexiRecordMarker_pen = 0, *KexiRecordMarker_plus = 0;

static void initRecordMarkerImages()
{
	if (!KexiRecordMarker_pen) {
/*! @warning not reentrant! */
		KexiRecordMarker_pen_deleter.setObject( 
			KexiRecordMarker_pen, new QImage(tableview_pen_xpm) );
		KexiRecordMarker_plus_deleter.setObject(
			KexiRecordMarker_plus, new QImage(tableview_plus_xpm) );
	}
}

//----------------------------------------------------------------

//! @internal
class KexiRecordMarker::Private
{
public:
	Private()
	 : rowHeight(1)
	 , offset(0)
	 , currentRow(-1)
	 , highlightedRow(-1)
	 , editRow(-1)
	 , rows(0)
	 , selectionBackgroundColor(qApp->palette().active().highlight())
	 , showInsertRow(true)
	{
	}
	int rowHeight;
	int offset;
	int currentRow;
	int highlightedRow;
	int editRow;
	int rows;
	QColor selectionBackgroundColor;
	bool showInsertRow : 1;
};

//----------------------------------------------------------------

KexiRecordMarker::KexiRecordMarker(QWidget *parent)
 : QWidget(parent)
 , d( new Private() )
{
	initRecordMarkerImages();
}

KexiRecordMarker::~KexiRecordMarker()
{
	delete d;
}

QImage* KexiRecordMarker::penImage()
{
	initRecordMarkerImages();
	return KexiRecordMarker_pen;
}

QImage* KexiRecordMarker::plusImage()
{
	initRecordMarkerImages();
	return KexiRecordMarker_plus;
}

void KexiRecordMarker::addLabel(bool upd)
{
	d->rows++;
	if (upd)
		update();
}

void KexiRecordMarker::removeLabel(bool upd)
{
	if (d->rows > 0) {
		d->rows--;
		if (upd)
			update();
	}
}

void KexiRecordMarker::addLabels(int num, bool upd)
{
	d->rows += num;
	if (upd)
		update();
}

void KexiRecordMarker::clear(bool upd)
{
	d->rows=0;
	if (upd)
		update();
}

int KexiRecordMarker::rows() const
{
	if (d->showInsertRow)
		return d->rows +1;
	else
		return d->rows;
}

void KexiRecordMarker::paintEvent(QPaintEvent *e)
{
	QPainter p(this);
	QRect r(e->rect());

	int first = (r.top()    + d->offset) / d->rowHeight;
	int last  = (r.bottom() + d->offset) / d->rowHeight;
	if(last > (d->rows-1+(d->showInsertRow?1:0)))
		last = d->rows-1+(d->showInsertRow?1:0);

	QColorGroup selectedColorGroup(colorGroup());
	selectedColorGroup.setColor( QColorGroup::Button, 
		KexiUtils::blendedColors( selectedColorGroup.color(QColorGroup::Background), 
			d->selectionBackgroundColor, 2, 1) );
	selectedColorGroup.setColor( QColorGroup::Background, 
		selectedColorGroup.color(QColorGroup::Button) ); //set background color as well (e.g. for thinkeramik)
	QColorGroup highlightedColorGroup(colorGroup());
	highlightedColorGroup.setColor( QColorGroup::Button, 
		KexiUtils::blendedColors( highlightedColorGroup.color(QColorGroup::Background), 
			d->selectionBackgroundColor, 4, 1) );
	highlightedColorGroup.setColor( QColorGroup::Background, 
		highlightedColorGroup.color(QColorGroup::Button) ); //set background color as well (e.g. for thinkeramik)
	for(int i=first; i <= last; i++)
	{
		int y = ((d->rowHeight * i)-d->offset);
		QRect r(0, y, width(), d->rowHeight);
		p.drawRect(r);
		QStyleOptionHeader optionHeader;
		optionHeader.initFrom(this);
		optionHeader.orientation = Qt::Vertical;
		optionHeader.rect = r;
		optionHeader.state = QStyle::State_Raised;
		style()->drawControl( QStyle::CE_HeaderSection, &optionHeader, &p );
#warning 2.0: TODO?
/* 2.0: TODO		
		, r,
			(d->currentRow == i) ? selectedColorGroup : (d->highlightedRow == i ? highlightedColorGroup : colorGroup()), 
			QStyle::Style_Raised | (isEnabled() ? QStyle::Style_Enabled : 0));*/
	}
	if (d->editRow!=-1 && d->editRow >= first && d->editRow <= (last/*+1 for insert row*/)) {
		//show pen when editing
		int ofs = d->rowHeight / 4;
		int pos = ((d->rowHeight*(d->currentRow>=0?d->currentRow:0))-d->offset)-ofs/2+1;
		p.drawImage((d->rowHeight-KexiRecordMarker_pen->width())/2,
			(d->rowHeight-KexiRecordMarker_pen->height())/2+pos,*KexiRecordMarker_pen);
	}
	else if (d->currentRow >= first && d->currentRow <= last 
		&& (!d->showInsertRow || (d->showInsertRow && d->currentRow < last)))/*don't display marker for 'insert' row*/ 
	{
		//show marker
		p.setBrush(colorGroup().foreground());
		p.setPen(QPen(Qt::NoPen));
		QPolygon points(3);
		int ofs = d->rowHeight / 4;
		int ofs2 = (width() - ofs) / 2 -1;
		int pos = ((d->rowHeight*d->currentRow)-d->offset)-ofs/2+2;
		points.putPoints(0, 3, ofs2, pos+ofs, ofs2 + ofs, pos+ofs*2, 
			ofs2,pos+ofs*3);
		p.drawPolygon(points);
//		kDebug() <<"KexiRecordMarker::paintEvent(): POLYGON" << endl;
/*		int half = d->rowHeight / 2;
		points.setPoints(3, 2, pos + 2, width() - 5, pos + half, 2, pos + (2 * half) - 2);*/
	}
	if (d->showInsertRow && d->editRow < last
		&& last == (d->rows-1+(d->showInsertRow?1:0)) ) {
		//show plus sign
		int pos = ((d->rowHeight*last)-d->offset)+(d->rowHeight-KexiRecordMarker_plus->height())/2;
//		p.drawImage((width()-d->plusImg.width())/2-1, pos, d->plusImg);
		p.drawImage((width()-KexiRecordMarker_plus->width())/2, pos, *KexiRecordMarker_plus);
	}
}

void KexiRecordMarker::setCurrentRow(int row)
{
	if (row == d->currentRow)
		return;
	int oldRow = d->currentRow;
	d->currentRow=row;
	
	if (oldRow != -1)
		update(0,(d->rowHeight*(oldRow))-d->offset-1, width()+2, d->rowHeight+2);
	if (d->currentRow != -1)
		update(0,(d->rowHeight*d->currentRow)-d->offset-1, width()+2, d->rowHeight+2);
}

void KexiRecordMarker::setHighlightedRow(int row)
{
	if (row == d->highlightedRow)
		return;
	int oldRow = d->highlightedRow;
	d->highlightedRow = row;

	if (oldRow != -1)
		update(0,(d->rowHeight*(oldRow))-d->offset-1, width()+2, d->rowHeight+2);
	if (d->currentRow != -1)
		update(0,(d->rowHeight*d->highlightedRow)-d->offset-1, width()+2, d->rowHeight+2);
}

void KexiRecordMarker::setOffset(int offset)
{
	int oldOff = d->offset;
	d->offset = offset;
	scroll(0,oldOff-offset);
}

void KexiRecordMarker::setCellHeight(int cellHeight)
{
	d->rowHeight = cellHeight;
}

void KexiRecordMarker::setEditRow(int row)
{
	d->editRow = row;
//TODO: update only needed area!
	update();
}

void KexiRecordMarker::showInsertRow(bool show)
{
	d->showInsertRow = show;
//TODO: update only needed area!
	update();
}

void KexiRecordMarker::setSelectionBackgroundColor(const QColor &color)
{
	d->selectionBackgroundColor = color;
}

QColor KexiRecordMarker::selectionBackgroundColor() const
{
	return d->selectionBackgroundColor;
}

#include "kexirecordmarker.moc"
