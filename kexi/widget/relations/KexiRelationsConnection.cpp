/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004-2007 Jaroslaw Staniek <js@iidea.pl>

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

#include <QPainter>
#include <QPixmap>
#include <QColor>
#include <QApplication>
#include <Q3PointArray>

#include <KDebug>

#include <math.h>

#include "KexiRelationsScrollArea.h"
#include "KexiRelationsView.h"
#include "KexiRelationsConnection.h"
#include <kexidb/tableschema.h>
#include <kexidb/utils.h>
#include <core/kexi.h>
#include <kexiutils/utils.h>

//! @internal
class KexiRelationsConnection::Private
{
	public:
		Private()
		{
		}
		QPointer<KexiRelationsTableContainer> masterTable;
		QPointer<KexiRelationsTableContainer> detailsTable;
		QString masterField;
		QString detailsField;
		QRect oldRect;
		bool selected;
		QPointer<KexiRelationsScrollArea> scrollArea;
};

//-------------------------

KexiRelationsConnection::KexiRelationsConnection(
	KexiRelationsTableContainer *masterTbl, KexiRelationsTableContainer *detailsTbl, 
	SourceConnection &c, KexiRelationsScrollArea *scrollArea)
 : d( new Private )
{
	d->scrollArea = scrollArea;
//	kDebug() << "KexiRelationsConnection::KexiRelationsConnection()" << endl;

	d->masterTable = masterTbl;
	if(!masterTbl || !detailsTbl)
	{
		kDebug() << "KexiRelationsConnection::KexiRelationsConnection(): expect sig11" << endl;
		kDebug() << "KexiRelationsConnection::KexiRelationsConnection()" << masterTbl << endl;
		kDebug() << "KexiRelationsConnection::KexiRelationsConnection()" << detailsTbl << endl;
	}

	d->detailsTable = detailsTbl;
	d->masterField = c.masterField;
	d->detailsField = c.detailsField;

	d->selected = false;
}

KexiRelationsConnection::~KexiRelationsConnection()
{
	delete d;
}

void
KexiRelationsConnection::drawConnection(QPainter *p)
{
	QPen origPen(p->pen());
	p->setPen(d->scrollArea->palette().color(QPalette::WindowText));
	int sx = d->masterTable->x() + d->masterTable->width() + d->scrollArea->horizontalScrollBar()->value();
	int sy = d->masterTable->globalY(d->masterField);
	int rx = d->detailsTable->x() + d->scrollArea->horizontalScrollBar()->value();
	int ry = d->detailsTable->globalY(d->detailsField);

	QFont f( KexiUtils::smallFont( d->scrollArea ) );
	QFontMetrics fm(f);
	int side1x=0, side1y=sy - fm.height(), 
		sideNx=0, sideNy=ry - fm.height();
//! @todo details char can be also just a '1' for some cases
	QChar sideNChar(0x221E); //infinity char
	uint sideNCharWidth = 2+2+ fm.width( sideNChar );
	QChar side1Char('1');
	uint side1CharWidth = 2+2+ fm.width( side1Char );
	p->setBrush(p->pen().color());

	QPen pen(p->pen());
	QPen mainLinePen(pen);
	if (d->selected) {
		mainLinePen.setWidth(5);
		QColor col(pen.color());
		col.setAlpha(90);
		mainLinePen.setColor(col);
		mainLinePen.setCapStyle(Qt::RoundCap);
	}

	if(d->masterTable->x() < d->detailsTable->x()) {
		//det. side
		p->drawLine(rx - sideNCharWidth, ry, rx-2, ry);
		Q3PointArray pa(3);
		pa.setPoint(0, rx - 4, ry - 3);
		pa.setPoint(1, rx - 4, ry + 3);
		pa.setPoint(2, rx - 1, ry);
		pen.setWidthF(0.5);
		p->setPen(pen);
			p->drawPolygon(pa, true);
		pen.setWidth(1);
		p->setPen(pen);
		
		//master side
		p->drawLine(sx, sy - 1, sx + side1CharWidth -1, sy - 1);
		p->drawLine(sx, sy, sx + side1CharWidth -1, sy);
		p->drawLine(sx, sy + 1, sx + side1CharWidth -1, sy + 1);

		side1x = sx;
//		side1y = sy - 7;

		sideNx = rx - sideNCharWidth - 1;
//		sideNy = ry - 6;

		QLine mainLine(sx + side1CharWidth, sy, rx - sideNCharWidth, ry);
		p->setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing, true);
		p->drawLine(mainLine);
		if (d->selected) {
			p->setPen(mainLinePen);
			p->setCompositionMode(QPainter::CompositionMode_Xor);
		}
		p->drawLine(mainLine);
	}
	else
	{
		int lx = rx + d->detailsTable->width();
		int rx = sx - d->masterTable->width();

		//det. side
		p->drawLine(lx +2, ry, lx + sideNCharWidth, ry);
		Q3PointArray pa(3);
		pa.setPoint(0, lx + 3, ry - 3);
		pa.setPoint(1, lx + 3, ry + 3);
		pa.setPoint(2, lx, ry);
		pen.setWidthF(0.5);
		p->setPen(pen);
			p->drawPolygon(pa, true);
		pen.setWidth(1);
		p->setPen(pen);
		
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

		QLine mainLine(lx + sideNCharWidth, ry, rx - side1CharWidth, sy);
		p->setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing, true);
		p->drawLine(mainLine);
		if (d->selected) {
			p->setPen(mainLinePen);
			p->setCompositionMode(QPainter::CompositionMode_Xor);
		}
		p->drawLine(mainLine);
	}
	if (d->selected) {
		p->setCompositionMode(QPainter::CompositionMode_SourceOver);
		p->setPen(pen);
	}

	p->drawText(side1x, side1y, side1CharWidth, fm.height(), Qt::AlignCenter, side1Char);
	p->drawText(sideNx, sideNy, sideNCharWidth, fm.height(), Qt::AlignCenter, sideNChar);
	p->setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing, false);
	p->setPen(origPen);
}

