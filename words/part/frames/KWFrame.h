/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2000-2006 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2005-2011 Sebastian Sauer <mail@dipe.org>
 * SPDX-FileCopyrightText: 2005-2006, 2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Pierre Ducroquet <pinaraf@pinaraf.info>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KWFRAME_H
#define KWFRAME_H

#include "Words.h"
#include "words_export.h"

#include <KoShape.h>
#include <KoShapeAnchor.h>
#include <KoShapeApplicationData.h>
#include <KoShapeSavingContext.h>

class KWFrameSet;

/**
 * This class represents a single frame.
 * A frame belongs to a frameset which states its contents.
 * A frame does NOT have contents, the frameset stores that.
 * A frame is really just a shape that is used to place the content
 * of a frameset.
 */
class WORDS_EXPORT KWFrame : public KoShapeApplicationData
{
public:
    /**
     * Constructor
     * @param shape the shape that displays the content, containing size/position
     * @param parent the parent frameset
     */
    KWFrame(KoShape *shape, KWFrameSet *parent);
    ~KWFrame() override;

    /**
     * Each frame will be rendered by a shape which also holds the position etc.
     * @return the shape that represents this frame.
     */
    KoShape *shape() const
    {
        return m_shape;
    }

    /**
     * Return the parent frameset.
     * @return the parent frameset
     */
    KWFrameSet *frameSetxx() const
    {
        return m_frameSet;
    }

    qreal anchoredFrameOffset() const
    {
        return m_anchoredFrameOffset;
    }
    void setAnchoredFrameOffset(qreal offset)
    {
        m_anchoredFrameOffset = offset;
    }

private:
    KoShape *m_shape;
    qreal m_anchoredFrameOffset;
    KWFrameSet *m_frameSet;
};

#endif
