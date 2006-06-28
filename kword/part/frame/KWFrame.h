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
 * A frame is really just a square that is used to place the content
 * of a frameset.
 */
class KWORD_TEST_EXPORT KWFrame {
public:
    /**
     * Constructor
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
    KWord::SheetSide sheetSide() const { return m_sheetSide; }
    /**
     * Determines if this frame will be copied on even or odd pages only.
     * Altering this does not change the frames placed until a new page is created.
     * @param side the new sheet side
     */
    void setSheetSide(KWord::SheetSide side) { m_sheetSide = side; }

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

    KWord::RunAroundSide runAroundSide() const { return m_runAroundSide; }
    void setRunAroundSide(KWord::RunAroundSide side) { m_runAroundSide = side; }

    double runAroundDistance() { return m_runAroundDistance; }
    void setRunAroundDistance(double distance) { m_runAroundDistance = distance; }

    KoShape *shape() { return m_shape; }

    KWFrameSet *frameSet() const { return m_frameSet; }

protected:
    KoShape *m_shape;

private:
    KWord::FrameBehavior m_frameBehavior;
    KWord::SheetSide m_sheetSide;
    KWord::NewFrameBehavior m_newFrameBehavior;
    KWord::RunAroundSide m_runAroundSide;
    double m_runAroundDistance;

    KWFrameSet *m_frameSet;
};

#endif
