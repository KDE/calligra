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

#include "ParagraphSettingsDialog.h"

#include "ParagraphIndentSpacing.h"
#include "ParagraphLayout.h"
#include "ParagraphBulletsNumbers.h"

#include <KoParagraphStyle.h>

#include <QTextBlock>


ParagraphSettingsDialog::ParagraphSettingsDialog(QWidget *parent)
    : KPageDialog(parent),
    m_style(0),
    m_ownStyle(false)
{
    setFaceType(KPageDialog::Tabbed);
    m_paragraphIndentSpacing = new ParagraphIndentSpacing (this);
    m_paragraphLayout = new ParagraphLayout (this);
    m_paragraphBulletsNumbers = new ParagraphBulletsNumbers (this);

    addPage(m_paragraphIndentSpacing, "Indent/Spacing");
    addPage(m_paragraphLayout, "General Layout");
    addPage(m_paragraphBulletsNumbers, "Bullets/Numbers");
}

ParagraphSettingsDialog::~ParagraphSettingsDialog() {
    if(m_ownStyle)
        delete m_style;
}

void ParagraphSettingsDialog::accept() {
    if(m_style) {
        m_paragraphIndentSpacing->save();
        m_paragraphLayout->save();
        m_paragraphBulletsNumbers->save();

        QTextBlock block = m_cursor.block();
        m_style->applyStyle(block);
    }

    QDialog::accept();
    deleteLater();
}

void ParagraphSettingsDialog::reject() {
    QDialog::reject();
    deleteLater();
}

void ParagraphSettingsDialog::open(const QTextCursor &cursor) {
    m_cursor = cursor;
    m_ownStyle = true;
    open( KoParagraphStyle::fromBlockFormat(m_cursor.blockFormat()) );
}

void ParagraphSettingsDialog::open(KoParagraphStyle *style) {
    m_style = style;
    m_paragraphIndentSpacing->open(style);
    m_paragraphLayout->open(style);
    m_paragraphBulletsNumbers->open(style);
}

void ParagraphSettingsDialog::setUnit(const KoUnit &unit) {
    m_paragraphIndentSpacing->setUnit(unit);
}

#include <ParagraphSettingsDialog.moc>
