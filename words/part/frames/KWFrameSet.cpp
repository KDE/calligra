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

#include "KWFrameSet.h"
#include "KWFrame.h"
#include "KWCopyShape.h"

#include <WordsDebug.h>

KWFrameSet::KWFrameSet(Words::FrameSetType type)
    : QObject(),
    m_type(type)
{
    debugWords << "type=" << m_type;
}

KWFrameSet::~KWFrameSet()
{
    while (!shapes().isEmpty()) { // deleting a shape can result in multiple KWFrame's and shapes being deleted (e.g. copy-shapes)
        KoShape *s = shapes().last();

        // we are going to delete the shape which under normal circumstances also removes it
        // But if we rely on that a signal will be emitted and we are being deleted
        m_shapes.removeAll(s);
        delete s; // deletes also the KWFrame and calls KWFrameSet::removeFrame
    }
}

void KWFrameSet::addShape(KoShape *shape)
{
    debugWords << "shape=" << shape << "frameSet=" << this;
    Q_ASSERT(!m_shapes.contains(shape));
    m_shapes.append(shape); // this one first, so we don't enter the addFrame twice.
    setupShape(shape);

    KWCopyShape* copyShape = dynamic_cast<KWCopyShape*>(shape);
    if (copyShape) {
        if (copyShape->original()) {
            addCopy(copyShape);
        }
    }
    emit shapeAdded(shape);
}

void KWFrameSet::removeShape(KoShape *shape)
{
    KWCopyShape* copyShape = dynamic_cast<KWCopyShape*>(shape);
    if (copyShape) {
        removeCopy(copyShape);
    } else {
        // Loop over all frames to see if there is a copy shape that references the removed
        // shape; if it does, then delete the copy too.
        for(int i = shapes().count() - 1; i >= 0; --i) {
            KoShape *s = shapes()[i];
            if (KWCopyShape *cs = dynamic_cast<KWCopyShape*>(s)) {
                if (cs->original() == shape) {
                    cleanupShape(cs);
                    removeShape(cs);
                    delete cs;
                }
            }
        }
    }

    if (m_shapes.removeAll(shape)) {
        emit shapeRemoved(shape);
    }
}

QList<KWCopyShape *> KWFrameSet::copyShapes() const
{
    return m_copyShapes;
}

void KWFrameSet::addCopy(KWCopyShape *copyShape)
{
    if (!m_copyShapes.contains(copyShape))
        m_copyShapes.append(copyShape);
}

void KWFrameSet::removeCopy(KWCopyShape *copyShape)
{
    m_copyShapes.removeAll(copyShape);
}
