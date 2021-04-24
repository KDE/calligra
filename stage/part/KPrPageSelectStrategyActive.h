/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2010 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KPRPAGESELECTSTRATEGYACTIVE_H
#define KPRPAGESELECTSTRATEGYACTIVE_H

#include "KPrPageSelectStrategyBase.h"

class KoPACanvasBase;

/**
 * Get the active page for the view
 */
class KPrPageSelectStrategyActive : public KPrPageSelectStrategyBase
{
public:
    explicit KPrPageSelectStrategyActive(KoPACanvasBase *canvas);
    ~KPrPageSelectStrategyActive() override;

    const KoPAPageBase *page() const override;

private:
    KoPACanvasBase *m_canvas;
};

#endif /* KPRPAGESELECTSTRATEGYACTIVE_H */
