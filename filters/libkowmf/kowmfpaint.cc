/* This file is part of the KDE libraries
 * Copyright (c) 2003 thierry lorthiois (lorthioist@wanadoo.fr)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License version 2 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "kowmfpaint.h"

#include <QPolygon>
#include <QPrinter>
#include <QFontMetrics>

#include <kdebug.h>


#define DEBUG_WMFPAINT 0


KoWmfPaint::KoWmfPaint()
    : KoWmfRead()
{
    mTarget = 0;
    mIsInternalPainter = true;
    mPainter = 0;
}


bool KoWmfPaint::play(QPaintDevice& target, bool relativeCoord)
{
    if (!mPainter)
        mPainter = new QPainter(&target);
    mIsInternalPainter = true;

    if (mPainter->isActive()) return false;
    mTarget = &target;
    mRelativeCoord = relativeCoord;

    // Play the wmf file
    return KoWmfRead::play();
}

bool KoWmfPaint::play(QPainter &painter, bool relativeCoord)
{
    // If there is already a painter and it's owned by us, then delete i
    if (mPainter && mIsInternalPainter)
        delete mPainter;

    // Set the new painter
    mIsInternalPainter = false;
    mPainter = &painter;

    mTarget = mPainter->device();
    mRelativeCoord = relativeCoord;

    // Play the wmf file
    return KoWmfRead::play();
}


//-----------------------------------------------------------------------------
// Virtual Painter

bool KoWmfPaint::begin()
{
    bool ret = true;

    // If the painter is our own, we have to call begin() on it.
    // If it's external, we suppose that it's already done for us.
    if (mIsInternalPainter)
        ret = mPainter->begin(mTarget);

    if (ret) {
        if (mRelativeCoord) {
            mInternalWorldMatrix.reset();
        } else {
            // Some wmf files doesn't call setwindowOrg and
            // setWindowExt, so it's better to do it here.  Note that
            // boundingRect() is the rect of the WMF file, not the device.
            QRect rec = boundingRect();
            kDebug(31000) << "BoundingRect: " << rec;
            mPainter->setWindow(rec.left(), rec.top(), rec.width(), rec.height());
        }
    }

#if DEBUG_WMFPAINT
    kDebug(31000) << "Using QPainter: " << mPainter->pen() << mPainter->brush() 
                  << "Background: " << mPainter->background() << " " << mPainter->backgroundMode();
    kDebug(31000) << "KoWmfPaint::begin returns " << ret;
#endif

    return ret;
}


bool KoWmfPaint::end()
{
    if (mRelativeCoord) {
        QRect rec = boundingRect();

        // Draw 2 invisible points
        // because QPicture::setBoundingRect() doesn't give expected result (QT3.1.2)
        // setBoundingRect( boundingRect() );
//        mPainter->setPen( Qt::NoPen );
//        mPainter->drawPoint( rec.left(), rec.top() );
//        mPainter->drawPoint( rec.right(), rec.bottom() );
    }

    bool ret = true;
    if (mIsInternalPainter)
        ret = mPainter->end();

    return ret;
}


void KoWmfPaint::save()
{
    mPainter->save();
}


void KoWmfPaint::restore()
{
    mPainter->restore();
}


void KoWmfPaint::setFont(const QFont &font)
{
#if DEBUG_WMFPAINT
    kDebug(31000) << font;
#endif
    mPainter->setFont(font);
}


void KoWmfPaint::setPen(const QPen &pen)
{
#if DEBUG_WMFPAINT
    kDebug(31000) << pen;
#endif

    QPen p = pen;
    int width = pen.width();

    // FIXME: For some reason, this seems to be necessary.  There are
    //        some calls to setPen() that contains the wrong color,
    //        but with width == 0.  If these are ignored, the image
    //        looks much more correct.  It should be investigated why
    //        this is so.
    if (width == 0)
        return;

    if (dynamic_cast<QPrinter *>(mTarget)) {
        width = 0;
    } else {
        // WMF spec: width of pen in logical coordinate
        // => width of pen proportional with device context width
        QRect rec = mPainter->window();
        // QPainter documentation says this is equivalent of xFormDev, but it doesn't compile. Bug reported.
#if 0
        QRect devRec = rec * mPainter->matrix();
        if (rec.width() != 0)
            width = (width * devRec.width()) / rec.width() ;
        else
            width = 0;
#endif
    }

    p.setWidth(width);
    mPainter->setPen(p);
}


const QPen &KoWmfPaint::pen() const
{
    return mPainter->pen();
}


void KoWmfPaint::setBrush(const QBrush &brush)
{
#if DEBUG_WMFPAINT
    kDebug(31000) << brush;
#endif
    mPainter->setBrush(brush);
}


void KoWmfPaint::setBackgroundColor(const QColor &c)
{
#if DEBUG_WMFPAINT
    kDebug(31000) << c;
#endif
    // FIXME: This needs more investigation, but it seems that the
    //        concept of "background" in WMF is the same as the
    //        "brush" in QPainter.
    mPainter->setBrush(QBrush(c));
    //mPainter->setBackground(QBrush(c));
}


void KoWmfPaint::setBackgroundMode(Qt::BGMode mode)
{
#if DEBUG_WMFPAINT
    kDebug(31000) << mode;
#endif

    mPainter->setBackgroundMode(mode);
}


void KoWmfPaint::setCompositionMode(QPainter::CompositionMode mode)
{
#if DEBUG_WMFPAINT
    kDebug(31000) << mode << "(ignored)";
#endif

    // FIXME: This doesn't work.  I don't understand why, but when I
    //        enable this all backgrounds become black.
    //mPainter->setCompositionMode(mode);
}


// ---------------------------------------------------------------------
// To change those functions it's better to have
// a large set of WMF files. WMF special case includes :
// - without call to setWindowOrg and setWindowExt
// - change the origin or the scale in the middle of the drawing
// - negative width or height
// and relative/absolute coordinate
void KoWmfPaint::setWindowOrg(int left, int top)
{
#if DEBUG_WMFPAINT
    kDebug(31000) << left << " " << top;
#endif
    //return;
    if (mRelativeCoord) {
        double dx = mInternalWorldMatrix.dx();
        double dy = mInternalWorldMatrix.dy();

        // translation : Don't use setWindow()
        mInternalWorldMatrix.translate(-dx, -dy);
        mPainter->translate(-dx, -dy);
        mInternalWorldMatrix.translate(-left, -top);
        mPainter->translate(-left, -top);
    } else {
        QRect rec = mPainter->window();
        mPainter->setWindow(left, top, rec.width(), rec.height());
    }
}


void KoWmfPaint::setWindowExt(int w, int h)
{
#if DEBUG_WMFPAINT
    kDebug(31000) << w << " " << h;
#endif
    //return;
    if (mRelativeCoord) {
        QRect r = mPainter->window();
        double dx = mInternalWorldMatrix.dx();
        double dy = mInternalWorldMatrix.dy();
        double sx = mInternalWorldMatrix.m11();
        double sy = mInternalWorldMatrix.m22();

        // scale : don't use setWindow()
        mInternalWorldMatrix.translate(-dx, -dy);
        mPainter->translate(-dx, -dy);
        mInternalWorldMatrix.scale(1 / sx, 1 / sy);
        mPainter->scale(1 / sx, 1 / sy);

        sx = (double)r.width() / (double)w;
        sy = (double)r.height() / (double)h;

        mInternalWorldMatrix.scale(sx, sy);
        mPainter->scale(sx, sy);
        mInternalWorldMatrix.translate(dx, dy);
        mPainter->translate(dx, dy);
    } else {
        QRect rec = mPainter->window();
        mPainter->setWindow(rec.left(), rec.top(), w, h);
    }
}


void KoWmfPaint::setMatrix(const QMatrix &wm, bool combine)
{
#if DEBUG_WMFPAINT
    kDebug(31000) << wm << " " << combine;
#endif
    mPainter->setMatrix(wm, combine);
}


void KoWmfPaint::setClipRegion(const QRegion &rec)
{
    mPainter->setClipRegion(rec);
}


QRegion KoWmfPaint::clipRegion()
{
    return mPainter->clipRegion();
}


void KoWmfPaint::moveTo(int x, int y)
{
#if DEBUG_WMFPAINT
    kDebug(31000)<< x << ", " << y;
#endif
    mLastPos = QPoint(x, y);
}


void KoWmfPaint::lineTo(int x, int y)
{
#if DEBUG_WMFPAINT
    kDebug(31000) << x << ", " << y << " using " << mPainter->pen()
                  << "linewidth: " << mPainter->pen().width();
#endif

    QPoint newPoint(x, y);
    mPainter->drawLine(mLastPos, newPoint);
    mLastPos = newPoint;
}


void KoWmfPaint::drawRect(int x, int y, int w, int h)
{
#if DEBUG_WMFPAINT
    kDebug(31000) << x << ", " << y << ", " << w << ", " << h;
    kDebug(31000) << "Using QPainter: " << mPainter->pen() << mPainter->brush();
#endif

    mPainter->drawRect(x, y, w, h);
}


void KoWmfPaint::drawRoundRect(int x, int y, int w, int h, int roudw, int roudh)
{
#if DEBUG_WMFPAINT
    kDebug(31000) << x << ", " << y << ", " << w << ", " << h;
#endif
    mPainter->drawRoundRect(x, y, w, h, roudw, roudh);
}


void KoWmfPaint::drawEllipse(int x, int y, int w, int h)
{
#if DEBUG_WMFPAINT
    kDebug(31000) << x << ", " << y << ", " << w << ", " << h;
#endif
    mPainter->drawEllipse(x, y, w, h);
}


void KoWmfPaint::drawArc(int x, int y, int w, int h, int a, int alen)
{
#if DEBUG_WMFPAINT
    kDebug(31000) << x << ", " << y << ", " << w << ", " << h;
#endif
    mPainter->drawArc(x, y, w, h, a, alen);
}


void KoWmfPaint::drawPie(int x, int y, int w, int h, int a, int alen)
{
#if DEBUG_WMFPAINT
    kDebug(31000) << x << ", " << y << ", " << w << ", " << h;
#endif
    mPainter->drawPie(x, y, w, h, a, alen);
}


void KoWmfPaint::drawChord(int x, int y, int w, int h, int a, int alen)
{
#if DEBUG_WMFPAINT
    kDebug(31000) << x << ", " << y << ", " << w << ", " << h
                  << ", " << a << ", " << alen;
#endif
    mPainter->drawChord(x, y, w, h, a, alen);
}


void KoWmfPaint::drawPolyline(const QPolygon &pa)
{
#if DEBUG_WMFPAINT
    kDebug(31000) << pa;
#endif
    mPainter->drawPolyline(pa);
}


void KoWmfPaint::drawPolygon(const QPolygon &pa, bool winding)
{
#if DEBUG_WMFPAINT
    kDebug(31000) << pa;
    kDebug(31000) << "Using QPainter: " << mPainter->pen() << mPainter->brush();
#endif

    if (winding)
        mPainter->drawPolygon(pa, Qt::WindingFill);
    else
        mPainter->drawPolygon(pa, Qt::OddEvenFill);
}


void KoWmfPaint::drawPolyPolygon(QList<QPolygon>& listPa, bool winding)
{
#if DEBUG_WMFPAINT
    kDebug(31000);
#endif

    mPainter->save();
    QBrush brush = mPainter->brush();

    // define clipping region
    QRegion region;
    foreach(const QPolygon & pa, listPa) {
        region = region.xored(pa);
    }
    mPainter->setClipRegion(region);

    // fill polygons
    if (brush != Qt::NoBrush) {
        kDebug(31000) << "Filling polygon with " << brush;
        mPainter->fillRect(region.boundingRect(), brush);
    }

    // draw polygon's border
    mPainter->setClipping(false);
    if (mPainter->pen().style() != Qt::NoPen) {
        mPainter->setBrush(Qt::NoBrush);
        foreach(const QPolygon & pa, listPa) {
#if DEBUG_WMFPAINT
            kDebug(31000) << pa;
#endif
            if (winding)
                mPainter->drawPolygon(pa, Qt::WindingFill);
            else
                mPainter->drawPolygon(pa, Qt::OddEvenFill);
        }
    }

    // restore previous state
    mPainter->restore();
}


void KoWmfPaint::drawImage(int x, int y, const QImage &img, int sx, int sy, int sw, int sh)
{
#if DEBUG_WMFPAINT
    kDebug(31000) << x << " " << y << " " << sx << " " << sy << " " << sw << " " << sh;
#endif
    mPainter->drawImage(x, y, img, sx, sy, sw, sh);
}


void KoWmfPaint::drawText(int x, int y, int w, int h, int flags, const QString& s, double)
{
#if DEBUG_WMFPAINT
    kDebug(31000) << x << " " << y << " " << w << " " << h << " " << flags << " " << s;
#endif

    // This enum is taken from the karbon WMF import filter.
    // FIXME: This is just a small subset of the align flags that WMF defines.  
    //        They should all be handled.
    enum TextFlags { CurrentPosition = 0x01, AlignHCenter = 0x06, AlignBottom = 0x08 };

    if (flags & CurrentPosition) {
        // (left, top) position = current logical position
        x = mLastPos.x();
        y = mLastPos.y();
    }

    // If this flag is set, the point to draw the text is the
    // baseline, otherwise it should be the upper left corner.
    if (!(flags & AlignBottom)) {
        QFontMetrics  fontMetrics(mPainter->font(), mTarget);
        y += fontMetrics.ascent();

#if DEBUG_WMFPAINT
        kDebug(31000) << "font = " << mPainter->font() << " pointSize = " << mPainter->font().pointSize()
                      << "ascent = " << fontMetrics.ascent() << " height = " << fontMetrics.height()
                      << "leading = " << fontMetrics.leading();
#endif
    }

    // Sometimes it happens that w and/or h == -1, and then the
    // bounding box isn't valid any more.  In that case, no text at
    // all is shown.
    if (w == -1 || h == -1)
        mPainter->drawText(x, y, s);
    else
        // FIXME: Find out which Qt flags should be there instead of the 0.
        mPainter->drawText(x, y, w, h, 0, s);
}
