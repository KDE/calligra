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

#ifndef SCRIPTING_TEXTDOCUMENT_H
#define SCRIPTING_TEXTDOCUMENT_H

#include <QPointer>
#include <QObject>
#include <koffice_export.h>

class QTextDocument;

namespace Scripting {

    /**
    * A frameset holds a number of \a Frame (zero or more) objects where
    * each frame holds the content that is displayed on screen.
    */
    class TextDocument : public QObject
    {
            Q_OBJECT
        public:
            explicit TextDocument( QObject* parent, QTextDocument* doc );
            ~TextDocument();

        public Q_SLOTS:

            QString toHtml( const QString& encoding = QString() ) const;
            void setHtml( const QString & html );

            QString toPlainText() const;
            void setPlainText( const QString & text );

        private:
            QPointer<QTextDocument> m_doc;
    };

}

#endif
