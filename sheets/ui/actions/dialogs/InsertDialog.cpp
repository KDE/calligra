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

#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>

#include <KLocalizedString>

using namespace Calligra::Sheets;

InsertDialog::InsertDialog(QWidget *parent, Mode _mode)
    : KoDialog(parent)
{
    setCaption("");
    setButtons(Ok | Cancel);
    setModal(true);

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
    }
    grp->setLayout(vbox);
    lay1->addWidget(grp);

    rb1->setChecked(true);

    connect(this, &KoDialog::okClicked, this, &InsertDialog::slotOk);
}

void InsertDialog::slotOk()
{
    accept();
}

bool InsertDialog::moveRowsChecked()
{
    return rb1->isChecked();
}

bool InsertDialog::moveColumnsChecked()
{
    return rb2->isChecked();
}

bool InsertDialog::insertRowsChecked()
{
    return rb3->isChecked();
}

bool InsertDialog::insertColumnsChecked()
{
    return rb3->isChecked();
}
