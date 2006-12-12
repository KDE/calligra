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

#include "TextCursor.h"
#include "TextList.h"
#include "TextTable.h"
#include "TextFormat.h"

#include <QObject>
#include <QTextCursor>
#include <QTextTableFormat>
#include <QTextListFormat>

using namespace Scripting;

TextCursor::TextCursor(QObject* parent, const QTextCursor& cursor)
    : QObject( parent ), m_cursor( cursor ) {}

TextCursor::~TextCursor() {}

int TextCursor::position() const {
    return m_cursor.position();
}

void TextCursor::setPosition(int pos) {
    m_cursor.setPosition(pos);
}

void TextCursor::insertText(const QString& text) {
    m_cursor.insertText(text);
}

void TextCursor::insertHtml(const QString& html) {
    m_cursor.insertHtml(html);
}

void TextCursor::insertBlock(QObject* textformat) {
    TextFormat* format = dynamic_cast<TextFormat*>(textformat);
    if(format)
        m_cursor.insertBlock(format->format().toBlockFormat());
    else
        m_cursor.insertBlock();
}

QObject* TextCursor::insertList(QObject* textformat) {
    TextFormat* format = dynamic_cast<TextFormat*>(textformat);
    QTextListFormat f;
    if(format)
        f = format->format().toListFormat();
    else {
        f.setStyle(QTextListFormat::ListDisc); f.setIndent(f.indent()+1); //testcase
    }
    QTextList* l = m_cursor.insertList(f);
    return l ? new TextList(this, l) : 0;
}

QObject* TextCursor::insertTable(int rows, int columns) {
    QTextTableFormat format;
    format.setCellPadding(5); format.setCellSpacing(5); //testcase
    QTextTable* t = m_cursor.insertTable(rows, columns, format);
    return t ? new TextTable(this, t) : 0;
}

#include "TextCursor.moc"
