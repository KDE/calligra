/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2009 Alexia Allanic <alexia_allanic@yahoo.fr>
 * SPDX-FileCopyrightText: 2009 Jérémy Lugagne <jejewindsurf@hotmail.com>
 * SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KPRPRESENTATIONDRAWSTRATEGY_H
#define KPRPRESENTATIONDRAWSTRATEGY_H

#include "KPrPresentationStrategyBase.h"

class KPrPresentationDrawStrategy : public KPrPresentationStrategyBase
{
public:
    explicit KPrPresentationDrawStrategy(KPrPresentationTool *tool);
    ~KPrPresentationDrawStrategy() override;

    bool keyPressEvent(QKeyEvent *event) override;
};

#endif /* KPRPRESENTATIONDRAWSTRATEGY_H */
