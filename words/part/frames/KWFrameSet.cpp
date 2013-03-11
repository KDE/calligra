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
#include <KoColorBackground.h>
#include "KWCopyShape.h"

#include <kdebug.h>

KWFrameSet::KWFrameSet(Words::FrameSetType type)
    : QObject(),
    m_type(type)
{
    kDebug(32001) << "type=" << m_type;
}

KWFrameSet::~KWFrameSet()
{
    kDebug(32001) << "type=" << m_type << "frameCount=" << frames().count();
    while (!frames().isEmpty()) { // deleting a shape can result in multiple KWFrame's and shapes being deleted (e.g. copy-shapes)
        KWFrame *f = frames().last();
        if (f->shape()) {
            delete f->shape(); // deletes also the KWFrame and calls KWFrameSet::removeFrame
            Q_ASSERT(!frames().contains(f));
        } else {
            removeFrame(f);
        }
    }
}

void KWFrameSet::addFrame(KWFrame *frame)
{
    Q_ASSERT(frame);
    kDebug(32001) << "frame=" << frame << "frameSet=" << frame->frameSet();
    Q_ASSERT(!m_frames.contains(frame));
    m_frames.append(frame); // this one first, so we don't enter the addFrame twice.
    frame->setFrameSet(this);
    setupFrame(frame);
    if (frame->isCopy()) {
        KWCopyShape* copyShape = dynamic_cast<KWCopyShape*>(frame->shape());
        if (copyShape && copyShape->original()) {
            KWFrame *originalFrame = dynamic_cast<KWFrame*>(copyShape->original()->applicationData());
            if (originalFrame) {
                originalFrame->addCopy(frame);
            }
        }
    }
    emit frameAdded(frame);
}

void KWFrameSet::removeFrame(KWFrame *frame, KoShape *shape)
{
    Q_ASSERT(frame);
    if (frame->isCopy()) {
        KWCopyShape* copyShape = dynamic_cast<KWCopyShape*>(frame->shape());
        if (copyShape && copyShape->original()) {
            KWFrame *originalFrame = dynamic_cast<KWFrame*>(copyShape->original()->applicationData());
            if (originalFrame) {
                originalFrame->removeCopy(frame);
            }
        }
    } else {
        //TODO use the copyFrame-list the KWFrame's remembers now
        // Loop over all frames to see if there is a copy frame that references the removed
        // frame; if it does, then delete the copy too.
        for(int i = frames().count() - 1; i >= 0; --i) {
            KWFrame *frame = frames()[i];
            if (KWCopyShape *cs = dynamic_cast<KWCopyShape*>(frame->shape())) {
                if (cs->original() == shape) {
                    Q_ASSERT(frame->frameSet() == this);
                    frame->cleanupShape(cs);
                    removeFrame(frame, cs);
                    delete cs;
                }
            }
        }
    }

    if (m_frames.removeAll(frame)) {
        frame->setFrameSet(0);
        emit frameRemoved(frame);
    }
}

