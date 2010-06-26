/* This file is part of the KDE project
   Copyright (C) 2002-2004 Ariya Hidayat <ariya@kde.org>
             (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 1999-2002 Laurent Montel <montel@kde.org>
             (C) 2001-2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2000-2001 Werner Trobin <trobin@kde.org>
             (C) 1998-2000 Torben Weis <weis@kde.org>

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
   Boston, MA 02110-1301, USA.
*/


#include "SeriesDialog.h"

#include "Selection.h"
#include "Sheet.h"

#include "commands/DataManipulators.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QGridLayout>
#include <klocale.h>
#include <QLabel>

#include <klineedit.h>
#include <kmessagebox.h>
#include <knumvalidator.h>

#include <QRadioButton>
#include <QCheckBox>

#include <knuminput.h>

using namespace KSpread;

SeriesDialog::SeriesDialog(QWidget* parent, Selection* selection)
        : KDialog(parent)
{
    setCaption(i18n("Series"));
    setButtons(Ok | Cancel);
    setModal(true);

    m_selection = selection;

    QWidget *page = new QWidget();
    setMainWidget(page);

    QVBoxLayout *grid1 = new QVBoxLayout(page);
    grid1->setSpacing(spacingHint());

    QGroupBox* gb1 = new QGroupBox(i18n("Insert Values"), page);
    QHBoxLayout *gb1layout = new QHBoxLayout(gb1);
    gb1layout->setSpacing(spacingHint());
    column = new QRadioButton(i18n("Vertical"), gb1);
    column->setWhatsThis(i18n("Insert the series vertically, one below the other"));
    row = new QRadioButton(i18n("Horizontal"), gb1);
    row->setWhatsThis(i18n("Insert the series horizontally, from left to right"));
    column->setChecked(true);

    gb1layout->addWidget(column);
    gb1layout->addWidget(row);

    QGroupBox* gb2 = new QGroupBox(i18n("Type"), page);
    QHBoxLayout *gb2layout = new QHBoxLayout(gb2);
    gb2layout->setSpacing(spacingHint());
    linear = new QRadioButton(i18n("Linear (2,4,6,...)"), gb2);
    linear->setWhatsThis(i18n("Generate a series from 'start' to 'end' and for each step add "
                              "the value provided in step. This creates a series where each value "
                              "is 'step' larger than the value before it."));
    geometric = new QRadioButton(i18n("Geometric (2,4,8,...)"), gb2);
    geometric->setWhatsThis(i18n("Generate a series from 'start' to 'end' and for each step multiply "
                                 "the value with the value provided in step. Using a step of 5 produces a list like: "
                                 "5, 25, 125, 625 since 5 multiplied by 5 (step) equals 25, and that multiplied by 5 equals 125, "
                                 "which multiplied by the same step-value of 5 equals 625."));
    linear->setChecked(true);

    gb2layout->addWidget(linear);
    gb2layout->addWidget(geometric);

    QGroupBox* gb = new QGroupBox(i18n("Parameters"), page);
    QGridLayout *gb_layout = new QGridLayout(gb);
    gb_layout->setSpacing(spacingHint());

    QLabel* label = new QLabel(i18n("Start value:"), gb);
    gb_layout->addWidget(label, 0, 0);
    start = new KDoubleNumInput(-999999.999, 999999.99, 0.0,
                                gb, 1.0, 3);
    gb_layout->addWidget(start, 0, 1);

    label = new QLabel(i18n("Stop value:"), gb);
    gb_layout->addWidget(label, 1, 0);
    end = new KDoubleNumInput(-999999.999, 999999.99, 0.0,
                              gb, 1.0, 3);
    gb_layout->addWidget(end, 1, 1);

    label = new QLabel(i18n("Step value:"), gb);
    gb_layout->addWidget(label, 2, 0);
    step = new KDoubleNumInput(-999999.999, 999999.99, 0.0,
                               gb, 1.0, 3);
    gb_layout->addWidget(step, 2, 1);
    gb_layout->setColumnStretch(1, 9);

    grid1->addWidget(gb);
    grid1->addWidget(gb1);
    grid1->addWidget(gb2);

    start->setFocus();
}

void SeriesDialog::slotButtonClicked(int button)
{
    if (button != KDialog::Ok) {
        KDialog::slotButtonClicked(button);
        return;
    }

    bool isColumn = column->isChecked();
    bool isLinear = linear->isChecked();

    QString tmp;
    double dstep, dend, dstart;
    Sheet * m_pSheet;
    m_pSheet = m_selection->activeSheet();

    dstart = start->value();
    dend = end->value();
    dstep = step->value();
    if (!isLinear) { // = Geometric
        if (dstart < 0.0 || dend < 0.0) {
            KMessageBox::error(this, i18n("End and start value must be positive."));
            return;
        }
        if (dstart > dend && dstep >= 1.0) {
            KMessageBox::error(this, i18n("End value must be greater than the start "
                                          "value or the step must be less than '1'."));
            return;
        }
        if (dstart == 0.0 || dend == 0.0 || dstep == 0.0) {
            KMessageBox::error(this, i18n("None of the Start, Stop or Step values "
                                          "may be equal to zero."));
            return;
        }
        if (dstep == 1.0) {
            KMessageBox::error(this, i18n("Step value must be different from 1"));
            return;
        }
        if (dstep < 0.0) {
            KMessageBox::error(this, i18n("Step is negative."));
            return;
        }
    }

    if (isLinear) { // Linear
        if (dstep == 0.0) {
            KMessageBox::error(this, i18n("The step value must be greater than zero; "
                                          "otherwise, the linear series is infinite."));
            return;
        }
        if ((dstep > 0.0) && (dend < dstart)) {
            KMessageBox::error(this, i18n("If the start value is greater than the "
                                          "end value the step must be less than zero."));
            return;
        }
        if ((dstep < 0.0) && (dstart <= dend)) {
            KMessageBox::error(this, i18n("If the step is negative, the start value "
                                          "must be greater then the end value."));
            return;
        }
    }

    SeriesManipulator *manipulator = new SeriesManipulator;
    manipulator->setSheet(m_selection->activeSheet());
    manipulator->setupSeries(m_selection->marker(), dstart, dend, dstep,
                             isColumn ? SeriesManipulator::Column : SeriesManipulator::Row,
                             isLinear ? SeriesManipulator::Linear : SeriesManipulator::Geometric);

    // setupSeries also called add(), so we can call execute directly
    manipulator->execute(m_selection->canvas());

    accept();
}


#include "SeriesDialog.moc"
