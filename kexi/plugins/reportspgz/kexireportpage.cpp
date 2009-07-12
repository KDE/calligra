/*
 * Kexi Report Plugin
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
 */

#include "kexireportpage.h"
#include <qwidget.h>
#include <kdebug.h>
#include <qcolor.h>
#include <qpixmap.h>
#include <KoPageFormat.h>
#include <KoUnit.h>
#include <KoGlobal.h>

#include <parsexmlutils.h>
#include <renderobjects.h>
#include <QPainter>
#include <krscreenrender.h>

KexiReportPage::KexiReportPage(QWidget *parent, ORODocument *r)
        : QWidget(parent)
{
    setAttribute(Qt::WA_NoBackground);
    kDebug() << "CREATED PAGE";
    m_reportDocument = r;
    m_page = 1;

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

    kDebug() << "PAGE IS " << pageWidth << "x" << pageHeight;
    m_repaint = true;
    m_pixmap = new QPixmap(pageWidth, pageHeight);
    setAutoFillBackground(true);
    renderPage(1);
}

void KexiReportPage::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.drawPixmap(QPoint(0, 0), *m_pixmap);
}

void KexiReportPage::renderPage(int p)
{
    kDebug() << "KexiReportPage::renderPage " << p;
    m_page = p;
    m_pixmap->fill();
    QPainter qp(m_pixmap);
    KRScreenRender sr;
    sr.setPainter(&qp);
    sr.render(m_reportDocument, p - 1);
    m_repaint = true;
    repaint();
}

KexiReportPage::~KexiReportPage()
{
}


#include "kexireportpage.moc"

