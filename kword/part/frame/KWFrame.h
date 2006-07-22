/* This file is part of the KDE project
 * Copyright (C) 2006 Thomas Zander <zander@kde.org>
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

#include "KWord.h"
#include "kword_export.h"

#include <KoShape.h>

class KWFrameSet;
class KoViewConverter;

/**
 * This class represents a single frame.
 * A frame belongs to a frameset which states its contents.
 * A frame does NOT have contents, the frameset stores that.
 * A frame is really just a shape that is used to place the content
 * of a frameset.
 */
class KWORD_TEST_EXPORT KWFrame {
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
    KWord::FrameBehavior frameBehavior() const { return m_frameBehavior; }
    /**
     * Set what should happen when the frame is full
     * @param fb the new FrameBehavior
     */
    void setFrameBehavior(KWord::FrameBehavior fb) { m_frameBehavior = fb; }

    /**
     * For frame duplication policy on new page creation.
     * Determines if this frame will be copied on even or odd pages only.
     */
    bool frameOnBothSheets() const { return m_copyToEverySheet; }
    /**
     * Determines if this frame will be copied on even or odd pages only.
     * Altering this does not change the frames placed until a new page is created.
     * @param both if true this frame will be copied to every page, if false only every other page
     */
    void setFrameOnBothSheets(bool both) { m_copyToEverySheet = both; }

    /**
     * For frame duplication policy on new page creation.
     */
    KWord::NewFrameBehavior newFrameBehavior() const { return m_newFrameBehavior; }
    /**
     * For frame duplication policy on new page creation.
     * Altering this does not change the frames placed until a new page is created.
     * @param nf the NewFrameBehavior.
     */
    void setNewFrameBehavior(KWord::NewFrameBehavior nf) { m_newFrameBehavior = nf; }

    /**
     * Return the side around text from another (text)frame should flow.
     */
    KWord::RunAroundSide runAroundSide() const { return m_runAroundSide; }
    /**
     * Set the side around text from another (text)frame should flow.
     * @param side the requested side
     */
    void setRunAroundSide(KWord::RunAroundSide side) { m_runAroundSide = side; }

    /**
     * Return the text runaround property for this frame.
     * This property specifies how text from another textframe will behave when this frame
     * intersects with it.
     */
    KWord::TextRunAround textRunAround() const { return m_runAround; }
    /**
     * Set the text runaround property for this frame.
     * This property specifies how text from another textframe will behave when this frame
     * intersects with it.
     */
    void setTextRunAround(KWord::TextRunAround runAround) { m_runAround = runAround; }

    /**
     * The space between this frames edge and the text when that text runs around this frame.
     * @return the space around this frame to keep free from text
     */
    double runAroundDistance() { return m_runAroundDistance; }
    /**
     * Set the space between this frames edge and the text when that text runs around this frame.
     * @param distance the space around this frame to keep free from text
     */
    void setRunAroundDistance(double distance) { m_runAroundDistance = distance; }

    /**
     * Each frame will be rendered by a shape which also holds the position etc.
     * @return the shape that represents this frame.
     */
    KoShape *shape() const { return m_shape; }

    /**
     * Return the parent frameset.
     * @return the parent frameset
     */
    KWFrameSet *frameSet() const { return m_frameSet; }
    /**
     * Set the frameset this frame will work on.
     * Altering the frameset requires you to remove this frame on the old and add the frame on the
     * new frameset
     * @param newFrameSet the new frameset
     */
    virtual void setFrameSet(KWFrameSet *newFrameSet);

    /**
     * States if this frame is a copy of the previous one.
     * If this frame is a copy, then this frame is drawn with the same content as the
     * previous frame in this frameset.
     * @return true if this is a copy
     */
    bool isCopy() { return m_isCopy; }
    /**
     * Sets if this frame is a copy of the previous one.
     * If this frame is a copy, then this frame is drawn with the same content as the
     * previous frame in this frameset.
     * @param on if true, the frame is now a copy
     */
    void setCopy(bool on) { m_isCopy = on; }

private:
    KoShape *m_shape;
    KWord::FrameBehavior m_frameBehavior;
    bool m_copyToEverySheet;
    KWord::NewFrameBehavior m_newFrameBehavior;
    KWord::RunAroundSide m_runAroundSide;
    KWord::TextRunAround m_runAround;
    double m_runAroundDistance;
    bool m_isCopy;

    KWFrameSet *m_frameSet;
};

#endif
