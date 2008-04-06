/*
 * OpenRPT report writer and rendering engine
 * Copyright (C) 2001-2007 by OpenMFG, LLC
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 * Please contact info@openmfg.com with any questions on this license.
 */

#include "krscreenrender.h"
#include "renderobjects.h"
#include <KoPageFormat.h>
#include <KoGlobal.h>
#include <kdebug.h>
#include <KoUnit.h>

KRScreenRender::KRScreenRender()
{
	_painter = 0;
}

KRScreenRender::~KRScreenRender()
{
}

void KRScreenRender::setPainter ( QPainter * pPainter )
{
	_painter = pPainter;
}

bool KRScreenRender::render ( ORODocument * pDocument , int page )
{
	if ( pDocument == 0 )
		return false;

	qreal xDpi = KoGlobal::dpiX();
	qreal yDpi = KoGlobal::dpiY();

	OROPage * p = pDocument->page ( page );
						 
	// Render Page Objects
	for ( int i = 0; i < p->primitives(); i++ )
	{
		OROPrimitive * prim = p->primitive ( i );
		
		if ( prim->type() == OROTextBox::TextBox )
		{
			OROTextBox * tb = ( OROTextBox* ) prim;

			QPointF ps = tb->position();
			QSizeF sz = tb->size();
			QRectF rc = QRectF (ps.x(),ps.y(), sz.width(),sz.height());

			_painter->save();
			//Background
			
			QColor bg = tb->textStyle().bgColor;
			bg.setAlpha(tb->textStyle().bgOpacity );
			
			//_painter->setBackgroundMode(Qt::OpaqueMode);
			_painter->setBackground(bg);
			_painter->fillRect( rc, bg);
	
			//Text
			_painter->setBackgroundMode(Qt::TransparentMode);
			_painter->setFont ( tb->textStyle().font );
			_painter->setPen(tb->textStyle().fgColor);
			_painter->drawText ( rc, tb->flags(), tb->text() );

			//outer line
			_painter->setPen ( QPen (tb->lineStyle().lnColor, tb->lineStyle().weight, tb->lineStyle().style) );
			_painter->drawRect ( rc );

			//Reset back to defaults for next element
			_painter->restore();

		}
		else if ( prim->type() == OROLine::Line )
		{
			OROLine * ln = ( OROLine* ) prim;
			QPointF s = ln->startPoint();
			QPointF e = ln->endPoint();
			//QPen pen ( _painter->pen() );
			QPen pen( ln->lineStyle().lnColor, ln->lineStyle().weight, ln->lineStyle().style);
			
			_painter->save();
			_painter->setRenderHint(QPainter::Antialiasing, true);
			_painter->setPen ( pen );
			_painter->drawLine ( QLineF ( s.x(), s.y(), e.x(), e.y() ) );
			_painter->setRenderHint(QPainter::Antialiasing, false);
			_painter->restore();
		}
		
		else if ( prim->type() == ORORect::Rect )
		{
			ORORect * re = ( ORORect* ) prim;

			QPointF ps = re->position();
			QSizeF sz = re->size();
			QRectF rc = QRectF ( ps.x(), ps.y(), sz.width(), sz.height() );

			_painter->save();
			_painter->setPen ( re->pen() );
			_painter->setBrush ( re->brush() );
			_painter->drawRect ( rc );
			_painter->restore();
		}
		else if ( prim->type() == OROEllipse::Ellipse )
		{
			OROEllipse * re = ( OROEllipse* ) prim;

			QPointF ps = re->position();
			QSizeF sz = re->size();
			QRectF rc = QRectF ( ps.x(), ps.y(), sz.width(), sz.height() );

			_painter->save();
			_painter->setPen ( re->pen() );
			_painter->setBrush ( re->brush() );
			_painter->drawEllipse ( rc );
			_painter->restore();
		}
		else if ( prim->type() == OROImage::Image )
		{
			OROImage * im = ( OROImage* ) prim;
			QPointF ps = im->position();
			QSizeF sz = im->size();
			QRectF rc = QRectF (ps.x(),ps.y(), sz.width(),sz.height());

			QImage img = im->image();
			if ( im->scaled() )
				img = img.scaled ( rc.size().toSize(), ( Qt::AspectRatioMode ) im->aspectRatioMode(), ( Qt::TransformationMode ) im->transformationMode() );

			QRectF sr = QRectF ( QPointF ( 0.0, 0.0 ), rc.size().boundedTo ( img.size() ) );
			_painter->drawImage ( rc.topLeft(), img, sr );
		}
		else
		{
			kDebug() << "unrecognized primitive type" << endl;
		}
	}


	return true;
}

