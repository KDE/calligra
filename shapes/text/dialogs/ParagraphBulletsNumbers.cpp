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

#include "ParagraphBulletsNumbers.h"

#include <KoParagraphStyle.h>
#include <KoCharSelectDia.h>

#include <KDebug>

ParagraphBulletsNumbers::ParagraphBulletsNumbers(QWidget *parent)
    : QWidget(parent),
    m_paragStyle(0),
    m_style(0)
{
    widget.setupUi(this);

    addStyle(i18n( "None" ), KoListStyle::NoItem);
    addStyle(i18n( "Arabic" ), KoListStyle::DecimalItem);
    addStyle(i18n( "Lower Alphabetical" ), KoListStyle::AlphaLowerItem);
    addStyle(i18n( "Upper Alphabetical" ), KoListStyle::UpperAlphaItem);
    addStyle(i18n( "Lower Roman" ), KoListStyle::RomanLowerItem);
    addStyle(i18n( "Upper Roman" ), KoListStyle::UpperRomanItem);
    addStyle(i18n( "Disc Bullet" ), KoListStyle::DiscItem);
    addStyle(i18n( "Square Bullet" ), KoListStyle::SquareItem);
    addStyle(i18n( "Box Bullet" ), KoListStyle::BoxItem);
    addStyle(i18n( "Circle Bullet" ), KoListStyle::CircleItem);
    addStyle(i18n( "Custom Bullet" ), KoListStyle::CustomCharItem);

    addStyle(i18n("Bengali"), KoListStyle::Bengali);
    addStyle(i18n("Gujarati"), KoListStyle::Gujarati);
    addStyle(i18n("Gurumukhi"), KoListStyle::Gurumukhi);
    addStyle(i18n("Kannada"), KoListStyle::Kannada);
    addStyle(i18n("Malayalam"), KoListStyle::Malayalam);
    addStyle(i18n("Oriya"), KoListStyle::Oriya);
    addStyle(i18n("Tamil"), KoListStyle::Tamil);
    addStyle(i18n("Telugu"), KoListStyle::Telugu);
    addStyle(i18n("Tibetan"), KoListStyle::Tibetan);
    addStyle(i18n("Thai"), KoListStyle::Thai);
    addStyle(i18n("Abjad"), KoListStyle::Abjad);
    addStyle(i18n("AbjadMinor"), KoListStyle::AbjadMinor);
    addStyle(i18n("ArabicAlphabet"), KoListStyle::ArabicAlphabet);

    widget.alignment->addItem(i18nc("Automatic horizontal alignment", "Auto"));
    widget.alignment->addItem(i18n("Left"));
    widget.alignment->addItem(i18n("Right"));
    widget.alignment->addItem(i18n("Centered"));

    connect(widget.listTypes, SIGNAL(currentRowChanged(int)), this, SLOT(styleChanged(int)));
    connect(widget.customCharacter, SIGNAL(clicked(bool)), this, SLOT(customCharButtonPressed()));
}

void ParagraphBulletsNumbers::addStyle(const QString &text, KoListStyle::Style style) {
    m_mapping.insert(widget.listTypes->count(), style);
    widget.listTypes->addItem(text);
}

void ParagraphBulletsNumbers::open(KoParagraphStyle *style) {
    m_paragStyle = style;
    m_style = style->listStyle();
    widget.listPropertiesPane->setEnabled(m_style);
    widget.customCharacter->setText("-");
    if(m_style == 0) {
        widget.listTypes->setCurrentRow(0);
        return;
    }
    widget.prefix->setText(m_style->listItemPrefix());
    widget.suffix->setText(m_style->listItemSuffix());
    KoListStyle::Style s = m_style->style();
    foreach(int row, m_mapping.keys()) {
        if(m_mapping[row] == s) {
            widget.listTypes->setCurrentRow(row);
            break;
        }
    }
    int align;
kDebug() << "a: " << m_style->alignment() << endl;
    if(m_style->alignment() == (Qt::AlignLeft | Qt::AlignAbsolute))
        align = 1;
    else if(m_style->alignment() == (Qt::AlignRight | Qt::AlignAbsolute))
        align = 2;
    else if(m_style->alignment() == Qt::AlignCenter)
        align = 3;
    else
        align = 0;

    widget.alignment->setCurrentIndex(align);
    widget.depth->setValue(m_style->level());
    widget.levels->setValue(m_style->displayLevel());
    widget.startValue->setValue(m_style->startValue());
    if(s == KoListStyle::CustomCharItem)
        widget.customCharacter->setText(m_style->bulletCharacter());

    // *** features not in GUI;
    // character style
    // relative bullet size (percent)
    // minimum label width
}

void ParagraphBulletsNumbers::save() {
    Q_ASSERT(m_paragStyle);
    KoListStyle::Style style = m_mapping[widget.listTypes->currentRow()];
    if(style == KoListStyle::NoItem) {
        m_style = 0;
        m_paragStyle->removeListStyle();
        return;
    }
    if(m_style == 0) {
        KoListStyle ls;
        m_paragStyle->setListStyle(ls);
        m_style = m_paragStyle->listStyle();
        Q_ASSERT(m_style);
    }
    m_style->setStyle(style);
    m_style->setLevel(widget.depth->value());
    m_style->setDisplayLevel(widget.levels->value());
    m_style->setStartValue(widget.startValue->value());
    m_style->setListItemPrefix(widget.prefix->text());
    m_style->setListItemSuffix(widget.suffix->text());

    Qt::Alignment align;
    switch(widget.alignment->currentIndex()) {
        case 0: align = Qt::AlignLeft; break;
        case 1: align = Qt::AlignLeft | Qt::AlignAbsolute; break;
        case 2: align = Qt::AlignRight | Qt::AlignAbsolute; break;
        case 3: align = Qt::AlignCenter; break;
        default:
            Q_ASSERT(false);
    }
    m_style->setAlignment(align);
}

void ParagraphBulletsNumbers::styleChanged(int index) {
//kDebug() << "ParagraphBulletsNumbers::styleChanged\n";
    KoListStyle::Style style = m_mapping[index];
    switch( style ) {
        case KoListStyle::SquareItem:
        case KoListStyle::DiscItem:
        case KoListStyle::CircleItem:
        case KoListStyle::BoxItem:
        case KoListStyle::KoListStyle::CustomCharItem:
        case KoListStyle::KoListStyle::NoItem:
            widget.startValue->setCounterType(KoListStyle::DecimalItem);
            widget.startValue->setValue(1);
            widget.startValue->setEnabled(false);
            break;
        default:
            widget.startValue->setEnabled(true);
            widget.startValue->setCounterType(style);
            int value = widget.startValue->value();
            widget.startValue->setValue(value +1);
            widget.startValue->setValue(value); // surely to trigger a change event.
    }
    widget.listPropertiesPane->setEnabled(style != KoListStyle::NoItem);
}

void ParagraphBulletsNumbers::customCharButtonPressed() {
    QString font;
    QChar character;
    if(KoCharSelectDia::selectChar(font, character, this)) {
        widget.customCharacter->setText(character);
        widget.customCharacter->setFont(QFont(font));
    }
    // also switch to the custom list style.
    foreach(int row, m_mapping.keys()) {
        if(m_mapping[row] == KoListStyle::CustomCharItem) {
            widget.listTypes->setCurrentRow(row);
            break;
        }
    }
}

#include "ParagraphBulletsNumbers.moc"
