/* This file is part of the KDE project
  Copyright (c) 2002 Igor Janssen (rm@kde.org)

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "koPainter.h"

#include <support/art_vpath_bpath.h>
#include <support/art_rgb.h>
#include <support/art_render.h>
#include <support/art_rgb_svp.h>
#include <support/art_vpath_dash.h>
#include <support/art_svp_vpath_stroke.h>

#include <qwidget.h>
#include <qimage.h>

#include <kdebug.h>

#include "koVectorPath.h"
#include "koOutline.h"

//#include <support/xlibrgb.h>
//#include <X11/Xlib.h>

KoPainter::KoPainter(QWidget *aWidget, int w, int h)
{
  mWidget = aWidget;
  mOutline = 0L;
  mFill = 0L;
  mBuffer = new QImage(w, h, 32);
  mWidth = w;
  mHeight = h;
//  xlib_rgb_init_with_depth(mWidget->x11Display(), XScreenOfDisplay(mWidget->x11Display(), mWidget->x11Screen()), mWidget->x11Depth());
//  gc = XCreateGC(mWidget->x11Display(), mWidget->handle(), 0, 0);
}

KoPainter::~KoPainter()
{
  delete mBuffer;
}

void KoPainter::outline(KoOutline *aOutline)
{
  mOutline = aOutline;
}

void KoPainter::fill(KoFill *aFill)
{
  mFill = aFill;
}

void KoPainter::resize(const int w, const int h)
{
  mBuffer->create(w, h, 32);
  mWidth = w;
  mHeight = h;
}

void KoPainter::resize(const QSize &size)
{
  kdDebug() << "ooooooooWIDTH = " << size.width() << endl;
  kdDebug() << "ooooooooHEIGHT = " << size.height() << endl;
  mBuffer->create(size, 32);
  mWidth = size.width();
  mHeight = size.height();
}

void KoPainter::fillAreaRGB(const QRect &r, const KoColor &c)
{
  QRgb a = c.color().rgb();
  int b = r.bottom();
  if(b < 0)
    return;
  if(b >= mHeight)
    b = mHeight - 1;
  int t = r.top();
  if(t >= mHeight)
    return;
  if(t < 0)
    t = 0;
  int l = r.left();
  if(l >= mWidth)
    return;
  if(l < 0)
    l = 0;
  int ri = r.right();
  if(ri < 0)
    return;
  if(ri >= mWidth)
    ri = mWidth - 1;
  kdDebug() << "LEFT = " << l << endl;
  kdDebug() << "RIGHT = " << ri << endl;
  kdDebug() << "TOP = " << t << endl;
  kdDebug() << "BOTTOM = " << b << endl;
  for(int y = t; y <= b; y++)
  {
    QRgb *ptr = reinterpret_cast<QRgb *>(mBuffer->scanLine(y));
    for(int x = l; x <= ri; x++)
      *(ptr + x) = a;
  }
}

void KoPainter::drawRectRGB(const QRect &r, const KoColor &c)
{
  QRgb a = c.color().rgb();
  QRgb *ptr;
  ptr = reinterpret_cast<QRgb *>(mBuffer->scanLine(r.top()));
  for(int x = r.left(); x <= r.right(); x++)
    *(ptr + x) = a;
  ptr = reinterpret_cast<QRgb *>(mBuffer->scanLine(r.bottom()));
  for(int x = r.left(); x <= r.right(); x++)
    *(ptr + x) = a;
  for(int y = r.top() + 1; y < r.bottom(); y++)
  {
    ptr = reinterpret_cast<QRgb *>(mBuffer->scanLine(y));
    *(ptr + r.left()) = a;
    *(ptr + r.right()) = a;
  }
}

void KoPainter::drawHorizLineRGB(const int x1, const int x2, const int y, const KoColor &c)
{
  QRgb a = c.color().rgb();
  QRgb *ptr = reinterpret_cast<QRgb *>(mBuffer->scanLine(y));
  for(int x = x1; x <= x2; x++)
    *(ptr + x) = a;
}

void KoPainter::drawVertLineRGB(const int x, const int y1, const int y2, const KoColor &c)
{
  QRgb a = c.color().rgb();
  QRgb *ptr;
  for(int y = y1; y <= y2; y++)
  {
    ptr = reinterpret_cast<QRgb *>(mBuffer->scanLine(y));
    *(ptr + x) = a;
  }
}

void KoPainter::drawLine(double x1, double y1, double x2, double y2)
{
  ArtVpath *vec = art_new(ArtVpath, 3);

  vec[0].code = ART_MOVETO_OPEN;
  vec[0].x = x1;
  vec[0].y = y1;

  vec[1].code = ART_LINETO;
  vec[1].x = x2;
  vec[1].y = y2;

  vec[2].code = ART_END;

  drawVPath(vec);
  delete vec;
}

void KoPainter::drawVectorPath(KoVectorPath *vp)
{
  drawVPath(vp->data());
}

void KoPainter::blit()
{
//  xlib_draw_rgb_image(mWidget->handle(), gc, 0, 0, mWidth, mHeight, XLIB_RGB_DITHER_NONE, mBuffer->bits(), mWidth * 3);
//  xlib_draw_rgb_32_image(mWidget->handle(), gc, 0, 0, mWidth, mHeight, XLIB_RGB_DITHER_NONE, mBuffer->bits(), mWidth * 4);
}

void KoPainter::memset(QRgb *p, int n, QRgb c)
{
}

void KoPainter::drawVPath(ArtVpath *vec)
{
  ArtSVP *svp;
/*  bool sameURI = false;

		if(m_drawShape->getFillColor()->paintType() == SVG_PAINTTYPE_URI && m_drawShape->getOutlineColor()->paintType() == SVG_PAINTTYPE_URI)
		{
			if(m_drawShape->getFillColor()->uri() == m_drawShape->getOutlineColor()->uri())
				sameURI = true;
		}
		
		if(m_drawShape->getFillColor()->paintType() == SVG_PAINTTYPE_URI)
		{
			SVGElementImpl *elem = m_drawShape->ownerSVGElement()->getElementById(m_drawShape->getFillColor()->uri());
			if(elem)
			{
				if(elem->nodeName() == SVGPatternElementImpl::TAG)
				{
					static_cast<SVGPatternElementImpl *>(elem)->setBBoxTarget(m_drawShape);

					elem->setAttributes();
					elem->render(this);
				}
				else
				{
					static_cast<SVGGradientElementImpl *>(elem)->setFillSVP(art_svp_from_vpath(vec));
					static_cast<SVGGradientElementImpl *>(elem)->setBBoxTarget(m_drawShape);

					if(!sameURI)
					{
						elem->setAttributes();
						elem->render(this);
					}
				}
			}
		}

		if(m_drawShape->getOutlineColor()->paintType() == SVG_PAINTTYPE_URI)
		{
			SVGElementImpl *elem = m_drawShape->ownerSVGElement()->getElementById(m_drawShape->getOutlineColor()->uri());
			if(elem)
			{
				if(static_cast<SVGGradientElementImpl *>(elem))
				{
					QRgb white = qRgb(255, 255, 255);
					art_u32 strokeColor = (qRed(white << 24) | (qGreen(white << 16) | (qBlue(white) << 8) | (qAlpha(white))));

					ArtSVP *svp = art_svp_vpath_stroke(vec, m_drawShape->getJoinStyle(), m_drawShape->getCapStyle(), m_penWidth, 4, 0.25);
					static_cast<SVGGradientElementImpl *>(elem)->setStrokeSVP(svp);
					static_cast<SVGGradientElementImpl *>(elem)->setBBoxTarget(m_drawShape);

					if(!sameURI)
					{
						elem->setAttributes();
						elem->render(this);
					}
				}
			}
		}

		if(sameURI)
		{
			SVGElementImpl *elem = m_drawShape->ownerSVGElement()->getElementById(m_drawShape->getFillColor()->uri());
			if(elem)
			{
				if(static_cast<SVGGradientElementImpl *>(elem))
				{
					elem->setAttributes();
					elem->render(this);
				}
			}
		}
	}
*/

