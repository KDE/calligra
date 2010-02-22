/*
 * OpenRPT report writer and rendering engine
 * Copyright (C) 2001-2007 by OpenMFG, LLC (info@openmfg.com)
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
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "krscreenrender.h"
#include "renderobjects.h"
#include <KoPageFormat.h>
#include <kdebug.h>
#include <KoUnit.h>

KRScreenRender::KRScreenRender()
{

}

KRScreenRender::~KRScreenRender()
{
}

//void KRScreenRender::setPainter(QPainter * pPainter)
//{
//    _context.painter = pPainter;
//}

bool KRScreenRender::render(KoReportRendererContext _context, ORODocument *pDocument , int page)
{
    if (pDocument == 0)
        return false;

    if (_context.painter == 0)
        return false;

    OROPage * p = pDocument->page(page);

    // Render Page Objects
    for (int i = 0; i < p->primitives(); i++) {
        OROPrimitive * prim = p->primitive(i);

        if (prim->type() == OROTextBox::TextBox) {
            OROTextBox * tb = (OROTextBox*) prim;

            QPointF ps = tb->position();
            QSizeF sz = tb->size();
            QRectF rc = QRectF(ps.x(), ps.y(), sz.width(), sz.height());

            _context.painter->save();
            //Background

            QColor bg = tb->textStyle().backgroundColor;
            bg.setAlpha((tb->textStyle().backgroundOpacity / 100) * 255);

            _context.painter->setBackground(bg);
            _context.painter->fillRect(rc, bg);

            //Text
            _context.painter->setBackgroundMode(Qt::TransparentMode);
            _context.painter->setFont(tb->textStyle().font);
            _context.painter->setPen(tb->textStyle().foregroundColor);
            _context.painter->drawText(rc.adjusted(2, 2, 0, 0), tb->flags(), tb->text());

            //outer line
            _context.painter->setPen(QPen(tb->lineStyle().lineColor, tb->lineStyle().weight, tb->lineStyle().style));
            _context.painter->drawRect(rc);

            //Reset back to defaults for next element
            _context.painter->restore();

        } else if (prim->type() == OROLine::Line) {
            OROLine * ln = (OROLine*) prim;
            QPointF s = ln->startPoint();
            QPointF e = ln->endPoint();
            //QPen pen ( _painter->pen() );
            QPen pen(ln->lineStyle().lineColor, ln->lineStyle().weight, ln->lineStyle().style);

            _context.painter->save();
            _context.painter->setRenderHint(QPainter::Antialiasing, true);
            _context.painter->setPen(pen);
            _context.painter->drawLine(QLineF(s.x(), s.y(), e.x(), e.y()));
            _context.painter->setRenderHint(QPainter::Antialiasing, false);
            _context.painter->restore();
        }

        else if (prim->type() == ORORect::Rect) {
            ORORect * re = (ORORect*) prim;

            QPointF ps = re->position();
            QSizeF sz = re->size();
            QRectF rc = QRectF(ps.x(), ps.y(), sz.width(), sz.height());

            _context.painter->save();
            _context.painter->setPen(re->pen());
            _context.painter->setBrush(re->brush());
            _context.painter->drawRect(rc);
            _context.painter->restore();
        } else if (prim->type() == OROEllipse::Ellipse) {
            OROEllipse * re = (OROEllipse*) prim;

            QPointF ps = re->position();
            QSizeF sz = re->size();
            QRectF rc = QRectF(ps.x(), ps.y(), sz.width(), sz.height());

            _context.painter->save();
            _context.painter->setPen(re->pen());
            _context.painter->setBrush(re->brush());
            _context.painter->drawEllipse(rc);
            _context.painter->restore();
        } else if (prim->type() == OROImage::Image) {
            OROImage * im = (OROImage*) prim;
            QPointF ps = im->position();
            QSizeF sz = im->size();
            QRectF rc = QRectF(ps.x(), ps.y(), sz.width(), sz.height());

            QImage img = im->image();
            if (im->scaled())
                img = img.scaled(rc.size().toSize(), (Qt::AspectRatioMode) im->aspectRatioMode(), (Qt::TransformationMode) im->transformationMode());

            QRectF sr = QRectF(QPointF(0.0, 0.0), rc.size().boundedTo(img.size()));
            _context.painter->drawImage(rc.topLeft(), img, sr);
        } else if (prim->type() == OROPicture::Picture) {
            OROPicture * im = (OROPicture*) prim;
            QPointF ps = im->position();
            QSizeF sz = im->size();
            QRectF rc = QRectF(ps.x(), ps.y(), sz.width(), sz.height());
            _context.painter->save();
            _context.painter->drawPicture(rc.topLeft(), *(im->picture()));
            _context.painter->restore();
        } else if (prim->type() == OROCheck::Check) {
            OROCheck * chk = (OROCheck*) prim;
            QPointF ps = chk->position();
            QSizeF sz = chk->size();
            QRectF rc = QRectF(ps.x(), ps.y(), sz.width(), sz.height());

            _context.painter->save();

            _context.painter->setBackgroundMode(Qt::OpaqueMode);
            _context.painter->setRenderHint(QPainter::Antialiasing);

            _context.painter->setPen(chk->foregroundColor());

            if (chk->lineStyle().style == Qt::NoPen || chk->lineStyle().weight <= 0) {
                _context.painter->setPen(QPen(QColor(224, 224, 224)));
            } else {
                _context.painter->setPen(QPen(chk->lineStyle().lineColor, chk->lineStyle().weight, chk->lineStyle().style));
            }

            qreal ox = sz.width() / 5;
            qreal oy = sz.height() / 5;

            //Checkbox Style
            if (chk->checkType() == "Cross") {
                _context.painter->drawRoundedRect(rc, sz.width() / 10 , sz.height() / 10);

                if (chk->value()) {
                    QPen lp;
                    lp.setColor(chk->foregroundColor());
                    lp.setWidth(ox > oy ? oy : ox);
                    _context.painter->setPen(lp);
                    _context.painter->drawLine(QPointF(ox, oy) + ps, QPointF(sz.width() - ox, sz.height() - oy) + ps);
                    _context.painter->drawLine(QPointF(ox, sz.height() - oy) + ps, QPoint(sz.width() - ox, oy) + ps);
                }
            } else if (chk->checkType() == "Dot") {
                //Radio Style
                _context.painter->drawEllipse(rc);

                if (chk->value()) {
                    QBrush lb(chk->foregroundColor());
                    _context.painter->setBrush(lb);
                    _context.painter->setPen(Qt::NoPen);
                    _context.painter->drawEllipse(rc.center(), sz.width() / 2 - ox, sz.height() / 2 - oy);
                }
            } else {
                //Tickbox Style
                _context.painter->drawRoundedRect(rc, sz.width() / 10 , sz.height() / 10);

                if (chk->value()) {
                    QPen lp;
                    lp.setColor(chk->foregroundColor());
                    lp.setWidth(ox > oy ? oy : ox);
                    _context.painter->setPen(lp);
                    _context.painter->drawLine(QPointF(ox, sz.height() / 2) + ps, QPointF(sz.width() / 2, sz.height() - oy) + ps);
                    _context.painter->drawLine(QPointF(sz.width() / 2, sz.height() - oy) + ps, QPointF(sz.width() - ox, oy) + ps);
                }
            }

            _context.painter->restore();

        } else {
            kDebug() << "unrecognized primitive type";
        }
    }


    return true;
}

