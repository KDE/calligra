/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005, 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_CONDITION_COMMAND
#define CALLIGRA_SHEETS_CONDITION_COMMAND

#include "AbstractRegionCommand.h"
#include "core/Condition.h"

#include <QLinkedList>

namespace Calligra
{
namespace Sheets
{

/**
 * \class ConditionCommand
 * \ingroup Commands
 * \brief Adds/Removes conditional formatting to/of a cell region.
 */
class ConditionCommand : public AbstractRegionCommand
{
public:
    ConditionCommand();
    void setConditionList(const QLinkedList<Conditional>& list);

protected:
    bool process(Element* element) override;

private:
    Conditions m_conditions;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_CONDITION_COMMAND
