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

#include <qimage.h>

#include <kdebug.h>

#include "koOutline.h"

KoPainter::KoPainter(int w, int h)
{
  mOutline = 0L;
  mFill = 0L;
  mBuffer = new QImage(w, h, 32);
  mWidth = w;
  mHeight = h;
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

void KoPainter::resize(int w, int h)
{
}

void KoPainter::fillAreaRGB(const QRect &r, const KoColor &c)
{
  QRgb a = c.color().rgb();
  for(int y = r.top(); y <= r.bottom(); y++)
  {
    QRgb *ptr = reinterpret_cast<QRgb *>(mBuffer->scanLine(y));
    for(int x = r.left(); x <= r.right(); x++)
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

void KoPainter::drawRect(double x, double y, double w, double h, double rx, double ry)
{
  if((int) rx != 0 && (int) ry != 0)
	{
		ArtVpath *res;
		ArtBpath *vec = art_new(ArtBpath, 10);

		int i = 0;

		if(rx > w / 2)
			rx = w / 2;

		if(ry > h / 2)
			ry = h / 2;

		vec[i].code = ART_MOVETO_OPEN;
		vec[i].x3 = x + rx;
		vec[i].y3 = y;

		i++;

		vec[i].code = ART_CURVETO;
		vec[i].x1 = x + rx * (1 - 0.552);
		vec[i].y1 = y;
		vec[i].x2 = x;
		vec[i].y2 = y + ry * (1 - 0.552);
		vec[i].x3 = x;
		vec[i].y3 = y + ry;

		i++;

		if(ry < h / 2)
		{
			vec[i].code = ART_LINETO;
			vec[i].x3 = x;
			vec[i].y3 = y + h - ry;

			i++;
		}

		vec[i].code = ART_CURVETO;
		vec[i].x1 = x;
		vec[i].y1 = y + h - ry * (1 - 0.552);
		vec[i].x2 = x + rx * (1 - 0.552);
		vec[i].y2 = y + h;
		vec[i].x3 = x + rx;
		vec[i].y3 = y + h;

		i++;

		if(rx < w / 2)
		{
			vec[i].code = ART_LINETO;
			vec[i].x3 = x + w - rx;
			vec[i].y3 = y + h;

			i++;
		}

		vec[i].code = ART_CURVETO;
		vec[i].x1 = x + w - rx * (1 - 0.552);
		vec[i].y1 = y + h;
		vec[i].x2 = x + w;
		vec[i].y2 = y + h - ry * (1 - 0.552);
		vec[i].x3 = x + w;

		vec[i].y3 = y + h - ry;

		i++;

		if(ry < h / 2)
		{
			vec[i].code = ART_LINETO;
			vec[i].x3 = x + w;
			vec[i].y3 = y + ry;

			i++;
		}

		vec[i].code = ART_CURVETO;
		vec[i].x1 = x + w;
		vec[i].y1 = y + ry * (1 - 0.552);
		vec[i].x2 = x + w - rx * (1 - 0.552);
		vec[i].y2 = y;
		vec[i].x3 = x + w - rx;
		vec[i].y3 = y;

		i++;

		if(rx < w / 2)
		{
			vec[i].code = ART_LINETO;
			vec[i].x3 = x + rx;
			vec[i].y3 = y;

			i++;
		}

		vec[i].code = ART_END;

		res = art_bez_path_to_vec(vec, 0.25);
		drawVPath(res);
	}
	else
	{
		ArtVpath *vec = art_new(ArtVpath, 6);

		vec[0].code = ART_MOVETO;
		vec[0].x = x;
		vec[0].y = y;

		vec[1].code = ART_LINETO;
		vec[1].x = x;
		vec[1].y = y + h;

		vec[2].code = ART_LINETO;
		vec[2].x = x + w;
		vec[2].y = y + h;

		vec[3].code = ART_LINETO;
		vec[3].x = x + w;
		vec[3].y = y;

		vec[4].code = ART_LINETO;
		vec[4].x = x;
		vec[4].y = y;

		vec[5].code = ART_END;

		drawVPath(vec);
	}
}

void KoPainter::blit(QWidget *w)
{
  bitBlt((QPaintDevice *)w, 0, 0, mBuffer, 0, 0, 800, 600);
}

void KoPainter::memset(QRgb *p, int n, QRgb c)
{
}

void KoPainter::drawVPath(ArtVpath *vec)
{
  ArtSVP *svp;

/*	double affine[6];
	affine[0] = m_matrix->a();
	affine[1] = m_matrix->b();
	affine[2] = m_matrix->c();
	affine[3] = m_matrix->d();
	affine[4] = m_matrix->e();
	affine[5] = m_matrix->f();
	vec = art_vpath_affine_transform(vec, affine);
*/

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
