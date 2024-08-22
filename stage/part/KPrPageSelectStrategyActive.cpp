/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrPageSelectStrategyActive.h"

#include <KoPACanvasBase.h>
#include <KoPAViewBase.h>

KPrPageSelectStrategyActive::KPrPageSelectStrategyActive(KoPACanvasBase *canvas)
    : m_canvas(canvas)
{
}

KPrPageSelectStrategyActive::~KPrPageSelectStrategyActive() = default;

const KoPAPageBase *KPrPageSelectStrategyActive::page() const
{
    KoPAViewBase *view = m_canvas->koPAView();
    Q_ASSERT(view);
    return view->activePage();
}
