/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoPAPrintJob.h"

#include "KoPADocument.h"
#include "KoPAPageBase.h"
#include "KoPAPageProvider.h"
#include "KoPAUtil.h"
#include "KoPAView.h"
#include <KoText.h>
#include <KoZoomHandler.h>

#include <QPainter>

KoPAPrintJob::KoPAPrintJob(KoPAView *view)
    : KoPrintJob(view)
    , m_pages(view->kopaDocument()->pages())
{
    QVariant var = view->kopaDocument()->resourceManager()->resource(KoText::PageProvider);
    m_pageProvider = static_cast<KoPAPageProvider *>(var.value<void *>());
    // TODO this feels wrong
    printer().setFromTo(1, m_pages.size());
}

KoPAPrintJob::~KoPAPrintJob() = default;

QPrinter &KoPAPrintJob::printer()
{
    return m_printer;
}

QList<QWidget *> KoPAPrintJob::createOptionWidgets() const
{
    return QList<QWidget *>();
}

// TODO create a lot of fancy options for printing
// e.g. print also notes
// For now we print to the center of the page honoring the margins
// The page is zoomed to be as big as possible
void KoPAPrintJob::startPrinting(RemovePolicy removePolicy)
{
    int fromPage = m_printer.fromPage() ? m_printer.fromPage() - 1 : 0;
    int toPage = m_printer.toPage() ? m_printer.toPage() - 1 : m_pages.size() - 1;

    Q_ASSERT(fromPage >= 0 && fromPage < m_pages.size());
    Q_ASSERT(toPage >= 0 && toPage < m_pages.size());

    KoZoomHandler zoomHandler;
    zoomHandler.setResolution(m_printer.resolution(), m_printer.resolution());

    QSizeF size = m_printer.paperRect(QPrinter::Millimeter).size();

    QPainter painter(&m_printer);
    for (int i = fromPage; i <= toPage; ++i) {
        painter.save();
        if (i != fromPage) {
            m_printer.newPage();
        }

        KoPAPageBase *page = m_pages.at(i);
        const KoPageLayout &layout = page->pageLayout();
        KoPAUtil::setZoom(layout, size, zoomHandler);
        QRect pageRect(KoPAUtil::pageRect(layout, size, zoomHandler));

        painter.setClipRect(pageRect);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.translate(pageRect.topLeft());
        m_pageProvider->setPageData(i + 1, page);
        page->paintPage(painter, zoomHandler);
        painter.restore();
    }

    if (removePolicy == DeleteWhenDone) {
        deleteLater();
    }
}
