/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001, 2002, 2003 Montel Laurent <lmontel@mandrakesoft.com>
   SPDX-FileCopyrightText: 2006 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2009 Pierre Stirnweiss <pstirnweiss@googlemail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CHARACTERHIGHLIGHTING_H
#define CHARACTERHIGHLIGHTING_H

#include <ui_CharacterHighlighting.h>

#include "KoCharacterStyle.h"
#include <kfontchooser.h>

class QColor;

class CharacterHighlighting : public QWidget
{
    Q_OBJECT

public:
    explicit CharacterHighlighting(QWidget *parent = nullptr);
    ~CharacterHighlighting() override = default;

    void setDisplay(KoCharacterStyle *style, bool directFormattingMode);
    void save(KoCharacterStyle *style);

    QStringList capitalizationList();
    QStringList fontLayoutPositionList();

Q_SIGNALS:
    void underlineChanged(KoCharacterStyle::LineType, KoCharacterStyle::LineStyle, QColor);
    void strikethroughChanged(KoCharacterStyle::LineType, KoCharacterStyle::LineStyle, QColor);
    void capitalizationChanged(QFont::Capitalization);
    void fontChanged(const QFont &font);
    void textColorChanged(QColor);
    void backgroundColorChanged(QColor);
    void charStyleChanged();

private Q_SLOTS:
    void underlineTypeChanged(int item);
    void underlineStyleChanged(int item);
    void underlineColorChanged(QColor color);
    void strikethroughTypeChanged(int item);
    void strikethroughStyleChanged(int item);
    void strikethroughColorChanged(QColor color);
    void capitalisationChanged(int item);
    void positionChanged(int item);
    void textToggled(bool state);
    void backgroundToggled(bool state);
    void clearTextColor();
    void clearBackgroundColor();
    void textColorChanged();
    void backgroundColorChanged();

private:
    KoCharacterStyle::LineType indexToLineType(int index);
    KoCharacterStyle::LineStyle indexToLineStyle(int index);
    int lineTypeToIndex(KoCharacterStyle::LineType type);
    int lineStyleToIndex(KoCharacterStyle::LineStyle type);

    Ui::CharacterHighlighting widget;

    KFontChooser *m_fontChooser;

    bool m_underlineInherited;
    bool m_strikeoutInherited;
    bool m_capitalizationInherited;
    bool m_positionInherited;
    bool m_hyphenateInherited;
    bool m_textColorChanged;
    bool m_textColorReset;
    bool m_backgroundColorChanged;
    bool m_backgroundColorReset;
};

#endif
