/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "Solver.h"

#include <QStandardPaths>

#include <KActionCollection>
#include <KPluginFactory>
#include <ktextedit.h>

#include "KoCanvasBase.h"

#include <kundo2command.h>

#include <core/Cell.h>
#include <core/CellStorage.h>
#include <core/Map.h>
#include <core/Sheet.h>
#include <engine/Formula.h>
#include <engine/Region.h>
#include <engine/Value.h>
#include <part/Doc.h>
#include <part/View.h>
#include <ui/Selection.h>

#include "SolverDialog.h"

using namespace Calligra::Sheets::Plugins;

// make the plugin available
K_PLUGIN_FACTORY_WITH_JSON(SolverFactory, "sheetssolver.json", registerPlugin<Calligra::Sheets::Plugins::Solver>();)

Calligra::Sheets::View *s_view = nullptr;
Calligra::Sheets::Formula *s_formula = nullptr;
double _function(const gsl_vector *vector, void *params);

class Q_DECL_HIDDEN Solver::Private
{
public:
    SolverDialog *dialog;
    View *view;
};

Solver::Solver(QObject *parent, const QVariantList &args)
    : d(new Private)
{
    Q_UNUSED(args)

    setXMLFile(QStandardPaths::locate(QStandardPaths::GenericDataLocation, "calligrasheets/viewplugins/solver.rc"), true);

    d->dialog = nullptr;
    d->view = qobject_cast<View *>(parent);
    if (!d->view) {
        errorSheets << "Solver: Parent object is not a Calligra::Sheets::View! Quitting." << Qt::endl;
        return;
    }

    QAction *solver = actionCollection()->addAction("sheetssolver");
    solver->setText(i18n("Function Optimizer..."));
    connect(solver, &QAction::triggered, this, &Solver::showDialog);
}

Solver::~Solver()
{
    delete d;
}

void Solver::showDialog()
{
    d->dialog = new SolverDialog(d->view->selection(), d->view);
    connect(d->dialog, &KoDialog::okClicked, this, &Solver::optimize);
    d->dialog->show();
}

void Solver::optimize()
{
    Sheet *const sheet = d->view->activeSheet();
    if (!sheet)
        return;

    if (d->dialog->function->textEdit()->toPlainText().isEmpty())
        return;

    if (d->dialog->parameters->textEdit()->toPlainText().isEmpty())
        return;

    Region region = d->view->doc()->map()->regionFromName(d->dialog->function->textEdit()->toPlainText(), d->view->activeSheet());
    if (!region.isValid())
        return;

    const QPoint point = (*region.constBegin())->rect().topLeft();
    const Cell formulaCell = Cell(sheet, point.x(), point.y());
    if (!formulaCell.isFormula())
        return;

    KUndo2Command *cmd = new KUndo2Command(kundo2_i18n("Optimize"));
    sheet->fullCellStorage()->startUndoRecording();

    debugSheets << formulaCell.userInput();
    s_formula = new Formula(sheet);
    if (d->dialog->minimizeButton->isChecked()) {
        s_formula->setExpression(formulaCell.userInput());
    } else if (d->dialog->maximizeButton->isChecked()) {
        // invert the formula
        s_formula->setExpression("=-(" + formulaCell.userInput().mid(1) + ')');
    } else { // if (d->dialog->valueButton->isChecked())
        // TODO
        s_formula->setExpression("=ABS(" + formulaCell.userInput().mid(1) + '-' + d->dialog->value->text() + ')');
    }

    // Determine the parameters
    int dimension = 0;
    Parameters *parameters = new Parameters;
    region = d->view->doc()->map()->regionFromName(d->dialog->parameters->textEdit()->toPlainText(), d->view->activeSheet());
    Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it(region.constBegin()); it != end; ++it) {
        QRect range = (*it)->rect();
        for (int col = range.left(); col <= range.right(); ++col) {
            for (int row = range.top(); row <= range.bottom(); ++row) {
                parameters->cells.append(Cell(sheet, col, row));
                ++dimension;
            }
        }
    }

    /* Initial vertex size vector with a step size of 1 */
    gsl_vector *stepSizes = gsl_vector_alloc(dimension);
    gsl_vector_set_all(stepSizes, 1.0);

    /* Initialize starting point */
    int index = 0;
    gsl_vector *x = gsl_vector_alloc(dimension);
    foreach (const Cell &cell, parameters->cells) {
        gsl_vector_set(x, index++, numToDouble(cell.value().asFloat()));
    }

    /* Initialize method and iterate */
    gsl_multimin_function functionInfo;
    functionInfo.f = &_function;
    functionInfo.n = dimension;
    functionInfo.params = static_cast<void *>(parameters);

    // Use the simplex minimizer. The others depend on the first derivative.
    const gsl_multimin_fminimizer_type *T = gsl_multimin_fminimizer_nmsimplex;
    gsl_multimin_fminimizer *minimizer = gsl_multimin_fminimizer_alloc(T, dimension);
    gsl_multimin_fminimizer_set(minimizer, &functionInfo, x, stepSizes);

    int status = 0;
    int iteration = 0;
    const int maxIterations = d->dialog->iterations->value();
    double size = 1;
    const double epsilon = d->dialog->precision->value();
    do {
        iteration++;
        status = gsl_multimin_fminimizer_iterate(minimizer);

        if (status)
            break;

        size = gsl_multimin_fminimizer_size(minimizer);
        status = gsl_multimin_test_size(size, epsilon);

        if (status == GSL_SUCCESS) {
            debugSheets << "converged to minimum after" << iteration << " iteration(s) at";
        }

        for (int i = 0; i < dimension; ++i) {
            printf("%10.3e ", gsl_vector_get(minimizer->x, i));
        }
        printf("f() = %7.3f size = %.3f\n", minimizer->fval, size);
    } while (status == GSL_CONTINUE && iteration < maxIterations);

    sheet->fullCellStorage()->stopUndoRecording(cmd);
    d->view->selection()->canvas()->addCommand(cmd);

    // free allocated memory
    gsl_vector_free(x);
    gsl_vector_free(stepSizes);
    gsl_multimin_fminimizer_free(minimizer);
    delete parameters;
    delete s_formula;
}

double Solver::evaluate(const gsl_vector *vector, void *parameters)
{
    Q_UNUSED(vector)
    Q_UNUSED(parameters)
    return 0.0;
}

double _function(const gsl_vector *vector, void *params)
{
    Solver::Parameters *parameters = static_cast<Solver::Parameters *>(params);

    for (int i = 0; i < parameters->cells.count(); ++i) {
        parameters->cells[i].setValue(Calligra::Sheets::Value(gsl_vector_get(vector, i)));
    }

    // TODO check for errors/correct type
    return numToDouble(s_formula->eval().asFloat());
}

#include "Solver.moc"
