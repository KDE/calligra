/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Daniel Molkentin <molkentin@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.

   Original Author:  Till Busch <till@bux.at>
   Original Project: buX (www.bux.at)
*/

#include <qcolor.h>

#include <qpixmap.h>
#include <qpainter.h>

#include "kexitablerm.h"

const char *KexiTableRM::arrow[]=
{
"7 13 2 1",
"# c #AF0000",
". c None",
"#......",
"##.....",
"###....",
"####...",
"#####..",
"######.",
"#######",
"######.",
"#####..",
"####...",
"###....",
"##.....",
"#......"
};

const char *KexiTableRM::star[]=
{
"7 7 2 1",
"  c None",
". c #AF0000",
"   .   ",
" . . . ",
"  ...  ",
".......",
"  ...  ",
" . . . ",
"   .   "
};

KexiTableRM::KexiTableRM(QWidget *parent)
:QWidget(parent)
{
	m_rowHeight = 1;
	m_offset=0;
	m_currentRow=-1;
	m_insertRow=-1;
	m_pointerColor = QColor(99,0,0);
	m_pArrowPixmap = new QPixmap(arrow);
}

KexiTableRM::~KexiTableRM()
{
	delete m_pArrowPixmap;
}

void KexiTableRM::paintEvent(QPaintEvent *e)
{
	QPainter p(this);
	QRect r(e->rect());

	int first = (r.top()    + m_offset) / m_rowHeight + 1;
	int last  = (r.bottom() + m_offset) / m_rowHeight + 1;

	p.setPen(m_pointerColor);
	if(m_currentRow >= first && m_currentRow <= last && m_currentRow != m_insertRow)
	{
		int pos = ((m_rowHeight*m_currentRow)-m_offset)-1;
		for(int i=0; i < (m_rowHeight - 2)/ 2; i++)
		{
			p.drawLine(i + 2, pos - m_rowHeight + 2 + i, i + 2, pos - 2 - i);
		}

	}

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

}

void KexiTableRM::setCurrentRow(int row)
{
	int oldRow = m_currentRow;
	m_currentRow=row+1;
	repaint(0,(m_rowHeight*(oldRow-1))-m_offset, width(), m_rowHeight, true);
	update(0,(m_rowHeight*row)-m_offset, width(), m_rowHeight);
}

void KexiTableRM::setOffset(int offset)
{
	int oldOff = m_offset;
	m_offset = offset;
	scroll(0,oldOff-offset);
	qDebug("KexiTableRM::setOffset(): offset is: %i", offset);
}

void KexiTableRM::setCellHeight(int cellHeight)
{
	m_rowHeight = cellHeight;
}

void KexiTableRM::setInsertRow(int row)
{
	m_insertRow = row;
}

void KexiTableRM::setColor(const QColor &newcolor)
{
	m_pointerColor = newcolor;
}

#include "kexitablerm.moc"
