/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2011 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ParagraphLayout.h"

#include <KoParagraphStyle.h>

ParagraphLayout::ParagraphLayout(QWidget *parent)
    : QWidget(parent)
{
    widget.setupUi(this);
    widget.mainLayout->insertStretch(0);
    widget.mainLayout->insertStretch(2);

    connect(widget.right, &QAbstractButton::toggled, this, &ParagraphLayout::slotAlignChanged);
    connect(widget.center, &QAbstractButton::toggled, this, &ParagraphLayout::slotAlignChanged);
    connect(widget.justify, &QAbstractButton::toggled, this, &ParagraphLayout::slotAlignChanged);
    connect(widget.left, &QAbstractButton::toggled, this, &ParagraphLayout::slotAlignChanged);
    connect(widget.keepTogether, &QCheckBox::stateChanged, this, &ParagraphLayout::keepTogetherChanged);
    connect(widget.breakAfter, &QCheckBox::stateChanged, this, &ParagraphLayout::breakAfterChanged);
    connect(widget.breakBefore, &QCheckBox::stateChanged, this, &ParagraphLayout::breakBeforeChanged);
    connect(widget.threshold, &QSpinBox::valueChanged, this, &ParagraphLayout::thresholdValueChanged);
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

    Q_EMIT parStyleChanged();
}

void ParagraphLayout::breakAfterChanged()
{
    m_breakAfterInherited = false;
    Q_EMIT parStyleChanged();
}

void ParagraphLayout::keepTogetherChanged()
{
    m_keepTogetherInherited = false;
    Q_EMIT parStyleChanged();
}

void ParagraphLayout::breakBeforeChanged()
{
    m_breakBeforeInherited = false;
    Q_EMIT parStyleChanged();
}

void ParagraphLayout::thresholdValueChanged()
{
    m_orphanThresholdInherited = false;
    Q_EMIT parStyleChanged();
}

void ParagraphLayout::setDisplay(KoParagraphStyle *style, bool directFormattingMode)
{
    switch (style->alignment()) {
    case Qt::AlignRight:
        widget.right->setChecked(true);
        break;
    case Qt::AlignHCenter:
        widget.center->setChecked(true);
        break;
    case Qt::AlignJustify:
        widget.justify->setChecked(true);
        break;
    case Qt::AlignLeft:
    default:
        widget.left->setChecked(true);
        break;
    }

    m_alignmentInherited = directFormattingMode || !style->hasProperty(QTextFormat::BlockAlignment);
    m_keepTogetherInherited = directFormattingMode || !style->hasProperty(QTextFormat::BlockNonBreakableLines);
    m_breakAfterInherited = directFormattingMode || !style->hasProperty(KoParagraphStyle::BreakAfter);
    m_breakBeforeInherited = directFormattingMode || !style->hasProperty(KoParagraphStyle::BreakBefore);
    m_orphanThresholdInherited = directFormattingMode || !style->hasProperty(KoParagraphStyle::OrphanThreshold);

    widget.keepTogether->setChecked(style->nonBreakableLines());
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
