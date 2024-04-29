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

using namespace Calligra::Sheets;

SpecialPasteDialog::SpecialPasteDialog(QWidget *parent)
    : ActionDialog(parent)
{
    setCaption(i18n("Special Paste"));
    setButtonText(Apply, i18n("Paste"));

    QWidget *widget = new QWidget(this);
    setupUi(widget);
    setMainWidget(widget);

    connect(formatButton, &QAbstractButton::toggled, this, &SpecialPasteDialog::slotToggled);
    connect(commentButton, &QAbstractButton::toggled, this, &SpecialPasteDialog::slotToggled);
}

void SpecialPasteDialog::onApply()
{
    Q_EMIT paste();
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

bool SpecialPasteDialog::wantEverything() const
{
    return everythingButton->isChecked();
}

bool SpecialPasteDialog::wantText() const
{
    return textButton->isChecked();
}

bool SpecialPasteDialog::wantFormat() const
{
    return formatButton->isChecked();
}

bool SpecialPasteDialog::wantNoBorder() const
{
    return noBorderButton->isChecked();
}

bool SpecialPasteDialog::wantComment() const
{
    return commentButton->isChecked();
}

bool SpecialPasteDialog::wantResult() const
{
    return resultButton->isChecked();
}

bool SpecialPasteDialog::opOverwrite() const
{
    return overwriteButton->isChecked();
}

bool SpecialPasteDialog::opAdd() const
{
    return additionButton->isChecked();
}

bool SpecialPasteDialog::opSub() const
{
    return subtractionButton->isChecked();
}

bool SpecialPasteDialog::opMul() const
{
    return multiplicationButton->isChecked();
}

bool SpecialPasteDialog::opDiv() const
{
    return divisionButton->isChecked();
}
