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

#include <QObject>
#include <QTextDocument>
#include <kdebug.h>
#include <KoVariable.h>
#include <KoInlineTextObjectManager.h>
#include <KoTextDocumentLayout.h>

#include "TextFrame.h"
#include "TextCursor.h"

namespace Scripting
{

/**
* The TextDocument object represents a QTextDocument within the Scribe
* text-engine KWord uses to enable editing of text content.
*/
class TextDocument : public QObject
{
    Q_OBJECT
public:
    TextDocument(QObject* parentFrameSet, QTextDocument* doc)
            : QObject(parentFrameSet), m_doc(doc) {
        connect(m_doc, SIGNAL(contentsChange(int, int, int)), this, SIGNAL(contentsChanged(int, int, int)));
        connect(m_doc, SIGNAL(cursorPositionChanged(const QTextCursor&)), this, SIGNAL(cursorPositionChanged()));
        connect(m_doc->documentLayout(), SIGNAL(documentSizeChanged(const QSizeF&)), this, SIGNAL(documentSizeChanged()));
    }
    virtual ~TextDocument() {}

    KoInlineTextObjectManager* inlineTextObjectManager() {
        KoTextDocumentLayout *layout = qobject_cast<KoTextDocumentLayout*>(m_doc->documentLayout());
        return layout ? layout->inlineTextObjectManager() : 0;
    }

    KoVariableManager* variableManager() {
        KoInlineTextObjectManager* objmanager = inlineTextObjectManager();
        return objmanager ? objmanager->variableManager() : 0;
    }

public slots:

    /** Return the width of the document in pt. */
    virtual qreal width() const {
        return m_doc->documentLayout()->documentSize().width();
    }
    /** Return the height of the document in pt. */
    virtual qreal height() const {
        return m_doc->documentLayout()->documentSize().height();
    }
    /** Return true if the document was modified else false is returned. */
    bool isModified() const {
        return m_doc->isModified();
    }

    //bool isEmpty() const { return m_doc->isEmpty(); }

    /** Return the root \a Frame object of the document. */
    QObject* rootFrame() {
        return new TextFrame(this, m_doc->rootFrame());
    }

    /** Return the \a Cursor object for the document. */
    QObject* cursor() {
        return new TextCursor(this, QTextCursor(m_doc));
    }
    /** Return the first \a Cursor object of the document. */
    QObject* firstCursor() {
        return new TextCursor(this, QTextCursor(m_doc->begin()));
    }
    /** Return the last \a Cursor object of the document. */
    QObject* lastCursor() {
        return new TextCursor(this, QTextCursor(m_doc->end()));
    }

    //QTextObject * object ( int objectIndex ) const
    //QTextObject * objectForFormat ( const QTextFormat & f ) const

    /** Return the content of the document as plain-text. */
    QString toText() const {
        return m_doc->toPlainText();
    }
    /** Set the content of the document to the \p text plain-text. */
    void setText(const QString & text) {
        m_doc->setPlainText(text);
    }
    /** Return the stylesheet. */
    QString defaultStyleSheet() const {
        return m_doc->defaultStyleSheet();
    }
    /** Set the stylesheet. */
    void setDefaultStyleSheet(const QString& stylesheet) {
        m_doc->setDefaultStyleSheet(stylesheet);
    }
    /** Return the content of the document as HTML-text. */
    QString toHtml(const QString& encoding = QString()) const {
        return m_doc->toHtml(encoding.isNull() ? QByteArray() : encoding.toLatin1());
    }
    /** Set the content of the document to the \p html HTML-text. */
    void setHtml(const QString & html) {
        m_doc->setHtml(html);
    }

    /** Return a list of all variablenames. */
    QStringList variableNames() {
        KoVariableManager* manager = variableManager();
        return manager ? QStringList(manager->variables()) : QStringList();
    }
    /** Return the value of a variable. */
    QString variableValue(const QString& variablename) {
        KoVariableManager* manager = variableManager();
        return manager ? manager->value(variablename) : QString();
    }
    /** Set the value of a variable. */
    bool setVariableValue(const QString& variablename, const QString& value) {
        KoVariableManager* manager = variableManager();
        if (! manager) return false;
        manager->setValue(variablename, value);
        return true;
    }

    //TODO move that method(s) to TextCursor
    /** Add a new variable with the name \p variablename and insert those
    new variable at the position of the \a cursor . If the variables was
    successful added true is returned else false is returned. */
    bool addVariable(QObject* cursor, const QString& variablename) {
        TextCursor *textcursor = qobject_cast<TextCursor*>(cursor);
        if (! textcursor) {
            kDebug(32010) << "No cursor";
            return false;
        }
        KoInlineTextObjectManager* objmanager = inlineTextObjectManager();
        if (! objmanager) {
            kDebug(32010) << "No textobjectmanager";
            return false;
        }
        KoVariableManager* varmanager = variableManager();
        KoVariable* variable = varmanager ? varmanager->createVariable(variablename) : 0;
        if (! variable) {
            kDebug(32010) << (varmanager ? "No variable" : "No variablemanager");
            return false;
        }
        objmanager->insertInlineObject(textcursor->cursor(), variable);
        return true;
    }
    /** Remove an existing variable. */
    void removeVariable(const QString& variablename) {
        KoVariableManager* manager = variableManager();
        if (manager) manager->remove(variablename);
    }

signals:

    /** This signal is emitted if content changed. */
    void contentsChanged(int position, int charsRemoved, int charsAdded);
    /** This signal is emitted if the cursor-position changed. */
    void cursorPositionChanged();
    /** This signal is emitted if the size of the document changed. */
    void documentSizeChanged();

private:
    QPointer<QTextDocument> m_doc;
};

}

#endif
