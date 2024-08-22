/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001, 2002, 2003 Montel Laurent <lmontel@mandrakesoft.com>
   SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef PARAGRAPHDECORATIONS_H
#define PARAGRAPHDECORATIONS_H

#include <ui_ParagraphDecorations.h>

#include <KoParagraphStyle.h>

class ParagraphDecorations : public QWidget
{
    Q_OBJECT

public:
    explicit ParagraphDecorations(QWidget *parent = nullptr);
    ~ParagraphDecorations() override = default;

    void setDisplay(KoParagraphStyle *style, bool directFormattingMode);
    void save(KoParagraphStyle *style) const;

Q_SIGNALS:
    void parStyleChanged();

private Q_SLOTS:
    void clearBackgroundColor();
    void slotBackgroundColorChanged();

private:
    Ui::ParagraphDecorations widget;

    bool m_backgroundColorChanged, m_backgroundColorReset;
};

#endif
