/* This file is part of the KDE project
   Copyright (C) 2003 Norbert Andres<nandres@web.de>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002 Ariya Hidayat <ariya@kde.org>
             (C) 1999-2002 Laurent Montel <montel@kde.org>
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
#include "AngleDialog.h"

#include <QPushButton>
#include <QVBoxLayout>

#include <kdebug.h>
#include <klocale.h>
#include <knuminput.h>

#include <KoCanvasBase.h>

#include "Cell.h"
#include "Selection.h"
#include "Sheet.h"

#include "commands/StyleCommand.h"
#include "commands/RowColumnManipulators.h"

using namespace KSpread;

AngleDialog::AngleDialog(QWidget* parent, Selection* selection)
        : KDialog(parent)
{
    setCaption(i18n("Change Angle"));
    setModal(true);
    setButtons(Ok | Cancel | Default);

    m_selection = selection;

    QWidget *page = new QWidget();
    setMainWidget(page);

    QVBoxLayout *lay = new QVBoxLayout(page);
    lay->setMargin(0);
    lay->setSpacing(spacingHint());
    m_pAngle = new KIntNumInput(page);
    m_pAngle->setRange(-90, 90, 1);
    m_pAngle->setLabel(i18n("Angle:"));
    m_pAngle->setSuffix(" ");
    lay->addWidget(m_pAngle);

    QWidget* spacer = new QWidget(page);
    spacer->setSizePolicy(QSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding));
    lay->addWidget(spacer);

    m_pAngle->setFocus();

    connect(this, SIGNAL(okClicked()), this, SLOT(slotOk()));
    connect(this, SIGNAL(defaultClicked()), this, SLOT(slotDefault()));
    int angle = - Cell(m_selection->activeSheet(), m_selection->marker()).style().angle();
    m_pAngle->setValue(angle);
}

void AngleDialog::slotOk()
{
    QUndoCommand* macroCommand = new QUndoCommand(i18n("Change Angle"));

    StyleCommand* manipulator = new StyleCommand(macroCommand);
    manipulator->setSheet(m_selection->activeSheet());
    manipulator->setAngle(-m_pAngle->value());
    manipulator->add(*m_selection);

    AdjustColumnRowManipulator* manipulator2 = new AdjustColumnRowManipulator(macroCommand);
    manipulator2->setSheet(m_selection->activeSheet());
    manipulator2->setAdjustColumn(true);
    manipulator2->setAdjustRow(true);
    manipulator2->add(*m_selection);

    m_selection->canvas()->addCommand(macroCommand);
    accept();
}

void AngleDialog::slotDefault()
{
    m_pAngle->setValue(0);
}


#include "AngleDialog.moc"
