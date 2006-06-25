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

#ifndef KWFRAMESET_H
#define KWFRAMESET_H

#include "KWord.h"

#include <QObject>

class KWFrame;

/// KWFrameSet
class KWFrameSet : public QObject {
    Q_OBJECT
public:
    KWFrameSet();
    virtual ~KWFrameSet();

    void addFrame(KWFrame *frame);
    void removeFrame(KWFrame *frame);

    void setName(const QString &name) { m_name = name; }
    const QString &name() const { return m_name; }

    const QList<KWFrame*> frames() const { return m_frames; }

    int frameCount() const { return m_frames.count(); }

signals:
    void frameAdded(KWFrame *);
    void frameRemoved(KWFrame *);

protected:
    /**
     * Called from addFrame.
     * Overwrite in inheriting classes to do something with the frame on add.
     * @param frame the frame that has just been added
     */
    virtual void setupFrame(KWFrame *frame) { Q_UNUSED(frame); }

private:
    QList<KWFrame*> m_frames;
    QString m_name;
};

#endif
