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
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

#include "TextFrame.h"
#include "TextTable.h"
#include "TextCursor.h"

namespace Scripting {

    /**
    *
    */
    class TextDocument : public QObject
    {
            Q_OBJECT
        public:
            TextDocument(QObject* parent, QTextDocument* doc) : QObject( parent ), m_doc( doc ) {
                connect(m_doc, SIGNAL(contentsChange(int,int,int)), this, SIGNAL(contentsChange(int,int,int)));
                connect(m_doc, SIGNAL(contentsChanged()), this, SIGNAL(contentsChanged()));
                connect(m_doc, SIGNAL(cursorPositionChanged(const QTextCursor&)), this, SIGNAL(cursorPositionChanged()));
                connect(m_doc->documentLayout(), SIGNAL(documentSizeChanged(const QSizeF&)), this, SIGNAL(documentSizeChanged()));
                connect(m_doc->documentLayout(), SIGNAL(pageCountChanged(int)), this, SIGNAL(pageCountChanged()));
            }
            virtual ~TextDocument() {}

        public Q_SLOTS:

            virtual double documentWidth() const { return m_doc->documentLayout()->documentSize().width(); }
            virtual double documentHeight() const { return m_doc->documentLayout()->documentSize().height(); }
            virtual int pageCount() const { return m_doc->documentLayout()->pageCount(); }
            //bool isEmpty() const { return m_doc->isEmpty(); }
            bool isModified() const { return m_doc->isModified(); }

            QObject* rootFrame() { return new TextFrame(this, m_doc->rootFrame()); }

            QObject* firstCursor() { return new TextCursor(this, QTextCursor(m_doc->begin())); }
            QObject* lastCursor() { return new TextCursor(this, QTextCursor(m_doc->end())); }

            //QTextObject * object ( int objectIndex ) const 
            //QTextObject * objectForFormat ( const QTextFormat & f ) const 

            QString toText() const { return m_doc->toPlainText(); }
            void setText(const QString & text) { m_doc->setPlainText(text); }
            QString defaultStyleSheet() const { return m_doc->defaultStyleSheet(); }
            void setDefaultStyleSheet(const QString& stylesheet) { m_doc->setDefaultStyleSheet(stylesheet); }
            QString toHtml(const QString& encoding = QString()) const { return m_doc->toHtml( encoding.isNull() ? QByteArray() : encoding.toLatin1() ); }
            void setHtml(const QString & html) { m_doc->setHtml(html); }

        signals:
            void contentsChange(int position, int charsRemoved, int charsAdded);
            void contentsChanged();
            void cursorPositionChanged();
            void documentSizeChanged();
            void pageCountChanged();

        private:
            QPointer<QTextDocument> m_doc;
    };

}

#endif
