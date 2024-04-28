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

#include "LayoutPage_Position.h"
#include "LayoutWidgets.h"

#include <KoUnitDoubleSpinBox.h>
#include <QButtonGroup>

using namespace Calligra::Sheets;

LayoutPagePosition::LayoutPagePosition(QWidget *parent, KoUnit unit)
    : QWidget(parent)
{
    setupUi(this);
    connect(angleRotation, &QAbstractSlider::valueChanged, spinBox3, &QSpinBox::setValue);
    connect(spinBox3, QOverload<int>::of(&QSpinBox::valueChanged), angleRotation, &QAbstractSlider::setValue);

    QButtonGroup *horizontalGroup = new QButtonGroup(this);
    horizontalGroup->addButton(left);
    horizontalGroup->addButton(center);
    horizontalGroup->addButton(right);
    horizontalGroup->addButton(standard);
    connect(horizontalGroup, &QButtonGroup::idClicked, this, &LayoutPagePosition::slotStateChanged);

    QGridLayout *grid2 = new QGridLayout(indentGroup);
    grid2->addItem(new QSpacerItem(0, indentGroup->fontMetrics().height() / 8), 0, 0); // groupbox title
    m_indent = new KoUnitDoubleSpinBox(indentGroup);
    m_indent->setMinimum(0.0);
    m_indent->setMaximum(400.0);
    m_indent->setLineStepPt(10.0);
    m_indent->setUnit(unit);
    grid2->addWidget(m_indent, 0, 0);

    multi->setStyleKey(Style::MultiRow);
    vertical->setStyleKey(Style::VerticalText);
    shrinkToFit->setStyleKey(Style::ShrinkToFit);

    connect(vertical, &QAbstractButton::clicked, this, &LayoutPagePosition::updateElements);
    connect(shrinkToFit, &QAbstractButton::clicked, this, &LayoutPagePosition::updateElements);
    connect(multi, &QAbstractButton::clicked, this, &LayoutPagePosition::updateElements);
    connect(angleRotation, &QAbstractSlider::valueChanged, this, &LayoutPagePosition::updateElements);

    slotStateChanged(0);
    this->resize(400, 400);
}

void LayoutPagePosition::slotStateChanged(int)
{
    updateElements();
}

void LayoutPagePosition::updateElements()
{
    if (right->isChecked() || center->isChecked())
        m_indent->setEnabled(false);
    else
        m_indent->setEnabled(true);

    if (angleRotation->value() == 0) {
        multi->setEnabled(true);
        vertical->setEnabled(true);
        shrinkToFit->setEnabled(true);
    } else {
        multi->setEnabled(false);
        vertical->setEnabled(false);
        shrinkToFit->setEnabled(false);
    }
}

void LayoutPagePosition::apply(Style *style, bool partial)
{
    Style defaultStyle;

    // Horizontal
    Style::HAlign halign = Style::HAlignUndefined;
    if (left->isChecked())
        halign = Style::Left;
    else if (center->isChecked())
        halign = Style::Center;
    else if (right->isChecked())
        halign = Style::Right;
    if (shouldApplyLayoutChange(halign == defaultStyle.halign(), halign != m_halign, partial))
        style->setHAlign(halign);

    // Vertical
    Style::VAlign valign = Style::VAlignUndefined;
    if (top->isChecked())
        valign = Style::Top;
    else if (middle->isChecked())
        valign = Style::Middle;
    else if (bottom->isChecked())
        valign = Style::Bottom;
    if (shouldApplyLayoutChange(valign == defaultStyle.valign(), valign != m_valign, partial))
        style->setVAlign(valign);

    // Rotation
    int angle = -1 * angleRotation->value();
    if (shouldApplyLayoutChange(angle == defaultStyle.angle(), angle != m_angle, partial))
        style->setAngle(angle);

    // Text Option
    if (multi->isEnabled())
        multi->saveTo(*style, partial);
    if (vertical->isEnabled())
        vertical->saveTo(*style, partial);
    if (shrinkToFit->isEnabled())
        shrinkToFit->saveTo(*style, partial);

    // Indent
    if (m_indent->isEnabled()) {
        double indent = m_indent->value();
        if (shouldApplyLayoutChange(indent == defaultStyle.indentation(), indent != m_indentation, partial))
            style->setIndentation(indent);
    }
}

void LayoutPagePosition::loadFrom(const Style &style, bool partial)
{
    // Horizontal
    m_halign = style.halign();
    if (m_halign == Style::Left)
        left->setChecked(true);
    else if (m_halign == Style::Center)
        center->setChecked(true);
    else if (m_halign == Style::Right)
        right->setChecked(true);
    else
        standard->setChecked(true);

    // Vertical
    m_valign = style.valign();
    if (m_valign == Style::Top)
        top->setChecked(true);
    else if (m_valign == Style::Middle)
        middle->setChecked(true);
    else if (m_valign == Style::Bottom)
        bottom->setChecked(true);

    // Rotation
    m_angle = style.angle();
    angleRotation->setValue(-1 * m_angle);
    spinBox3->setValue(-1 * m_angle);

    // Text Option
    multi->loadFrom(style, partial);
    vertical->loadFrom(style, partial);
    shrinkToFit->loadFrom(style, partial);

    // Indent
    m_indentation = style.indentation();
    m_indent->changeValue(m_indentation);

    updateElements();
}
