/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrPresenterViewBaseInterface.h"

KPrPresenterViewBaseInterface::KPrPresenterViewBaseInterface(const QList<KoPAPageBase *> &pages, QWidget *parent)
    : QWidget(parent)
    , m_pages(pages)
{
}

void KPrPresenterViewBaseInterface::setActivePage(KoPAPageBase *page)
{
    setActivePage(m_pages.indexOf(page));
}

void KPrPresenterViewBaseInterface::setActivePage(int pageIndex)
{
    m_activePage = pageIndex;
}
