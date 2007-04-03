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

#include <KDebug>

ParagraphBulletsNumbers::ParagraphBulletsNumbers(QWidget *parent)
    : QWidget(parent)
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

    connect(widget.listTypes, SIGNAL(currentRowChanged(int)), this, SLOT(styleChanged(int)));
}

void ParagraphBulletsNumbers::addStyle(const QString &text, KoListStyle::Style style) {
    m_mapping.insert(widget.listTypes->count(), style);
    widget.listTypes->addItem(text);
}

void ParagraphBulletsNumbers::open(KoParagraphStyle *style) {
    m_style = style->listStyle();
    widget.listPropertiesPane->setEnabled(m_style);
    if(m_style == 0)
        return;
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
    switch(m_style->alignment()) {
        case Qt::AlignLeft: align = 0; break;
        case Qt::AlignRight: align = 1; break;
        default: align = 2;
    }
    widget.alignment->setCurrentIndex(align);
    widget.depth->setValue(m_style->level());
    widget.levels->setValue(m_style->displayLevel());
    widget.startValue->setValue(m_style->startValue());

    // *** features not in GUI;
    // character style
    // relative bullet size (percent)
    // minimum label width
}

void ParagraphBulletsNumbers::save() {
    if(m_style == 0) return;
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
    if(style == KoListStyle::CustomCharItem)
        widget.customCharacter->setText(m_style->bulletCharacter());
    else
        widget.customCharacter->setText("-");

    widget.listPropertiesPane->setEnabled(style != KoListStyle::NoItem);
}

#include "ParagraphBulletsNumbers.moc"
