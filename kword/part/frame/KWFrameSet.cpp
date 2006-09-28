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

#include <kdebug.h>

KWFrameSet::KWFrameSet()
: QObject()
{
}

KWFrameSet::~KWFrameSet() {
    qDeleteAll(m_frames);
}

void KWFrameSet::addFrame(KWFrame *frame) {
    Q_ASSERT(frame);
    Q_ASSERT(! m_frames.contains(frame));
    m_frames.append(frame);
    setupFrame(frame);
    emit frameAdded(frame);
}

void KWFrameSet::removeFrame(KWFrame *frame) {
    Q_ASSERT(frame);
    m_frames.removeAll(frame);
    emit frameRemoved(frame);
}

#ifndef NDEBUG
void KWFrameSet::printDebug() {
    //kDebug() << " |  Visible: " << isVisible() << endl;
    int i=0;
    foreach(KWFrame *frame, frames()) {
        kDebug() << " +-- Frame " << i++ << " of "<< frameCount() << "    (" << frame << ")  " /*<<
        (frame->isCopy() ? "[copy]" : "") */<< endl;
        printDebug(frame);
    }
}

void KWFrameSet::printDebug(KWFrame *frame) {
    static const char * runaround[] = { "No Runaround", "Bounding Rect", "Skip", "ERROR" };
    static const char * runaroundSide[] = { "Biggest", "Left", "Right", "ERROR" };
    static const char * frameBh[] = { "AutoExtendFrame", "AutoCreateNewFrame", "Ignore", "ERROR" };
    static const char * newFrameBh[] = { "Reconnect", "NoFollowup", "Copy" };
    kDebug() << "     Rectangle : " << frame->shape()->position().x() << "," << frame->shape()->position().y() << " " << frame->shape()->size().width() << "x" << frame->shape()->size().height() << endl;
    kDebug() << "     RunAround: "<< runaround[ frame->textRunAround() ] << " side:" << runaroundSide[ frame->runAroundSide() ]<< endl;
    kDebug() << "     FrameBehavior: "<< frameBh[ frame->frameBehavior() ] << endl;
    kDebug() << "     NewFrameBehavior: "<< newFrameBh[ frame->newFrameBehavior() ] << endl;
    if(frame->shape()->background().style() == Qt::NoBrush)
        kDebug() << "     BackgroundColor: Transparent\n";
    else {
        QColor col = frame->shape()->background().color();
        kDebug() << "     BackgroundColor: "<< ( col.isValid() ? col.name() : QString("(default)") ) << endl;
    }
    kDebug() << "     frameOnBothSheets: "<< frame->frameOnBothSheets() << endl;
    kDebug() << "     Z Order: " << frame->shape()->zIndex() << endl;

    //kDebug() << "     minFrameHeight "<< frame->minimumFrameHeight() << endl;
    //QString page = pageManager() && pageManager()->pageCount() > 0 ? QString::number(frame->pageNumber()) : " [waiting for pages to be created]";
}
#endif

#include "KWFrameSet.moc"
