/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 1999-2002 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 2000-2001 Werner Trobin <trobin@kde.org>
             SPDX-FileCopyrightText: 2000 David Faure <faure@kde.org>
             SPDX-FileCopyrightText: 1999 Stephan Kulow <coolo@kde.org>
             SPDX-FileCopyrightText: 1998-2000 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "InsertDialog.h"

// #include <QRadioButton>
// #include <QGroupBox>
// #include <QVBoxLayout>

// #include <KLocalizedString>

// #include "SheetsDebug.h"
// #include "ui/Selection.h"
// #include "Sheet.h"

// commands
// #include "commands/DataManipulators.h"
// #include "commands/RowColumnManipulators.h"

using namespace Calligra::Sheets;

InsertDialog::InsertDialog(QWidget* parent, Selection* selection, Mode _mode)
        : KoDialog(parent)
{
    setCaption("");
    setButtons(Ok | Cancel);
    setModal(true);

    m_selection = selection;
    insRem = _mode;

    QWidget *page = new QWidget();
    setMainWidget(page);
    QVBoxLayout *lay1 = new QVBoxLayout(page);

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
        debugSheets << "Error in kspread_dlg_InsertDialog";
    grp->setLayout(vbox);
    lay1->addWidget(grp);

    rb1->setChecked(true);


    connect(this, &KoDialog::okClicked, this, &InsertDialog::slotOk);
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
        debugSheets << "Error in kspread_dlg_InsertDialog";
    }

    accept();
}
