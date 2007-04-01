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


ParagraphSettingsDialog::ParagraphSettingsDialog(QWidget *parent)
    : KPageDialog(parent)
{
    setFaceType(KPageDialog::Tabbed);
    m_paragraphIndentSpacing = new ParagraphIndentSpacing (this);
    m_paragraphLayout = new ParagraphLayout (this);
    m_paragraphBulletsNumbers = new ParagraphBulletsNumbers (this);

    addPage(m_paragraphIndentSpacing, "Indent/Spacing");
    addPage(m_paragraphLayout, "General Layout");
    addPage(m_paragraphBulletsNumbers, "Bullets/Numbers");
}

void ParagraphSettingsDialog::accept() {
    QDialog::accept();
    deleteLater();
}

void ParagraphSettingsDialog::reject() {
    QDialog::reject();
    deleteLater();
}

#include <ParagraphSettingsDialog.moc>
