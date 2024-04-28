/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2012-2013 Jigar Raisinghani <jigarraisinghani@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "pivotsubtotals.h"
#include "ui_pivotsubtotals.h"

// #include "Sheet.h"
// #include "ui/Selection.h"

using namespace Calligra::Sheets;

class PivotSubTotals::Private
{
public:
    Selection *selection;
    Ui::PivotSubTotals mainWidget;
};

PivotSubTotals::PivotSubTotals(QWidget *parent, Selection *selection)
    : KoDialog(parent)
    , d(new Private)
{
    QWidget *widget = new QWidget(this);
    d->mainWidget.setupUi(widget);
    setMainWidget(widget);
    d->selection = selection;
    d->mainWidget.None->setChecked(true);
    d->mainWidget.SortBy->setChecked(true);
    selectLabels();
}

public:
    explicit PivotOptions(QWidget *parent, Selection *selection);
    ~PivotOptions();
    QString returnFunction();
public Q_SLOTS:
    void on_Ok_clicked();

private:
    //     void selectBase();
    class Private;
    Private *const d;

    void PivotFilters::selectLabels()
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

            if (text.length() > 0) {
                d->mainWidget.LabelSelect->addItem(text);
            }
        }
    }

PivotSubTotals::~PivotSubTotals()
{
    delete d;
}
