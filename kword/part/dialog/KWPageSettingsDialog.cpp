/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
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

#include "KWPageSettingsDialog.h"
#include "KWPageLayout.h"
#include "KWPagePreview.h"
#include "KWDocument.h"
#include "commands/KWPagePropertiesCommand.h"

//#include <KDebug>

KWPageSettingsDialog::KWPageSettingsDialog(QWidget *parent, KWDocument *document, KWPage *page)
    : KDialog(parent),
    m_document(document),
    m_page(page)
{
    Q_ASSERT(document);
    Q_ASSERT(page);
    m_layout = m_page->pageLayout();
    QWidget *widget = new QWidget(this);
    setMainWidget(widget);
    QHBoxLayout *lay = new QHBoxLayout(widget);
    lay->setMargin(0);
    widget->setLayout(lay);

    m_pageLayoutWidget = new KWPageLayout(widget, m_layout);
    m_pageLayoutWidget->showUnitchooser(false);
    m_pageLayoutWidget->forSinglePage(true);
    m_pageLayoutWidget->setStartPageNumber(m_document->startPage());
    m_pageLayoutWidget->layout()->setMargin(0);
    lay->addWidget(m_pageLayoutWidget);

    KWPagePreview *prev = new KWPagePreview(widget);
    prev->setPageLayout(m_layout);
    lay->addWidget(prev);

    connect (m_pageLayoutWidget, SIGNAL(layoutChanged(const KoPageLayout&)),
            prev, SLOT(setPageLayout(const KoPageLayout&)));
    connect (m_pageLayoutWidget, SIGNAL(layoutChanged(const KoPageLayout&)),
            this, SLOT(setPageLayout(const KoPageLayout&)));
}

void KWPageSettingsDialog::setPageLayout(const KoPageLayout &layout) {
    m_layout = layout;
}

void KWPageSettingsDialog::accept() {
    if(m_layout.orientation == KoPageFormat::Landscape)
        qSwap(m_layout.width, m_layout.height);

    if(m_pageLayoutWidget->marginsForDocument()) {
        // TODO
    }
    else {
        KWPagePropertiesCommand *cmd = new KWPagePropertiesCommand(m_document, m_page, m_layout);
        m_document->addCommand(cmd);
    }

    m_document->setStartPage(m_pageLayoutWidget->startPageNumber());

    QDialog::accept();
    deleteLater();
}

void KWPageSettingsDialog::reject() {
    QDialog::reject();
    deleteLater();
}

#include <KWPageSettingsDialog.moc>
