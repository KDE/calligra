/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KPRPRESENTATIONBLACKSTRATEGY_H
#define KPRPRESENTATIONBLACKSTRATEGY_H

#include "KPrPresentationStrategyBase.h"

class KPrPresentationBlackStrategy : public KPrPresentationStrategyBase
{
public:
    explicit KPrPresentationBlackStrategy(KPrPresentationTool *tool);
    ~KPrPresentationBlackStrategy() override;

    bool keyPressEvent(QKeyEvent *event) override;
};

#endif /* KPRPRESENTATIONBLACKSTRATEGY_H */
