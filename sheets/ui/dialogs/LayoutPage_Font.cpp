/* This file is part of the KDE project
             SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
             SPDX-FileCopyrightText: 2004-2022 Tomas Mecir <mecirt@gmail.com>
             SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
             SPDX-FileCopyrightText: 2002-2004 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 1999-2002 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 1999-2002 Harri Porten <porten@kde.org>
             SPDX-FileCopyrightText: 2000-2001 David Faure <faure@kde.org>
             SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>
             SPDX-FileCopyrightText: 2000 Werner Trobin <trobin@kde.org>
             SPDX-FileCopyrightText: 1999 Reginald Stadlbauer <reggie@kde.org>
             SPDX-FileCopyrightText: 1998-1999 Stephan Kulow <coolo@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "LayoutPage_Font.h"

#include <QFontDatabase>

using namespace Calligra::Sheets;

LayoutPageFont::LayoutPageFont(QWidget *parent)
    : QWidget(parent)
{
    setupUi(this);

    connect(textColorButton, &KColorButton::changed, this, &LayoutPageFont::slotSetTextColor);

    connect(family_combo, &QListWidget::currentTextChanged, this, &LayoutPageFont::display_example);

    connect(size_spinner, qOverload<int>(&QSpinBox::valueChanged), this, &LayoutPageFont::display_example);

    connect(weight_combo, qOverload<int>(&QComboBox::activated), this, &LayoutPageFont::display_example);

    connect(style_combo, qOverload<int>(&QComboBox::activated), this, &LayoutPageFont::display_example);

    connect(strike, &QAbstractButton::clicked, this, &LayoutPageFont::display_example);

    connect(underline, &QAbstractButton::clicked, this, &LayoutPageFont::display_example);

    example_label->setText(i18n("Dolor Ipse"));

    underline->setStyleKey(Style::FontUnderline);
    strike->setStyleKey(Style::FontStrike);

    this->resize(400, 400);
}

void LayoutPageFont::slotSetTextColor(const QColor &)
{
    m_colorChanged = true;
}

void LayoutPageFont::apply(Style *style, bool partial)
{
    Style defaultStyle;

    // font family
    int idx = 0;
    auto items = family_combo->selectedItems();
    if (items.size()) {
        idx = family_combo->row(items[0]);
        QString fontFamily = items[0]->text();
        if (idx && shouldApplyLayoutChange(fontFamily == defaultStyle.fontFamily(), fontFamily != m_family, partial))
            style->setFontFamily(fontFamily);
    }

    // font size
    int fontSize = size_spinner->value();
    if (fontSize && shouldApplyLayoutChange(fontSize == defaultStyle.fontSize(), fontSize != m_size, partial))
        style->setFontSize(fontSize);

    // bold and italic
    idx = weight_combo->currentIndex();
    if (idx && shouldApplyLayoutChange(idx == 1, true, partial))
        style->setFontBold(idx == 2);
    idx = style_combo->currentIndex();
    if (idx && shouldApplyLayoutChange(idx == 1, true, partial))
        style->setFontItalic(idx == 2);

    // underline + strikeout
    underline->saveTo(*style, partial);
    strike->saveTo(*style, partial);

    // font color
    QColor fontColor = textColorButton->color();
    if (shouldApplyLayoutChange(fontColor == style->fontColor(), m_colorChanged, partial))
        style->setFontColor(fontColor);
}

void LayoutPageFont::loadFrom(const Style &style, bool partial)
{
    // font family
    m_family = QString();
    family_combo->clear();
    QFontDatabase fontDatabase;
    family_combo->addItem("");
    family_combo->addItems(fontDatabase.families());

    if (partial && (!style.hasAttribute(Style::FontFamily))) {
        family_combo->setCurrentRow(0);
    } else {
        m_family = style.fontFamily();
        auto matches = family_combo->findItems(m_family, Qt::MatchExactly);
        if (matches.size() == 0)
            family_combo->setCurrentRow(0);
        else
            family_combo->setCurrentItem(matches[0]);
    }

    // font size
    m_size = 0;
    if (partial && (!style.hasAttribute(Style::FontSize))) {
        size_spinner->setValue(0);
    } else {
        m_size = style.fontSize();
        size_spinner->setValue(m_size);
    }

    // bold and italic
    if (partial && (!style.hasAttribute(Style::FontBold))) {
        weight_combo->setCurrentIndex(0);
    } else {
        bool val = style.bold();
        weight_combo->setCurrentIndex(val ? 2 : 1);
    }
    if (partial && (!style.hasAttribute(Style::FontItalic))) {
        style_combo->setCurrentIndex(0);
    } else {
        bool val = style.italic();
        style_combo->setCurrentIndex(val ? 2 : 1);
    }

    // underline + strikeout
    underline->loadFrom(style, partial);
    strike->loadFrom(style, partial);

    // font color
    m_colorChanged = false;
    m_color = style.fontColor(); // default or the chosen one
    textColorButton->setColor(m_color);
}

void LayoutPageFont::display_example()
{
    Style defaultStyle;

    auto items = family_combo->selectedItems();
    QString family = items.size() ? items[0]->text() : defaultStyle.fontFamily();
    int fontSize = size_spinner->value();
    if (!fontSize)
        fontSize = defaultStyle.fontSize();

    QFont f(family, fontSize);
    if (weight_combo->currentIndex() == 2)
        f.setBold(true);
    if (style_combo->currentIndex() == 2)
        f.setItalic(true);
    if (underline->checkState() == Qt::Checked)
        f.setUnderline(true);
    if (strike->checkState() == Qt::Checked)
        f.setStrikeOut(true);

    example_label->setFont(f);
    example_label->repaint();
}
