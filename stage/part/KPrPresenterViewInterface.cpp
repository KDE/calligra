/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrPresenterViewInterface.h"

#include <QBoxLayout>
#include <QFrame>
#include <QKeyEvent>
#include <QLabel>
#include <QPainter>
#include <QTextEdit>
#include <QToolButton>

#include <KLocalizedString>

#include <KoPACanvas.h>
#include <KoPAPageBase.h>
#include <KoShape.h>
#include <KoTextShapeData.h>

#include "KPrEndOfSlideShowPage.h"
#include "KPrNotes.h"
#include "KPrPage.h"
#include "StageDebug.h"

KPrPresenterViewInterface::KPrPresenterViewInterface(const QList<KoPAPageBase *> &pages, KoPACanvas *canvas, QWidget *parent)
    : KPrPresenterViewBaseInterface(pages, parent)
    , m_canvas(canvas)
{
    QVBoxLayout *vLayout = new QVBoxLayout;
    QHBoxLayout *hLayout = new QHBoxLayout;

    QFrame *frame = new QFrame;
    QVBoxLayout *frameLayout = new QVBoxLayout;
    m_currentSlideLabel = new QLabel;
    frameLayout->addWidget(m_currentSlideLabel, 0, Qt::AlignHCenter);
    frameLayout->addWidget(m_canvas);
    frameLayout->addStretch();
    frame->setLayout(frameLayout);

    hLayout->addWidget(frame);
    hLayout->addStretch();

    frame = new QFrame;
    frameLayout = new QVBoxLayout;
    m_nextSlideLabel = new QLabel(i18n("Next Slide"));
    m_nextSlidePreview = new QLabel;
    frameLayout->addWidget(m_nextSlideLabel, 0, Qt::AlignHCenter);
    frameLayout->addWidget(m_nextSlidePreview);
    frameLayout->addStretch();
    frame->setLayout(frameLayout);
    hLayout->addWidget(frame);

    vLayout->addLayout(hLayout);

    vLayout->addWidget(new QLabel(i18n("Speaker Notes")));
    m_notesTextEdit = new QTextEdit;
    m_notesTextEdit->setReadOnly(true);
    vLayout->addWidget(m_notesTextEdit);

    setLayout(vLayout);
}

void KPrPresenterViewInterface::setActivePage(int pageIndex)
{
    KPrPresenterViewBaseInterface::setActivePage(pageIndex);

    Q_ASSERT(pageIndex >= 0 && pageIndex < m_pages.size());
    KoPAPageBase *page = m_pages.at(pageIndex);
    int pageCount = dynamic_cast<KPrEndOfSlideShowPage *>(m_pages.last()) ? m_pages.count() - 1 : m_pages.count();

    // set the thumbnail for next page preview
    KoPAPageBase *nextPage = nullptr;
    if (pageIndex != pageCount) {
        nextPage = m_pages.at(pageIndex + 1);
        m_nextSlidePreview->setPixmap(nextPage->thumbnail(m_previewSize));
    } else { // End of presentation, just a black pixmap for the next slide preview
        QPixmap pixmap(m_previewSize);
        pixmap.fill(Qt::black);
        m_nextSlidePreview->setPixmap(pixmap);
    }

    // update the label
    m_currentSlideLabel->setText(pageIndex != pageCount ? i18n("Current Slide %1 of %2", pageIndex + 1, pageCount) : i18n("End of Slide Show"));

    // set the presentation notes
    KPrPage *prPage = dynamic_cast<KPrPage *>(page);
    Q_ASSERT(prPage);
    KPrNotes *pageNotes = prPage->pageNotes();
    KoShape *textShape = pageNotes->textShape();
    KoTextShapeData *textShapeData = qobject_cast<KoTextShapeData *>(textShape->userData());
    Q_ASSERT(textShapeData);
    QTextDocument *document = textShapeData->document()->clone(m_notesTextEdit);
    m_notesTextEdit->setDocument(document);
}

void KPrPresenterViewInterface::setPreviewSize(const QSize &size)
{
    m_previewSize = size;
    m_canvas->setFixedSize(size);

    // set the thumbnail for next page preview
    Q_ASSERT(m_activePage != -1);
    KoPAPageBase *nextPage = nullptr;
    if (m_activePage != m_pages.count() - 1) {
        nextPage = m_pages.at(m_activePage + 1);
    } else {
        nextPage = m_pages.at(m_activePage);
    }
    m_nextSlidePreview->setPixmap(nextPage->thumbnail(m_previewSize));
}
