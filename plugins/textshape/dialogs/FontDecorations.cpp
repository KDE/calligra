/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001, 2002 Montel Laurent <lmontel@mandrakesoft.com>
   SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
   SPDX-FileCopyrightText: 2009 Pierre Stirnweiss <pstirnweiss@googlemail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "FontDecorations.h"

FontDecorations::FontDecorations(bool uniqueFormat, QWidget *parent)
    : QWidget(parent)
    , m_uniqueFormat(uniqueFormat)
{
    widget.setupUi(this);

    connect(widget.hyphenate, &QCheckBox::stateChanged, this, &FontDecorations::hyphenateStateChanged);

    widget.shadowGroupBox->setVisible(false);
    widget.positionGroupBox->setVisible(false);
}

void FontDecorations::hyphenateStateChanged()
{
    m_hyphenateInherited = false;
}

void FontDecorations::setDisplay(KoCharacterStyle *style)
{
    if (!style)
        return;

    m_hyphenateInherited = !style->hasProperty(KoCharacterStyle::HasHyphenation);
    if (!m_uniqueFormat) {
        widget.hyphenate->setTristate(true);
        widget.hyphenate->setCheckState(Qt::PartiallyChecked);
    } else
        widget.hyphenate->setChecked(style->hasHyphenation());
}

void FontDecorations::save(KoCharacterStyle *style) const
{
    if (!style)
        return;

    if (!m_hyphenateInherited) {
        if (widget.hyphenate->checkState() == Qt::Checked)
            style->setHasHyphenation(true);
        else if (widget.hyphenate->checkState() == Qt::Unchecked)
            style->setHasHyphenation(false);
    }
}
