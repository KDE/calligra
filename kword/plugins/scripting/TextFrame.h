/* This file is part of the KOffice project
 * Copyright (C) 2006 Sebastian Sauer <mail@dipe.org>
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

#ifndef SCRIPTING_TEXTFRAME_H
#define SCRIPTING_TEXTFRAME_H

#include <QObject>
#include <QPointer>
#include <QTextFrame>

#include "TextCursor.h"

namespace Scripting {

    /**
    * The TextFrame class represents a frame within a \a TextDocument .
    */
    class TextFrame : public QObject
    {
            Q_OBJECT
        public:
            TextFrame(QObject* parent, QTextFrame* frame)
                : QObject( parent ), m_frame( frame ) {}
            virtual ~TextFrame() {}

        public Q_SLOTS:

            /** Return the frame content as plain text. */
            QString text() const {
                return QTextCursor(m_frame).block().text();
            }

            /** Return the \a TextCursor object for this frame. */
            QObject* cursor() {
                return new TextCursor(this, QTextCursor(m_frame));
            }

            /** Return the first position in the frame. */
            int firstPosition() const {
                return m_frame->firstPosition();
            }

            /** Return a \a TextCursor object for the first position in the frame. */
            QObject* firstCursorPosition() {
                return new TextCursor(this, m_frame->firstCursorPosition());
            }

            /** Return the last position in the frame. */
            int lastPosition() const {
                return m_frame->lastPosition();
            }

            /** Return a \a TextCursor object for the last position in the frame. */
            QObject* lastCursorPosition() {
                return new TextCursor(this, m_frame->lastCursorPosition());
            }

            /** Return a child \a TextFrame object with the defined \p index . */
            QObject* childFrame(int index) {
                return (index >= 0 && index < m_frame->childFrames().count()) ? m_frame->childFrames().at(index) : 0;
            }

            /** Return the number of \a TextCursor objects. */
            int childFrameCount() const {
                return m_frame->childFrames().count();
            }

        private:
            QPointer<QTextFrame> m_frame;
    };

}

#endif
