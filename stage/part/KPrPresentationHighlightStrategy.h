/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Alexia Allanic <alexia_allanic@yahoo.fr>
 * SPDX-FileCopyrightText: 2009 Jérémy Lugagne <jejewindsurf@hotmail.com>
 * SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KPRPRESENTATIONHIGHLIGHTSTRATEGY_H
#define KPRPRESENTATIONHIGHLIGHTSTRATEGY_H

#include "KPrPresentationStrategyBase.h"

class KPrPresentationHighlightStrategy : public KPrPresentationStrategyBase
{
public:
    explicit KPrPresentationHighlightStrategy(KPrPresentationTool *tool);
    ~KPrPresentationHighlightStrategy() override;

    bool keyPressEvent(QKeyEvent *event) override;
};

#endif /* KPRPRESENTATIONHIGHLIGHTSTRATEGY_H */
