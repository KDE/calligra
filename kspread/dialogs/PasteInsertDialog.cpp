/* This file is part of the KDE project
   Copyright (C) 2003 Norbert Andres <nandres@web.de>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2000-2002 Laurent Montel <montel@kde.org>

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
#include "PasteInsertDialog.h"

#include <QApplication>
#include <QGroupBox>
#include <QVBoxLayout>
#include <klocale.h>

#include <kdebug.h>
#include <QRadioButton>
#include <QCheckBox>

#include "commands/PasteCommand.h"
#include "Map.h"
#include "ui/Selection.h"
#include "Sheet.h"

using namespace KSpread;

PasteInsertDialog::PasteInsertDialog(QWidget* parent, Selection* selection)
        : KDialog(parent)
{
    setCaption(i18n("Paste Inserting Cells"));
    setObjectName("PasteInsertDialog");
    setModal(true);
    setButtons(Ok | Cancel);
    m_selection = selection;
    rect = selection->lastRange();

    QWidget *page = new QWidget();
    setMainWidget(page);
    QVBoxLayout *lay1 = new QVBoxLayout(page);
    lay1->setMargin(KDialog::marginHint());
    lay1->setSpacing(KDialog::spacingHint());

    QGroupBox *grp = new QGroupBox(i18n("Insert"), page);
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(rb1 = new QRadioButton(i18n("Move towards right")));
    vbox->addWidget(rb2 = new QRadioButton(i18n("Move towards bottom")));
    rb1->setChecked(true);
    grp->setLayout(vbox);
    lay1->addWidget(grp);

    connect(this, SIGNAL(okClicked()), this, SLOT(slotOk()));
}

void PasteInsertDialog::slotOk()
{
    PasteCommand *const command = new PasteCommand();
    command->setSheet(m_selection->activeSheet());
    command->add(*m_selection);
    command->setMimeData(QApplication::clipboard()->mimeData());
    if (rb1->isChecked()) {
        command->setInsertionMode(PasteCommand::ShiftCellsRight);
    } else if (rb2->isChecked()) {
        command->setInsertionMode(PasteCommand::ShiftCellsDown);
    }
    m_selection->activeSheet()->map()->addCommand(command);
    accept();
}

#include "PasteInsertDialog.moc"
