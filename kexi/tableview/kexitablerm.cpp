/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Daniel Molkentin <molkentin@kde.org>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Original Author:  Till Busch <till@bux.at>
   Original Project: buX (www.bux.at)
*/

#include "kexitablerm.h"

#include <qcolor.h>
#include <qstyle.h>
#include <qpixmap.h>
#include <qpainter.h>

#include <kdebug.h>

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
    { 17, 12, 32, (const unsigned char*)img_pen_data, 57, 0, 0, TRUE, "tableview_pen.png" },
    { 10, 10, 32, (const unsigned char*)img_pen_data, 50, 0, 0, TRUE, "tableview_plus.png" }
};

void getImg(QImage &image, const unsigned char* data, int id)
{
	QByteArray baunzip;
	baunzip = qUncompress( data, embed_image[id].compressed );
	QImage img((uchar*)baunzip.data(),
			embed_image[id].width, embed_image[id].height,
			embed_image[id].depth, (QRgb*)embed_image[id].colorTable,
			embed_image[id].numColors, QImage::BigEndian
	);
	image = img.copy();
	if ( embed_image[id].alpha )
		image.setAlphaBuffer(TRUE);
}

KexiTableRM::KexiTableRM(QWidget *parent)
:QWidget(parent)
{
	m_rowHeight = 1;
	m_offset=0;
	m_currentRow=-1;
	m_editRow=-1;
	m_pointerColor = QColor(99,0,0);
	m_max = 0;
	m_showInsertRow = true;//false;
	
	getImg(m_penImg, img_pen_data, 0);
	getImg(m_plusImg, img_plus_data, 1);
}

KexiTableRM::~KexiTableRM()
{
}

void KexiTableRM::addLabel()
{
	m_max++;
	update();
}

void KexiTableRM::removeLabel()
{
	if (m_max > 0) {
		m_max--;
		update();
	}
}

void KexiTableRM::addLabels(int num)
{
	m_max += num;
	update();
}

void KexiTableRM::clear()
{
	m_max=0;
	update();
}

int KexiTableRM::maxRow() const
{
	if (m_showInsertRow)
		return m_max +1;
	else
		return m_max;
}

void KexiTableRM::paintEvent(QPaintEvent *e)
{
	QPainter p(this);
	QRect r(e->rect());

	int first = (r.top()    + m_offset) / m_rowHeight + 1;
	int last  = (r.bottom() + m_offset) / m_rowHeight + 1;
	if(last > maxRow())
		last = maxRow();

	for(int i=first - 1; i < last; i++)
	{
		int y = ((m_rowHeight * i)-m_offset) - 1;
		QRect r(0, y, width(), m_rowHeight + 1);
		p.drawRect(r);
		style().drawPrimitive(QStyle::PE_HeaderSection, &p, r, colorGroup());
	}

	if (m_editRow >= first && m_editRow <= (last+1/*+1 for insert row*/)) {
		//show pen when editing
		int ofs = m_rowHeight / 4;
		int pos = ((m_rowHeight*m_currentRow)-m_offset)-ofs/2+1;
		p.drawImage((m_rowHeight-m_penImg.width())/2+2,(m_rowHeight-m_penImg.height())/2+pos,m_penImg);
	}
	else if(m_currentRow >= first-1 && m_currentRow <= last) {
		//show marker
		p.setBrush(colorGroup().foreground());
		QPointArray points(3);
		int ofs = m_rowHeight / 4;
		int ofs2 = (width() - ofs) / 2 -1;
		int pos = ((m_rowHeight*m_currentRow)-m_offset)-ofs/2+1;
		points.putPoints(0, 3, ofs2, pos+ofs, ofs2 + ofs, pos+ofs*2, 
			ofs2,pos+ofs*3);
		p.drawPolygon(points);
/*		int half = m_rowHeight / 2;
		points.setPoints(3, 2, pos + 2, width() - 5, pos + half, 2, pos + (2 * half) - 2);*/
	}
	if (m_showInsertRow && m_editRow < (last+1)) {
		//show sign
		int pos = ((m_rowHeight*(maxRow()-1))-m_offset)+(m_rowHeight-m_plusImg.height())/2;
		p.drawImage((width()-m_plusImg.width())/2-1, pos, m_plusImg);
	}
	
#if 0

	p.setPen(QColor(0,0,99));
/*	if(m_insertRow >= first && m_insertRow <= last)
	{
		int pos = ((m_rowHeight*m_insertRow)-m_offset)-1;
		for(int i=0; i < (m_rowHeight - 2)/ 2; i++)
		{
			p.drawLine(i + 2, pos - m_rowHeight + 2 + i, i + 2, pos - 2 - i);
		}

	}
*/

	if(m_insertRow >= first && m_insertRow <= last)
	{
		int poss = ((m_insertRow*m_rowHeight)-m_offset)-1;
		p.drawLine(2, poss - (m_rowHeight/2) - 1, width() - 4, poss - (m_rowHeight/2) - 1);
		p.drawLine(2, poss - (m_rowHeight/2), width() - 4, poss - (m_rowHeight/2));
		p.drawLine(2, poss - (m_rowHeight/2) + 1, width() - 4, poss - (m_rowHeight/2) + 1);

		p.drawLine((m_rowHeight - 4)/2 - 1, poss - m_rowHeight + 3, (m_rowHeight- 4)/2 - 1, poss - 3);
		p.drawLine((m_rowHeight - 4)/2, poss - m_rowHeight + 3, (m_rowHeight- 4)/2, poss - 3);
		p.drawLine((m_rowHeight - 4)/2 + 1, poss - m_rowHeight + 3, (m_rowHeight- 4)/2 + 1, poss - 3);
	}
#endif
}

void KexiTableRM::setCurrentRow(int row)
{
	int oldRow = m_currentRow;
	m_currentRow=row;//+1;
	
	update(0,(m_rowHeight*(oldRow))-m_offset-1, width()+2, m_rowHeight+2);
//	update(0,(m_rowHeight*row)-m_offset, width(), m_rowHeight);
	update(0,(m_rowHeight*row)-m_offset-1, width()+2, m_rowHeight+2);
//	update(0,(m_rowHeight*row+1)-m_offset, width(), m_rowHeight);
	
//	update();
//	update(0, (m_rowHeight*(row+1))-m_offset, width(), m_rowHeight);
}

void KexiTableRM::setOffset(int offset)
{
	int oldOff = m_offset;
	m_offset = offset;
	scroll(0,oldOff-offset);
}

void KexiTableRM::setCellHeight(int cellHeight)
{
	m_rowHeight = cellHeight;
}

void KexiTableRM::setEditRow(int row)
{
	m_editRow = row;
}

void KexiTableRM::showInsertRow(bool show)
{
	m_showInsertRow = show;
}

void KexiTableRM::setColor(const QColor &newcolor)
{
	m_pointerColor = newcolor;
}

#include "kexitablerm.moc"
