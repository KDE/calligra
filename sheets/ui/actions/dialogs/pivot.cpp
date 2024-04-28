/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012-2013 Jigar Raisinghani <jigarraisinghani@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local

#include "pivot.h"
#include "pivotmain.h"
#include "ui_pivot.h"
#include "ui_pivotmain.h"

#include <QMessageBox>

using namespace Calligra::Sheets;

class PivotDialog::Private
{
public:
    Selection *selection;
    Ui::Pivot mainWidget;
};

PivotDialog::PivotDialog(QWidget *parent, Selection *selection)
    : KoDialog(parent)
    , d(new Private)
{
    setCaption(i18n("Select Source"));

    QWidget *widget = new QWidget(this);
    d->mainWidget.setupUi(widget);
    setButtons(Ok | Cancel);
    d->mainWidget.Current->setChecked(true);
    setMainWidget(widget);
    d->selection = selection;
    connect(this, &KoDialog::okClicked, this, &PivotDialog::slotUser2Clicked);
}

PivotDialog::~PivotDialog()
{
    delete d;
}

void PivotDialog::slotUser2Clicked()
{
    if (d->mainWidget.Current->isChecked()) {
        PivotMain *pMain = new PivotMain(this, d->selection);
        pMain->setModal(true);
        pMain->exec();
    }

    if (d->mainWidget.External->isChecked()) {
        QMessageBox msgBox;
        msgBox.setText("Functionality Yet to be Added");
        msgBox.exec();

        PivotDialog *p = new PivotDialog(this, d->selection);
        p->setModal(true);
        p->exec();
    }
}