/*	if(m_brush)
	{
		art_u32 fillColor = (qRed(m_fillColor.rgb()) << 24) | (qGreen(m_fillColor.rgb()) << 16) | (qBlue(m_fillColor.rgb()) << 8) | (qAlpha(m_fillColor.rgb()));

		ArtSVP *temp;
		ArtSvpWriter *swr;
		temp = art_svp_from_vpath(vec);
	
		if(m_drawShape->getFillRule() == "evenodd")
			swr = art_svp_writer_rewind_new(ART_WIND_RULE_ODDEVEN);
		else
			swr = art_svp_writer_rewind_new(ART_WIND_RULE_NONZERO);
		art_svp_intersector(temp, swr);
		svp = art_svp_writer_rewind_reap(swr);
		art_rgb_svp_alpha(svp, 0, 0, m_width, m_height, fillColor, m_buffer, m_width * 3, 0);

		art_svp_free(temp);
		art_svp_free(svp);
	}
*/

  if(mOutline)
  {
    QRgb color = mOutline->color().color().rgb();

    if(mOutline->dashes().count() > 0)
    {
      /* dashes */
      ArtVpathDash dash;
      dash.offset = mOutline->dashOffset();
      dash.n_dash = mOutline->dashes().count();
      dash.dash = mOutline->dashes().data();
      /* get the dashed VPath and use that for the stroke render operation */
      ArtVpath *vec2 = art_vpath_dash(vec, &dash);
      art_free(vec);
      vec = vec2;
    }

    svp = art_svp_vpath_stroke(vec, (ArtPathStrokeJoinType)mOutline->join(), (ArtPathStrokeCapType)mOutline->cap(), mOutline->width(), 4, 0.25);
    art_rgb_svp_alpha(svp, 0, 0, mWidth, mHeight, (art_u32)color, mOutline->opacity(), mBuffer->bits(), mWidth * 4, 0);
    art_free(svp);
  }
}
