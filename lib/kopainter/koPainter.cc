/* This file is part of the KDE project
  Copyright (c) 2002 Igor Jansen (rm@kde.org)

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

#include "art_rgb.h"
#include "art_rgb_svp.h"

#include <libart_lgpl/art_render.h>
#include <libart_lgpl/art_render_svp.h>
#include <libart_lgpl/art_render_gradient.h>
#include <libart_lgpl/art_vpath_dash.h>
#include <libart_lgpl/art_svp_vpath_stroke.h>
#include <libart_lgpl/art_svp_vpath.h>
#include <libart_lgpl/art_svp_intersect.h>
#include <libart_lgpl/art_rgb_affine.h>

#include <qwidget.h>
#include <qwmatrix.h>
#include <qimage.h>

#include <kdebug.h>

#include "koVectorPath.h"
#include "koOutline.h"
#include "koDash.h"
#include "koFill.h"

//#include <support/xlibrgb.h>
//#include <X11/Xlib.h>

KoPainter::KoPainter(QWidget *aWidget, int w, int h)
{
  mWidget = aWidget;
  mOutline = 0L;
  mFill = 0L;
  mBuffer = new QImage(w, h, 32);
  mDeleteBuffer = true;
  mWidth = w;
  mHeight = h;
//  xlib_rgb_init_with_depth(mWidget->x11Display(), XScreenOfDisplay(mWidget->x11Display(), mWidget->x11Screen()), mWidget->x11Depth());
//  gc = XCreateGC(mWidget->x11Display(), mWidget->handle(), 0, 0);
}

KoPainter::KoPainter(QImage *aBuffer)
{
  mWidget = 0L;
  mOutline = 0L;
  mFill = 0L;
  mBuffer = aBuffer;
  mDeleteBuffer = false;
  mWidth = mBuffer->width();
  mHeight = mBuffer->height();
}

KoPainter::~KoPainter()
{
  if(mDeleteBuffer)
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
  for(int y = t; y <= b; y++)
  {
    QRgb *ptr = reinterpret_cast<QRgb *>(mBuffer->scanLine(y));
    if ( !ptr )
        return;
    for(int x = l; x <= ri; x++)
      *(ptr + x) = a;
  }
}

void KoPainter::drawRectRGB(const QRect &r, const KoColor &c)
{
  QRgb a = c.color().rgb();
  QRgb *ptr;
  ptr = reinterpret_cast<QRgb *>(mBuffer->scanLine(r.top()));
  if ( !ptr)
      return;
  for(int x = r.left(); x <= r.right(); x++)
    *(ptr + x) = a;
  ptr = reinterpret_cast<QRgb *>(mBuffer->scanLine(r.bottom()));
  if ( !ptr )
      return;
  for(int x = r.left(); x <= r.right(); x++)
    *(ptr + x) = a;
  for(int y = r.top() + 1; y < r.bottom(); y++)
  {
    ptr = reinterpret_cast<QRgb *>(mBuffer->scanLine(y));
    if ( !ptr )
        return;
    *(ptr + r.left()) = a;
    *(ptr + r.right()) = a;
  }
}

void KoPainter::drawHorizLineRGB(const int x1, const int x2, const int y, const QColor &c)
{
  QRgb a = c.rgb();
  QRgb *ptr = reinterpret_cast<QRgb *>(mBuffer->scanLine(y));
  if ( !ptr)
      return;
  for(int x = x1; x <= x2; x++)
    *(ptr + x) = a;
}

void KoPainter::drawVertLineRGB(const int x, const int y1, const int y2, const QColor &c)
{
  QRgb a = c.rgb();
  QRgb *ptr;

  for(int y = y1; y <= y2; y++)
  {
    ptr = reinterpret_cast<QRgb *>(mBuffer->scanLine(y));
    if ( !ptr )
        return;
    *(ptr + x) = a;
  }
}

void KoPainter::drawVectorPathOutline(KoVectorPath *vp)
{
  if(mOutline)
  {
    ArtVpath *vec = vp->data();
    QRgb color = mOutline->color().color().rgb();

    if(mOutline->dash())
      if(mOutline->dash()->dashes().count() > 0)
      {
        // dashes
        ArtVpathDash dash;
        dash.offset = mOutline->dash()->offset();
        dash.n_dash = mOutline->dash()->dashes().count();
        dash.dash = mOutline->dash()->dashes().data();
        // get the dashed VPath and use that for the stroke render operation
        vec = art_vpath_dash(vec, &dash);
      }

    ArtSVP *svp = art_svp_vpath_stroke(vec, (ArtPathStrokeJoinType)mOutline->join(), (ArtPathStrokeCapType)mOutline->cap(), mOutline->width(), 4, 0.25);
    art_rgb_svp_alpha_(svp, 0, 0, mWidth, mHeight, (art_u32)color, mOutline->opacity(), mBuffer->bits(), mWidth * 4, 0);
    art_free(svp);
    if(vec != vp->data())
      art_free(vec);
  }
}

void KoPainter::drawVectorPathFill(KoVectorPath *vp)
{
  if(mFill)
  {
    QRgb color = mFill->color().color().rgb();

    ArtSVP *temp;
    ArtSvpWriter *swr;
    temp = art_svp_from_vpath(vp->data());
    swr = art_svp_writer_rewind_new(ART_WIND_RULE_ODDEVEN);
//    if(m_drawShape->getFillRule() == "evenodd")
//			swr = art_svp_writer_rewind_new(ART_WIND_RULE_ODDEVEN);
//		else
//			swr = art_svp_writer_rewind_new(ART_WIND_RULE_NONZERO);
    art_svp_intersector(temp, swr);
    ArtSVP *svp = art_svp_writer_rewind_reap(swr);
    art_rgb_svp_alpha_(svp, 0, 0, mWidth, mHeight, (art_u32)color, mFill->opacity(), mBuffer->bits(), mWidth * 4, 0);
    art_svp_free(temp);
    art_svp_free(svp);
  }
}

void KoPainter::drawVectorPath(KoVectorPath *vp)
{
  drawVectorPathFill(vp);
  drawVectorPathOutline(vp);
}

void KoPainter::blit()
{
//  xlib_draw_rgb_image(mWidget->handle(), gc, 0, 0, mWidth, mHeight, XLIB_RGB_DITHER_NONE, mBuffer->bits(), mWidth * 3);
//  xlib_draw_rgb_32_image(mWidget->handle(), gc, 0, 0, mWidth, mHeight, XLIB_RGB_DITHER_NONE, mBuffer->bits(), mWidth * 4);
}

void KoPainter::memset(QRgb */*p*/, int /*n*/, QRgb /*c*/)
{
}

