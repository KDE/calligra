/* This file is part of the KDE project
   Copyright (C) 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
             (C) 1999-2004 Laurent Montel <montel@kde.org>
             (C) 2003 Norbert Andres <nandres@web.de>
             (C) 2002 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002 John Dailey <dailey@vt.edu>
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
#include "SpecialPasteDialog.h"


#include <klocale.h>

#include "commands/PasteCommand.h"
#include "Map.h"
#include "Selection.h"
#include "Sheet.h"

using namespace KSpread;

SpecialPasteDialog::SpecialPasteDialog(QWidget* parent, Selection* selection)
        : KDialog(parent),
        m_selection(selection)
{
    setButtons(Ok | Cancel);
    setCaption(i18n("Special Paste"));
    QWidget* widget = new QWidget(this);
    setupUi(widget);
    setMainWidget(widget);

    connect(this, SIGNAL(okClicked()),
            this, SLOT(slotOk()));
    connect(formatButton, SIGNAL(toggled(bool)),
            this, SLOT(slotToggled(bool)));
    connect(commentButton, SIGNAL(toggled(bool)),
            this, SLOT(slotToggled(bool)));
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
    if (substractionButton->isChecked())
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
    substractionButton->setEnabled(!b);
    multiplicationButton->setEnabled(!b);
    divisionButton->setEnabled(!b);
}

#include "SpecialPasteDialog.moc"
