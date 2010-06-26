/* This file is part of the KDE project
   Copyright (C) 2002-2003 Norbert Andres <nandres@web.de>
             (C) 2002-2003 Philipp Mueller <philipp.mueller@gmx.de>
             (C) 2002 Laurent Montel <montel@kde.org>
             (C) 2002 John Dailey <dailey@vt.edu>
             (C) 2002 Ariya Hidayat <ariya@kde.org>
             (C) 2002 Werner Trobin <trobin@kde.org>
             (C) 2002 Harri Porten <porten@kde.org>

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

#include "GoalSeekDialog.h"

#include <KTextEdit>

#include "CalculationSettings.h"
#include "Cell.h"
#include "ui/RegionSelector.h"
#include "Formula.h"
#include "Map.h"
#include "Selection.h"
#include "Sheet.h"
#include "Util.h"

// commands
#include "commands/DataManipulators.h"

#include "ui_GoalSeekWidget.h"

#include <KMessageBox>

#include <QCloseEvent>

#include <math.h>

using namespace KSpread;

class GoalSeekDialog::Private
{
public:
    Selection   * selection;
    Cell          sourceCell;
    Cell          targetCell;
    double        result;
    int           maxIter;
    double        oldSource;
    bool          firstRun;

    Ui::GoalSeekWidget widget;
};


GoalSeekDialog::GoalSeekDialog(QWidget* parent, Selection* selection)
    : KDialog(parent)
    , d(new Private)
{
    d->selection = selection;
    d->result = 0.0;
    d->maxIter = 1000; // TODO make this configurable
    d->oldSource = 0.0;
    d->firstRun = true;

    setButtons(Ok | Cancel);
    enableButtonOk(false);
    setModal(false);

    setObjectName("GoalSeekDialog");

    setWindowTitle(i18n("Goal Seek"));

    QWidget* mainWidget = new QWidget(this);
    d->widget.setupUi(mainWidget);
    d->widget.selector1->setDialog(this);
    d->widget.selector1->setSelection(d->selection);
    d->widget.selector1->setSelectionMode(RegionSelector::SingleCell);
    d->widget.selector2->setDialog(this);
    d->widget.selector2->setSelection(d->selection);
    d->widget.selector2->setSelectionMode(RegionSelector::SingleCell);
    d->widget.selector3->setDialog(this);
    d->widget.selector3->setSelection(d->selection);
    d->widget.selector3->setSelectionMode(RegionSelector::SingleCell);
    d->widget.preview->hide();
    setMainWidget(mainWidget);

    // connections
    connect(d->widget.selector1->textEdit(), SIGNAL(textChanged()),
            this, SLOT(textChanged()));
    connect(d->widget.selector2->textEdit(), SIGNAL(textChanged()),
            this, SLOT(textChanged()));
    connect(d->widget.selector3->textEdit(), SIGNAL(textChanged()),
            this, SLOT(textChanged()));

    // Allow the user to select cells on the spreadsheet.
//   d->selection->canvasWidget()->startChoose();
}

GoalSeekDialog::~GoalSeekDialog()
{
    delete d;
}

void GoalSeekDialog::closeEvent(QCloseEvent * e)
{
    d->selection->endReferenceSelection();
    e->accept();
    deleteLater();
}

void GoalSeekDialog::textChanged()
{
    d->widget.preview->hide();
    const bool s1 = !d->widget.selector1->textEdit()->toPlainText().isEmpty();
    const bool s2 = !d->widget.selector2->textEdit()->toPlainText().isEmpty();
    const bool s3 = !d->widget.selector3->textEdit()->toPlainText().isEmpty();
    enableButtonOk(s1 && s2 && s3);
}

void GoalSeekDialog::accept()
{
    if (!d->widget.preview->isVisible()) {
        Sheet * sheet = d->selection->activeSheet();

        const Region source(d->widget.selector3->textEdit()->toPlainText(), sheet->map(), sheet);
        if (!source.isValid() || !source.isSingular()) {
            KMessageBox::error(this, i18n("Cell reference is invalid."));
            d->widget.selector3->textEdit()->selectAll();
            d->widget.selector3->textEdit()->setFocus();

            d->selection->emitModified();
            return;
        }

        const Region target(d->widget.selector1->textEdit()->toPlainText(), sheet->map(), sheet);
        if (!target.isValid() || !target.isSingular()) {
            KMessageBox::error(this, i18n("Cell reference is invalid."));
            d->widget.selector1->textEdit()->selectAll();
            d->widget.selector1->textEdit()->setFocus();

            d->selection->emitModified();
            return;
        }

        bool ok = false;
        double goal = d->selection->activeSheet()->map()->calculationSettings()->locale()->readNumber(d->widget.selector2->textEdit()->toPlainText(), &ok );
        if (!ok) {
            KMessageBox::error(this, i18n("Target value is invalid."));
            d->widget.selector2->textEdit()->selectAll();
            d->widget.selector2->textEdit()->setFocus();

            d->selection->emitModified();
            return;
        }

        d->sourceCell = Cell(source.firstSheet(), source.firstRange().topLeft());
        d->targetCell = Cell(target.firstSheet(), target.firstRange().topLeft());

        if (!d->sourceCell.value().isNumber()) {
            KMessageBox::error(this, i18n("Source cell must contain a numeric value."));
            d->widget.selector3->textEdit()->selectAll();
            d->widget.selector3->textEdit()->setFocus();

            d->selection->emitModified();
            return;
        }

        if (!d->targetCell.isFormula()) {
            KMessageBox::error(this, i18n("Target cell must contain a formula."));
            d->widget.selector1->textEdit()->selectAll();
            d->widget.selector1->textEdit()->setFocus();

            d->selection->emitModified();
            return;
        }

        enableButtonOk(false);
        enableButtonCancel(false);

        d->widget.preview->show();

        startCalc(numToDouble(d->sourceCell.value().asFloat()), goal);
        d->selection->emitModified();
        return;
    }

    // Reset the value for a proper undo value.
    const Value value = d->sourceCell.value();
    d->sourceCell.setValue(Value(d->oldSource));
    Sheet *const sheet = d->selection->activeSheet();
    DataManipulator *const command = new DataManipulator();
    command->setSheet(sheet);
    command->add(Region(d->sourceCell.cellPosition(), sheet));
    command->setValue(value);
    sheet->map()->addCommand(command);

    d->selection->endReferenceSelection();
    d->selection->emitModified();
    deleteLater();
}

void GoalSeekDialog::reject()
{
    if (d->widget.preview->isVisible()) {
        d->sourceCell.setValue(Value(d->oldSource));
    }

    deleteLater();
}

void GoalSeekDialog::startCalc(double _start, double _goal)
{
    d->widget.label4->setText( i18n( "Starting..." ) );
    d->widget.label5->setText(i18n("Iteration:"));

    // lets be optimistic
    bool ok = true;

    // TODO: make this configurable
    double eps = 0.0000001;

    double startA = 0.0, startB;
    double resultA, resultB;

    // save old value
  if (d->firstRun) {
    d->firstRun = false;
    d->oldSource = numToDouble(d->sourceCell.value().asFloat());
  }
  resultA = numToDouble(d->targetCell.value().asFloat()) - _goal;

    // initialize start value
    startB = _start;
    double x = startB + 0.5;

  int iterations = d->maxIter;
  const Formula formula = d->targetCell.formula();

    // while the result is not close enough to zero
    // or while the max number of iterations is not reached...
    while (fabs(resultA) > eps && (iterations >= 0)) {
        startA = startB;
        startB = x;

        d->sourceCell.setValue(Value(startA));
        const double targetValueA = numToDouble(formula.eval().asFloat());
        resultA = targetValueA - _goal;
//         kDebug() << "Target A:" << targetValueA << "," << d->targetCell.userInput() << "Calc:" << resultA;

        d->sourceCell.setValue(Value(startB));
        const double targetValueB = numToDouble(formula.eval().asFloat());
        resultB = targetValueB - _goal;
//         kDebug() << "Target B:" << targetValueB << "," << d->targetCell.userInput() << "Calc:" << resultB;

//         kDebug() <<"Iteration:" << iterations <<", StartA:" << startA
//                  << ", ResultA: " << resultA << " (eps: " << eps << "), StartB: "
//                  << startB << ", ResultB: " << resultB << endl;


        // find zero with secant method (rough implementation was provided by Franz-Xaver Meier):
        // if the function returns the same for two different
        // values we have something like a horizontal line
        // => can't get zero.
        if (resultB == resultA) {
//         kDebug() <<" resultA == resultB";
            if (fabs(resultA) < eps) {
                ok = true;
                break;
            }

            ok = false;
            break;
        }

        // Point of intersection of secant with x-axis
        x = (startA * resultB - startB * resultA) / (resultB - resultA);

        if (fabs(x) > 100000000) {
//             kDebug() <<"fabs(x) > 100000000:" << x;
            ok = false;
            break;
        }

//         kDebug() <<"X:" << x <<", fabs (resultA):" << fabs(resultA) <<", Real start:" << startA <<", Real result:" << resultA <<", Iteration:" << iterations;

    --iterations;
    if ( iterations % 20 == 0 )
      d->widget.newValue->setText( QString::number(iterations) );
    }

  d->widget.label5->setText( i18n( "New value:" ) );
    if (ok) {
    d->sourceCell.setValue( Value(startA ) );

    d->widget.label4->setText( i18n( "Goal seeking with cell %1 found a solution:",
                                 d->widget.selector3->textEdit()->toPlainText() ) );
    d->widget.newValue->setText( d->selection->activeSheet()->map()->calculationSettings()->locale()->formatNumber( startA ) );
    d->widget.currentValue->setText( d->selection->activeSheet()->map()->calculationSettings()->locale()->formatNumber( d->oldSource ) );
    } else {
        // restore the old value
    d->sourceCell.setValue( Value( d->oldSource ) );
    d->widget.label4->setText( i18n( "Goal seeking with cell %1 has found NO solution.",
                                 d->widget.selector3->textEdit()->toPlainText() ) );
    d->widget.newValue->setText( "" );
    d->widget.currentValue->setText( d->selection->activeSheet()->map()->calculationSettings()->locale()->formatNumber( d->oldSource ) );
    }

  enableButtonOk(ok);
  enableButtonCancel(true);
}

#include "GoalSeekDialog.moc"

