/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002-2004 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 1999-2002 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2000-2001 Werner Trobin <trobin@kde.org>
             SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "SeriesDialog.h"

#include <KLocalizedString>
#include <KMessageBox>

#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>

using namespace Calligra::Sheets;

SeriesDialog::SeriesDialog(QWidget *parent)
    : ActionDialog(parent)
{
    setCaption(i18n("Series"));
    setObjectName(QLatin1String("SeriesDialog"));
    setButtonText(Apply, i18n("Insert Series"));

    QWidget *page = new QWidget();
    setMainWidget(page);

    QVBoxLayout *grid1 = new QVBoxLayout(page);

    QGroupBox *gb1 = new QGroupBox(i18n("Insert Values"), page);
    QHBoxLayout *gb1layout = new QHBoxLayout(gb1);
    column = new QRadioButton(i18n("Vertical"), gb1);
    column->setWhatsThis(i18n("Insert the series vertically, one below the other"));
    row = new QRadioButton(i18n("Horizontal"), gb1);
    row->setWhatsThis(i18n("Insert the series horizontally, from left to right"));
    column->setChecked(true);

    gb1layout->addWidget(column);
    gb1layout->addWidget(row);

    QGroupBox *gb2 = new QGroupBox(i18n("Type"), page);
    QHBoxLayout *gb2layout = new QHBoxLayout(gb2);
    linear = new QRadioButton(i18n("Linear (2,4,6,...)"), gb2);
    linear->setWhatsThis(
        i18n("Generate a series from 'start' to 'end' and for each step add "
             "the value provided in step. This creates a series where each value "
             "is 'step' larger than the value before it."));
    geometric = new QRadioButton(i18n("Geometric (2,4,8,...)"), gb2);
    geometric->setWhatsThis(
        i18n("Generate a series from 'start' to 'end' and for each step multiply "
             "the value with the value provided in step. Using a step of 5 produces a list like: "
             "5, 25, 125, 625 since 5 multiplied by 5 (step) equals 25, and that multiplied by 5 equals 125, "
             "which multiplied by the same step-value of 5 equals 625."));
    linear->setChecked(true);

    gb2layout->addWidget(linear);
    gb2layout->addWidget(geometric);

    QGroupBox *gb = new QGroupBox(i18n("Parameters"), page);
    QGridLayout *gb_layout = new QGridLayout(gb);

    QLabel *label = new QLabel(i18n("Start value:"), gb);
    gb_layout->addWidget(label, 0, 0);
    start = new QDoubleSpinBox(gb);
    start->setValue(0.0);
    start->setRange(-999999.999, 999999.99);
    start->setSingleStep(1.0);
    start->setDecimals(3);
    gb_layout->addWidget(start, 0, 1);

    label = new QLabel(i18n("Stop value:"), gb);
    gb_layout->addWidget(label, 1, 0);
    end = new QDoubleSpinBox(gb);
    end->setValue(0.0);
    end->setRange(-999999.999, 999999.99);
    end->setSingleStep(1.0);
    end->setDecimals(3);
    gb_layout->addWidget(end, 1, 1);

    label = new QLabel(i18n("Step value:"), gb);
    gb_layout->addWidget(label, 2, 0);
    step = new QDoubleSpinBox(gb);
    step->setValue(0.0);
    step->setRange(-999999.999, 999999.99);
    step->setSingleStep(1.0);
    step->setDecimals(3);
    gb_layout->addWidget(step, 2, 1);
    gb_layout->setColumnStretch(1, 9);

    grid1->addWidget(gb);
    grid1->addWidget(gb1);
    grid1->addWidget(gb2);

    start->setFocus();
}

void SeriesDialog::onApply()
{
    bool isLinear = linear->isChecked();

    m_dstart = start->value();
    m_dend = end->value();
    m_dstep = step->value();
    if (!isLinear) { // = Geometric
        if (m_dstart < 0.0 || m_dend < 0.0) {
            KMessageBox::error(this, i18n("End and start value must be positive."));
            return;
        }
        if (m_dstart > m_dend && m_dstep >= 1.0) {
            KMessageBox::error(this,
                               i18n("End value must be greater than the start "
                                    "value or the step must be less than '1'."));
            return;
        }
        if (m_dstart == 0.0 || m_dend == 0.0 || m_dstep == 0.0) {
            KMessageBox::error(this,
                               i18n("None of the Start, Stop or Step values "
                                    "may be equal to zero."));
            return;
        }
        if (m_dstep == 1.0) {
            KMessageBox::error(this, i18n("Step value must be different from 1"));
            return;
        }
        if (m_dstep < 0.0) {
            KMessageBox::error(this, i18n("Step is negative."));
            return;
        }
    }

    if (isLinear) { // Linear
        if (m_dstep == 0.0) {
            KMessageBox::error(this,
                               i18n("The step value must be greater than zero; "
                                    "otherwise, the linear series is infinite."));
            return;
        }
        if ((m_dstep > 0.0) && (m_dend < m_dstart)) {
            KMessageBox::error(this,
                               i18n("If the start value is greater than the "
                                    "end value the step must be less than zero."));
            return;
        }
        if ((m_dstep < 0.0) && (m_dstart <= m_dend)) {
            KMessageBox::error(this,
                               i18n("If the step is negative, the start value "
                                    "must be greater then the end value."));
            return;
        }
    }

    Q_EMIT insertSeries(m_dstart, m_dend, m_dstep, column->isChecked(), isLinear);
}
