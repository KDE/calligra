/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRPAGESELECTSTRATEGYFIXED_H
#define KPRPAGESELECTSTRATEGYFIXED_H

#include "KPrPageSelectStrategyBase.h"

/**
 * Get a fixed page
 */
class KPrPageSelectStrategyFixed : public KPrPageSelectStrategyBase
{
public:
    explicit KPrPageSelectStrategyFixed(const KoPAPageBase *page);
    ~KPrPageSelectStrategyFixed() override;

    const KoPAPageBase *page() const override;

private:
    const KoPAPageBase *m_page;
};

#endif /* KPRPAGESELECTSTRATEGYFIXED_H */
