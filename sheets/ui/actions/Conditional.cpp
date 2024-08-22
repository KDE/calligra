/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Conditional.h"
#include "Actions.h"
#include "ui/actions/dialogs/ConditionalDialog.h"

#include <KLocalizedString>

#include "core/Cell.h"
#include "core/CellStorage.h"
#include "core/Map.h"
#include "core/Sheet.h"
#include "core/StyleManager.h"
#include "engine/ValueConverter.h"
#include "engine/ValueParser.h"
#include "ui/Selection.h"

using namespace Calligra::Sheets;

SetCondition::SetCondition(Actions *actions)
    : DialogCellAction(actions, "conditional", i18n("Conditional Styles..."), QIcon(), i18n("Set cell style based on certain conditions"))
{
}

SetCondition::~SetCondition() = default;

ActionDialog *SetCondition::createDialog(QWidget *canvasWidget)
{
    auto dlg = new ConditionalDialog(canvasWidget);
    connect(dlg, &ConditionalDialog::applyCondition, this, &SetCondition::applyCondition);
    return dlg;
}

void SetCondition::onSelectionChanged()
{
    ConditionalDialog *dlg = dynamic_cast<ConditionalDialog *>(m_dlg);

    Map *map = m_selection->activeSheet()->fullMap();

    QStringList styles(map->styleManager()->styleNames());
    dlg->setStyleNames(styles);

    QList<Conditional> conditionList = activeCell().conditions().conditionList();

    dlg->clear();
    ValueConverter *converter = map->converter();
    int numCondition = 0;
    for (Conditional &c : conditionList) {
        ++numCondition;
        QString val1 = converter->asString(c.value1).asString();
        QString val2 = converter->asString(c.value2).asString();
        dlg->setValueRow(numCondition, c.cond, val1, val2, c.styleName);
    }
}

void SetCondition::applyCondition()
{
    ConditionalDialog *dlg = dynamic_cast<ConditionalDialog *>(m_dlg);
    Map *map = m_selection->activeSheet()->fullMap();

    ValueParser *parser = map->parser();
    QList<Conditional> newList;
    int id = 1;
    while (dlg->getType(id) != Validity::None) {
        Conditional newCondition;
        newCondition.cond = dlg->getType(id);
        newCondition.value1 = parser->parse(dlg->getValue1(id));
        newCondition.value2 = parser->parse(dlg->getValue1(id));
        newCondition.styleName = dlg->getStyleName(id);
        newList.append(newCondition);
        ++id;
    }

    ConditionCommand *manipulator = new ConditionCommand();
    manipulator->setSheet(m_selection->activeSheet());
    manipulator->setConditionList(newList);
    manipulator->add(*m_selection);
    manipulator->execute(m_selection->canvas());
}

// this is inside a Clear submenu, hence 'Conditional'
ClearCondition::ClearCondition(Actions *actions)
    : CellAction(actions, "clearConditional", i18n("Conditional Styles"), QIcon(), i18n("Remove the conditional cell styles"))
{
}

ClearCondition::~ClearCondition() = default;

QAction *ClearCondition::createAction()
{
    QAction *res = CellAction::createAction();
    res->setIconText(i18n("Remove Conditional Styles"));
    return res;
}

// We're just keeping this enabled for everything, no need to check every selection change
void ClearCondition::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    ConditionCommand *manipulator = new ConditionCommand();
    manipulator->setSheet(sheet);
    manipulator->setConditionList({});
    manipulator->add(*selection);
    manipulator->execute(selection->canvas());
}

ConditionCommand::ConditionCommand()
    : AbstractRegionCommand()
{
}

bool ConditionCommand::process(Element *element)
{
    m_sheet->fullCellStorage()->setConditions(Region(element->rect()), m_conditions);
    return true;
}

void ConditionCommand::setConditionList(const QList<Conditional> &list)
{
    m_conditions.setConditionList(list);
    if (m_conditions.isEmpty())
        setText(kundo2_i18n("Remove Conditional Formatting"));
    else
        setText(kundo2_i18n("Add Conditional Formatting"));
}
