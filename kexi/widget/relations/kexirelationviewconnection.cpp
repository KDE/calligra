/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004-2005 Jaroslaw Staniek <js@iidea.pl>

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

#include <qpainter.h>
#include <qpixmap.h>
#include <qcolor.h>
#include <qapplication.h>
#include <q3pointarray.h>

#include <kdebug.h>

#include <math.h>

#include "kexirelationview.h"
#include "kexirelationviewtable.h"
#include "kexirelationviewconnection.h"
#include <kexidb/tableschema.h>
#include <kexidb/utils.h>
#include <core/kexi.h>
#include <kexiutils/utils.h>

//#include "r1.xpm"
//#include "rn.xpm"

KexiRelationViewConnection::KexiRelationViewConnection(
	KexiRelationViewTableContainer *masterTbl, KexiRelationViewTableContainer *detailsTbl, 
	SourceConnection &c, KexiRelationView *parent)
{
	m_parent = parent;
//	kDebug() << "KexiRelationViewConnection::KexiRelationViewConnection()" << endl;

	m_masterTable = masterTbl;
	if(!masterTbl || !detailsTbl)
	{
		kDebug() << "KexiRelationViewConnection::KexiRelationViewConnection(): expect sig11" << endl;
		kDebug() << "KexiRelationViewConnection::KexiRelationViewConnection()" << masterTbl << endl;
		kDebug() << "KexiRelationViewConnection::KexiRelationViewConnection()" << detailsTbl << endl;
	}

	m_detailsTable = detailsTbl;
	m_masterField = c.masterField;
	m_detailsField = c.detailsField;

	m_selected = false;
}

KexiRelationViewConnection::~KexiRelationViewConnection()
{
}

void
KexiRelationViewConnection::drawConnection(QPainter *p)
{
	p->setPen(m_parent->palette().active().foreground());
	int sx = m_masterTable->x() + m_masterTable->width() + m_parent->contentsX();
	int sy = m_masterTable->globalY(m_masterField);
	int rx = m_detailsTable->x() + m_parent->contentsX();
	int ry = m_detailsTable->globalY(m_detailsField);

	QFont f( KexiUtils::smallFont( m_parent ) );
	QFontMetrics fm(f);
	int side1x=0, side1y=sy - fm.height(), 
		sideNx=0, sideNy=ry - fm.height();
//! @todo details char can be also just a '1' for some cases
	QChar sideNChar(0x221E); //infinity char
	uint sideNCharWidth = 2+2+ fm.width( sideNChar );
	QChar side1Char('1');
	uint side1CharWidth = 2+2+ fm.width( side1Char );
	p->setBrush(p->pen().color());

	if(m_masterTable->x() < m_detailsTable->x())
	{
		//det. side
		p->drawLine(rx - sideNCharWidth, ry, rx, ry);
		Q3PointArray pa(3);
		pa.setPoint(0, rx - 4, ry - 3);
		pa.setPoint(1, rx - 4, ry + 3);
		pa.setPoint(2, rx - 1, ry);
		p->drawPolygon(pa, true);
		
		//master side
		p->drawLine(sx, sy - 1, sx + side1CharWidth -1, sy - 1);
		p->drawLine(sx, sy, sx + side1CharWidth -1, sy);
		p->drawLine(sx, sy + 1, sx + side1CharWidth -1, sy + 1);

		side1x = sx;
//		side1y = sy - 7;

		sideNx = rx - sideNCharWidth - 1;
//		sideNy = ry - 6;

		QPen pen(p->pen());
		if(m_selected)
		{
			QPen pen(p->pen());
			pen.setWidth(2);
			p->setPen(pen);
		}

		p->drawLine(sx + side1CharWidth, sy, rx - sideNCharWidth, ry);

		if(m_selected)
		{
			QPen pen(p->pen());
			pen.setWidth(1);
			p->setPen(pen);
		}

	}
	else
	{
		int lx = rx + m_detailsTable->width();
		int rx = sx - m_masterTable->width();

		//det. side
		p->drawLine(lx, ry, lx + sideNCharWidth, ry);
		Q3PointArray pa(3);
		pa.setPoint(0, lx + 3, ry - 3);
		pa.setPoint(1, lx + 3, ry + 3);
		pa.setPoint(2, lx, ry);
		p->drawPolygon(pa, true);
		
//		p->drawLine(lx, ry, lx + 8, ry);
//		p->drawPoint(lx + 1, ry - 1);
//		p->drawPoint(lx + 1, ry + 1);
//		p->drawLine(lx + 2, ry - 2, lx + 2, ry + 2);

		//master side
		p->drawLine(rx - side1CharWidth +1, sy - 1, rx, sy - 1);
		p->drawLine(rx - side1CharWidth +1, sy + 1, rx, sy + 1);
		p->drawLine(rx - side1CharWidth +1, sy, rx, sy);

		side1x = rx - side1CharWidth;
//		side1y = sy - 7;

		sideNx = lx + 1;
//		sideNy = ry - 6;

		if(m_selected)
		{
			QPen pen(p->pen());
			pen.setWidth(2);
			p->setPen(pen);
		}

		p->drawLine(lx + sideNCharWidth, ry, rx - side1CharWidth, sy);

		if(m_selected)
		{
			QPen pen(p->pen());
			pen.setWidth(1);
			p->setPen(pen);
		}
	}

	p->drawText(side1x, side1y, side1CharWidth, fm.height(), Qt::AlignCenter, side1Char);
	p->drawText(sideNx, sideNy, sideNCharWidth, fm.height(), Qt::AlignCenter, sideNChar);
	//p->drawRect(QRect(connectionRect().topLeft(), QSize(50,50)));
//	p->drawPixmap(side1, QPixmap(r1_xpm));
//	p->drawPixmap(sideN, QPixmap(rn_xpm));
}

