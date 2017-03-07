/* This file is part of the KDE project
   Copyright (C) 2010 Benjamin Port <port.benjamin@gmail.com>
   Copyright (C) 2009 Thorsten Zachmann <zachmann@kde.org>

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
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "KPrPdfPrintJob.h"

#include "KPrView.h"
#include "KoPAPageBase.h"
#include "KoPAUtil.h"
#include "KoPAPageProvider.h"
#include <KoPageLayout.h>
#include <KoZoomHandler.h>
#include <QPainter>
KPrPdfPrintJob::KPrPdfPrintJob(KPrView *view)
: KoPAPrintJob(view)
{
}

KPrPdfPrintJob::~KPrPdfPrintJob()
{
}

void KPrPdfPrintJob::startPrinting(RemovePolicy removePolicy)
{
    int fromPage = m_printer.fromPage() > 0 ? m_printer.fromPage() - 1: 0;
    int toPage = m_printer.toPage() > 0 ? m_printer.toPage() - 1: m_pages.size() - 1;

    Q_ASSERT( fromPage >= 0 && fromPage < m_pages.size() );
    Q_ASSERT( toPage >= 0 && toPage < m_pages.size() );

    KoZoomHandler zoomHandler;
    zoomHandler.setResolution( m_printer.resolution(), m_printer.resolution() );
    m_printer.setFullPage(true);
    const KoPageLayout & firstLayout = m_pages.at(fromPage)->pageLayout();
    m_printer.setPaperSize(QSizeF(firstLayout.width,firstLayout.height),QPrinter::Millimeter);
    QPainter painter( &m_printer );

    for ( int i = fromPage; i <= toPage; ++i ) {

        KoPAPageBase *page = m_pages.at(i);
        const KoPageLayout & layout = page->pageLayout();
        m_printer.setPaperSize(QSizeF(layout.width,layout.height),QPrinter::Millimeter);
        QSize size = m_printer.pageRect().size();
        painter.save();
        if (i != fromPage) {
            m_printer.newPage();
        }
        KoPAUtil::setZoom(layout, size, zoomHandler);
        QRect pageRect(KoPAUtil::pageRect( layout, size, zoomHandler));
        painter.setClipRect( pageRect );
        painter.setRenderHint( QPainter::Antialiasing );
        painter.translate( pageRect.topLeft() );
        m_pageProvider->setPageData(i + 1, page);
        page->paintPage( painter, zoomHandler );
        painter.restore();
    }

    if ( removePolicy == DeleteWhenDone ) {
        deleteLater();
    }
}
