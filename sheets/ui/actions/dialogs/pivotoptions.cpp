/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012-2013 Jigar Raisinghani <jigarraisinghani@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "pivotoptions.h"
#include "ui_pivotoptions.h"

using namespace Calligra::Sheets;

class PivotOptions::Private
{
public:
    Selection *selection;
    Ui::PivotOptions mainWidget;
    QString function;
}; // Private

PivotOptions::PivotOptions(QWidget *parent, Selection *selection)
    : KoDialog(parent)
    , d(new Private)
{
    setButtons(Ok | Cancel);
    QWidget *widget = new QWidget;
    d->mainWidget.setupUi(widget);
    setCaption(i18n("Pivot Options"));
    setMainWidget(widget);
    d->selection = selection;
    //    selectBase();
    enableButton(Ok, true);
    d->mainWidget.SelectFunction->addItem("prod");
    d->mainWidget.SelectFunction->addItem("devsq");

    connect(this, &KoDialog::okClicked, this, &PivotOptions::on_Ok_clicked);

} // PivotOptions

QString PivotOptions::returnFunction()
{
    d->function = d->mainWidget.SelectFunction->currentText();
    return d->function;

} // returnFunction

/*
void PivotOptions::selectBase()
{
    Sheet *const sheet = d->selection->lastSheet();
    const QRect range = d->selection->lastRange();

    int r = range.right();
    int row = range.top();

    Cell cell;

    QString text;

    int index = 0;
    for (int i = range.left(); i <= r; ++i) {
        cell = Cell(sheet, i, row);
        text = cell.displayText();

    if(text.length() >0)
    {
           d->mainWidget.BaseItem->addItem(text);
       d->mainWidget.BaseField->addItem(text);

    }
    }
}
*/
void PivotOptions::on_Ok_clicked()
{
    // returnFunction();
}

PivotOptions::~PivotOptions()
{
    delete d;
} //~PivotOptions
