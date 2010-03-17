/*
 * KPlato Report Plugin
 * Copyright (C) 2007-2008 by Adam Pigg (adam@piggz.co.uk)
 * Copyright (C) 2010 by Dag Andersen <danders@get2net.dk>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

#include "reportpage.h"

#include <renderobjects.h>
//#include <parsexmlutils.h>

#include <kdebug.h>

#include <KoPageFormat.h>
#include <KoUnit.h>
#include <KoGlobal.h>

#include <QPainter>
#include <qwidget.h>
#include <qcolor.h>
#include <qpixmap.h>

namespace KPlato
{

ReportPage::ReportPage(QWidget *parent, ORODocument *r)
    : QWidget(parent)
{
    setAttribute(Qt::WA_NoBackground);
    m_reportDocument = r;

    QString pageSize = r->pageOptions().getPageSize();
    int pageWidth = 0;
    int pageHeight = 0;
    if (pageSize == "Custom") {
        // if this is custom sized sheet of paper we will just use those values
        pageWidth = (int)(r->pageOptions().getCustomWidth());
        pageHeight = (int)(r->pageOptions().getCustomHeight());
    } else {
        // lookup the correct size information for the specified size paper
        pageWidth = r->pageOptions().widthPx();
        pageHeight = r->pageOptions().heightPx();
    }


    setFixedSize(pageWidth, pageHeight);

    m_pixmap = new QPixmap(pageWidth, pageHeight);
    setAutoFillBackground(true);

    m_renderer = m_factory.createInstance("screen");

    renderPage(1);
}

ReportPage::~ReportPage()
{
    delete m_renderer;
    m_renderer = 0;
}

void ReportPage::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.drawPixmap(QPoint(0, 0), *m_pixmap);
}

void ReportPage::renderPage(int p)
{
    m_pixmap->fill();
    QPainter qp(m_pixmap);
    if (m_reportDocument) {
        KoReportRendererContext cxt;
        cxt.painter = &qp;
        m_renderer->render(cxt, m_reportDocument, p - 1);
    }
    repaint();
}

} //namespace KPlato

#include "reportpage.moc"

