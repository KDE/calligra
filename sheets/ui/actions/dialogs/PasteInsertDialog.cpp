/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2003 Norbert Andres <nandres@web.de>
             SPDX-FileCopyrightText: 2002 Philipp Mueller <philipp.mueller@gmx.de>
             SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
             SPDX-FileCopyrightText: 2000-2002 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "PasteInsertDialog.h"

#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>

#include <KLocalizedString>

#include "ui/Selection.h"

using namespace Calligra::Sheets;

PasteInsertDialog::PasteInsertDialog(QWidget *parent)
    : KoDialog(parent)
{
    setCaption(i18n("Paste Inserting Cells"));
    setObjectName(QLatin1String("PasteInsertDialog"));
    setModal(true);
    setButtons(Ok | Cancel);

    QWidget *page = new QWidget();
    setMainWidget(page);
    QVBoxLayout *lay1 = new QVBoxLayout(page);

    QGroupBox *grp = new QGroupBox(i18n("Insert"), page);
    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(rb1 = new QRadioButton(i18n("Move towards right")));
    vbox->addWidget(rb2 = new QRadioButton(i18n("Move towards bottom")));
    rb1->setChecked(true);
    grp->setLayout(vbox);
    lay1->addWidget(grp);

    connect(this, &KoDialog::okClicked, this, &PasteInsertDialog::slotOk);
}

bool PasteInsertDialog::checkedRight() const
{
    return rb1->isChecked();
}

bool PasteInsertDialog::checkedBottom() const
{
    return rb2->isChecked();
}

void PasteInsertDialog::slotOk()
{
    accept();
}
