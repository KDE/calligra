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

#include "orprintrender.h"
#include "renderobjects.h"
#include <KoPageFormat.h>
#include <kdebug.h>

ORPrintRender::ORPrintRender()
{
    m_printer = 0;
    m_painter = 0;
}

ORPrintRender::~ORPrintRender()
{
}

void ORPrintRender::setPrinter(QPrinter * pPrinter)
{
    m_printer = pPrinter;
}

void ORPrintRender::setPainter(QPainter * pPainter)
{
    m_painter = pPainter;
}

bool ORPrintRender::setupPrinter(ORODocument * pDocument, QPrinter * pPrinter)
{
    if (pDocument == 0 || pPrinter == 0)
        return false;

    pPrinter->setCreator("OpenRPT Print Renderer");
    pPrinter->setDocName(pDocument->title());
    pPrinter->setFullPage(true);
    pPrinter->setOrientation((pDocument->pageOptions().isPortrait() ? QPrinter::Portrait : QPrinter::Landscape));
    pPrinter->setPageOrder(QPrinter::FirstPageFirst);

    if (pDocument->pageOptions().getPageSize().isEmpty())
        pPrinter->setPageSize(QPrinter::Custom);
    else
        pPrinter->setPageSize(KoPageFormat::printerPageSize(KoPageFormat::formatFromString(pDocument->pageOptions().getPageSize())));

    return true;
}

