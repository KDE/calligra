/* This file is part of the KDE project
   Copyright (C)  2001, 2002 Montel Laurent <lmontel@mandrakesoft.com>
   Copyright (C)  2006 Thomas Zander <zander@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
*/

#include "CharacterHighlighting.h"

#include <KoText.h>
#include <KoCharacterStyle.h>

#include <kdebug.h>

CharacterHighlighting::CharacterHighlighting( QWidget* parent)
    : QWidget ( parent)
{
    widget.setupUi(this);

    widget.underlineStyle->addItems( KoText::underlineTypeList() );
    widget.underlineLineStyle->addItems( KoText::underlineStyleList() );

    connect( widget.underlineStyle, SIGNAL( activated( int ) ), this, SLOT( underlineChanged( int ) ) );
}

void CharacterHighlighting::underlineChanged(int item) {
    widget.underlineLineStyle->setEnabled(item != 0);
    widget.underlineColor->setEnabled(item != 0);
}

void CharacterHighlighting::open(KoCharacterStyle *style) {
    m_style = style;
    if(m_style == 0)
        return;

    widget.underlineStyle->setCurrentIndex(1);
    switch(style->underlineStyle()) {
        case QTextCharFormat::SingleUnderline:
            widget.underlineLineStyle->setCurrentIndex(0);
            break;
        case QTextCharFormat::DashUnderline:
            widget.underlineLineStyle->setCurrentIndex(1);
            break;
        case QTextCharFormat::DotLine:
            widget.underlineLineStyle->setCurrentIndex(2);
            break;
        case QTextCharFormat::DashDotLine:
            widget.underlineLineStyle->setCurrentIndex(3);
            break;
        case QTextCharFormat::DashDotDotLine:
            widget.underlineLineStyle->setCurrentIndex(4);
            break;
        case QTextCharFormat::WaveUnderline:
            widget.underlineLineStyle->setCurrentIndex(5);
            break;
        default:
            widget.underlineStyle->setCurrentIndex(0);
            break;
    }
    if(style->underlineType() == KoCharacterStyle::NoLineType)
        widget.underlineStyle->setCurrentIndex(0);
    // TODO represent single and double lines.

    switch(style->transform()) {
        case KoCharacterStyle::NoTransform: widget.normal->setChecked(true); break;
        case KoCharacterStyle::SmallCaps: widget.smallcaps->setChecked(true); break;
        case KoCharacterStyle::Uppercase: widget.uppercase->setChecked(true); break;
        case KoCharacterStyle::Lowercase: widget.lowercase->setChecked(true); break;
        case KoCharacterStyle::Capitalize: widget.capitalize->setChecked(true); break;
    }

    underlineChanged(widget.underlineStyle->currentIndex());
    widget.underlineColor->setColor(style->underlineColor());

    widget.strikethrough->setChecked(style->strikeOutStyle() != KoCharacterStyle::NoLineStyle);
}

void CharacterHighlighting::save() {
    if(m_style == 0)
        return;
    switch(widget.underlineStyle->currentIndex()) {
        case 0:
            m_style->setUnderlineType(KoCharacterStyle::NoLineType);
            m_style->setUnderlineStyle(KoCharacterStyle::NoLineStyle);
            break;
        case 1:
            m_style->setUnderlineType(KoCharacterStyle::SingleLine);
            QTextCharFormat::UnderlineStyle style;
            switch(widget.underlineLineStyle->currentIndex()) {
                case 0: style = QTextCharFormat::SingleUnderline; break;
                case 1: style = QTextCharFormat::DashUnderline; break;
                case 2: style = QTextCharFormat::DotLine; break;
                case 3: style = QTextCharFormat::DashDotLine; break;
                case 4: style = QTextCharFormat::DashDotDotLine; break;
                case 5: style = QTextCharFormat::WaveUnderline; break;
                default:
                    style = QTextCharFormat::SingleUnderline; break;
                    kWarning() << "Unknown items in the underlineLineStyle combobox!\n";
            }
            m_style->setUnderlineStyle((KoCharacterStyle::LineStyle) style);
            m_style->setUnderlineColor(widget.underlineColor->color());
            break;
        case 2: // double underlining unsupported by Qt right now :(  TODO
            m_style->setUnderlineType(KoCharacterStyle::DoubleLine);
            m_style->setUnderlineStyle(KoCharacterStyle::SolidLine);
            m_style->setUnderlineColor(widget.underlineColor->color());
            break;
        default:
            kWarning() << "Unknown items in the underlineStyle combobox!\n";
    }

    if (widget.strikethrough->isChecked())
        m_style->setStrikeOutStyle(KoCharacterStyle::SolidLine);
    else
        m_style->setStrikeOutStyle(KoCharacterStyle::NoLineStyle);

    if (widget.normal->isChecked())
        m_style->setTransform(KoCharacterStyle::NoTransform);
    else if (widget.smallcaps->isChecked())
        m_style->setTransform(KoCharacterStyle::SmallCaps);
    else if (widget.uppercase->isChecked())
        m_style->setTransform(KoCharacterStyle::Uppercase);
    else if (widget.lowercase->isChecked())
        m_style->setTransform(KoCharacterStyle::Lowercase);
    else if (widget.capitalize->isChecked())
        m_style->setTransform(KoCharacterStyle::Capitalize);
}

#include "CharacterHighlighting.moc"

