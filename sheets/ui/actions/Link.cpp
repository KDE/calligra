/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Link.h"
#include "Actions.h"
#include "ui/actions/dialogs/LinkDialog.h"

#include <KLocalizedString>

#include "engine/CellBaseStorage.h"
#include "engine/MapBase.h"
#include "engine/NamedAreaManager.h"
#include "core/Sheet.h"
#include "ui/Selection.h"



using namespace Calligra::Sheets;


Link::Link(Actions *actions)
    : CellAction(actions, "insertHyperlink", i18n("&Link..."), koIcon("insert-link"), i18n("Insert an Internet hyperlink"))
    , m_dlg(nullptr)
{
    m_closeEditor = true;
}

Link::~Link()
{
    if (m_dlg) delete m_dlg;
}


void Link::execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget)
{
    m_selection = selection;
    Cell cell(sheet, selection->marker());
    
    const NamedAreaManager *manager = sheet->map()->namedAreaManager();
    QList<QString> items = manager->areaNames();
    items.append(cell.fullName());

    m_dlg = new LinkDialog(canvasWidget, items);
    m_dlg->setWindowTitle(i18n("Insert Link"));

    if (!cell.isNull()) {
        m_dlg->setText(cell.userInput());
        if (!cell.link().isEmpty()) {
            m_dlg->setWindowTitle(i18n("Edit Link"));
            m_dlg->setLink(cell.link());
        }
    }

    if (m_dlg->exec() == QDialog::Accepted) {
        QString text = m_dlg->text();
        QString link = m_dlg->link();
        if (text.isEmpty()) text = link;
        LinkCommand* command = new LinkCommand(m_dlg->text(), m_dlg->link());
        command->setSheet(sheet);
        command->add(*selection);
        command->execute(selection->canvas());
    }

    delete m_dlg;
    m_dlg = nullptr;
}




// this is inside a Clear submenu, hence 'Link'
ClearLink::ClearLink(Actions *actions)
    : CellAction(actions, "clearHyperlink", i18n("Link"), koIcon("view-sort-ascending"), i18n("Remove a link"))
{
}

ClearLink::~ClearLink()
{
}

QAction *ClearLink::createAction() {
    QAction *res = CellAction::createAction();
    res->setIconText(i18n("Remove Link"));
    return res;
}

// We're just keeping this enabled for everything, no need to check every selection change
void ClearLink::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    LinkCommand* command = new LinkCommand(QString(), QString());
    command->setSheet(sheet);
    command->add(*selection);
    command->execute(selection->canvas());
}





LinkCommand::LinkCommand(const QString& text, const QString& link)
{
    newText = text;
    newLink = link;

    setText(newLink.isEmpty() ? kundo2_i18n("Remove Link") : kundo2_i18n("Set Link"));
}

bool LinkCommand::process(Element* element)
{
    QRect range = element->rect();
    for (int col = range.left(); col <= range.right(); ++col)
        for (int row = range.top(); row <= range.bottom(); ++row) {
            Cell cell = Cell(m_sheet, col, row);
            if (cell.isPartOfMerged()) cell = cell.masterCell();

            cell.parseUserInput(newText);
            cell.setLink(newLink);
        }
    return true;
}


