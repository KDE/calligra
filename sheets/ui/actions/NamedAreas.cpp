/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "NamedAreas.h"
#include "Actions.h"

#include "ui/actions/dialogs/AddNamedAreaDialog.h"
#include "ui/actions/dialogs/NamedAreaDialog.h"

#include <KLocalizedString>
#include <KMessageBox>
#include <QAction>

#include "core/Sheet.h"
#include "engine/MapBase.h"
#include "engine/NamedAreaManager.h"
#include "ui/Selection.h"

#include "ui/commands/NamedAreaCommand.h"

using namespace Calligra::Sheets;

AddNamedArea::AddNamedArea(Actions *actions)
    : DialogCellAction(actions, "setAreaName", i18n("Area Name..."), QIcon(), i18n("Set a name for a region of the spreadsheet"))
{
}

AddNamedArea::~AddNamedArea() = default;

void AddNamedArea::addArea(const QString &name)
{
    if (name.isEmpty())
        return;

    NamedAreaManager *manager = m_selection->activeSheet()->map()->namedAreaManager();
    const Region region(m_selection->lastRange(), m_selection->activeSheet());
    if (manager->namedArea(name) == region) {
        m_dlg->close();
        return; // nothing to do
    }

    NamedAreaCommand *command = nullptr;
    bool okay = true;
    bool replace = false;

    if (manager->contains(name)) {
        replace = true;
        const QString question = i18n(
            "The named area '%1' already exists.\n"
            "Do you want to replace it?",
            name);
        int result = KMessageBox::warningContinueCancel(m_dlg, question, i18n("Replace Named Area"), KStandardGuiItem::overwrite());
        if (result == KMessageBox::Cancel)
            okay = false;
    }

    if (okay) {
        command = new NamedAreaCommand();
        command->setSheet(m_selection->activeSheet());
        if (replace)
            command->setText(kundo2_i18n("Replace Named Area"));
        command->setAreaName(name);
        command->add(region);
        command->execute(m_selection->canvas());
        m_dlg->close();
    }
}

ActionDialog *AddNamedArea::createDialog(QWidget *canvasWidget)
{
    AddNamedAreaDialog *dlg = new AddNamedAreaDialog(canvasWidget);
    connect(dlg, &AddNamedAreaDialog::addArea, this, &AddNamedArea::addArea);
    return dlg;
}

ManageNamedAreas::ManageNamedAreas(Actions *actions)
    : DialogCellAction(actions, "namedAreaDialog", i18n("Named Areas..."), koIcon("bookmarks"), i18n("Edit or select named areas"))
{
}

ManageNamedAreas::~ManageNamedAreas() = default;

QAction *ManageNamedAreas::createAction()
{
    QAction *res = CellAction::createAction();
    res->setShortcut(QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_G));
    res->setIconText(i18n("Named Areas"));
    return res;
}

ActionDialog *ManageNamedAreas::createDialog(QWidget *canvasWidget)
{
    NamedAreaDialog *dlg = new NamedAreaDialog(canvasWidget, m_selection);
    connect(dlg, &NamedAreaDialog::requestSelection, this, &ManageNamedAreas::updateSelection);
    return dlg;
}

void ManageNamedAreas::updateSelection(const Region &region, Sheet *sheet)
{
    if (sheet && sheet != m_selection->activeSheet())
        m_selection->emitVisibleSheetRequested(sheet);
    m_selection->initialize(region);
    m_selection->emitModified();
}
