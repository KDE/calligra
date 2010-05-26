/* This file is part of the KOffice project
 * Copyright (C) 2008 Thomas Zander <zander@kde.org>
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
#include "KWPageTextInfo.h"
#include "KWPageManager_p.h"

KWPageTextInfo::KWPageTextInfo(const KWPage &page)
    : m_page(page)
{
}

int KWPageTextInfo::pageNumber(PageSelection select, int adjustment) const
{
    KWPage page = m_page;
    switch (select) {
    case KoTextPage::CurrentPage: break;
    case KoTextPage::PreviousPage:
        page = page.previous();
        break;
    case KoTextPage::NextPage:
        page = page.next();
        break;
    }

    if (! page.isValid())
        return -1;

    if (adjustment != 0) {
        const int wantedPageNumber = page.pageNumber() + adjustment;
        Q_ASSERT(page.priv); // it would have been invalid above otherwise
        if (! page.priv->pageNumbers.contains(wantedPageNumber))
            return -1; // doesn't exist.
        return wantedPageNumber;
    }

    return page.pageNumber();
}

KWPage KWPageTextInfo::page() const
{
    return m_page;
}

