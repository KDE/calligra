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

KexiTableRM::KexiTableRM(QWidget *parent)
:QWidget(parent)
{
	m_rowHeight = 1;
	m_offset=0;
	m_currentRow=-1;
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

	if(m_currentRow >= first && m_currentRow <= last)
	{
		int pos = ((m_rowHeight*m_currentRow)-m_offset)-1;
		p.drawPixmap(2,pos-15,*m_pArrowPixmap);
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
}

void KexiTableRM::setCellHeight(int cellHeight)
{
	m_rowHeight = cellHeight;
}

#include "kexitablerm.moc"
