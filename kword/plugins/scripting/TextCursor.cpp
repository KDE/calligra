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
#include "TextFrame.h"
#include "TextList.h"
#include "TextTable.h"

#include <QObject>
#include <QTextCursor>
#include <QTextTableFormat>
#include <QTextListFormat>

using namespace Scripting;

TextCursor::TextCursor(QObject* parent, const QTextCursor& cursor)
    : QObject( parent ), m_cursor( cursor )
{
}

TextCursor::~TextCursor()
{
}

int TextCursor::position() const
{
    return m_cursor.position();
}

void TextCursor::setPosition(int pos)
{
    m_cursor.setPosition(pos);
}

void TextCursor::insertText(const QString& text)
{
    m_cursor.insertText(text);
}

void TextCursor::insertHtml(const QString& html)
{
    m_cursor.insertHtml(html);
}

void TextCursor::insertBlock()
{
    m_cursor.insertBlock();
}

void TextCursor::insertDefaultBlock()
{
    QTextBlockFormat bf;
    QTextCharFormat cf;
    m_cursor.insertBlock(bf, cf);
}

QObject* TextCursor::insertFrame()
{
    QTextFrameFormat f;
    QTextFrame* frame = m_cursor.insertFrame(f);
    return frame ? new TextFrame(this, frame) : 0;
}

QObject* TextCursor::insertList()
{
    QTextListFormat f;

    //testcase
    //f.setStyle(QTextListFormat::ListDisc);
    //f.setIndent(f.indent()+1);

    QTextList* l = m_cursor.insertList(f);
    return l ? new TextList(this, l) : 0;
}

QObject* TextCursor::insertTable(int rows, int columns)
{
    QTextTableFormat format;
    //format.setColumns(columns);
    //format.setHeaderRowCount(1);
    format.setBackground(QColor("#e0e0e0"));
    //format.setCellPadding(1); format.setCellSpacing(1); //testcase

QVector<QTextLength> constraints;
constraints << QTextLength(QTextLength::PercentageLength, 16);
constraints << QTextLength(QTextLength::PercentageLength, 28);
constraints << QTextLength(QTextLength::PercentageLength, 28);
constraints << QTextLength(QTextLength::PercentageLength, 28);
format.setColumnWidthConstraints(constraints);

    QTextTable* table = m_cursor.insertTable(rows, columns, format);
    //QTextTable* t = m_cursor.insertTable(rows, columns);

QTextTableCell cell = table->cellAt(0, 0);
cell.firstCursorPosition().insertText(tr("aaa") /*, QTextCharFormat::charFormat*/);
table->cellAt(0, 1).firstCursorPosition().insertText(tr("bbb"));

    return table ? new TextTable(this, table) : 0;
}

#include "TextCursor.moc"
