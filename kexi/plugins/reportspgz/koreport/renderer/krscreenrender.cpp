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
 * License along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "krscreenrender.h"
#include "renderobjects.h"
#include <KoPageFormat.h>
#include <kdebug.h>
#include <KoUnit.h>

KRScreenRender::KRScreenRender()
{
    m_painter = 0;
}

KRScreenRender::~KRScreenRender()
{
}

void KRScreenRender::setPainter(QPainter * pPainter)
{
    m_painter = pPainter;
}

bool KRScreenRender::render(ORODocument * pDocument , int page)
{
    if (pDocument == 0)
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

            m_painter->save();
            //Background

            QColor bg = tb->textStyle().backgroundColor;
            bg.setAlpha((tb->textStyle().backgroundOpacity / 100) * 255);

            m_painter->setBackground(bg);
            m_painter->fillRect(rc, bg);

            //Text
            m_painter->setBackgroundMode(Qt::TransparentMode);
            m_painter->setFont(tb->textStyle().font);
            m_painter->setPen(tb->textStyle().foregroundColor);
            m_painter->drawText(rc.adjusted(2, 2, 0, 0), tb->flags(), tb->text());

            //outer line
            m_painter->setPen(QPen(tb->lineStyle().lineColor, tb->lineStyle().weight, tb->lineStyle().style));
            m_painter->drawRect(rc);

            //Reset back to defaults for next element
            m_painter->restore();

        } else if (prim->type() == OROLine::Line) {
            OROLine * ln = (OROLine*) prim;
            QPointF s = ln->startPoint();
            QPointF e = ln->endPoint();
            //QPen pen ( _painter->pen() );
            QPen pen(ln->lineStyle().lineColor, ln->lineStyle().weight, ln->lineStyle().style);

            m_painter->save();
            m_painter->setRenderHint(QPainter::Antialiasing, true);
            m_painter->setPen(pen);
            m_painter->drawLine(QLineF(s.x(), s.y(), e.x(), e.y()));
            m_painter->setRenderHint(QPainter::Antialiasing, false);
            m_painter->restore();
        }

        else if (prim->type() == ORORect::Rect) {
            ORORect * re = (ORORect*) prim;

            QPointF ps = re->position();
            QSizeF sz = re->size();
            QRectF rc = QRectF(ps.x(), ps.y(), sz.width(), sz.height());

            m_painter->save();
            m_painter->setPen(re->pen());
            m_painter->setBrush(re->brush());
            m_painter->drawRect(rc);
            m_painter->restore();
        } else if (prim->type() == OROEllipse::Ellipse) {
            OROEllipse * re = (OROEllipse*) prim;

            QPointF ps = re->position();
            QSizeF sz = re->size();
            QRectF rc = QRectF(ps.x(), ps.y(), sz.width(), sz.height());

            m_painter->save();
            m_painter->setPen(re->pen());
            m_painter->setBrush(re->brush());
            m_painter->drawEllipse(rc);
            m_painter->restore();
        } else if (prim->type() == OROImage::Image) {
            OROImage * im = (OROImage*) prim;
            QPointF ps = im->position();
            QSizeF sz = im->size();
            QRectF rc = QRectF(ps.x(), ps.y(), sz.width(), sz.height());

            QImage img = im->image();
            if (im->scaled())
                img = img.scaled(rc.size().toSize(), (Qt::AspectRatioMode) im->aspectRatioMode(), (Qt::TransformationMode) im->transformationMode());

            QRectF sr = QRectF(QPointF(0.0, 0.0), rc.size().boundedTo(img.size()));
            m_painter->drawImage(rc.topLeft(), img, sr);
        } else if (prim->type() == OROPicture::Picture) {
            OROPicture * im = (OROPicture*) prim;
            QPointF ps = im->position();
            QSizeF sz = im->size();
            QRectF rc = QRectF(ps.x(), ps.y(), sz.width(), sz.height());
            m_painter->save();
            m_painter->drawPicture(rc.topLeft(), *(im->picture()));
            m_painter->restore();
        } else if (prim->type() == OROCheck::Check) {
            OROCheck * chk = (OROCheck*) prim;
            QPointF ps = chk->position();
            QSizeF sz = chk->size();
            QRectF rc = QRectF(ps.x(), ps.y(), sz.width(), sz.height());

            m_painter->save();

            m_painter->setBackgroundMode(Qt::OpaqueMode);
            m_painter->setRenderHint(QPainter::Antialiasing);

            m_painter->setPen(chk->foregroundColor());

            if (chk->lineStyle().style == Qt::NoPen || chk->lineStyle().weight <= 0) {
                m_painter->setPen(QPen(QColor(224, 224, 224)));
            } else {
                m_painter->setPen(QPen(chk->lineStyle().lineColor, chk->lineStyle().weight, chk->lineStyle().style));
            }

            qreal ox = sz.width() / 5;
            qreal oy = sz.height() / 5;

            //Checkbox Style
            if (chk->checkType() == "Cross") {
                m_painter->drawRoundedRect(rc, sz.width() / 10 , sz.height() / 10);

                if (chk->value()) {
                    QPen lp;
                    lp.setColor(chk->foregroundColor());
                    lp.setWidth(ox > oy ? oy : ox);
                    m_painter->setPen(lp);
                    m_painter->drawLine(QPointF(ox, oy) + ps, QPointF(sz.width() - ox, sz.height() - oy) + ps);
                    m_painter->drawLine(QPointF(ox, sz.height() - oy) + ps, QPoint(sz.width() - ox, oy) + ps);
                }
            } else if (chk->checkType() == "Dot") {
                //Radio Style
                m_painter->drawEllipse(rc);

                if (chk->value()) {
                    QBrush lb(chk->foregroundColor());
                    m_painter->setBrush(lb);
                    m_painter->setPen(Qt::NoPen);
                    m_painter->drawEllipse(rc.center(), sz.width() / 2 - ox, sz.height() / 2 - oy);
                }
            } else {
                //Tickbox Style
                m_painter->drawRoundedRect(rc, sz.width() / 10 , sz.height() / 10);

                if (chk->value()) {
                    QPen lp;
                    lp.setColor(chk->foregroundColor());
                    lp.setWidth(ox > oy ? oy : ox);
                    m_painter->setPen(lp);
                    m_painter->drawLine(QPointF(ox, sz.height() / 2) + ps, QPointF(sz.width() / 2, sz.height() - oy) + ps);
                    m_painter->drawLine(QPointF(sz.width() / 2, sz.height() - oy) + ps, QPointF(sz.width() - ox, oy) + ps);
                }
            }

            m_painter->restore();

        } else {
            kDebug() << "unrecognized primitive type";
        }
    }


    return true;
}

