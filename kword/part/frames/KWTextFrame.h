/* This file is part of the KDE project
 * Copyright (C) 2006, 2009 Thomas Zander <zander@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef KWTEXTFRAME_H
#define KWTEXTFRAME_H

#include "KWFrame.h"
#include "kword_export.h"

#include <KoShape.h>
#include <KoViewConverter.h>

class KWTextFrameSet;

/**
 * A single frame capable of showing text from a KWTextFrameSet
 */
class KWORD_EXPORT KWTextFrame : public KWFrame
{
public:
    /**
     * Constructor
     * @param shape a ready made shape that is capable of showing text
     *      This shape should be created using a KoShapeFactoryBase from the textShape plugin
     * @param parent the frameSet this frame will be associated with.
     */
    KWTextFrame(KoShape *shape, KWTextFrameSet *parent, int pageNumber = -1);
    ~KWTextFrame();

    /**
     * Set the sorting Id that influences the sorting order of the text frames.
     * Text frames have an order in which they are used to flow the text in,
     * this typically is from top/left to bottom/right in the document in contrary
     * to order of insertion.
     * Since KWord2 the sorting Id is used to allow frames to have an explicit position
     * in the list of frames and thus the ordering of the text.
     * @return the id of sorting, where -1 means sorting based on position, and heigher
     *      means later in the sequence.
     */
    int sortingId() const {
        return m_sortingId;
    }
    /**
     * Set the sorting id of the frame in the frameset.
     * @param id of sorting, where -1 means sorting based on position, and heigher
     *      means later in the sequence.
     * @see sortingId()
     */
    void setSortingId(int id) {
        m_sortingId = id;
    }

    /// A layout property. Return if the frame can autogrow. @see allowToGrow
    bool canAutoGrow();
    /// A layout property. Marks the frame able to get bigger when there is more text to fit.
    void allowToGrow();
    /// A layout option.  Asks the frame to shrink to a maximum of requestedHeight because there is no more text.
    void autoShrink(qreal requestedHeight);

    void setMinimumFrameHeight(qreal minimumFrameHeight) { m_minimumFrameHeight = minimumFrameHeight; }
    qreal minimumFrameHeight() const { return m_minimumFrameHeight; }

private:
    /* TODO
    bool m_drawFootNoteLine; */

    int m_sortingId;
    qreal m_lastHeight, m_minimumFrameHeight;
    bool m_canGrow;
};

#endif
