/* This file is part of the KDE project
// SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>
   SPDX-FileCopyrightText: 2007, 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 1999-2007 The KSpread Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "PasteCommand.h"

#include <QMimeData>

#include "DataManipulators.h"
#include "DeleteCommand.h"
#include "PageBreakCommand.h"
#include "RowColumnManipulators.h"

#include "engine/DependencyManager.h"
#include "engine/MapBase.h"
#include "core/CellStorage.h"
#include "core/Sheet.h"
#include "core/ksp/SheetsKsp.h"


using namespace Calligra::Sheets;


PasteCommand::PasteCommand(KUndo2Command *parent)
        : AbstractRegionCommand(parent)
        , m_mimeData(0)
        , m_insertMode(NoInsertion)
        , m_pasteMode(Paste::Normal)
        , m_operation(Paste::OverWrite)
        , m_pasteFC(false)
        , m_sameApp(true)
{
}

PasteCommand::~PasteCommand()
{
}

const QMimeData* PasteCommand::mimeData() const
{
    return m_mimeData;
}

bool PasteCommand::setMimeData(const QMimeData *mimeData)
{
    if (!mimeData) {
        return false;
    }
    m_mimeData = mimeData;
    return true;
}

void PasteCommand::setInsertionMode(InsertionMode mode)
{
    m_insertMode = mode;
}

void PasteCommand::setMode(Paste::Mode mode)
{
    m_pasteMode = mode;
}

void PasteCommand::setOperation(Paste::Operation operation)
{
    m_operation = operation;
}

void PasteCommand::setPasteFC(bool force)
{
    m_pasteFC = force;
}

void PasteCommand::setSameApp(bool same)
{
    m_sameApp = same;
}

bool PasteCommand::isApproved() const
{
    if (supports(m_mimeData)) {
        return AbstractRegionCommand::isApproved();
    }
    warnSheets << "Unrecognized MIME type(s):" << m_mimeData->formats().join(", ");
    return false;
}

// static
bool PasteCommand::supports(const QMimeData *mimeData)
{
    if (mimeData->hasFormat("application/x-calligra-sheets-snippet")) {
        return true;
    } else if (mimeData->hasText()) {
        return true;
    } else if (mimeData->hasHtml()) {
        // TODO handle HTML tables
        return false;
    } else if (mimeData->hasFormat("text/csv")) {
        // TODO parse CSV data
        return false;
    }
    return false;
}

// static
bool PasteCommand::unknownShiftDirection(const QMimeData *mimeData)
{
    if (!mimeData) return false;

    QByteArray byteArray;

    if (mimeData->hasFormat("application/x-calligra-sheets-snippet")) {
        byteArray = mimeData->data("application/x-calligra-sheets-snippet");
    } else {
        return false;
    }

    QString data = QString::fromUtf8(byteArray);
    // find a column or row entry
    QStringList lines = data.split("\n");
    if (lines.isEmpty()) return false;
    for (QString line : lines) {
        if (line.startsWith("row ")) return false;
        if (line.startsWith("column ")) return false;
    }
    return true;
}

bool PasteCommand::performCommands()
{
    const QList<Element *> elements = cells();

    bool isSnippet = m_mimeData->hasFormat("application/x-calligra-sheets-snippet");
    if (!m_sameApp) isSnippet = false;     // cannot use the snippet if it is not ours (it only contains coordinates)
    QString data;
    if (!isSnippet) {
        QString data = m_mimeData->text();
        for (int i = 0; i < elements.count(); ++i)
            processTextPlain(elements[i], data);
        return true;
    }

    QByteArray byteArray = m_mimeData->data("application/x-calligra-sheets-snippet");
    data = QString::fromUtf8(byteArray);
    QStringList list = data.split('\n');
    if (!list.size()) return true;  // nothing to do?
    // cut or copy?
    bool isCut = false;
    if (list.size() && (list[0] == "CUT")) {
        isCut = true;
        list.removeFirst();
    }
    int areas = 0;
    QList<QRect> rects;
    QList<SheetBase *> sheets;
    for (QString e : list) {
        QStringList parts = e.split(' ');
        if (parts.size() < 6) continue;
        bool ok;
        int x1 = parts.at(1).toUInt(&ok);
        if (!ok) continue;
        int y1 = parts.at(2).toUInt(&ok);
        if (!ok) continue;
        int x2 = parts.at(3).toUInt(&ok);
        if (!ok) continue;
        int y2 = parts.at(4).toUInt(&ok);
        if (!ok) continue;
        QString sheetName;
        for (int i = 5; i < parts.size(); ++i) {
            if (i > 5) sheetName += ' ';
            sheetName += parts.at(i);
        }
        QRect rect = QRect(QPoint(x1, y1), QPoint(x2, y2));
        SheetBase *sheet = m_sheet->map()->findSheet (sheetName);
        if (!sheet) sheet = m_sheet;
        rects.append(rect);
        sheets.append(sheet);
        areas++;
    }
    if (!areas) return true;

    // Iterate over all region elements and build the sub-commands.
    int datapos = 0;
    for (int i = 0; i < elements.count(); ++i) {
        processSnippetData(elements[i], rects[datapos], sheets[datapos], isCut);
        datapos = (datapos + 1) % areas;
    }
    return true;
}

bool PasteCommand::processSnippetData(Element *element, QRect sourceRect, SheetBase *sourceSheet, bool isCut)
{
    const QRect pasteArea = element->rect();
    Sheet *sheet = dynamic_cast<Sheet *>(element->sheet());
    Q_ASSERT(sheet == m_sheet);

    const int sourceWidth = sourceRect.width();
    const int sourceHeight = sourceRect.height();

    int pasteWidth = sourceWidth;
    // If we have a selection that is bigger, but doesn't span a full row, use that instead.
    if ((pasteArea.width() >= sourceWidth) && (pasteArea.width() < KS_colMax)) 
        pasteWidth = pasteArea.width();

    // Same for height.
    int pasteHeight = sourceHeight;
    if ((pasteArea.height() >= sourceHeight) && (pasteArea.height() < KS_rowMax))
        pasteHeight = pasteArea.height();

    // Determine the shift direction, if needed.
    if (m_insertMode == ShiftCells) {
        m_insertMode = ShiftCellsRight;
        if (sourceWidth == KS_colMax) m_insertMode = ShiftCellsDown;
    }

    // Shift cells down.
    if (m_insertMode == ShiftCellsDown) {
        if (pasteWidth == KS_colMax) {
            // Rows present.
            InsertDeleteRowManipulator *const command = new InsertDeleteRowManipulator(this);
            command->setSheet(sheet);
            command->add(Region(pasteArea.x(), pasteArea.y(), pasteWidth, pasteHeight, sheet));
        } else {
            ShiftManipulator *const command = new ShiftManipulator(this);
            command->setSheet(sheet);
            command->add(Region(pasteArea.x(), pasteArea.y(), pasteWidth, pasteHeight, sheet));
            command->setDirection(ShiftManipulator::ShiftBottom);
        }
    }
    // Shift cells right.
    if (m_insertMode == ShiftCellsRight) {
        if (pasteHeight == KS_rowMax) {
            // Columns present.
            InsertDeleteColumnManipulator *const command = new InsertDeleteColumnManipulator(this);
            command->setSheet(sheet);
            command->add(Region(pasteArea.x(), pasteArea.y(), pasteWidth, pasteHeight, sheet));
        } else {
            // Neither columns, nor rows present.
            ShiftManipulator *const command = new ShiftManipulator(this);
            command->setSheet(sheet);
            command->add(Region(pasteArea.x(), pasteArea.y(), pasteWidth, pasteHeight, sheet));
            command->setDirection(ShiftManipulator::ShiftRight);
        }
    }

    Sheet *fullSourceSheet = dynamic_cast<Sheet *>(sourceSheet);

    // If the areas partially overlap, we need to make sure that we don't overwrite data that hasn't been copied yet
    // This will set reverse mode even if it isn't needed due to no overlap, but that's fine.
    bool reverseRows = false, reverseCols = false;
    if (sheet == fullSourceSheet) {
        if (sourceRect.top() > pasteArea.top()) reverseRows = true;
        if (sourceRect.left() > pasteArea.left()) reverseCols = true;
    }

    if (fullSourceSheet) {

        // If full rows/columns are involved, we copy their attributes, too.
        if (sourceWidth == KS_colMax) {
            RowFormatStorage *rowFormats = fullSourceSheet->rowFormats();

            for (int yy = 0; yy < pasteHeight; yy++) {
                int y = reverseRows ? (pasteHeight - 1 - yy) : yy;
                int srcRow = sourceRect.top() + y;
                int tgRow = pasteArea.top() + y;

                Region region = Region(1, tgRow, 1, 1, sheet);
                ResizeRowManipulator *const resize = new ResizeRowManipulator(this);
                resize->setSheet(sheet);
                resize->add(region);
                resize->setSize(rowFormats->rowHeight(srcRow));
                HideShowManipulator *const hideShow = new HideShowManipulator(this);
                hideShow->setManipulateColumns(false);
                hideShow->setSheet(sheet);
                hideShow->add(region);
                hideShow->setHide(!rowFormats->isHidden(srcRow));
                PageBreakCommand *const pageBreak = new PageBreakCommand(this);
                pageBreak->setMode(PageBreakCommand::BreakBeforeRow);
                pageBreak->setSheet(sheet);
                pageBreak->add(region);
                pageBreak->setBreak(rowFormats->hasPageBreak(srcRow));
            }
        }
        if (sourceHeight == KS_rowMax) {
            ColFormatStorage *columnFormats = fullSourceSheet->columnFormats();
            for (int xx = 0; xx < pasteWidth; xx++) {
                int x = reverseCols ? (pasteWidth - 1 - xx) : xx;
                int srcCol = sourceRect.left() + x;
                int tgCol = pasteArea.left() + x;

                Region region = Region(tgCol, 1, 1, 1, sheet);
                ResizeColumnManipulator *const resize = new ResizeColumnManipulator(this);
                resize->setSheet(sheet);
                resize->add(region);
                resize->setSize(columnFormats->colWidth(srcCol));
                HideShowManipulator *const hideShow = new HideShowManipulator(this);
                hideShow->setManipulateColumns(true);
                hideShow->setSheet(sheet);
                hideShow->add(region);
                hideShow->setHide(!columnFormats->isHidden(srcCol));
                PageBreakCommand *const pageBreak = new PageBreakCommand(this);
                pageBreak->setMode(PageBreakCommand::BreakBeforeColumn);
                pageBreak->setSheet(sheet);
                pageBreak->add(region);
                pageBreak->setBreak(columnFormats->hasPageBreak(srcCol));
            }
        }
    }

    // Now copy the actual cells.
    int sourceMaxX = sourceSheet->cellStorage()->columns();
    int sourceMaxY = sourceSheet->cellStorage()->rows();
    for (int yy = 0; yy < pasteHeight; yy++) {
        int y = reverseRows ? (pasteHeight - 1 - yy) : yy;
        for (int xx = 0; xx < pasteWidth; xx++) {
            int x = reverseCols ? (pasteWidth - 1 - xx) : xx;
            int srcX = sourceRect.left() + x;
            int srcY = sourceRect.top() + y;
            int tgX = pasteArea.left() + x;
            int tgY = pasteArea.top() + y;

            // do not copy empty cells
            if (srcX > sourceMaxX) break;
            if (srcY > sourceMaxY) break;
            if (tgX > KS_colMax) break;
            if (tgY > KS_rowMax) break;

            // The coordinates are good.
            Cell srcCell = Cell(fullSourceSheet, srcX, srcY);
            Cell tgCell = Cell(sheet, tgX, tgY);
            tgCell.copyAll(srcCell, m_pasteMode, m_operation);
        }
    }

    if (isCut) {
        DeleteCommand* command = new DeleteCommand(this);
        command->setSheet(fullSourceSheet);
        command->add(sourceRect);
    }

    return true;
}

bool PasteCommand::processTextPlain(Element *element, const QString &text)
{
    if (text.isEmpty()) {
        return false;
    }

    // Split the text into lines.
    const QStringList list = text.split('\n');

//    const int mx = 1; // always one column
    const int my = list.count();

    // Put the lines into an array value.
    Value value(Value::Array);
    for (int i = 0; i < my; ++i) {
        value.setElement(0, i, Value(list[i]));
    }

    // FIXME Determine and tile the destination area.
//     Region range(mx, my, 1, list.size());

    // create a command, configure it and execute it
    DataManipulator *command = new DataManipulator(this);
    command->setSheet(m_sheet);
    command->setParsing(false);
    command->setValue(value);
    command->add(element->rect(), m_sheet);
    return true;
}
