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

//#include <kdebug.h>

KWPageSettingsDialog::KWPageSettingsDialog(QWidget *parent, KWPage *page)
    : KDialog(parent),
    m_page(page)
{
    Q_ASSERT(page);
    m_layout = m_page->pageLayout();
    QWidget *widget = new QWidget(this);
    setMainWidget(widget);
    QHBoxLayout *lay = new QHBoxLayout(widget);
    lay->setMargin(0);
    widget->setLayout(lay);

    KWPageLayout *pageLayout = new KWPageLayout(widget, m_layout);
    pageLayout->showUnitchooser(false);
    lay->addWidget(pageLayout);

    KWPagePreview *prev = new KWPagePreview(widget);
    prev->setPageLayout(m_layout);
    lay->addWidget(prev);

    connect (widget, SIGNAL(layoutChanged(const KoPageLayout&)), this, SLOT(setPageLayout(const KoPageLayout&)));
    connect (pageLayout, SIGNAL(layoutChanged(const KoPageLayout&)), prev, SLOT(setPageLayout(const KoPageLayout&)));
}

void KWPageSettingsDialog::setPageLayout(const KoPageLayout &layout) {
    m_layout = layout;
}

void KWPageSettingsDialog::accept() {
    double w = m_layout.width;
    double h = m_layout.height;
    if(m_layout.orientation == KoPageFormat::Landscape)
        qSwap(w, h);
    m_page->setWidth(w);
    m_page->setHeight(h);
    m_page->setTopMargin(m_layout.top);
    m_page->setBottomMargin(m_layout.bottom);
    m_page->setPageEdgeMargin(m_layout.pageEdge);
    m_page->setMarginClosestBinding(m_layout.bindingSide);
    m_page->setLeftMargin(m_layout.left);
    m_page->setRightMargin(m_layout.right);

/*
    if(m_layout.left < 0)
        m_page->setPageSide(PageSpread);
    else
        m_page
*/

    QDialog::accept();
    deleteLater();
}

void KWPageSettingsDialog::reject() {
    QDialog::reject();
    deleteLater();
}

#include <KWPageSettingsDialog.moc>
