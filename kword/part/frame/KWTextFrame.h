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
class KWORD_TEST_EXPORT KWTextFrame : public KWFrame {
public:
    /**
     * Constructor
     * @param shape a ready made shape that is capable of showing text
     *      This shape should be created using a KoShapeFactory from the textShape plugin
     * @param parent the frameSet this frame will be associated with.
     */
    KWTextFrame(KoShape *shape, KWTextFrameSet *parent);
    ~KWTextFrame();

private:
    /* TODO
    int m_minimumFrameHeight;
    bool m_drawFootNoteLine; */
};

#endif
