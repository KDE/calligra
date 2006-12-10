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

#include "TextDocument.h"

#include <QTextDocument>

using namespace Scripting;

TextDocument::TextDocument( QObject* parent, QTextDocument* doc )
    : QObject( parent )
    , m_doc( doc )
{
}

TextDocument::~TextDocument()
{
}

QString TextDocument::toHtml( const QString& encoding ) const
{
    return m_doc->toHtml( encoding.isNull() ? QByteArray() : encoding.toLatin1() );
}

void TextDocument::setHtml( const QString & html )
{
    m_doc->setHtml(html);
}

QString TextDocument::toPlainText() const
{
    return m_doc->toPlainText();
}

void TextDocument::setPlainText( const QString & text )
{
    m_doc->setPlainText(text);
}

#include "TextDocument.moc"