const QRect
KexiRelationViewConnection::connectionRect()
{
	int sx = m_masterTable->x() + m_parent->contentsX();
	int rx = m_detailsTable->x() + m_parent->contentsX();
	int ry = m_detailsTable->globalY(m_detailsField);
	int sy = m_masterTable->globalY(m_masterField);

	int width, leftX, rightX;

	if(sx < rx)
	{
		leftX = sx;
		rightX = rx;
		width = m_masterTable->width();
	}
	else
	{
		leftX = rx;
		rightX = sx;
		width = m_detailsTable->width();
	}


	int dx = QABS((leftX + width) - rightX);
	int dy = QABS(sy - ry) + 2;

	int top = qMin(sy, ry);
	int left = leftX + width;


//	return QRect(sx - 1, sy - 1, (rx + m_detailsTable->width()) - sx + 1, ry - sy + 1);
	QRect rect(left - 150, top - 150, dx + 150, dy + 150);
//	kDebug() << "KexiRelationViewConnection::connectionRect():" << m_oldRect << "," << rect << endl;
	
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

	int sx = m_masterTable->x() + m_masterTable->width();
	int sy = m_masterTable->globalY(m_masterField);
	int rx = m_detailsTable->x();
	int ry = m_detailsTable->globalY(m_detailsField);

	int x1 = sx + 8;
	int y1 = sy;
	int x2 = rx - 8;
	int y2 = ry;

	if(sx > rx)
	{
		x1 = m_detailsTable->x() + m_detailsTable->width();
		x2 = m_masterTable->x();
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
	kDebug() << "KexiRelationViewConnection::matchesPoint(): u: " << u << endl;

	float iX = x1 + u * (x2 - x1);
	float iY = y1 + u * (y2 - y1);
	kDebug() << "KexiRelationViewConnection::matchesPoint(): px: " << p.x() << endl;
	kDebug() << "KexiRelationViewConnection::matchesPoint(): py: " << p.y() << endl;
	kDebug() << "KexiRelationViewConnection::matchesPoint(): ix: " << iX << endl;
	kDebug() << "KexiRelationViewConnection::matchesPoint(): iy: " << iY << endl;

	float dX = iX - p.x();
	float dY = iY - p.y();

	kDebug() << "KexiRelationViewConnection::matchesPoint(): dx: " << dX << endl;
	kDebug() << "KexiRelationViewConnection::matchesPoint(): dy: " << dY << endl;

	float distance = sqrt(dX * dX + dY * dY);
	kDebug() << "KexiRelationViewConnection::matchesPoint(): distance: " << distance << endl;

	if(distance <= tolerance)
		return true;

	return false;
}

QString
KexiRelationViewConnection::toString() const
{
	QString str;
/*! @todo what about query? */
	if (m_masterTable && m_masterTable->schema()->table()) {
		str += (QString(m_masterTable->schema()->name()) + "." + m_masterField);
	}
	if (m_detailsTable && m_detailsTable->schema()->table()) {
		str += " - ";
		str += (QString(m_detailsTable->schema()->name()) + "." + m_detailsField);
	}
	return str;
}
