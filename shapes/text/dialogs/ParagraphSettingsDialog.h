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
#ifndef PARAGRAPHSETTINGSDIALOG_H
#define PARAGRAPHSETTINGSDIALOG_H

#include <KPageDialog>

class ParagraphBulletsNumbers;
class ParagraphIndentSpacing;
class ParagraphLayout;

/// A dialog to show the settings for a paragraph
class ParagraphSettingsDialog : public KPageDialog {
    Q_OBJECT
public:
    explicit ParagraphSettingsDialog(QWidget *parent);

private:
    void accept();
    void reject();

    ParagraphIndentSpacing *m_paragraphIndentSpacing;
    ParagraphLayout *m_paragraphLayout;
    ParagraphBulletsNumbers *m_paragraphBulletsNumbers;
};

#endif
