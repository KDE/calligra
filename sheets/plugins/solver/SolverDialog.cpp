/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2006 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Local
#include "SolverDialog.h"

#include "ui/RegionSelector.h"

using namespace Calligra::Sheets::Plugins;
using namespace Calligra::Sheets;

SolverDialog::SolverDialog(Selection *selection, QWidget *parent)
    : KoDialog(parent)
{
    setCaption(i18n("Function Optimizer"));
    setButtons(Ok | Cancel | Details);
    setAttribute(Qt::WA_DeleteOnClose);

    QWidget *widget = new QWidget(this);
    Ui::Solver::setupUi(widget);
    setMainWidget(widget);
    setModal(false);
    function->setSelection(selection);
    function->setDialog(this);
    function->setSelectionMode(RegionSelector::SingleCell);
    parameters->setSelection(selection);
    parameters->setDialog(this);
    parameters->setSelectionMode(RegionSelector::MultipleCells);

    QWidget *detailsWidget = new QWidget(this);
    Ui::SolverDetails::setupUi(detailsWidget);
    setDetailsWidget(detailsWidget);

    connect(this, &KoDialog::okClicked, this, &SolverDialog::finishDialog);
    connect(this, &KoDialog::cancelClicked, this, &SolverDialog::finishDialog);
}

void SolverDialog::finishDialog()
{
    deleteLater();
}
