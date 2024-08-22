/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2023 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2023 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Paste.h"
#include "Actions.h"
#include "dialogs/PasteInsertDialog.h"
#include "dialogs/SpecialPasteDialog.h"

#include "core/Map.h"
#include "core/Sheet.h"
#include "core/odf/SheetsOdf.h"

#include "ui/CellEditorBase.h"
#include "ui/CellToolBase.h"
#include "ui/commands/DataManipulators.h"
#include "ui/commands/PasteCommand.h"
#include "ui/commands/RowColumnManipulators.h"

#include <KLocalizedString>
#include <KStandardAction>

#include <QApplication>
#include <QBuffer>
#include <QClipboard>
#include <QMimeData>

using namespace Calligra::Sheets;

PasteRegular::PasteRegular(Actions *actions)
    : CellAction(actions, "paste", QString(), QIcon(), i18n("Paste the contents of the clipboard at the cursor"))
{
}

PasteRegular::~PasteRegular() = default;

QAction *PasteRegular::createAction()
{
    QAction *action = KStandardAction::paste(nullptr, nullptr, m_actions->tool());
    connect(action, &QAction::triggered, this, &PasteRegular::triggered);
    action->setToolTip(m_tooltip);
    return action;
}

void PasteRegular::execute(Selection *selection, Sheet *sheet, QWidget *)
{
    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData(QClipboard::Clipboard);

    if (mimeData->hasFormat("application/vnd.oasis.opendocument.spreadsheet")) {
        QByteArray returnedTypeMime = "application/vnd.oasis.opendocument.spreadsheet";
        QByteArray arr = mimeData->data(returnedTypeMime);
        if (arr.isEmpty())
            return;
        QBuffer buffer(&arr);
        Map *map = sheet->fullMap();
        if (!Odf::paste(buffer, map))
            return;
    }

    CellToolBase *tool = m_actions->tool();
    if (tool->editor()) {
        tool->editor()->paste();
        return;
    }

    // If we are pasting regular text, redirect to the insertFromClipboard action, if it exists.
    if (!mimeData->hasFormat("application/x-calligra-sheets-snippet") && !mimeData->hasHtml() && mimeData->hasText()
        && mimeData->text().split('\n').count() >= 2) {
        CellAction *a = m_actions->cellAction("insertFromClipboard");
        if (a)
            a->trigger();
        return;
    }

    // debugSheetsUI <<"Pasting. Rect=" << selection->lastRange() <<" bytes";
    PasteCommand *const command = new PasteCommand();
    command->setSheet(sheet);
    command->add(*selection);
    command->setMimeData(mimeData, clipboard->ownsClipboard());
    command->setPasteFC(true);
    command->execute(selection->canvas());

    selection->initialize(*command, sheet);
    selection->emitModified();
}

PasteSpecial::PasteSpecial(Actions *actions)
    : DialogCellAction(actions,
                       "specialPaste",
                       i18n("Special Paste..."),
                       koIcon("special_paste"),
                       i18n("Paste the contents of the clipboard with special options"))
{
}

PasteSpecial::~PasteSpecial() = default;

ActionDialog *PasteSpecial::createDialog(QWidget *canvasWidget)
{
    SpecialPasteDialog *dlg = new SpecialPasteDialog(canvasWidget);
    connect(dlg, &SpecialPasteDialog::paste, this, &PasteSpecial::paste);
    return dlg;
}

void PasteSpecial::paste()
{
    SpecialPasteDialog *dlg = dynamic_cast<SpecialPasteDialog *>(m_dlg);

    Paste::Mode sp = Paste::Normal;
    Paste::Operation op = Paste::OverWrite;

    if (dlg->wantEverything())
        sp = Paste::Normal;
    else if (dlg->wantText())
        sp = Paste::Text;
    else if (dlg->wantFormat())
        sp = Paste::Format;
    else if (dlg->wantNoBorder())
        sp = Paste::NoBorder;
    else if (dlg->wantComment())
        sp = Paste::Comment;
    else if (dlg->wantResult())
        sp = Paste::Result;

    if (dlg->opOverwrite())
        op = Paste::OverWrite;
    if (dlg->opAdd())
        op = Paste::Add;
    if (dlg->opSub())
        op = Paste::Sub;
    if (dlg->opMul())
        op = Paste::Mul;
    if (dlg->opDiv())
        op = Paste::Div;

    QClipboard *clipboard = QApplication::clipboard();
    PasteCommand *const command = new PasteCommand();
    command->setSheet(m_selection->activeSheet());
    command->add(*m_selection);
    command->setMimeData(clipboard->mimeData(), clipboard->ownsClipboard());
    command->setMode(sp);
    command->setOperation(op);
    command->execute(m_selection->canvas());
}

PasteWithInsert::PasteWithInsert(Actions *actions)
    : CellAction(actions,
                 "pasteWithInsertion",
                 i18n("Paste with Insertion"),
                 koIcon("insertcellcopy"),
                 i18n("Inserts a cell from the clipboard into the spreadsheet"))
    , m_dlg(nullptr)
{
}

PasteWithInsert::~PasteWithInsert()
{
    if (m_dlg)
        delete m_dlg;
}

void PasteWithInsert::execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget)
{
    QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *const mimeData = clipboard->mimeData();

    PasteCommand *const command = new PasteCommand();
    command->setSheet(sheet);
    command->add(*selection);
    command->setMimeData(mimeData, clipboard->ownsClipboard()); // this sets the source, if applicable

    // First the insertion, then the actual pasting
    int shiftMode = -1; // 0 = right, 1 = down

    if (command->unknownShiftDirection()) {
        m_dlg = new PasteInsertDialog(canvasWidget);
        int res = m_dlg->exec();
        if (m_dlg->checkedRight())
            shiftMode = 0; // right
        else
            shiftMode = 1; // down
        delete m_dlg;
        m_dlg = nullptr;
        if (res != QDialog::Accepted) {
            delete command;
            return;
        }
    } else {
        // Determine the shift direction, if needed.
        if (command->isColumnSelected())
            shiftMode = 0; // right
        else
            shiftMode = 1; // down
    }

    // The paste command has now generated the correct areas for us, so let's perform the insertion.
    for (Region::ConstIterator it = command->constBegin(); it != command->constEnd(); ++it) {
        QRect moverect = (*it)->rect();
        SheetBase *movesheet = (*it)->sheet();
        AbstractRegionCommand *shiftCommand = nullptr;

        // Shift cells down.
        if (shiftMode == 1) {
            if (moverect.width() >= KS_colMax) {
                // Rows present.
                shiftCommand = new InsertDeleteRowManipulator();
            } else {
                ShiftManipulator *const command = new ShiftManipulator();
                command->setDirection(ShiftManipulator::ShiftBottom);
                shiftCommand = command;
            }
        }
        // Shift cells right.
        if (shiftMode == 0) {
            if (moverect.height() >= KS_rowMax) {
                // Columns present.
                shiftCommand = new InsertDeleteColumnManipulator();
            } else {
                // Neither columns, nor rows present.
                ShiftManipulator *const command = new ShiftManipulator();
                command->setDirection(ShiftManipulator::ShiftRight);
                shiftCommand = command;
            }
        }

        // shift the data
        if (shiftCommand) {
            shiftCommand->setSheet(dynamic_cast<Sheet *>(movesheet));
            shiftCommand->add(Region(moverect, movesheet));

            shiftCommand->execute(selection->canvas());
        }
    }

    // And now we can actually execute the paste command.
    // TODO - this works, but undo is separate for cell shift and actual paste ... maybe group these?
    command->execute(selection->canvas());
}
