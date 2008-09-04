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

void KRScreenRender::setPainter(QPainter * pPainter)
{
    _painter = pPainter;
}

bool KRScreenRender::render(ORODocument * pDocument , int page)
{
    if (pDocument == 0)
        return false;

    qreal xDpi = KoGlobal::dpiX();
    qreal yDpi = KoGlobal::dpiY();

    OROPage * p = pDocument->page(page);

    // Render Page Objects
    for (int i = 0; i < p->primitives(); i++) {
        OROPrimitive * prim = p->primitive(i);

        if (prim->type() == OROTextBox::TextBox) {
            OROTextBox * tb = (OROTextBox*) prim;

            QPointF ps = tb->position();
            QSizeF sz = tb->size();
            QRectF rc = QRectF(ps.x(), ps.y(), sz.width(), sz.height());

            _painter->save();
            //Background

            QColor bg = tb->textStyle().bgColor;
            bg.setAlpha(tb->textStyle().bgOpacity);

            //_painter->setBackgroundMode(Qt::OpaqueMode);
            _painter->setBackground(bg);
            _painter->fillRect(rc, bg);

            //Text
            _painter->setBackgroundMode(Qt::TransparentMode);
            _painter->setFont(tb->textStyle().font);
            _painter->setPen(tb->textStyle().fgColor);
            _painter->drawText(rc, tb->flags(), tb->text());

            //outer line
            _painter->setPen(QPen(tb->lineStyle().lnColor, tb->lineStyle().weight, tb->lineStyle().style));
            _painter->drawRect(rc);

            //Reset back to defaults for next element
            _painter->restore();

        } else if (prim->type() == OROLine::Line) {
            OROLine * ln = (OROLine*) prim;
            QPointF s = ln->startPoint();
            QPointF e = ln->endPoint();
            //QPen pen ( _painter->pen() );
            QPen pen(ln->lineStyle().lnColor, ln->lineStyle().weight, ln->lineStyle().style);

            _painter->save();
            _painter->setRenderHint(QPainter::Antialiasing, true);
            _painter->setPen(pen);
            _painter->drawLine(QLineF(s.x(), s.y(), e.x(), e.y()));
            _painter->setRenderHint(QPainter::Antialiasing, false);
            _painter->restore();
        }

        else if (prim->type() == ORORect::Rect) {
            ORORect * re = (ORORect*) prim;

            QPointF ps = re->position();
            QSizeF sz = re->size();
            QRectF rc = QRectF(ps.x(), ps.y(), sz.width(), sz.height());

            _painter->save();
            _painter->setPen(re->pen());
            _painter->setBrush(re->brush());
            _painter->drawRect(rc);
            _painter->restore();
        } else if (prim->type() == OROEllipse::Ellipse) {
            OROEllipse * re = (OROEllipse*) prim;

            QPointF ps = re->position();
            QSizeF sz = re->size();
            QRectF rc = QRectF(ps.x(), ps.y(), sz.width(), sz.height());

            _painter->save();
            _painter->setPen(re->pen());
            _painter->setBrush(re->brush());
            _painter->drawEllipse(rc);
            _painter->restore();
        } else if (prim->type() == OROImage::Image) {
            OROImage * im = (OROImage*) prim;
            QPointF ps = im->position();
            QSizeF sz = im->size();
            QRectF rc = QRectF(ps.x(), ps.y(), sz.width(), sz.height());

            QImage img = im->image();
            if (im->scaled())
                img = img.scaled(rc.size().toSize(), (Qt::AspectRatioMode) im->aspectRatioMode(), (Qt::TransformationMode) im->transformationMode());

            QRectF sr = QRectF(QPointF(0.0, 0.0), rc.size().boundedTo(img.size()));
            _painter->drawImage(rc.topLeft(), img, sr);
        } else if (prim->type() == OROPicture::Picture) {
            OROPicture * im = (OROPicture*) prim;
            QPointF ps = im->position();
            QSizeF sz = im->size();
            QRectF rc = QRectF(ps.x(), ps.y(), sz.width(), sz.height());
            _painter->save();
            _painter->drawPicture(rc.topLeft(), *(im->picture()));
            _painter->restore();
        } else if (prim->type() == OROCheck::Check) {
            OROCheck * chk = (OROCheck*) prim;
            QPointF ps = chk->position();
            QSizeF sz = chk->size();
            QRectF rc = QRectF(ps.x(), ps.y(), sz.width(), sz.height());

            _painter->save();

            _painter->setBackgroundMode ( Qt::OpaqueMode );
            _painter->setRenderHint(QPainter::Antialiasing);
            QColor bg = chk->backgroundColor();
            //bg.setAlpha(chk->);

            _painter->setBackground(bg);
            _painter->setPen(chk->foregroundColor());

            if (chk->lineStyle().style == Qt::NoPen || chk->lineStyle().weight <= 0) {
                _painter->setPen(QPen(QColor(224, 224, 224)));
            } else {
                _painter->setPen(QPen(chk->lineStyle().lnColor, chk->lineStyle().weight, chk->lineStyle().style));
            }

            qreal ox = sz.width()/5;
            qreal oy = sz.height()/5;

            //Checkbox Style
            if (chk->checkType() == "Cross"){
                _painter->drawRoundedRect(rc, sz.width()/10 , sz.height()/10);

                if (chk->value()) {
                    QPen lp;
                    lp.setColor(chk->foregroundColor());
                    lp.setWidth(ox > oy ? oy : ox);
                    _painter->setPen(lp);
                    _painter->drawLine(QPointF(ox,oy) + ps, QPointF(sz.width() - ox, sz.height() - oy) + ps);
                    _painter->drawLine(QPointF(ox, sz.height() - oy) + ps, QPoint(sz.width() - ox, oy) + ps);
                }
            }
            else if (chk->checkType() == "Dot"){
            //Radio Style
                _painter->drawEllipse(rc);

                if (chk->value()) {
                    QBrush lb(chk->foregroundColor());
                    _painter->setBrush(lb);
                    _painter->setPen(Qt::NoPen);
                    _painter->drawEllipse(rc.center(), sz.width()/2 - ox, sz.height()/2 - oy);
                }
            }
            else {
            //Tickbox Style
                _painter->drawRoundedRect(rc, sz.width()/10 , sz.height()/10);

                if (chk->value()) {
                    QPen lp;
                    lp.setColor(chk->foregroundColor());
                    lp.setWidth(ox > oy ? oy : ox);
                    _painter->setPen(lp);
                    _painter->drawLine(QPointF(ox,sz.height()/2) + ps, QPointF(sz.width() / 2, sz.height() - oy) + ps);
                    _painter->drawLine(QPointF(sz.width() / 2, sz.height() - oy) + ps, QPointF(sz.width() - ox, oy) + ps);
                }
            }

            _painter->restore();

        } else {
            kDebug() << "unrecognized primitive type";
        }
    }


    return true;
}

