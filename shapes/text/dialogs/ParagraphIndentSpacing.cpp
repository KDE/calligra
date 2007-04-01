/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
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

#include "ParagraphIndentSpacing.h"

#include <KoParagraphStyle.h>

ParagraphIndentSpacing::ParagraphIndentSpacing(QWidget *parent)
    : QWidget(parent)
{
    widget.setupUi(this);
}

void ParagraphIndentSpacing::open(KoParagraphStyle *style) {
    m_style = style;
    widget.first->changeValue(style->textIndent());
    widget.left->changeValue(style->leftMargin());
    widget.right->changeValue(style->rightMargin());
    widget.before->changeValue(style->topMargin());
    widget.after->changeValue(style->bottomMargin());
}

void ParagraphIndentSpacing::save() {
    m_style->setTextIndent( widget.first->value() );
    m_style->setLeftMargin( widget.left->value() );
    m_style->setRightMargin( widget.right->value() );
    m_style->setTopMargin( widget.before->value() );
    m_style->setBottomMargin( widget.after->value() );
}

#include "ParagraphIndentSpacing.moc"
