/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 * Copyright (C)  2011 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
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

#include "ParagraphLayout.h"

#include <KoParagraphStyle.h>

ParagraphLayout::ParagraphLayout(QWidget *parent)
        : QWidget(parent)
{
    widget.setupUi(this);

    connect(widget.right, &QAbstractButton::toggled, this, &ParagraphLayout::slotAlignChanged);
    connect(widget.center, &QAbstractButton::toggled, this, &ParagraphLayout::slotAlignChanged);
    connect(widget.justify, &QAbstractButton::toggled, this, &ParagraphLayout::slotAlignChanged);
    connect(widget.left, &QAbstractButton::toggled, this, &ParagraphLayout::slotAlignChanged);
    connect(widget.keepTogether, &QCheckBox::stateChanged, this, &ParagraphLayout::keepTogetherChanged);
    connect(widget.keepWithNext, &QCheckBox::stateChanged, this, &ParagraphLayout::keepWithNextChanged);
    connect(widget.breakAfter, &QCheckBox::stateChanged, this, &ParagraphLayout::breakAfterChanged);
    connect(widget.breakBefore, &QCheckBox::stateChanged, this, &ParagraphLayout::breakBeforeChanged);
    connect(widget.threshold, QOverload<int>::of(&QSpinBox::valueChanged), this, &ParagraphLayout::thresholdValueChanged);
}

void ParagraphLayout::slotAlignChanged()
{
    Qt::Alignment align;
    if (widget.right->isChecked())
        align = Qt::AlignRight;
    else if (widget.center->isChecked())
        align = Qt::AlignHCenter;
    else if (widget.justify->isChecked())
        align = Qt::AlignJustify;
    else
        align = Qt::AlignLeft;

    m_alignmentInherited = false;

    emit parStyleChanged();
}

void ParagraphLayout::breakAfterChanged()
{
   m_breakAfterInherited = false;
   emit parStyleChanged();
}

void ParagraphLayout::keepTogetherChanged()
{
   m_keepTogetherInherited = false;
   emit parStyleChanged();
}

void ParagraphLayout::keepWithNextChanged()
{
    m_keepWithNextInherited = false;
    emit parStyleChanged();
}

void ParagraphLayout::breakBeforeChanged()
{
    m_breakBeforeInherited = false;
    emit parStyleChanged();
}

void ParagraphLayout::thresholdValueChanged()
{
    m_orphanThresholdInherited = false;
    emit parStyleChanged();
}

void ParagraphLayout::setDisplay(KoParagraphStyle *style, bool directFormattingMode)
{
    switch (style->alignment()) {
    case Qt::AlignRight: widget.right->setChecked(true); break;
    case Qt::AlignHCenter: widget.center->setChecked(true); break;
    case Qt::AlignJustify: widget.justify->setChecked(true); break;
    case Qt::AlignLeft:
    default:
        widget.left->setChecked(true); break;
    }

    m_alignmentInherited = directFormattingMode|| !style->hasProperty(QTextFormat::BlockAlignment);
    m_keepTogetherInherited = directFormattingMode|| !style->hasProperty(QTextFormat::BlockNonBreakableLines);
    m_keepWithNextInherited = directFormattingMode || !style->hasProperty(KoParagraphStyle::KeepWithNext);
    m_breakAfterInherited = directFormattingMode|| !style->hasProperty(KoParagraphStyle::BreakAfter);
    m_breakBeforeInherited = directFormattingMode|| !style->hasProperty(KoParagraphStyle::BreakBefore);
    m_orphanThresholdInherited = directFormattingMode|| !style->hasProperty(KoParagraphStyle::OrphanThreshold);

    widget.keepTogether->setChecked(style->nonBreakableLines());
    widget.keepWithNext->setChecked(style->keepWithNext());
    widget.breakBefore->setChecked(style->breakBefore());
    widget.breakAfter->setChecked(style->breakAfter());

    widget.threshold->setValue(style->orphanThreshold());
}

void ParagraphLayout::save(KoParagraphStyle *style)
{
    if (!m_alignmentInherited) {
        Qt::Alignment align;
        if (widget.right->isChecked())
            align = Qt::AlignRight;
        else if (widget.center->isChecked())
            align = Qt::AlignHCenter;
        else if (widget.justify->isChecked())
            align = Qt::AlignJustify;
        else
            align = Qt::AlignLeft;
        style->setAlignment(align);
    }

    if (!m_keepTogetherInherited) {
        style->setNonBreakableLines(widget.keepTogether->isChecked());
    }
    if (!m_keepWithNextInherited) {
        style->setKeepWithNext(widget.keepWithNext->isChecked());
    }
    if (!m_breakBeforeInherited) {
        if (widget.breakBefore->isChecked())
            style->setBreakBefore(KoText::PageBreak);
        else
            style->setBreakBefore(KoText::NoBreak);
    }
    if (!m_breakAfterInherited) {
        if (widget.breakAfter->isChecked())
            style->setBreakAfter(KoText::PageBreak);
        else
            style->setBreakAfter(KoText::NoBreak);
    }

    if (!m_orphanThresholdInherited) {
        style->setOrphanThreshold(widget.threshold->value());
    }
}
