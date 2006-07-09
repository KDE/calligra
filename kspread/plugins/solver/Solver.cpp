/* This file is part of the KDE project
   Copyright (C) 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#include <kdebug.h>
#include <kgenericfactory.h>
#include <ktextedit.h>

#include <Formula.h>
#include <Cell.h>
#include <Sheet.h>
#include <Value.h>
#include <View.h>
#include <Region.h>

#include "SolverDialog.h"

#include "Solver.h"

using namespace KSpread::Plugins;

// make the plugin available
typedef KGenericFactory<KSpread::Plugins::Solver> SolverFactory;
K_EXPORT_COMPONENT_FACTORY( libkspreadsolver, SolverFactory("kspreadsolver") )

KSpread::View* s_view = 0;
KSpread::Formula* s_formula = 0;
double function(const gsl_vector* vector, void *params);


class Solver::Private
{
public:
  SolverDialog* dialog;
  View* view;
};

Solver::Solver( QObject* parent, const QStringList& args )
  : KParts::Plugin( parent ),
    d( new Private )
{
  Q_UNUSED(args)

  d->dialog = 0;
  d->view = qobject_cast<View*>( parent );
  if ( !d->view )
  {
    kError() << "Solver: Parent object is not a KSpread::View! Quitting." << endl;
    return;
  }

  KAction* solver = new KAction( i18n("Function Optimizer..."),
                                 actionCollection(), "kspreadsolver" );
  connect( solver, SIGNAL( triggered(bool) ), this, SLOT( showDialog() ) );
}

Solver::~Solver()
{
  delete d;
}

void Solver::showDialog()
{
  d->dialog = new SolverDialog( d->view, d->view );
  connect( d->dialog, SIGNAL( okClicked() ), this, SLOT( optimize() ) );
  d->dialog->show();
}

void Solver::optimize()
{
  register Sheet * const sheet = d->view->activeSheet();
  if ( !sheet )
    return;

  if (d->dialog->function->textEdit()->toPlainText().isEmpty())
    return;

  if (d->dialog->parameters->textEdit()->toPlainText().isEmpty())
    return;

  Region region( d->view, d->dialog->function->textEdit()->toPlainText() );
  if (!region.isValid())
    return;

  const QPoint point = (*region.constBegin())->rect().topLeft();
  const Cell* formulaCell = sheet->cellAt( point.x(), point.y() );
  if (!formulaCell->isFormula())
    return;

  kDebug() << formulaCell->text() << endl;
  s_formula = new Formula( sheet );
  if (d->dialog->minimizeButton->isChecked())
  {
    s_formula->setExpression( formulaCell->text() );
  }
  else if (d->dialog->maximizeButton->isChecked())
  {
    // invert the formula
    s_formula->setExpression( "=-(" + formulaCell->text().mid(1) + ')' );
  }
  else // if (d->dialog->valueButton->isChecked())
  {
    // TODO
    s_formula->setExpression( "=ABS(" + formulaCell->text().mid(1) + '-'
                                      + d->dialog->value->text() + ')' );
  }

  // Determine the parameters
  int dimension = 0;
  Parameters* parameters = new Parameters;
  region = Region( d->view, d->dialog->parameters->textEdit()->toPlainText() );
  Region::ConstIterator end( region.constEnd() );
  for ( Region::ConstIterator it( region.constBegin() ); it != end; ++it )
  {
    QRect range = (*it)->rect();
    for ( int col = range.left(); col <= range.right(); ++col )
    {
      for ( int row = range.top(); row <= range.bottom(); ++row )
      {
        parameters->cells.append( sheet->cellAt( col, row ) );
        ++dimension;
      }
    }
  }

  /* Initial vertex size vector with a step size of 1 */
  gsl_vector* stepSizes = gsl_vector_alloc( dimension );
  gsl_vector_set_all(stepSizes, 1.0);

  /* Initialize starting point */
  int index = 0;
  gsl_vector* x = gsl_vector_alloc( dimension );
  foreach (Cell* cell, parameters->cells)
  {
    gsl_vector_set( x, index++, cell->value().asFloat() );
  }

  /* Initialize method and iterate */
  gsl_multimin_function functionInfo;
  functionInfo.f = &function;
  functionInfo.n = dimension;
  functionInfo.params = static_cast<void*>( parameters );

  // Use the simplex minimizer. The others depend on the first derivative.
  const gsl_multimin_fminimizer_type *T = gsl_multimin_fminimizer_nmsimplex;
  gsl_multimin_fminimizer* minimizer = gsl_multimin_fminimizer_alloc( T, dimension );
  gsl_multimin_fminimizer_set( minimizer, &functionInfo, x, stepSizes );

  int status = 0;
  int iteration = 0;
  const int maxIterations = d->dialog->iterations->value();
  double size = 1;
  const double epsilon = d->dialog->precision->value();
  do
  {
    iteration++;
    status = gsl_multimin_fminimizer_iterate( minimizer );

    if ( status )
      break;

    size = gsl_multimin_fminimizer_size( minimizer );
    status = gsl_multimin_test_size( size, epsilon );

    if ( status == GSL_SUCCESS )
    {
      kDebug() << "converged to minimum after " << iteration << " iteration(s) at" << endl;
    }

    for ( int i = 0; i < dimension; ++i )
    {
      printf( "%10.3e ", gsl_vector_get( minimizer->x, i ) );
    }
    printf( "f() = %7.3f size = %.3f\n", minimizer->fval, size );
  }
  while ( status == GSL_CONTINUE && iteration < maxIterations );


  // free allocated memory
  gsl_vector_free(x);
  gsl_vector_free(stepSizes);
  gsl_multimin_fminimizer_free(minimizer);
  delete parameters;
  delete s_formula;
}

double Solver::evaluate(const gsl_vector* vector, void *parameters)
{
  Q_UNUSED(vector)
  Q_UNUSED(parameters)
  return 0.0;
}

double function(const gsl_vector* vector, void *params)
{
  Solver::Parameters* parameters = static_cast<Solver::Parameters*>( params );

  for ( int i = 0; i < parameters->cells.count(); ++i )
  {
    parameters->cells[i]->setValue( gsl_vector_get(vector, i) );
  }

  // TODO check for errors/correct type
  return s_formula->eval().asFloat();
}

#include "Solver.moc"
