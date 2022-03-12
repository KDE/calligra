/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
             SPDX-FileCopyrightText: 1999-2004 Laurent Montel <montel@kde.org>
             SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 1998-1999 Torben Weis <weis@kde.org>
   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "SpecialPasteDialog.h"


// #include <KLocalizedString>

// #include "commands/PasteCommand.h"
// #include "Global.h"
// #include "Map.h"
// #include "ui/Selection.h"
// #include "Sheet.h"

using namespace Calligra::Sheets;

SpecialPasteDialog::SpecialPasteDialog(QWidget* parent, Selection* selection)
        : KoDialog(parent),
        m_selection(selection)
{
    setButtons(Ok | Cancel);
    setCaption(i18n("Special Paste"));
    QWidget* widget = new QWidget(this);
    setupUi(widget);
    setMainWidget(widget);

    connect(this, &KoDialog::okClicked,
            this, &SpecialPasteDialog::slotOk);
    connect(formatButton, &QAbstractButton::toggled,
            this, &SpecialPasteDialog::slotToggled);
    connect(commentButton, &QAbstractButton::toggled,
            this, &SpecialPasteDialog::slotToggled);
}

void SpecialPasteDialog::slotOk()
{
    Paste::Mode sp = Paste::Normal;
    Paste::Operation op = Paste::OverWrite;

    /* if( everythingButton->isChecked() )
    sp = cb->isChecked() ? NormalAndTranspose : Normal;
    else if( textButton->isChecked() )
    sp = cb->isChecked() ? TextAndTranspose : Text;
    else if( formatButton->isChecked() )
    sp = cb->isChecked() ? FormatAndTranspose : Format;
    else if( noBorderButton->isChecked() )
    sp = cb->isChecked() ? NoBorderAndTranspose : NoBorder; */

    if (everythingButton->isChecked())
        sp = Paste::Normal;
    else if (textButton->isChecked())
        sp = Paste::Text;
    else if (formatButton->isChecked())
        sp = Paste::Format;
    else if (noBorderButton->isChecked())
        sp = Paste::NoBorder;
    else if (commentButton->isChecked())
        sp = Paste::Comment;
    else if (resultButton->isChecked())
        sp = Paste::Result;

    if (overwriteButton->isChecked())
        op = Paste::OverWrite;
    if (additionButton->isChecked())
        op = Paste::Add;
    if (subtractionButton->isChecked())
        op = Paste::Sub;
    if (multiplicationButton->isChecked())
        op = Paste::Mul;
    if (divisionButton->isChecked())
        op = Paste::Div;

    PasteCommand *const command = new PasteCommand();
    command->setSheet(m_selection->activeSheet());
    command->add(*m_selection);
    command->setMimeData(QApplication::clipboard()->mimeData());
    command->setMode(sp);
    command->setOperation(op);
    m_selection->activeSheet()->map()->addCommand(command);
    accept();
}

void SpecialPasteDialog::slotToggled(bool b)
{
    overwriteButton->setEnabled(!b);
    additionButton->setEnabled(!b);
    subtractionButton->setEnabled(!b);
    multiplicationButton->setEnabled(!b);
    divisionButton->setEnabled(!b);
}
