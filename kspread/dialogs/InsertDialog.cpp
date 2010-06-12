/* This file is part of the KDE project
   Copyright (C) 2003 Norbert Andres <nandres@web.de>
             (C) 1999-2002 Laurent Montel <montel@kde.org>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2000-2001 Werner Trobin <trobin@kde.org>
             (C) 2000 David Faure <faure@kde.org>
             (C) 1999 Stephan Kulow <coolo@kde.org>
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

// Local
#include "InsertDialog.h"

#include <QRadioButton>
#include <QGroupBox>
#include <QVBoxLayout>


#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>

#include "Selection.h"
#include "Sheet.h"

// commands
#include "commands/DataManipulators.h"
#include "commands/RowColumnManipulators.h"

using namespace KSpread;

InsertDialog::InsertDialog(QWidget* parent, Selection* selection, Mode _mode)
        : KDialog(parent)
{
    setCaption("");
    setButtons(Ok | Cancel);
    setModal(true);

    m_selection = selection;
    insRem = _mode;

    QWidget *page = new QWidget();
    setMainWidget(page);
    QVBoxLayout *lay1 = new QVBoxLayout(page);
    lay1->setMargin(KDialog::marginHint());
    lay1->setSpacing(KDialog::spacingHint());

    QGroupBox *grp = new QGroupBox(i18n("Insert"), page);
    QVBoxLayout *vbox = new QVBoxLayout;
    if (insRem == Insert) {
        vbox->addWidget(rb1 = new QRadioButton(i18n("Move towards right")));
        vbox->addWidget(rb2 = new QRadioButton(i18n("Move towards bottom")));
        vbox->addWidget(rb3 = new QRadioButton(i18n("Insert rows")));
        vbox->addWidget(rb4 = new QRadioButton(i18n("Insert columns")));
        setWindowTitle(i18n("Insert Cells"));
    } else if (insRem == Remove) {
        grp->setTitle(i18n("Remove"));
        vbox->addWidget(rb1 = new QRadioButton(i18n("Move towards left")));
        vbox->addWidget(rb2 = new QRadioButton(i18n("Move towards top")));
        vbox->addWidget(rb3 = new QRadioButton(i18n("Remove rows")));
        vbox->addWidget(rb4 = new QRadioButton(i18n("Remove columns")));
        setWindowTitle(i18n("Remove Cells"));
    } else
        kDebug(36001) << "Error in kspread_dlg_InsertDialog";
    grp->setLayout(vbox);
    lay1->addWidget(grp);

    rb1->setChecked(true);


    connect(this, SIGNAL(okClicked()), this, SLOT(slotOk()));
}

void InsertDialog::slotOk()
{
    if (rb1->isChecked()) {
        if (insRem == Insert) {
            ShiftManipulator* manipulator = new ShiftManipulator();
            manipulator->setSheet(m_selection->activeSheet());
            manipulator->setDirection(ShiftManipulator::ShiftRight);
            manipulator->add(*m_selection);
            manipulator->execute(m_selection->canvas());
        } else if (insRem == Remove) {
            ShiftManipulator* manipulator = new ShiftManipulator();
            manipulator->setSheet(m_selection->activeSheet());
            manipulator->setDirection(ShiftManipulator::ShiftRight);
            manipulator->setReverse(true);
            manipulator->add(*m_selection);
            manipulator->execute(m_selection->canvas());
        }
    } else if (rb2->isChecked()) {
        if (insRem == Insert) {
            ShiftManipulator* manipulator = new ShiftManipulator();
            manipulator->setSheet(m_selection->activeSheet());
            manipulator->setDirection(ShiftManipulator::ShiftBottom);
            manipulator->add(*m_selection);
            manipulator->execute(m_selection->canvas());
        } else if (insRem == Remove) {
            ShiftManipulator* manipulator = new ShiftManipulator();
            manipulator->setSheet(m_selection->activeSheet());
            manipulator->setDirection(ShiftManipulator::ShiftBottom);
            manipulator->setReverse(true);
            manipulator->add(*m_selection);
            manipulator->execute(m_selection->canvas());
        }
    } else if (rb3->isChecked()) {
        if (insRem == Insert) {
            InsertDeleteRowManipulator* manipulator = new InsertDeleteRowManipulator();
            manipulator->setSheet(m_selection->activeSheet());
            manipulator->add(*m_selection);
            manipulator->execute(m_selection->canvas());
        } else if (insRem == Remove) {
            InsertDeleteRowManipulator* manipulator = new InsertDeleteRowManipulator();
            manipulator->setSheet(m_selection->activeSheet());
            manipulator->setReverse(true);
            manipulator->add(*m_selection);
            manipulator->execute(m_selection->canvas());
        }
    } else if (rb4->isChecked()) {
        if (insRem == Insert) {
            InsertDeleteColumnManipulator* manipulator = new InsertDeleteColumnManipulator();
            manipulator->setSheet(m_selection->activeSheet());
            manipulator->add(*m_selection);
            manipulator->execute(m_selection->canvas());
        } else if (insRem == Remove) {
            InsertDeleteColumnManipulator* manipulator = new InsertDeleteColumnManipulator();
            manipulator->setSheet(m_selection->activeSheet());
            manipulator->setReverse(true);
            manipulator->add(*m_selection);
            manipulator->execute(m_selection->canvas());
        }
    } else {
        kDebug(36001) << "Error in kspread_dlg_InsertDialog";
    }

    accept();
}

#include "InsertDialog.moc"
