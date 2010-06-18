/* This file is part of the KDE project
   Copyright (C) 2002-2004 Ariya Hidayat <ariya@kde.org>
             (C) 2003 Norbert Andres <nandres@web.de>
             (C) 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 1999-2002 Laurent Montel <montel@kde.org>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2000 David Faure <faure@kde.org>
             (C) 1998-1999 Torben Weis <weis@kde.org>

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

// Local
#include "Resize2Dialog.h"

#include <float.h>

// Qt
#include <QGridLayout>
#include <QLabel>

// KDE
#include <knuminput.h>

// KOffice
#include <KoCanvasBase.h>
#include <KoUnit.h>
#include <KoUnitDoubleSpinBox.h>

// KSpread
#include <Global.h>
#include <Localization.h>
#include "Map.h"
#include "RowColumnFormat.h"
#include "Selection.h"
#include <Sheet.h>

// commands
#include "commands/RowColumnManipulators.h"

using namespace KSpread;

ResizeRow::ResizeRow(QWidget* parent, Selection* selection)
        : KDialog(parent)
{
    setCaption(i18n("Resize Row"));
    setModal(true);
    setButtons(Ok | Cancel | Default);
    m_selection = selection;

    const RowFormat* rl = m_selection->activeSheet()->rowFormat(selection->lastRange().top());
    rowHeight = rl->height();

    QWidget *page = new QWidget();
    setMainWidget(page);

    QGridLayout* gridLayout = new QGridLayout(page);
    gridLayout->setColumnStretch(1, 1);

    QLabel * label1 = new QLabel(page);
    label1->setText(i18n("Height:"));
    gridLayout->addWidget(label1, 0, 0);

    m_pHeight = new KoUnitDoubleSpinBox(page);
    m_pHeight->setValue(rowHeight);
    m_pHeight->setUnit(m_selection->canvas()->unit());
    gridLayout->addWidget(m_pHeight, 0, 1);

    m_pHeight->setFocus();

    //store the visible value, for later check for changes
    rowHeight = m_pHeight->value();
    connect(this, SIGNAL(okClicked()), this, SLOT(slotOk()));
    connect(this, SIGNAL(defaultClicked()), this, SLOT(slotDefault()));
}

void ResizeRow::slotOk()
{
    double height = m_pHeight->value();

    //Don't generate a resize, when there isn't a change or the change is only a rounding issue
    if (fabs(height - rowHeight) > DBL_EPSILON) {
        ResizeRowManipulator* manipulator = new ResizeRowManipulator();
        manipulator->setSheet(m_selection->activeSheet());
        manipulator->setSize(height);
        manipulator->add(*m_selection);
        manipulator->execute(m_selection->canvas());
    }
    accept();
}

void ResizeRow::slotDefault()
{
    Sheet* sheet = m_selection->activeSheet();
    if (!sheet)
        return;
    double points = sheet->map()->defaultRowFormat()->height();
    m_pHeight->setValue(m_selection->canvas()->unit().toUserValue(points));
}

ResizeColumn::ResizeColumn(QWidget* parent, Selection* selection)
        : KDialog(parent)
{
    setCaption(i18n("Resize Column"));
    setModal(true);
    setButtons(Ok | Cancel | Default);
    m_selection = selection;

    const ColumnFormat* cl = m_selection->activeSheet()->columnFormat(selection->lastRange().left());
    columnWidth = cl->width();

    QWidget *page = new QWidget();
    setMainWidget(page);

    QGridLayout* gridLayout = new QGridLayout(page);
    gridLayout->setColumnStretch(1, 1);

    QLabel * label1 = new QLabel(page);
    label1->setText(i18n("Width:"));
    gridLayout->addWidget(label1, 0, 0);

    m_pWidth = new KoUnitDoubleSpinBox(page);
    m_pWidth->setValue(columnWidth);
    m_pWidth->setUnit(m_selection->canvas()->unit());
    gridLayout->addWidget(m_pWidth, 0, 1);

    m_pWidth->setFocus();

    //store the visible value, for later check for changes
    columnWidth = m_pWidth->value();
    connect(this, SIGNAL(okClicked()), this, SLOT(slotOk()));
    connect(this, SIGNAL(defaultClicked()), this, SLOT(slotDefault()));

}

void ResizeColumn::slotOk()
{
    double width = m_pWidth->value();

    //Don't generate a resize, when there isn't a change or the change is only a rounding issue
    if (fabs(width - columnWidth) > DBL_EPSILON) {
        ResizeColumnManipulator* manipulator = new ResizeColumnManipulator();
        manipulator->setSheet(m_selection->activeSheet());
        manipulator->setSize(width);
        manipulator->add(*m_selection);
        manipulator->execute(m_selection->canvas());
    }
    accept();
}

void ResizeColumn::slotDefault()
{
    Sheet* sheet = m_selection->activeSheet();
    if (!sheet)
        return;
    double points = sheet->map()->defaultColumnFormat()->width();
    m_pWidth->setValue(m_selection->canvas()->unit().toUserValue(points));
}


#include "Resize2Dialog.moc"
