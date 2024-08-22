/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Indent.h"
#include "Actions.h"

#include <KLocalizedString>

#include <QApplication>

#include "core/ApplicationSettings.h"
#include "core/CellStorage.h"
#include "core/Map.h"
#include "core/Sheet.h"
#include "core/Style.h"

using namespace Calligra::Sheets;

Indent::Indent(Actions *actions, bool negative)
    : CellAction(actions,
                 negative ? "decreaseIndentation" : "increaseIndentation",
                 negative ? i18n("Decrease Indent") : i18n("Increase Indent"),
                 QIcon(),
                 negative ? i18n("Decrease the indentation") : i18n("Increase the indentation"))
    , m_negative(negative)
{
    m_negative = negative;
}

Indent::~Indent() = default;

QAction *Indent::createAction()
{
    QAction *res = CellAction::createAction();
    bool negicon = m_negative;
    if (QApplication::isRightToLeft())
        negicon = !negicon;
    res->setIcon(negicon ? koIcon("format-indent-less") : koIcon("format-indent-more"));
    return res;
}

void Indent::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    IndentationCommand *command = new IndentationCommand();
    command->setSheet(sheet);
    double val = sheet->fullMap()->applicationSettings()->indentValue();
    if (m_negative)
        val = -1 * val;
    command->setIndent(val);
    command->add(*selection);
    command->execute(selection->canvas());
}

bool Indent::enabledForSelection(Selection *, const Cell &activeCell)
{
    if (!m_negative)
        return true;
    Style style = activeCell.style();
    if (style.indentation() <= 0.0)
        return false;
    return true;
}

IndentationCommand::IndentationCommand()
    : AbstractRegionCommand()
{
    setIndent(0);
}

bool IndentationCommand::performCommands()
{
    Style curStyle = m_sheet->fullCellStorage()->style(boundingRect().left(), boundingRect().top());
    double cur = curStyle.indentation();
    double indent = cur + m_indent;
    if (indent < 0)
        indent = 0;

    if (cur != indent) {
        Style style;
        style.setIndentation(indent);
        m_sheet->fullCellStorage()->setStyle(*this, style);
    }

    return true;
}

void IndentationCommand::setIndent(double val)
{
    m_indent = val;

    if (val >= 0)
        setText(kundo2_i18n("Increase Indentation"));
    else
        setText(kundo2_i18n("Decrease Indentation"));
}
