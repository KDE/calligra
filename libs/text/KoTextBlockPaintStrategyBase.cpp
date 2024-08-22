/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoTextBlockPaintStrategyBase.h"

#include <QBrush>

KoTextBlockPaintStrategyBase::KoTextBlockPaintStrategyBase() = default;

KoTextBlockPaintStrategyBase::~KoTextBlockPaintStrategyBase() = default;

QBrush KoTextBlockPaintStrategyBase::background(const QBrush &defaultBackground) const
{
    return defaultBackground;
}

void KoTextBlockPaintStrategyBase::applyStrategy(QPainter *)
{
}

bool KoTextBlockPaintStrategyBase::isVisible() const
{
    return true;
}
