/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Conditional.h"
#include "Actions.h"
#include "ui/actions/dialogs/ConditionalDialog.h"

#include <KLocalizedString>

#include "engine/ValueConverter.h"
#include "engine/ValueParser.h"
#include "core/Cell.h"
#include "core/CellStorage.h"
#include "core/Map.h"
#include "core/Sheet.h"
#include "core/StyleManager.h"
#include "ui/Selection.h"




using namespace Calligra::Sheets;


SetCondition::SetCondition(Actions *actions)
    : CellAction(actions, "conditional", i18n("Conditional Styles..."), QIcon(), i18n("Set cell style based on certain conditions"))
    , m_dlg(nullptr)
{
}

SetCondition::~SetCondition()
{
    if (m_dlg) delete m_dlg;
}


void SetCondition::execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget)
{
    m_dlg = new ConditionalDialog(canvasWidget);
    
    QStringList styles(sheet->fullMap()->styleManager()->styleNames());
    m_dlg->setStyleNames(styles);

    QLinkedList<Conditional> conditionList = Cell(sheet, selection->marker()).conditions().conditionList();

    ValueConverter *converter = sheet->map()->converter();
    int numCondition = 0;
    for (Conditional &c : conditionList) {
        ++numCondition;
        QString val1 = converter->asString(c.value1).asString();
        QString val2 = converter->asString(c.value2).asString();
        m_dlg->setValueRow(numCondition, c.cond, val1, val2, c.styleName);
    }

    if (m_dlg->exec()) {
        ValueParser *parser = sheet->map()->parser();
        QLinkedList<Conditional> newList;
        int id = 1;
        while (m_dlg->getType(id) != Validity::None) {
            Conditional newCondition;
            newCondition.cond = m_dlg->getType(id);
            newCondition.value1 = parser->parse(m_dlg->getValue1(id));
            newCondition.value2 = parser->parse(m_dlg->getValue1(id));
            newCondition.styleName = m_dlg->getStyleName(id);
            newList.append(newCondition);
            ++id;
        }

        ConditionCommand* manipulator = new ConditionCommand();
        manipulator->setSheet(sheet);
        manipulator->setConditionList(newList);
        manipulator->add(*selection);
        manipulator->execute(selection->canvas());
    }

    delete m_dlg;
    m_dlg = nullptr;
}



// this is inside a Clear submenu, hence 'Conditional'
ClearCondition::ClearCondition(Actions *actions)
    : CellAction(actions, "clearConditional", i18n("Conditional Styles"), QIcon(), i18n("Remove the conditional cell styles"))
{
}

ClearCondition::~ClearCondition()
{
}

QAction *ClearCondition::createAction() {
    QAction *res = CellAction::createAction();
    res->setIconText(i18n("Remove Conditional Styles"));
    return res;
}

// We're just keeping this enabled for everything, no need to check every selection change
void ClearCondition::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    ConditionCommand* manipulator = new ConditionCommand();
    manipulator->setSheet(sheet);
    manipulator->setConditionList(QLinkedList<Conditional>());
    manipulator->add(*selection);
    manipulator->execute(selection->canvas());
}



ConditionCommand::ConditionCommand()
        : AbstractRegionCommand()
{
}

bool ConditionCommand::process(Element* element)
{
    m_sheet->fullCellStorage()->setConditions(Region(element->rect()), m_conditions);
    return true;
}

void ConditionCommand::setConditionList(const QLinkedList<Conditional>& list)
{
    m_conditions.setConditionList(list);
    if (m_conditions.isEmpty())
        setText(kundo2_i18n("Remove Conditional Formatting"));
    else
        setText(kundo2_i18n("Add Conditional Formatting"));
}

