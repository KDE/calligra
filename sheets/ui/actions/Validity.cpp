/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Validity.h"
#include "Actions.h"
#include "./dialogs/ValidityDialog.h"

#include "engine/MapBase.h"
#include "core/CellStorage.h"
#include "core/Sheet.h"

#include <KLocalizedString>


using namespace Calligra::Sheets;

SetValidity::SetValidity(Actions *actions)
    : CellAction(actions, "validity", i18n("Validity..."), QIcon(), i18n("Set tests to confirm cell data is valid"))
    , m_dlg(nullptr)
{

}

SetValidity::~SetValidity()
{
    if (m_dlg) delete m_dlg;
}


void SetValidity::execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget)
{
    m_selection = selection;
    CalculationSettings *settings = sheet->map()->calculationSettings();
    ValueParser *parser = sheet->map()->parser();
    m_dlg = new ValidityDialog(canvasWidget, settings, parser);

    Validity validity = Cell(sheet, selection->marker()).validity();
    m_dlg->setValidity(validity);

    if (m_dlg->exec()) {
        validity = m_dlg->getValidity();

        ValidityCommand* manipulator = new ValidityCommand();
        manipulator->setSheet(sheet);
        manipulator->setValidity(validity);
        manipulator->add(*selection);
        manipulator->execute(selection->canvas());
    }

    delete m_dlg;
    m_dlg = nullptr;
}



// This is in the 'Clear' submenu, hence the Validity name and not 'Clear Validity'
ClearValidity::ClearValidity(Actions *actions)
    : CellAction(actions, "clearValidity", i18n("Validity"), QIcon(), i18n("Remove the validity tests on this cell"))
{
}

ClearValidity::~ClearValidity()
{
}


QAction *ClearValidity::createAction() {
    QAction *res = CellAction::createAction();
    res->setIconText(i18n("Remove Validity"));
    return res;
}

void ClearValidity::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    if (sheet->areaIsEmpty(*selection, Sheet::Validity))
        return;

    ValidityCommand* command = new ValidityCommand();
    command->setSheet(sheet);
    command->setValidity(Validity()); // empty object removes validity
    command->add(*selection);
    command->execute(selection->canvas());
}

bool ClearValidity::enabledForSelection(Selection *selection, const Cell &)
{
    Sheet *sheet = selection->activeSheet();
    if (sheet->areaIsEmpty(*selection, Sheet::Validity)) return false;
    return true;
}

ValidityCommand::ValidityCommand()
        : AbstractRegionCommand()
{
}

bool ValidityCommand::process(Element* element)
{
    m_sheet->cellStorage()->setValidity(Region(element->rect()), m_validity);
    return true;
}

void ValidityCommand::setValidity(Validity validity)
{
    m_validity = validity;
    if (m_validity.isEmpty())
        setText(kundo2_i18n("Remove Validity Check"));
    else
        setText(kundo2_i18n("Add Validity Check"));
}

