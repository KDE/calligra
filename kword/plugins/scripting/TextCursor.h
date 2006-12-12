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
    *
    */
    class TextCursor : public QObject
    {
            Q_OBJECT
        public:
            TextCursor(QObject* parent, const QTextCursor& cursor);
            virtual ~TextCursor();

        public Q_SLOTS:

            int position() const;
            void setPosition(int pos);

            //int selectionStart () const
            //int selectionEnd () const
            //QString selectedText () const
            //QString selectionToHtml(const QString& encoding = QString()) const
            //QString selectionToPlainText() const

            void insertText(const QString& text);
            void insertHtml(const QString& html);
            void insertBlock(QObject* textformat = 0);
            QObject* insertFrame(QObject* textformat = 0);
            QObject* insertList(QObject* textformat = 0);
            QObject* insertTable(int rows, int columns);
            //void insertFragment ( const QTextDocumentFragment & fragment )
            //void insertImage ( const QTextImageFormat & format, QTextFrameFormat::Position alignment )

        private:
            QTextCursor m_cursor;
    };

}

#endif
