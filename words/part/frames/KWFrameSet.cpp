/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2000-2006 David Faure <faure@kde.org>
 * SPDX-FileCopyrightText: 2005-2011 Sebastian Sauer <mail@dipe.org>
 * SPDX-FileCopyrightText: 2005-2006, 2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2008 Pierre Ducroquet <pinaraf@pinaraf.info>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KWFrameSet.h"
#include "KWCopyShape.h"
#include "KWFrame.h"

#include <WordsDebug.h>

KWFrameSet::KWFrameSet(Words::FrameSetType type)
    : QObject()
    , m_type(type)
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

    KWCopyShape *copyShape = dynamic_cast<KWCopyShape *>(shape);
    if (copyShape) {
        if (copyShape->original()) {
            addCopy(copyShape);
        }
    }
    Q_EMIT shapeAdded(shape);
}

void KWFrameSet::removeShape(KoShape *shape)
{
    KWCopyShape *copyShape = dynamic_cast<KWCopyShape *>(shape);
    if (copyShape) {
        removeCopy(copyShape);
    } else {
        // Loop over all frames to see if there is a copy shape that references the removed
        // shape; if it does, then delete the copy too.
        for (int i = shapes().count() - 1; i >= 0; --i) {
            KoShape *s = shapes()[i];
            if (KWCopyShape *cs = dynamic_cast<KWCopyShape *>(s)) {
                if (cs->original() == shape) {
                    cleanupShape(cs);
                    removeShape(cs);
                    delete cs;
                }
            }
        }
    }

    if (m_shapes.removeAll(shape)) {
        Q_EMIT shapeRemoved(shape);
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
