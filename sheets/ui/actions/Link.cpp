/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Link.h"
#include "Actions.h"
#include "ui/actions/dialogs/LinkDialog.h"

#include <KLocalizedString>

#include "core/Sheet.h"
#include "engine/CellBaseStorage.h"
#include "engine/MapBase.h"
#include "engine/NamedAreaManager.h"
#include "ui/Selection.h"

using namespace Calligra::Sheets;

Link::Link(Actions *actions)
    : DialogCellAction(actions, "insertHyperlink", i18n("&Link..."), koIcon("insert-link"), i18n("Insert an Internet hyperlink"))
{
    m_closeEditor = true;
}

Link::~Link() = default;

ActionDialog *Link::createDialog(QWidget *canvasWidget)
{
    const NamedAreaManager *manager = m_selection->activeSheet()->map()->namedAreaManager();
    QList<QString> items = manager->areaNames();
    LinkDialog *dlg = new LinkDialog(canvasWidget, items);
    connect(dlg, &LinkDialog::applyLink, this, &Link::setLink);
    return dlg;
}

void Link::setLink(const QString &text, const QString &link)
{
    QString txt = text;
    if (txt.isEmpty())
        txt = link;
    LinkCommand *command = new LinkCommand(txt, link);
    command->setSheet(m_selection->activeSheet());
    command->add(*m_selection);
    command->execute(m_selection->canvas());
}

void Link::onSelectionChanged()
{
    LinkDialog *dlg = dynamic_cast<LinkDialog *>(m_dlg);
    Cell cell = activeCell();
    if (cell.isNull()) {
        dlg->setText(QString());
        dlg->setLink(QString());
    } else {
        dlg->setText(cell.userInput());
        dlg->setLink(cell.link());
    }
}

// this is inside a Clear submenu, hence 'Link'
ClearLink::ClearLink(Actions *actions)
    : CellAction(actions, "clearHyperlink", i18n("Link"), koIcon("view-sort-ascending"), i18n("Remove a link"))
{
}

ClearLink::~ClearLink() = default;

QAction *ClearLink::createAction()
{
    QAction *res = CellAction::createAction();
    res->setIconText(i18n("Remove Link"));
    return res;
}

// We're just keeping this enabled for everything, no need to check every selection change
void ClearLink::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    LinkCommand *command = new LinkCommand(QString(), QString());
    command->setSheet(sheet);
    command->add(*selection);
    command->execute(selection->canvas());
}

LinkCommand::LinkCommand(const QString &text, const QString &link)
{
    newText = text;
    newLink = link;

    setText(newLink.isEmpty() ? kundo2_i18n("Remove Link") : kundo2_i18n("Set Link"));
}

bool LinkCommand::process(Element *element)
{
    QRect range = element->rect();
    for (int col = range.left(); col <= range.right(); ++col)
        for (int row = range.top(); row <= range.bottom(); ++row) {
            Cell cell = Cell(m_sheet, col, row);
            if (cell.isPartOfMerged())
                cell = cell.masterCell();

            cell.parseUserInput(newText);
            cell.setLink(newLink);
        }
    return true;
}
