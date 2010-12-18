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

#include "KWFrameSet.h"
#include "KWFrame.h"
#include <KoColorBackground.h>
#include "KWCopyShape.h"

#include <kdebug.h>

KWFrameSet::KWFrameSet(KWord::FrameSetType type)
    : QObject(),
    m_type(type)
{
}

KWFrameSet::~KWFrameSet()
{
    foreach (KWFrame *frame, frames())
        delete frame->shape();
}

void KWFrameSet::addFrame(KWFrame *frame)
{
    Q_ASSERT(frame);
    if (m_frames.contains(frame))
        return;
    m_frames.append(frame); // this one first, so we don't enter the addFrame twice.
    frame->setFrameSet(this);
    setupFrame(frame);
    emit frameAdded(frame);
}

void KWFrameSet::removeFrame(KWFrame *frame, KoShape *shape)
{
    Q_ASSERT(frame);
    if (!frame->isCopy()) {
        // Loop over all frames to see if there is a copy frame that references the removed
        // frame; if it does, then mark the copy as obsolete
        foreach (KWFrame *f, frames()) {
            if (KWCopyShape *cs = dynamic_cast<KWCopyShape*>(f->shape())) {
                if (cs->original() == shape) {
                    cs->retire();
                }
            }
        }
    }

    if (m_frames.removeAll(frame)) {
        frame->setFrameSet(0);
        emit frameRemoved(frame);
    }
}

#ifndef NDEBUG
void KWFrameSet::printDebug()
{
    int i = 1;
    foreach (KWFrame *frame, frames()) {
        kDebug(32001) << " +-- Frame" << i++ << "of" << frameCount() << "    (" << frame << frame->shape() << ")"
            << (frame->isCopy() ? "[copy]" : "");
        printDebug(frame);
    }
}

void KWFrameSet::printDebug(KWFrame *frame)
{
    static const char * runaround[] = { "No Runaround", "Bounding Rect", "Skip", "ERROR" };
    static const char * runaroundSide[] = { "Biggest", "Left", "Right", "Auto", "Both", "ERROR" };
    static const char * frameBh[] = { "AutoExtendFrame", "AutoCreateNewFrame", "Ignore", "ERROR" };
    static const char * newFrameBh[] = { "Reconnect", "NoFollowup", "Copy" };
    kDebug(32001) << "     Rectangle :" << frame->shape()->position().x() << "," << frame->shape()->position().y() << frame->shape()->size().width() << "x" << frame->shape()->size().height();
    kDebug(32001) << "     RunAround:" << runaround[frame->textRunAround()] << "side:" << runaroundSide[frame->runAroundSide()];
    kDebug(32001) << "     FrameBehavior:" << frameBh[frame->frameBehavior()];
    kDebug(32001) << "     NewFrameBehavior:" << newFrameBh[frame->newFrameBehavior()];
    if (!frame->shape()->background())
        kDebug(32001) << "     BackgroundColor: Transparent";
    else {
        KoColorBackground * fill = dynamic_cast<KoColorBackground*>(frame->shape()->background());
        QColor col;
        if (fill)
            col = fill->color();
        kDebug(32001) << "     BackgroundColor:" << (col.isValid() ? col.name() : QString("(default)"));
    }
    kDebug(32001) << "     frameOnBothSheets:" << frame->frameOnBothSheets();
    kDebug(32001) << "     Z Order:" << frame->shape()->zIndex();
    kDebug(32001) << "     Visible:" << frame->shape()->isVisible();

    //kDebug(32001) <<"     minFrameHeight"<< frame->minimumFrameHeight();
    //QString page = pageManager() && pageManager()->pageCount() > 0 ? QString::number(frame->pageNumber()) : " [waiting for pages to be created]";
}
#endif

