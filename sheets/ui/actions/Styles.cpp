/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Styles.h"
#include "Actions.h"

#include "core/Cell.h"
#include "core/Map.h"
#include "core/Sheet.h"
#include "core/Style.h"
#include "core/StyleManager.h"

#include "ui/CellToolBase.h"
#include "ui/Selection.h"
#include "ui/commands/StyleCommand.h"

#include "dialogs/StyleManagerDialog.h"

#include <KLocalizedString>
#include <KMessageBox>
#include <KSelectAction>
#include <QInputDialog>

using namespace Calligra::Sheets;

StyleFromCell::StyleFromCell(Actions *actions)
    : CellAction(actions, "createStyleFromCell", i18n("Create Style From Cell..."), QIcon(), i18n("Create a new style based on the currently selected cell"))
{
}

StyleFromCell::~StyleFromCell() = default;

QAction *StyleFromCell::createAction()
{
    QAction *res = CellAction::createAction();
    res->setIconText(i18n("Style From Cell"));
    return res;
}

void StyleFromCell::execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget)
{
    Cell cell = Cell(sheet, selection->cursor());

    bool ok = false;
    QString styleName;
    StyleManager *manager = sheet->fullMap()->styleManager();

    while (true) {
        styleName = QInputDialog::getText(canvasWidget, i18n("Create Style From Cell"), i18n("Enter name:"), QLineEdit::Normal, styleName, &ok);

        if (!ok) // User pushed an OK button.
            return;

        styleName = styleName.trimmed();

        if (styleName.length() < 1) {
            KMessageBox::error(canvasWidget, i18n("The style name cannot be empty."));
            continue;
        }

        if (manager->style(styleName) != nullptr) {
            KMessageBox::error(canvasWidget, i18n("A style with this name already exists."));
            continue;
        }
        break;
    }

    const Style cellStyle = cell.style();
    CustomStyle *style = new CustomStyle(styleName);
    style->merge(cellStyle);

    manager->insertStyle(style);
    cell.setStyle(*style);
}

bool StyleFromCell::enabledForSelection(Selection *selection, const Cell &)
{
    return selection->isSingular();
}

ShowStyleManager::ShowStyleManager(Actions *actions)
    : DialogCellAction(actions, "styleDialog", i18n("Style Manager..."), QIcon(), i18n("Edit and organize cell styles"))
{
}

ShowStyleManager::~ShowStyleManager() = default;

ActionDialog *ShowStyleManager::createDialog(QWidget *canvasWidget)
{
    StyleManager *const styleManager = m_selection->activeSheet()->fullMap()->styleManager();
    StyleManagerDialog *dlg = new StyleManagerDialog(canvasWidget, m_selection, styleManager);
    connect(dlg, &StyleManagerDialog::setStyle, this, &ShowStyleManager::setStyle);
    return dlg;
}

void ShowStyleManager::setStyle(const QString &name)
{
    Selection *sel = m_actions->tool()->selection();
    Sheet *sheet = sel->activeSheet();
    StyleManager *const styleManager = sheet->fullMap()->styleManager();
    CustomStyle *style = styleManager->style(name);
    if (!style)
        style = styleManager->defaultStyle();

    StyleCommand *command = new StyleCommand();
    command->setSheet(sheet);
    Style s;
    if (style->isDefault())
        s.setDefault();
    else
        s.setParentName(name);
    command->setStyle(s);
    command->add(*sel);
    command->execute(sel->canvas());
}

SetStyle::SetStyle(Actions *actions)
    : CellAction(actions, "setStyle", i18n("Style"), QIcon(), i18n("Apply a predefined style to the selected cells"))
{
}

SetStyle::~SetStyle() = default;

void SetStyle::init()
{
    Selection *sel = m_actions->tool()->selection();
    const StyleManager *styleManager = sel->activeSheet()->fullMap()->styleManager();
    connect(styleManager, &StyleManager::styleListChanged, this, &SetStyle::fillStyles);
    fillStyles();
}

QAction *SetStyle::createAction()
{
    m_selectAction = new KSelectAction(m_caption, m_actions->tool());
    m_selectAction->setToolTip(m_tooltip);
    connect(m_selectAction, &KSelectAction::textTriggered, this, &SetStyle::triggeredSelect);
    return m_selectAction;
}

// TODO this duplicates setStyle above, can we merge them somehow sanely?
void SetStyle::triggeredSelect(const QString &name)
{
    Selection *sel = m_actions->tool()->selection();
    Sheet *sheet = sel->activeSheet();
    StyleManager *const styleManager = sheet->fullMap()->styleManager();
    CustomStyle *style = styleManager->style(name);
    if (!style)
        style = styleManager->defaultStyle();

    StyleCommand *command = new StyleCommand();
    command->setSheet(sheet);
    Style s;
    if (style->isDefault())
        s.setDefault();
    else
        s.setParentName(name);
    command->setStyle(s);
    command->add(*sel);
    command->execute(sel->canvas());
}

void SetStyle::fillStyles()
{
    // Initialize cell style selection action.
    Selection *sel = m_actions->tool()->selection();
    const StyleManager *styleManager = sel->activeSheet()->fullMap()->styleManager();
    m_selectAction->setItems(styleManager->styleNames());
}
