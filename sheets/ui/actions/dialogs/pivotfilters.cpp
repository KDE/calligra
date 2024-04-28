/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012-2013 Jigar Raisinghani <jigarraisinghani@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "pivotfilters.h"
#include "pivotmain.h"
#include "ui_pivotfilters.h"

#include "core/Cell.h"
#include "core/Sheet.h"
#include "ui/Selection.h"

using namespace Calligra::Sheets;

class PivotFilters::Private
{
public:
    Selection *selection;
    Ui::PivotFilters mainWidget;
    int operatorselected1, operatorselected2;
    bool isoperatorselected;
    QVector<QString> conditions;
};

PivotFilters::PivotFilters(QWidget *parent, Selection *selection)
    : KoDialog(parent)
    , d(new Private)
{
    QWidget *widget = new QWidget(this);
    d->mainWidget.setupUi(widget);
    setMainWidget(widget);
    d->selection = selection;
    selectFields(d->mainWidget.Field);
    d->operatorselected1 = 1;
    d->operatorselected2 = 1;
    d->isoperatorselected = false;

    //     setButtons(Ok|Cancel);
    //     enableButton(Ok,"true");
    connect(d->mainWidget.Operator, QOverload<const QString &>::of(&QComboBox::activated), this, &PivotFilters::activateBoxes);
    connect(d->mainWidget.Operator2, QOverload<const QString &>::of(&QComboBox::activated), this, &PivotFilters::activateBoxes2);
    connect(d->mainWidget.Field, QOverload<const QString &>::of(&QComboBox::activated), this, &PivotFilters::fillValue);
    connect(d->mainWidget.Field2, QOverload<const QString &>::of(&QComboBox::activated), this, &PivotFilters::fillValue2);
    connect(d->mainWidget.Field3, QOverload<const QString &>::of(&QComboBox::activated), this, &PivotFilters::fillValue3);
}

void PivotFilters::selectFields(QComboBox *box)
{
    Sheet *const sheet = dynamic_cast<Sheet *>(d->selection->lastSheet());
    const QRect range = d->selection->lastRange();

    int r = range.right();
    int row = range.top();

    Cell cell;

    QString text;
    //  int index = 0;
    for (int i = range.left(); i <= r; ++i) {
        cell = Cell(sheet, i, row);
        text = cell.displayText();

        if (text.length() > 0) {
            box->addItem(text);
        }
    }
}
void PivotFilters::activateBoxes()
{
    if (d->mainWidget.Operator->currentText() == "None") {
        d->mainWidget.Field2->clear();
        d->mainWidget.Operator2->clear();
        d->mainWidget.Field3->clear();
        d->mainWidget.Value2->clear();
        d->mainWidget.Value3->clear();
        d->mainWidget.Condition2->clear();
        d->mainWidget.Condition3->clear();
        d->operatorselected2 = 0;
        d->operatorselected1 = 0;
        d->isoperatorselected = true;
    }
    if (d->operatorselected1 == 1) {
        selectFields(d->mainWidget.Field2);
        d->mainWidget.Operator2->addItem("None");
        d->mainWidget.Operator2->addItem("And");
        d->mainWidget.Operator2->addItem("Or");
        d->mainWidget.Condition2->addItem("<");
        d->mainWidget.Condition2->addItem(">");
        d->mainWidget.Condition2->addItem("==");
        d->mainWidget.Condition2->addItem("!=");
    }
    d->operatorselected1++;
}
void PivotFilters::activateBoxes2()
{
    if (d->mainWidget.Operator2->currentText() == "None") {
        d->mainWidget.Field3->clear();
        d->mainWidget.Value3->clear();
        d->mainWidget.Condition3->clear();

        d->operatorselected2 = 0;
    }

    if (d->operatorselected2 == 1 || d->isoperatorselected == true) {
        selectFields(d->mainWidget.Field3);
        d->mainWidget.Condition3->addItem("<");
        d->mainWidget.Condition3->addItem(">");
        d->mainWidget.Condition3->addItem("==");
        d->mainWidget.Condition3->addItem("!=");
        d->isoperatorselected = false;
    }
    d->operatorselected2++;
}
void PivotFilters::fillValue()
{
    PivotMain *pmain = new PivotMain(this, d->selection);
    QVector<QString> str = pmain->ValueData(d->mainWidget.Field->currentText());
    d->mainWidget.Value->clear();
    for (int i = 0; i < str.count(); i++) {
        d->mainWidget.Value->addItem(str.at(i));
    }
}
void PivotFilters::fillValue2()
{
    PivotMain *pmain = new PivotMain(this, d->selection);
    QVector<QString> str = pmain->ValueData(d->mainWidget.Field2->currentText());
    d->mainWidget.Value2->clear();
    for (int i = 0; i < str.count(); i++) {
        d->mainWidget.Value2->addItem(str.at(i));
    }
}
void PivotFilters::fillValue3()
{
    PivotMain *pmain = new PivotMain(this, d->selection);
    QVector<QString> str = pmain->ValueData(d->mainWidget.Field3->currentText());
    d->mainWidget.Value3->clear();
    for (int i = 0; i < str.count(); i++) {
        d->mainWidget.Value3->addItem(str.at(i));
    }
}

QVector<QString> PivotFilters::filterData()
{
    QVector<QString> data;

    data.append(d->mainWidget.Field->currentText());
    data.append(d->mainWidget.Condition->currentText());
    data.append(d->mainWidget.Value->currentText());

    if (d->mainWidget.Operator->currentText() != "None") {
        data.append(d->mainWidget.Operator->currentText());
        data.append(d->mainWidget.Field2->currentText());
        data.append(d->mainWidget.Condition2->currentText());
        data.append(d->mainWidget.Value2->currentText());
    }
    if (d->mainWidget.Operator2->currentText() != "None" && d->mainWidget.Operator->currentText() != "None") {
        data.append(d->mainWidget.Operator2->currentText());
        data.append(d->mainWidget.Field3->currentText());
        data.append(d->mainWidget.Condition3->currentText());
        data.append(d->mainWidget.Value3->currentText());
    }

    return data;
}

PivotFilters::~PivotFilters()
{
    delete d;
}
