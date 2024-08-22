/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009-2010 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoPATextPage.h"

KoPATextPage::KoPATextPage(int pageNumber, KoPAPageBase *page)
    : m_pageNumber(pageNumber)
    , m_page(page)
{
}

KoPATextPage::~KoPATextPage() = default;

int KoPATextPage::pageNumber() const
{
    return m_pageNumber;
}

int KoPATextPage::visiblePageNumber(PageSelection select, int adjustment) const
{
    Q_UNUSED(select);
    return m_pageNumber + adjustment;
}

KoPAPageBase *KoPATextPage::page() const
{
    return m_page;
}

QRectF KoPATextPage::rect() const
{
    return QRectF();
}
