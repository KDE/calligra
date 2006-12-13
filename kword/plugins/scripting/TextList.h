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

#ifndef SCRIPTING_TEXTLIST_H
#define SCRIPTING_TEXTLIST_H

#include <QObject>
#include <QPointer>
#include <QTextList>
#include <QTextBlock>
#include <QTextCursor>

#include "TextCursor.h"

namespace Scripting {

    /**
    * The TextList provides a list of items within a \a TextDocument .
    */
    class TextList : public QObject
    {
            Q_OBJECT
        public:
            TextList(QObject* parent, QTextList* list)
                : QObject( parent ), m_list( list ) {}
            virtual ~TextList() {}

        public Q_SLOTS:

            /** Return the number of items the list has. */
            int countItems() {
                return m_list->count();
            }

            /** Return a \a TextCursor object for the item at the position \p index . */
            QObject* item(int index) {
                QTextCursor cursor( m_list->item(index) );
                return cursor.isNull() ? 0 : new TextCursor(this, cursor);
            }

#if 0
            QObject* addItem() {
                QTextBlock block;
                m_list->add(block); //CRASHES
                QTextCursor cursor(block);
                return cursor.isNull() ? 0 : new TextCursor(this, cursor);
            }
#endif

            /** Remove the item at position \p index . */
            void removeItem(int index) {
                m_list->removeItem(index);
            }

        private:
            QPointer<QTextList> m_list;
    };

}

#endif
