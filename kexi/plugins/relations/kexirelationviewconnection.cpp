/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>

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
*/

#include <qpainter.h>
#include <qpixmap.h>
#include <qcolor.h>

#include <kdebug.h>

#include <math.h>

#include <kexirelation.h>
#include "kexirelationview.h"
#include "kexirelationviewtable.h"
#include "kexirelationviewconnection.h"

#include "r1.xpm"
#include "rn.xpm"

KexiRelationViewConnection::KexiRelationViewConnection(KexiRelationViewTableContainer *srcTbl, KexiRelationViewTableContainer *rcvTbl,
   SourceConnection &c, KexiRelationView *parent)
{
	m_parent = parent;
	kdDebug() << "KexiRelationViewConnection::KexiRelationViewConnection()" << endl;

	m_srcTable = srcTbl;
	if(!srcTbl || !rcvTbl)
	{
		kdDebug() << "KexiRelationViewConnection::KexiRelationViewConnection(): expect sig11" << endl;
		kdDebug() << "KexiRelationViewConnection::KexiRelationViewConnection()" << srcTbl << endl;
		kdDebug() << "KexiRelationViewConnection::KexiRelationViewConnection()" << rcvTbl << endl;
	}

	m_conn = c;
	m_rcvTable = rcvTbl;
	m_srcField = c.srcField;
	m_rcvField = c.rcvField;

	m_selected = false;
}

void
KexiRelationViewConnection::drawConnection(QPainter *p)
{
	p->setPen(QColor(0,0,0));
	int sx = m_srcTable->x() + m_srcTable->width() + m_parent->contentsX();
	int sy = m_srcTable->globalY(m_srcField);
	int rx = m_rcvTable->x() + m_parent->contentsX();
	int ry = m_rcvTable->globalY(m_rcvField);

	QPoint side1(0, 0);
	QPoint sideN(0, 0);

	if(m_srcTable->x() < m_rcvTable->x())
	{
		p->drawLine(rx - 8, ry, rx, ry);
		p->drawPoint(rx - 2, ry - 1);
		p->drawPoint(rx - 2, ry + 1);
		p->drawLine(rx - 3, ry - 2, rx - 3, ry + 2);

		p->drawLine(sx, sy - 1, sx + 5, sy - 1);
		p->drawLine(sx, sy, sx + 6, sy);
		p->drawLine(sx, sy + 1, sx + 5, sy + 1);

		side1.setX(sx + 2);
		side1.setY(sy - 7);

		sideN.setX(rx - 10);
		sideN.setY(ry - 6);

		if(m_selected)
		{
			QPen pen(p->pen());
			pen.setWidth(2);
			p->setPen(pen);
		}

		p->drawLine(sx + 6, sy, rx - 8, ry);

		if(m_selected)
		{
			QPen pen(p->pen());
			pen.setWidth(1);
			p->setPen(pen);
		}

	}
	else
	{
		int lx = rx + m_rcvTable->width();
		int rx = sx - m_srcTable->width();


		p->drawLine(lx, ry, lx + 8, ry);
		p->drawPoint(lx + 1, ry - 1);
		p->drawPoint(lx + 1, ry + 1);
		p->drawLine(lx + 2, ry - 2, lx + 2, ry + 2);

		p->drawLine(rx - 7, sy - 1, rx, sy - 1);
		p->drawLine(rx - 7, sy + 1, rx, sy + 1);
		p->drawLine(rx - 8, sy, rx, sy);

		side1.setX(rx - 4);
		side1.setY(sy - 7);

		sideN.setX(lx + 3);
		sideN.setY(ry - 6);

		if(m_selected)
		{
			QPen pen(p->pen());
			pen.setWidth(2);
			p->setPen(pen);
		}

		p->drawLine(lx + 8, ry, rx - 8, sy);

		if(m_selected)
		{
			QPen pen(p->pen());
			pen.setWidth(1);
			p->setPen(pen);
		}


	}

	p->drawPixmap(side1, QPixmap(r1_xpm));
	p->drawPixmap(sideN, QPixmap(rn_xpm));
}

const QRect
KexiRelationViewConnection::connectionRect()
{

	int sx = m_srcTable->x() + m_parent->contentsX();
	int rx = m_rcvTable->x() + m_parent->contentsX();
	int ry = m_rcvTable->globalY(m_rcvField);
	int sy = m_srcTable->globalY(m_srcField);

	int width, leftX, rightX;

	if(sx < rx)
	{
		leftX = sx;
		rightX = rx;
		width = m_srcTable->width();
	}
	else
	{
		leftX = rx;
		rightX = sx;
		width = m_rcvTable->width();
	}


	int dx = QABS((leftX + width) - rightX);
	int dy = QABS(sy - ry) + 2;

	int top = QMIN(sy, ry);
	int left = leftX + width;


//	return QRect(sx - 1, sy - 1, (rx + m_rcvTable->width()) - sx + 1, ry - sy + 1);
	QRect rect(left - 3, top - 7, dx + 3, dy + 10);
	m_oldRect = rect;

	return rect;
}

bool
KexiRelationViewConnection::matchesPoint(const QPoint &p, int tolerance)
{
	QRect we = connectionRect();

	if(!we.contains(p))
		return false;

	/** get our coordinats
	 *  you know what i mean the x1, y1 is the top point
	 *  and the x2, y2 is the bottom point
	 *  (quite tirvial :) although that was the entrace to the magic
	 *  gate...
	 */

	int sx = m_srcTable->x() + m_srcTable->width();
	int sy = m_srcTable->globalY(m_srcField);
	int rx = m_rcvTable->x();
	int ry = m_rcvTable->globalY(m_rcvField);

	int x1 = sx + 8;
	int y1 = sy;
	int x2 = rx - 8;
	int y2 = ry;

	if(sx > rx)
	{
		x1 = m_rcvTable->x() + m_rcvTable->width();
		x2 = m_srcTable->x();
		y2 = sy;
		y1 = ry;
	}

	/*
	  here we call pythagoras (the greek math geek :p)
	  see: http://w1.480.telia.com/%7Eu48019406/geekporn.gif if you don't know
	  how these people have got sex :)
	 */
	float mx = x2-x1;
	float my = y2-y1;
	float mag = sqrt(mx * mx + my * my);
	float u = (((p.x() - x1)*(x2 - x1))+((p.y() - y1)*(y2 - y1)))/(mag * mag);
	kdDebug() << "KexiRelationViewConnection::matchesPoint(): u: " << u << endl;

	float iX = x1 + u * (x2 - x1);
	float iY = y1 + u * (y2 - y1);
	kdDebug() << "KexiRelationViewConnection::matchesPoint(): px: " << p.x() << endl;
	kdDebug() << "KexiRelationViewConnection::matchesPoint(): py: " << p.y() << endl;
	kdDebug() << "KexiRelationViewConnection::matchesPoint(): ix: " << iX << endl;
	kdDebug() << "KexiRelationViewConnection::matchesPoint(): iy: " << iY << endl;

	float dX = iX - p.x();
	float dY = iY - p.y();

	kdDebug() << "KexiRelationViewConnection::matchesPoint(): dx: " << dX << endl;
	kdDebug() << "KexiRelationViewConnection::matchesPoint(): dy: " << dY << endl;

	float distance = sqrt(dX * dX + dY * dY);
	kdDebug() << "KexiRelationViewConnection::matchesPoint(): distance: " << distance << endl;

	if(distance <= tolerance)
		return true;

	return false;
}

KexiRelationViewConnection::~KexiRelationViewConnection()
{
}

