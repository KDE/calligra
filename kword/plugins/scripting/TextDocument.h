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

#include <KoVariable.h>
#include <KoVariableManager.h>
#include <KoInlineTextObjectManager.h>
#include <KWTextDocumentLayout.h>

#include "TextFrame.h"
#include "TextTable.h"
#include "TextCursor.h"

namespace Scripting {

    /**
    * The TextDocument object represents a QTextDocument within the Scribe
    * text-engine KWord uses to enable editing of text content.
    */
    class TextDocument : public QObject
    {
            Q_OBJECT
        public:
            TextDocument(QObject* parentFrameSet, QTextDocument* doc)
                : QObject( parentFrameSet ), m_doc( doc )
            {
                connect(m_doc, SIGNAL(contentsChange(int,int,int)), this, SIGNAL(contentsChange(int,int,int)));
                connect(m_doc, SIGNAL(cursorPositionChanged(const QTextCursor&)), this, SIGNAL(cursorPositionChanged()));
                connect(m_doc->documentLayout(), SIGNAL(documentSizeChanged(const QSizeF&)), this, SIGNAL(documentSizeChanged()));
                connect(m_doc->documentLayout(), SIGNAL(pageCountChanged(int)), this, SIGNAL(pageCountChanged()));
            }
            virtual ~TextDocument() {}

            KoVariableManager* variableManager() {
                KWTextDocumentLayout* layout = dynamic_cast< KWTextDocumentLayout* >( m_doc->documentLayout() );
                if( ! layout ) return 0;
                KoInlineTextObjectManager* textobjmanager = layout->inlineObjectTextManager();
                return textobjmanager ? textobjmanager->variableManager() : 0;
            }

        public Q_SLOTS:

            /** Return the width of the document in pt. */
            virtual double width() const { return m_doc->documentLayout()->documentSize().width(); }
            /** Return the height of the document in pt. */
            virtual double height() const { return m_doc->documentLayout()->documentSize().height(); }
            /** Return the number of pages the document has. */
            virtual int pageCount() const { return m_doc->documentLayout()->pageCount(); }
            /** Return true if the document was modified else false is returned. */
            bool isModified() const { return m_doc->isModified(); }

            //bool isEmpty() const { return m_doc->isEmpty(); }

            /** Return the root \a Frame object of the document. */
            QObject* rootFrame() { return new TextFrame(this, m_doc->rootFrame()); }

            /** Return the first \a Cursior object of the document. */
            QObject* firstCursor() { return new TextCursor(this, QTextCursor(m_doc->begin())); }
            /** Return the last \a Cursior object of the document. */
            QObject* lastCursor() { return new TextCursor(this, QTextCursor(m_doc->end())); }

            //QTextObject * object ( int objectIndex ) const 
            //QTextObject * objectForFormat ( const QTextFormat & f ) const 

            /** Return the content of the document as plain-text. */
            QString toText() const { return m_doc->toPlainText(); }
            /** Set the content of the document to the \p text plain-text. */
            void setText(const QString & text) { m_doc->setPlainText(text); }
            /** Return the stylesheet. */
            QString defaultStyleSheet() const { return m_doc->defaultStyleSheet(); }
            /** Set the stylesheet. */
            void setDefaultStyleSheet(const QString& stylesheet) { m_doc->setDefaultStyleSheet(stylesheet); }
            /** Return the content of the document as HTML-text. */
            QString toHtml(const QString& encoding = QString()) const { return m_doc->toHtml( encoding.isNull() ? QByteArray() : encoding.toLatin1() ); }
            /** Set the content of the document to the \p html HTML-text. */
            void setHtml(const QString & html) { m_doc->setHtml(html); }

            /** Return a list of all variablenames. */
            QStringList variableNames() {
                KoVariableManager* manager = variableManager();
                return manager ? manager->variables() : QStringList();
            }
            /** Return the value of a variable. */
            QString variableValue(const QString& variablename) {
                KoVariableManager* manager = variableManager();
                return manager ? manager->value(variablename) : QString();
            }
            /** Set the value of a variable. */
            bool setVariableValue(const QString& variablename, const QString& value) {
                KoVariableManager* manager = variableManager();
                if( ! manager ) return false;
                manager->setValue(variablename, value);
                return true;
            }
            /** Add a new variable. */
            bool addVariable(const QString& variablename, const QString& value) {
                KoVariableManager* manager = variableManager();
                if( ! manager ) return false;
                KoVariable* v = manager->createVariable(variablename);
                v->setValue(value);
                return true;
            }
            /** Remove a value. */
            void removeVariable(const QString& variablename) {
                KoVariableManager* manager = variableManager();
                if( manager ) manager->remove(variablename);
            }

        signals:

            /** This signal is emitted if content changed. */
            void contentsChange(int position, int charsRemoved, int charsAdded);
            /** This signal is emitted if the cursor-position changed. */
            void cursorPositionChanged();
            /** This signal is emitted if the size of the document changed. */
            void documentSizeChanged();
            /** This signal is emitted if the number of pages changed. */
            void pageCountChanged();

        private:
            QPointer<QTextDocument> m_doc;
    };

}

#endif
