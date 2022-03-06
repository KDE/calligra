/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002-2004 Ariya Hidayat <ariya@kde.org>
             SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 1999-2002 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 2000 David Faure <faure@kde.org>
             SPDX-FileCopyrightText: 1998-1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "Resize2Dialog.h"

#include <float.h>

// Qt
#include <QGridLayout>
#include <QLabel>

// Calligra
#include <KoCanvasBase.h>
#include <KoUnit.h>
#include <KoUnitDoubleSpinBox.h>

// Sheets
#include <Localization.h>
#include "Map.h"
#include "RowColumnFormat.h"
#include "RowFormatStorage.h"
#include "ui/Selection.h"
#include <Sheet.h>

// commands
#include "commands/RowColumnManipulators.h"

using namespace Calligra::Sheets;

ResizeRow::ResizeRow(QWidget* parent, Selection* selection)
        : KoDialog(parent)
{
    setCaption(i18n("Resize Row"));
    setModal(true);
    setButtons(Ok | Cancel | Default);
    m_selection = selection;

    rowHeight = m_selection->activeSheet()->rowFormats()->rowHeight(m_selection->lastRange().top());

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
    connect(this, &KoDialog::okClicked, this, &ResizeRow::slotOk);
    connect(this, &KoDialog::defaultClicked, this, &ResizeRow::slotDefault);
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
        : KoDialog(parent)
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
    connect(this, &KoDialog::okClicked, this, &ResizeColumn::slotOk);
    connect(this, &KoDialog::defaultClicked, this, &ResizeColumn::slotDefault);

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
