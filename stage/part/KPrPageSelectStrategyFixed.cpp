/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrPageSelectStrategyFixed.h"

KPrPageSelectStrategyFixed::KPrPageSelectStrategyFixed(const KoPAPageBase *page)
    : m_page(page)
{
}

KPrPageSelectStrategyFixed::~KPrPageSelectStrategyFixed() = default;

const KoPAPageBase *KPrPageSelectStrategyFixed::page() const
{
    return m_page;
}
