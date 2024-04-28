/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KSPREAD_SOLVER_DIALOG
#define KSPREAD_SOLVER_DIALOG

#include <KoDialog.h>

#include "ui_SolverDetailsWidget.h"
#include "ui_SolverMainWidget.h"

class QWidget;

namespace Calligra
{
namespace Sheets
{
class Selection;

namespace Plugins
{

/**
 * \class SolverDialog
 * \author Stefan Nikolaus <stefan.nikolaus@kdemail.net>
 */
class SolverDialog : public KoDialog, public Ui::Solver, public Ui::SolverDetails
{
    Q_OBJECT

public:
    /**
     * Constructor.
     * \p view The view pointer. Used to access the cells.
     * \p parent the parent widget
     */
    SolverDialog(Selection *selection, QWidget *parent);

protected Q_SLOTS:
    /**
     * Deletes the dialog.
     * Called after one of button is clicked and the dialog will be closed.
     */
    void finishDialog();
};

} // namespace Plugins
} // namespace Sheets
} // namespace Calligra

#endif
