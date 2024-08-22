/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001, 2002, 2003 Montel Laurent <lmontel@mandrakesoft.com>
   SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef FONTDECORATIONS_H
#define FONTDECORATIONS_H

#include <ui_FontDecorations.h>

#include <KoCharacterStyle.h>

class FontDecorations : public QWidget
{
    Q_OBJECT

public:
    explicit FontDecorations(bool uniqueFormat, QWidget *parent = nullptr);
    ~FontDecorations() override = default;

    void setDisplay(KoCharacterStyle *style);
    void save(KoCharacterStyle *style) const;

private Q_SLOTS:
    void hyphenateStateChanged();

private:
    Ui::FontDecorations widget;

    bool m_hyphenateInherited;
    bool m_uniqueFormat;
};

#endif
