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
#include <qstyle.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qimage.h>
#include <qapplication.h>
#include <Q3PointArray>
#include <QPaintEvent>

#include <kdebug.h>
#include <kstaticdeleter.h>

#include <kexiutils/utils.h>

static KStaticDeleter<QImage> KexiRecordMarker_pen_deleter, KexiRecordMarker_plus_deleter;
QImage* KexiRecordMarker_pen = 0, *KexiRecordMarker_plus = 0;

static const unsigned char img_pen_data[] = {
    0x00,0x00,0x03,0x30,0x78,0x9c,0xfb,0xff,0xff,0x3f,0xc3,0x7f,0x32,0x30,
    0x10,0x80,0x88,0xff,0xe4,0xe8,0x85,0xe9,0xc7,0xc6,0x26,0x55,0x3f,0x3a,
    0x4d,0x8e,0x7e,0x72,0xfc,0x32,0xd2,0xf5,0xa3,0xeb,0xa5,0xb5,0x7e,0x5c,
    0xe9,0x85,0x54,0xfb,0xb1,0xa5,0x1b,0x52,0xdc,0x0e,0x00,0xf2,0xea,0x0a,
    0x13
};
static const unsigned char img_plus_data[] = {
    0x00,0x00,0x01,0x90,0x78,0x9c,0xfb,0xff,0xff,0x3f,0xc3,0x7f,0x28,0x86,
    0x82,0xff,0x50,0x0c,0x17,0x47,0xc7,0xd4,0x50,0x87,0x05,0xc0,0xd5,0xe1,
    0x10,0xa7,0x16,0x26,0xca,0x5e,0x7c,0xfe,0x20,0x47,0x1d,0xb2,0x5a,0x5c,
    0xea,0x40,0x72,0x00,0x03,0x6e,0x74,0x8c
};

static struct EmbedImage {
    int width, height, depth;
    const unsigned char *data;
    ulong compressed;
    int numColors;
    const QRgb *colorTable;
    bool alpha;
    const char *name;
} embed_image[] = {
    { 17, 12, 32, (const unsigned char*)img_pen_data, 57, 0, 0, true, "tableview_pen.png" },
    { 10, 10, 32, (const unsigned char*)img_pen_data, 50, 0, 0, true, "tableview_plus.png" }
};

QImage* getImg(const unsigned char* data, int id)
{
	QByteArray baunzip;
	baunzip = qUncompress( data, embed_image[id].compressed );
	QImage *img = new QImage( QImage((uchar*)baunzip.data(),
			embed_image[id].width, embed_image[id].height,
			embed_image[id].depth, (QRgb*)embed_image[id].colorTable,
			embed_image[id].numColors, QImage::BigEndian
	).copy() );
	if ( embed_image[id].alpha )
		img->setAlphaBuffer(true);
	return img;
}

static void initRecordMarkerImages()
{
	if (!KexiRecordMarker_pen) {
/*! @warning not reentrant! */
		KexiRecordMarker_pen_deleter.setObject( KexiRecordMarker_pen, getImg(img_pen_data, 0) );
		KexiRecordMarker_plus_deleter.setObject( KexiRecordMarker_plus, getImg(img_plus_data, 1) );
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

KexiRecordMarker::KexiRecordMarker(QWidget *parent, const char* name)
 : QWidget(parent, name)
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
		style().drawPrimitive( QStyle::PE_HeaderSection, &p, r,
			(d->currentRow == i) ? selectedColorGroup : (d->highlightedRow == i ? highlightedColorGroup : colorGroup()), 
			QStyle::Style_Raised | (isEnabled() ? QStyle::Style_Enabled : 0));
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
		Q3PointArray points(3);
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
