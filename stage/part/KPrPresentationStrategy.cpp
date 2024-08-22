/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Alexia Allanic <alexia_allanic@yahoo.fr>
 * SPDX-FileCopyrightText: 2009 Jérémy Lugagne <jejewindsurf@hotmail.com>
 * SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrPresentationStrategy.h"
#include <QtGlobal>

KPrPresentationStrategy::KPrPresentationStrategy(KPrPresentationTool *tool)
    : KPrPresentationStrategyBase(tool)
{
}

KPrPresentationStrategy::~KPrPresentationStrategy() = default;

bool KPrPresentationStrategy::keyPressEvent(QKeyEvent *event)
{
    Q_UNUSED(event);
    return false;
}
