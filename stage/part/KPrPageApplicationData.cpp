/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
   SPDX-FileCopyrightText: 2020 Dag Andersen <danders@get2net.dk>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrPageApplicationData.h"
#include "pageeffects/KPrPageEffect.h"

KPrPageApplicationData::KPrPageApplicationData()
    : m_pageEffect(nullptr)
{
}

KPrPageApplicationData::~KPrPageApplicationData()
{
    delete m_pageEffect;
}

KPrPageEffect *KPrPageApplicationData::pageEffect()
{
    return m_pageEffect;
}

void KPrPageApplicationData::setPageEffect(KPrPageEffect *effect)
{
    m_pageEffect = effect;
}

KPrPageTransition &KPrPageApplicationData::pageTransition()
{
    return m_pageTransition;
}

void KPrPageApplicationData::setPageTransition(const KPrPageTransition &pageTransition)
{
    m_pageTransition = pageTransition;
}
