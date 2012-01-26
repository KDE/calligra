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

    connect(widget.right, SIGNAL(toggled(bool)), this, SLOT(slotAlignChanged()));
    connect(widget.center, SIGNAL(toggled(bool)), this, SLOT(slotAlignChanged()));
    connect(widget.justify, SIGNAL(toggled(bool)), this, SLOT(slotAlignChanged()));
    connect(widget.left, SIGNAL(toggled(bool)), this, SLOT(slotAlignChanged()));
    connect(widget.keepTogether, SIGNAL(stateChanged(int)), this, SLOT(slotKeepTogetherChanged()));
    connect(widget.breakAfter, SIGNAL(stateChanged(int)), this, SLOT(breakAfterChanged()));
    connect(widget.breakBefore, SIGNAL(stateChanged(int)), this, SLOT(breakBeforeChanged()));
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

    if (!m_ignoreSignals) {
        m_alignmentInherited = false;
    }

    emit horizontalAlignmentChanged(align);
}

void ParagraphLayout::slotKeepTogetherChanged()
{
    if (!m_ignoreSignals) {
        m_nonBreakableLineInherited = false;
    }
}

void ParagraphLayout::breakAfterChanged()
{
    if (!m_ignoreSignals) {
        m_breakAfterInherited = false;
    }
}

void ParagraphLayout::breakBeforeChanged()
{
    if (!m_ignoreSignals) {
        m_breakBeforeInherited = false;
    }
}

void ParagraphLayout::setDisplay(KoParagraphStyle *style)
{
    m_ignoreSignals = true;
    switch (style->alignment()) {
    case Qt::AlignRight: widget.right->setChecked(true); break;
    case Qt::AlignHCenter: widget.center->setChecked(true); break;
    case Qt::AlignJustify: widget.justify->setChecked(true); break;
    case Qt::AlignLeft:
    default:
        widget.left->setChecked(true); break;
    }

    m_alignmentInherited = !style->hasProperty(QTextFormat::BlockAlignment);
    m_breakAfterInherited = !style->hasProperty(KoParagraphStyle::BreakAfter);
    m_breakBeforeInherited = !style->hasProperty(KoParagraphStyle::BreakBefore);
    m_nonBreakableLineInherited = !style->hasProperty(QTextFormat::BlockNonBreakableLines);

    widget.keepTogether->setChecked(style->nonBreakableLines());
    widget.breakBefore->setChecked(style->breakBefore());
    widget.breakAfter->setChecked(style->breakAfter());
    m_ignoreSignals = false;
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

    if (!m_nonBreakableLineInherited) {
        style->setNonBreakableLines(widget.keepTogether->isChecked());
    }
    if (!m_breakBeforeInherited){
        if (widget.breakBefore->isChecked())
            style->setBreakBefore(KoText::PageBreak);
        else
            style->setBreakBefore(KoText::NoBreak);
    }
    if (!m_breakAfterInherited){
        if (widget.breakAfter->isChecked())
            style->setBreakAfter(KoText::PageBreak);
        else
            style->setBreakAfter(KoText::NoBreak);
    }
}

#include <ParagraphLayout.moc>
