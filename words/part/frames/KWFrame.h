/* This file is part of the KDE project
 * Copyright (C) 2000-2006 David Faure <faure@kde.org>
 * Copyright (C) 2005-2011 Sebastian Sauer <mail@dipe.org>
 * Copyright (C) 2005-2006, 2009 Thomas Zander <zander@kde.org>
 * Copyright (C) 2008 Pierre Ducroquet <pinaraf@pinaraf.info>
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

#ifndef KWFRAME_H
#define KWFRAME_H

#include "Words.h"
#include "words_export.h"

#include <KoShape.h>
#include <KoShapeSavingContext.h>
#include <KoShapeApplicationData.h>
#include <KoShapeAnchor.h>

class KWFrameSet;
class KoViewConverter;
class KWOutlineShape;
class KWPage;

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
    virtual ~KWFrame();

    /**
     * This property what should happen when the frame is full
     */
    Words::FrameBehavior frameBehavior() const {
        return m_frameBehavior;
    }
    /**
     * Set what should happen when the frame is full
     * @param fb the new FrameBehavior
     */
    void setFrameBehavior(Words::FrameBehavior fb) {
        m_frameBehavior = fb;
    }

    /**
     * For frame duplication policy on new page creation.
     */
    Words::NewFrameBehavior newFrameBehavior() const {
        return m_newFrameBehavior;
    }
    /**
     * For frame duplication policy on new page creation.
     * Altering this does not change the frames placed until a new page is created.
     * @param nf the NewFrameBehavior.
     */
    void setNewFrameBehavior(Words::NewFrameBehavior nf) {
        m_newFrameBehavior = nf;
    }

    /**
     * Set the minimum height of the frame.
     * @param minimumFrameHeight the minimum height of the frame.
     */
    void setMinimumFrameHeight(qreal minimumFrameHeight);
    /**
     * Return the minimum height of the frame.
     * @return the minimum height of the frame. Default is 0.0.
     */
    qreal minimumFrameHeight() const;

    /**
     * Each frame will be rendered by a shape which also holds the position etc.
     * @return the shape that represents this frame.
     */
    KoShape *shape() const {
        return m_shape;
    }

    /**
     * Return the parent frameset.
     * @return the parent frameset
     */
    KWFrameSet *frameSet() const {
        return m_frameSet;
    }
    /**
     * Set the frameset this frame will work on.
     * Altering the frameset requires you to remove this frame on the old and add the frame on the
     * new frameset
     * @param newFrameSet the new frameset
     */
    virtual void setFrameSet(KWFrameSet *newFrameSet);

    void cleanupShape(KoShape* shape);

    /*
    void clearLoadingData() {
        m_anchoredPageNumber = -1;
    }
    */

    int anchoredPageNumber() const {
        return m_shape->anchor() ? m_shape->anchor()->pageNumber() : -1;
    }
    qreal anchoredFrameOffset() const {
        return m_anchoredFrameOffset;
    }
    void setAnchoredFrameOffset(qreal offset) {
        m_anchoredFrameOffset = offset;
    }

    /**
     * Returns the list of copy-shapes, see @a KWCopyShape , that
     * are copies of this KWFrame.
     */
    QList<KWFrame*> copies() const;

    void addCopy(KWFrame* frame);
    void removeCopy(KWFrame* frame);

    /**
     * States if this frame is a copy of the previous one.
     * If this frame is a copy, then this frame is drawn with the same content as the
     * previous frame in this frameset.
     * @return true if this is a copy
     */
    bool isCopy() const;

    /**
     * Copy all the settings from the parameter frame and apply them to this frame.
     * @param frame the frame to use as original
     */
    void copySettings(const KWFrame *frame);

    /**
     * Save the frame as ODF
     * @param context the context for saving.
     */
    void saveOdf(KoShapeSavingContext &context, const KWPage &page, int pageZIndexOffset = 0) const;

private:
    KoShape *m_shape;
    Words::FrameBehavior m_frameBehavior;
    bool m_copyToEverySheet;
    Words::NewFrameBehavior m_newFrameBehavior;
    qreal m_anchoredFrameOffset;
    KWFrameSet *m_frameSet;
    qreal m_minimumFrameHeight;
    QList<KWFrame*> m_copyShapes;
};

#endif
