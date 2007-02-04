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

#ifndef SCRIPTING_TEXTCURSOR_H
#define SCRIPTING_TEXTCURSOR_H

#include <QObject>
#include <QTextCursor>

namespace Scripting {

    /**
    * A cursor to provides a control structure for the successive traversal
    * of content within a \a TextDocument .
    */
    class TextCursor : public QObject
    {
            Q_OBJECT
        public:
            TextCursor(QObject* parent, const QTextCursor& cursor);
            virtual ~TextCursor();
            QTextCursor& cursor() { return m_cursor; }

        public Q_SLOTS:

            /** Return the position the cursor is on. */
            int position() const;
            /** Set the position the cursor is on. */
            void setPosition(int pos);

            //int selectionStart () const
            //int selectionEnd () const
            //QString selectedText () const
            //QString selectionToHtml(const QString& encoding = QString()) const
            //QString selectionToPlainText() const

            /** Insert the \p text plain-text at the current cursor position. */
            void insertText(const QString& text);
            /** Insert the \p html html-text at the current cursor position. */
            void insertHtml(const QString& html);
            /** Insert a block element at the current cursor position. */
            void insertBlock();
            void insertDefaultBlock();
            /** Insert and return a new \a TextFrame object at the current cursor position. */
            QObject* insertFrame();
            /** Insert and return a new \a TextList object at the current cursor position. */
            QObject* insertList();
            /** Insert and return a new \a TextTable object at the current cursor position. */
            QObject* insertTable(int rows, int columns);

            //void insertFragment ( const QTextDocumentFragment & fragment )
            //void insertImage ( const QTextImageFormat & format, QTextFrameFormat::Position alignment )

        private:
            QTextCursor m_cursor;
    };

}

#endif
