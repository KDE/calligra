/* This file is part of the Calligra project
 * SPDX-FileCopyrightText: 2006 Sebastian Sauer <mail@dipe.org>
 * SPDX-FileCopyrightText: 2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SCRIPTING_TEXTFRAME_H
#define SCRIPTING_TEXTFRAME_H

#include "TextCursor.h"

#include <QObject>
#include <QWeakPointer>
#include <QTextFrame>

namespace Scripting
{

/**
* The TextFrame class represents a frame within a \a TextDocument .
* Note; this class is likely not very useful to script writers and is considered for
* removal in a future release of Words.
*/
class TextFrame : public QObject
{
    Q_OBJECT
public:
    TextFrame(QObject* parent, QTextFrame* frame)
            : QObject(parent), m_frame(frame) {}
    virtual ~TextFrame() {}

public Q_SLOTS:

    /** Return the frame content as plain text. */
    QString text() const {
        QTextFrame *frame = m_frame.data();
        if (frame)
            return QTextCursor(frame).block().text();
        return QString();
    }

    /** Return the \a TextCursor object for this frame. */
    QObject* cursor() {
        QTextFrame *frame = m_frame.data();
        if (frame)
            return new TextCursor(this, QTextCursor(frame));
        return 0;
    }

    /** Return the first position in the frame. */
    int firstPosition() const {
        QTextFrame *frame = m_frame.data();
        if (frame)
            return frame->firstPosition();
        return 0;
    }

    /** Return a \a TextCursor object for the first position in the frame. */
    QObject* firstCursorPosition() {
        QTextFrame *frame = m_frame.data();
        if (frame)
            return new TextCursor(this, frame->firstCursorPosition());
        return 0;
    }

    /** Return the last position in the frame. */
    int lastPosition() const {
        QTextFrame *frame = m_frame.data();
        if (frame)
            return frame->lastPosition();
        return 0;
    }

    /** Return a \a TextCursor object for the last position in the frame. */
    QObject* lastCursorPosition() {
        QTextFrame *frame = m_frame.data();
        if (frame)
            return new TextCursor(this, frame->lastCursorPosition());
        return 0;
    }

    /** Return a child \a TextFrame object with the defined \p index . */
    QObject* childFrame(int index) {
        QTextFrame *frame = m_frame.data();
        if (frame)
            return (index >= 0 && index < frame->childFrames().count()) ? frame->childFrames().at(index) : 0;
        return 0;
    }

    /** Return the number of \a TextCursor objects. */
    int childFrameCount() const {
        QTextFrame *frame = m_frame.data();
        if (frame)
            return frame->childFrames().count();
        return 0;
    }

private:
    QWeakPointer<QTextFrame> m_frame;
};

}

#endif
