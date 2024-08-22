/* This file is part of the Calligra libraries
   SPDX-FileCopyrightText: 2009 Boudewijn Rempt <boud@valdyas.org>

SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "KoFilterChainLinkList.h"
#include "KoFilterChainLink.h"

namespace CalligraFilter
{

ChainLinkList::ChainLinkList() = default;

ChainLinkList::~ChainLinkList()
{
    deleteAll();
}

void ChainLinkList::deleteAll()
{
    while (!m_chainLinks.isEmpty()) {
        delete m_chainLinks.takeFirst();
    }
}

int ChainLinkList::count() const
{
    return m_chainLinks.count();
}

ChainLink *ChainLinkList::current() const
{
    // use value() because m_current might be out of range for m_chainLinks
    return m_chainLinks.value(m_current);
}

ChainLink *ChainLinkList::first()
{
    m_current = 0;
    return current();
}

ChainLink *ChainLinkList::next()
{
    ++m_current;
    return current();
}

void ChainLinkList::prepend(ChainLink *link)
{
    Q_ASSERT(link);
    m_chainLinks.prepend(link);
    m_current = 0;
}

void ChainLinkList::append(ChainLink *link)
{
    Q_ASSERT(link);
    m_chainLinks.append(link);
    m_current = m_chainLinks.count() - 1;
}
}
