/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009-2010 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoPAPageProvider.h"

#include "KoPATextPage.h"

KoPAPageProvider::KoPAPageProvider()
    : m_pageNumber(0)
    , m_page(nullptr)
{
}

KoPAPageProvider::~KoPAPageProvider() = default;

KoTextPage *KoPAPageProvider::page(KoShape *shape)
{
    Q_UNUSED(shape);
    return new KoPATextPage(m_pageNumber, m_page);
}

void KoPAPageProvider::setPageData(int pageNumber, KoPAPageBase *page)
{
    m_pageNumber = pageNumber;
    m_page = page;
}
