/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "NamedAreas.h"
#include "Actions.h"

#include "ui/actions/dialogs/AddNamedAreaDialog.h"
#include "ui/actions/dialogs/NamedAreaDialog.h"

#include <QAction>
#include <KLocalizedString>
#include <KMessageBox>

#include "engine/MapBase.h"
#include "engine/NamedAreaManager.h"
#include "core/Sheet.h"
#include "ui/Selection.h"

#include "ui/commands/NamedAreaCommand.h"


using namespace Calligra::Sheets;


AddNamedArea::AddNamedArea(Actions *actions)
    : CellAction(actions, "setAreaName", i18n("Area Name..."), QIcon(), i18n("Set a name for a region of the spreadsheet"))
    , m_dlg(nullptr)
{
}

AddNamedArea::~AddNamedArea()
{
    if (m_dlg) delete m_dlg;
}


void AddNamedArea::execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget)
{
    m_selection = selection;

    m_dlg = new AddNamedAreaDialog(canvasWidget);
    if (m_dlg->exec()) {

        const QString name = m_dlg->areaName();
        if (name.isEmpty())
            return;

        NamedAreaManager *manager = sheet->map()->namedAreaManager();
        const Region region(selection->lastRange(), sheet);
        if (manager->namedArea(name) == region)
            return; // nothing to do

        NamedAreaCommand* command = 0;
        bool okay = true;
        bool replace = false;

        if (manager->contains(name)) {
            replace = true;
            const QString question = i18n("The named area '%1' already exists.\n"
                                          "Do you want to replace it?", name);
            int result = KMessageBox::warningContinueCancel(canvasWidget, question, i18n("Replace Named Area"),
                         KStandardGuiItem::overwrite());
            if (result == KMessageBox::Cancel) okay = false;
        }

        if (okay) {
            command = new NamedAreaCommand();
            command->setSheet(sheet);
            if (replace) command->setText(kundo2_i18n("Replace Named Area"));
            command->setAreaName(name);
            command->add(region);
            command->execute(selection->canvas());
        }
    }

    delete m_dlg;
    m_dlg = nullptr;
}





ManageNamedAreas::ManageNamedAreas(Actions *actions)
    : CellAction(actions, "namedAreaDialog", i18n("Named Areas..."), koIcon("bookmarks"), i18n("Edit or select named areas"))
{
}

ManageNamedAreas::~ManageNamedAreas()
{
}

QAction *ManageNamedAreas::createAction() {
    QAction *res = CellAction::createAction();
    res->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_G));
    res->setIconText(i18n("Named Areas"));
    return res;
}

void ManageNamedAreas::execute(Selection *selection, Sheet *, QWidget *canvasWidget)
{
    if (!m_dlg) {
        m_dlg = new NamedAreaDialog(canvasWidget, selection);
        connect(m_dlg, &QDialog::finished, this, &ManageNamedAreas::dialogClosed);
    }
    m_dlg->show();
    m_dlg->raise();
    m_dlg->activateWindow();
}

void ManageNamedAreas::dialogClosed()
{
    delete m_dlg;
    m_dlg = nullptr;
}