QRect
KexiRelationsConnection::connectionRect()
{
	int sx = d->masterTable->x() + d->scrollArea->horizontalScrollBar()->value();
	int rx = d->detailsTable->x() + d->scrollArea->horizontalScrollBar()->value(); //Qt 4 d->scrollArea->contentsX();
	int ry = d->detailsTable->globalY(d->detailsField);
	int sy = d->masterTable->globalY(d->masterField);

	int width, leftX, rightX;

	if(sx < rx)
	{
		leftX = sx;
		rightX = rx;
		width = d->masterTable->width();
	}
	else
	{
		leftX = rx;
		rightX = sx;
		width = d->detailsTable->width();
	}


	int dx = qAbs((leftX + width) - rightX);
	int dy = qAbs(sy - ry) + 2;

	int top = qMin(sy, ry);
	int left = leftX + width;


//	return QRect(sx - 1, sy - 1, (rx + d->detailsTable->width()) - sx + 1, ry - sy + 1);
//	QRect rect(left - 150, top - 150, dx + 150, dy + 150);
	QRect rect(left - 30, top - 30, dx + 60, dy + 60);
//	kDebug() << "KexiRelationsConnection::connectionRect():" << d->oldRect << "," << rect << endl;
	
	d->oldRect = rect;

	return rect;
}

bool
KexiRelationsConnection::matchesPoint(const QPoint &p, int tolerance)
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

	int sx = d->masterTable->x() + d->masterTable->width();
	int sy = d->masterTable->globalY(d->masterField);
	int rx = d->detailsTable->x();
	int ry = d->detailsTable->globalY(d->detailsField);

	int x1 = sx + 8;
	int y1 = sy;
	int x2 = rx - 8;
	int y2 = ry;

	if(sx > rx)
	{
		x1 = d->detailsTable->x() + d->detailsTable->width();
		x2 = d->masterTable->x();
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
	kDebug() << "KexiRelationsConnection::matchesPoint(): u: " << u << endl;

	float iX = x1 + u * (x2 - x1);
	float iY = y1 + u * (y2 - y1);
	kDebug() << "KexiRelationsConnection::matchesPoint(): px: " << p.x() << endl;
	kDebug() << "KexiRelationsConnection::matchesPoint(): py: " << p.y() << endl;
	kDebug() << "KexiRelationsConnection::matchesPoint(): ix: " << iX << endl;
	kDebug() << "KexiRelationsConnection::matchesPoint(): iy: " << iY << endl;

	float dX = iX - p.x();
	float dY = iY - p.y();

	kDebug() << "KexiRelationsConnection::matchesPoint(): dx: " << dX << endl;
	kDebug() << "KexiRelationsConnection::matchesPoint(): dy: " << dY << endl;

	float distance = sqrt(dX * dX + dY * dY);
	kDebug() << "KexiRelationsConnection::matchesPoint(): distance: " << distance << endl;

	if(distance <= tolerance)
		return true;

	return false;
}

QString
KexiRelationsConnection::toString() const
{
	QString str;
/*! @todo what about query? */
	if (d->masterTable && d->masterTable->schema()->table()) {
		str += (QString(d->masterTable->schema()->name()) + "." + d->masterField);
	}
	if (d->detailsTable && d->detailsTable->schema()->table()) {
		str += " - ";
		str += (QString(d->detailsTable->schema()->name()) + "." + d->detailsField);
	}
	return str;
}

bool KexiRelationsConnection::selected() const { return d->selected; }

void KexiRelationsConnection::setSelected(bool s) { d->selected = s; }

QRect KexiRelationsConnection::oldRect() const { return d->oldRect; }

KexiRelationsTableContainer* KexiRelationsConnection::masterTable() const { return d->masterTable; }

KexiRelationsTableContainer* KexiRelationsConnection::detailsTable() const { return d->detailsTable; }

QString KexiRelationsConnection::masterField() const { return d->masterField; }

QString KexiRelationsConnection::detailsField() const { return d->detailsField; }
