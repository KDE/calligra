/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOTEXTBLOCKPAINTSTRATEGYBASE_H
#define KOTEXTBLOCKPAINTSTRATEGYBASE_H

#include "kotext_export.h"

class QPainter;
class QBrush;

/**
 * This class is used to control aspects of textblock painting
 * Which is used when Stage animates text.
 */
class KOTEXT_EXPORT KoTextBlockPaintStrategyBase
{
public:
    KoTextBlockPaintStrategyBase();
    virtual ~KoTextBlockPaintStrategyBase();
    /// returns a background for the block, the default implementation returns the defaultBackground
    virtual QBrush background(const QBrush &defaultBackground) const;
    /// A strategy implementing this class can apply its settings by modifying the \a painter
    virtual void applyStrategy(QPainter *painter);
    /// Returns true if the block should be painted at all or false when it should be skipped
    virtual bool isVisible() const;
};

#endif
