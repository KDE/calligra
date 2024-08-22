/* This file is part of the KDE project

   SPDX-FileCopyrightText: 2011 Mojtaba Shahi Senobari <mojtaba.shahi3000@gmail.com>
   SPDX-FileCopyrightText: 2012 Gopalakrishna Bhat A <gopalakbhat@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "ParagraphDropCaps.h"

#include "KoParagraphStyle.h"
#include <QMessageBox>

ParagraphDropCaps::ParagraphDropCaps(QWidget *parent)
    : QWidget(parent)
{
    widget.setupUi(this);
    widget.mainLayout->insertStretch(0);
    widget.mainLayout->insertStretch(2);

    widget.distance->changeValue(0);
    widget.characters->setSpecialValueText(i18n("Whole Word"));
    widget.characters->setValue(0);
    widget.lines->setValue(2);

    connect(widget.capsState, &QCheckBox::stateChanged, this, &ParagraphDropCaps::dropCapsStateChanged);
    connect(widget.distance, &KoUnitDoubleSpinBox::valueChangedPt, this, &ParagraphDropCaps::paragraphDistanceChanged);
    connect(widget.characters, &QSpinBox::valueChanged, this, &ParagraphDropCaps::dropedCharacterCountChanged);
    connect(widget.lines, &QSpinBox::valueChanged, this, &ParagraphDropCaps::dropsLineSpanChanged);
}

void ParagraphDropCaps::setSettingEnabled(bool enabled)
{
    widget.distanceLabel->setEnabled(enabled);
    widget.distance->setEnabled(enabled);
    widget.charactersLabel->setEnabled(enabled);
    widget.characters->setEnabled(enabled);
    widget.linesLabel->setEnabled(enabled);
    widget.lines->setEnabled(enabled);
}

void ParagraphDropCaps::dropCapsStateChanged()
{
    if (widget.capsState->isChecked()) {
        setSettingEnabled(true);
        m_dropCapsInherited = false;
    } else {
        setSettingEnabled(false);
    }
    Q_EMIT parStyleChanged();
}

void ParagraphDropCaps::setDisplay(KoParagraphStyle *style, bool directFormattingMode)
{
    if (!style) {
        return;
    }
    if (!style->dropCaps()) {
        setSettingEnabled(false);
        return;
    }

    widget.capsState->setChecked(true);
    widget.distance->changeValue(style->dropCapsDistance());
    widget.characters->setValue(style->dropCapsLength());
    widget.lines->setValue(style->dropCapsLines());

    m_dropCapsInherited = directFormattingMode || !style->hasProperty(KoParagraphStyle::DropCaps);
    m_capsDistanceInherited = directFormattingMode || !style->hasProperty(KoParagraphStyle::DropCapsDistance);
    m_capsLengthInherited = directFormattingMode || !style->hasProperty(KoParagraphStyle::DropCapsLength);
    m_capsLinesInherited = directFormattingMode || !style->hasProperty(KoParagraphStyle::DropCapsLines);
}

void ParagraphDropCaps::save(KoParagraphStyle *style)
{
    if (!style)
        return;

    if (!m_dropCapsInherited) {
        style->setDropCaps(widget.capsState->isChecked());
    }

    if (!m_capsDistanceInherited) {
        style->setDropCapsDistance(widget.distance->value());
    }

    if (!m_capsLengthInherited) {
        style->setDropCapsLength(widget.characters->value());
    }

    if (!m_capsLinesInherited) {
        style->setDropCapsLines(widget.lines->value());
    }
}

void ParagraphDropCaps::setUnit(const KoUnit &unit)
{
    widget.distance->setUnit(unit);
}

void ParagraphDropCaps::paragraphDistanceChanged(qreal distance)
{
    Q_UNUSED(distance);
    m_capsDistanceInherited = false;
    Q_EMIT parStyleChanged();
}

void ParagraphDropCaps::dropsLineSpanChanged(int lineSpan)
{
    Q_UNUSED(lineSpan);
    m_capsLinesInherited = false;
    Q_EMIT parStyleChanged();
}

void ParagraphDropCaps::dropedCharacterCountChanged(int count)
{
    Q_UNUSED(count);
    m_capsLengthInherited = false;
    Q_EMIT parStyleChanged();
}