/*  Some code for gradients
  double a = 1/32.0;
  double b = -1/33.00000;
  double c = -1/3.0;
  ArtGradientSpread spread = ART_GRADIENT_REPEAT;
  ArtRender *render;
  ArtPixMaxDepth color[3] = {0x0000, 0x0000, 0x8000 };
  ArtGradientLinear gradient;
  ArtGradientStop stops[3] = {
    { 0.02, { 0x7fff, 0x0000, 0x0000, 0x7fff }},
    { 0.5, { 0x0000, 0x0000, 0x0000, 0x1000 }},
    { 0.98, { 0x0000, 0x7fff, 0x0000, 0x7fff }}
  };

  gradient.a = a;
  gradient.b = b;
  gradient.c = c;
  gradient.spread = spread;

  gradient.n_stops = sizeof(stops) / sizeof(stops[0]);
  gradient.stops = stops;

  render = art_render_new(0, 0,
			   mWidth, mHeight,
			   mBuffer->bits(), mWidth * 4,
			   3, 8, ART_ALPHA_NONE,
			   0L);
  art_render_clear_rgb (render, 0xfff0c0);
  art_render_svp (render, svp);
//  art_render_gradient_linear (render, &gradient, ART_FILTER_NEAREST);
  //  art_render_image_solid (render, color);
  art_render_invoke (render);*/

void KoPainter::drawImage(QImage *img, int /*alpha*/, QWMatrix &m)
{
  double affine[6];
  affine[0] = m.m11();
  affine[1] = m.m12();
  affine[2] = m.m21();
  affine[3] = m.m22();
  affine[4] = m.dx();
  affine[5] = m.dy();
  art_rgb_affine(mBuffer->bits(), 0, 0, mWidth, mHeight, mWidth * 4,
		img->bits(), img->width(), img->height(), img->width() * 4,
		affine, ART_FILTER_NEAREST, 0L);
}