bool ORPrintRender::render(ORODocument * pDocument)
{
    if (pDocument == 0 || m_printer == 0)
        return false;

    m_printer->setFullPage(true);

    bool deleteWhenComplete = false;
    bool endWhenComplete = false;

    QPainter localPainter;
    if (m_painter == 0) {
        deleteWhenComplete = true;
        m_painter = &localPainter;
    }

    if (!m_painter->isActive()) {
        endWhenComplete = true;
        if (!m_painter->begin(m_printer))
            return false;
    }

    int fromPage = m_printer->fromPage();
    if (fromPage > 0)
        fromPage -= 1;
    int toPage = m_printer->toPage();
    if (toPage == 0 || toPage > pDocument->pages())
        toPage = pDocument->pages();
    for (int copy = 0; copy < m_printer->numCopies(); copy++) {
        for (int page = fromPage; page < toPage; page++) {
            if (page > 0)
                m_printer->newPage();

            OROPage * p = pDocument->page(page);
            if (m_printer->pageOrder() == QPrinter::LastPageFirst)
                p = pDocument->page(toPage - 1 - page);


            // Render Page Objects
            for (int i = 0; i < p->primitives(); i++) {
                OROPrimitive * prim = p->primitive(i);
                kDebug() << "Rendering object" << i << "type" << prim->type();
                if (prim->type() == OROTextBox::TextBox) {
                    kDebug() << "Text Box";
                    OROTextBox * tb = (OROTextBox*) prim;

                    QPointF ps = tb->position();
                    QSizeF sz = tb->size();
                    QRectF rc = QRectF(ps.x(), ps.y(), sz.width(), sz.height());

                    m_painter->save();
                    //Background

                    QColor bg = tb->textStyle().bgColor;
                    bg.setAlpha(tb->textStyle().bgOpacity);

                    //_painter->setBackgroundMode(Qt::OpaqueMode);
                    m_painter->setBackground(bg);
                    m_painter->fillRect(rc, bg);

                    //Text
                    m_painter->setBackgroundMode(Qt::TransparentMode);
                    m_painter->setFont(tb->textStyle().font);
                    m_painter->setPen(tb->textStyle().fgColor);
                    m_painter->drawText(rc, tb->flags(), tb->text());

                    //outer line
                    m_painter->setPen(QPen(tb->lineStyle().lnColor, tb->lineStyle().weight, tb->lineStyle().style));
                    m_painter->drawRect(rc);

                    //Reset back to defaults for next element
                    m_painter->restore();

                } else if (prim->type() == OROLine::Line) {
                    kDebug() << "Line";
                    OROLine * ln = (OROLine*) prim;
                    QPointF s = ln->startPoint();
                    QPointF e = ln->endPoint();
                    //QPen pen ( _painter->pen() );
                    QPen pen(ln->lineStyle().lnColor, ln->lineStyle().weight, ln->lineStyle().style);

                    m_painter->save();
                    m_painter->setRenderHint(QPainter::Antialiasing, true);
                    m_painter->setPen(pen);
                    m_painter->drawLine(QLineF(s.x(), s.y(), e.x(), e.y()));
                    m_painter->setRenderHint(QPainter::Antialiasing, false);
                    m_painter->restore();
                } else if (prim->type() == OROImage::Image) {
                    kDebug() << "Image";
                    OROImage * im = (OROImage*) prim;
                    QPointF ps = im->position();
                    QSizeF sz = im->size();
                    QRectF rc = QRectF(ps.x(), ps.y(), sz.width(), sz.height());

                    QImage img = im->image();
                    if (im->scaled())
                        img = img.scaled(rc.size().toSize(), (Qt::AspectRatioMode) im->aspectRatioMode(), (Qt::TransformationMode) im->transformationMode());

                    QRectF sr = QRectF(QPointF(0.0, 0.0), rc.size().boundedTo(img.size()));
                    m_painter->drawImage(rc.topLeft(), img, sr);
                } else if (prim->type() == ORORect::Rect) {
                    kDebug() << "Rect";
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
                } else if (prim->type() == OROPicture::Picture) {
                    OROPicture * im = (OROPicture*) prim;
                    QPointF ps = im->position();
                    QSizeF sz = im->size();
                    QRectF rc = QRectF(ps.x(), ps.y(), sz.width(), sz.height());
                    m_painter->drawPicture(rc.topLeft(), *(im->picture()));
                } else if (prim->type() == OROCheck::Check) {
                    OROCheck * chk = (OROCheck*) prim;
                    QPointF ps = chk->position();
                    QSizeF sz = chk->size();
                    QRectF rc = QRectF(ps.x(), ps.y(), sz.width(), sz.height());

                    m_painter->save();

                    m_painter->setBackgroundMode ( Qt::OpaqueMode );
                    m_painter->setRenderHint(QPainter::Antialiasing);

                    m_painter->setPen(chk->foregroundColor());

                    if (chk->lineStyle().style == Qt::NoPen || chk->lineStyle().weight <= 0) {
                        m_painter->setPen(QPen(QColor(224, 224, 224)));
                    } else {
                        m_painter->setPen(QPen(chk->lineStyle().lnColor, chk->lineStyle().weight, chk->lineStyle().style));
                    }

                    qreal ox = sz.width()/5;
                    qreal oy = sz.height()/5;

                    //Checkbox Style
                    if (chk->checkType() == "Cross"){
                        m_painter->drawRoundedRect(rc, sz.width()/10 , sz.height()/10);

                        if (chk->value()) {
                            QPen lp;
                            lp.setColor(chk->foregroundColor());
                            lp.setWidth(ox > oy ? oy : ox);
                            m_painter->setPen(lp);
                            m_painter->drawLine(QPointF(ox,oy) + ps, QPointF(sz.width() - ox, sz.height() - oy) + ps);
                            m_painter->drawLine(QPointF(ox, sz.height() - oy) + ps, QPoint(sz.width() - ox, oy) + ps);
                        }
                    }
                    else if (chk->checkType() == "Dot"){
                    //Radio Style
                        m_painter->drawEllipse(rc);

                        if (chk->value()) {
                            QBrush lb(chk->foregroundColor());
                            m_painter->setBrush(lb);
                            m_painter->setPen(Qt::NoPen);
                            m_painter->drawEllipse(rc.center(), sz.width()/2 - ox, sz.height()/2 - oy);
                        }
                    }
                    else {
                    //Tickbox Style
                        m_painter->drawRoundedRect(rc, sz.width()/10 , sz.height()/10);

                        if (chk->value()) {
                            QPen lp;
                            lp.setColor(chk->foregroundColor());
                            lp.setWidth(ox > oy ? oy : ox);
                            m_painter->setPen(lp);
                            m_painter->drawLine(QPointF(ox,sz.height()/2) + ps, QPointF(sz.width() / 2, sz.height() - oy) + ps);
                            m_painter->drawLine(QPointF(sz.width() / 2, sz.height() - oy) + ps, QPointF(sz.width() - ox, oy) + ps);
                        }
                    }

                    m_painter->restore();

                } else {
                    qDebug("unrecognized primitive type");
                }
            }
        }
    }

    if (endWhenComplete)
        m_painter->end();

    if (deleteWhenComplete)
        m_painter = 0;

    return true;
}



#include <math.h>
#include <QFontDatabase>

