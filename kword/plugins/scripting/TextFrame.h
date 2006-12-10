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
#include <QTextFrame>

#include "TextCursor.h"

namespace Scripting {

    /**
    *
    */
    class TextFrame : public QObject
    {
            Q_OBJECT
        public:
            TextFrame(QObject* parent, QTextFrame* frame)
                : QObject( parent ), m_frame( frame ) {}
            virtual ~TextFrame() {}

        public Q_SLOTS:

            int firstPosition() const {
                return m_frame->firstPosition();
            }

            QObject* firstCursorPosition() {
                return new TextCursor(this, m_frame->firstCursorPosition());
            }

            int lastPosition() const {
                return m_frame->lastPosition();
            }

            QObject* lastCursorPosition() {
                return new TextCursor(this, m_frame->lastCursorPosition());
            }

        private:
            QTextFrame* m_frame;
    };

}

#endif
