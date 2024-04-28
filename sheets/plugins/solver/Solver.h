/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KSPREAD_SOLVER
#define KSPREAD_SOLVER

#include <gsl/gsl_multimin.h>

#include <KXMLGUIClient>

#include <Cell.h>

#include <QObject>
#include <QVariantList>

namespace Calligra
{
namespace Sheets
{
namespace Plugins
{

/**
 * \class Solver
 * \brief Function Optimizer
 * \author Stefan Nikolaus <stefan.nikolaus@kdemail.net>
 */
class Solver : public QObject, public KXMLGUIClient
{
    Q_OBJECT

public:
    struct Parameters {
        QList<Cell> cells;
    };

    /**
     * Constructor.
     */
    Solver(QObject *parent, const QVariantList &args);

    /**
     * Destructor.
     */
    ~Solver();

    double evaluate(const gsl_vector *vector, void *parameters);

protected Q_SLOTS:
    /**
     * Called when the Solver action is triggered.
     * Opens the dialog.
     */
    void showDialog();

    /**
     * This method does the real work.
     * Uses the parameters of the dialog to optimize the given function.
     */
    void optimize();

private:
    Q_DISABLE_COPY(Solver)

    class Private;
    Private *const d;
};

} // namespace Plugins
} // namespace Sheets
} // namespace Calligra

#endif
