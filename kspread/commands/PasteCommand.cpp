/* This file is part of the KDE project
   Copyright 2007,2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 1999-2007 The KSpread Team <koffice-devel@kde.org>
   Copyright 1998,1999 Torben Weis <weis@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "PasteCommand.h"

#include <QApplication>
#include <QMimeData>

#include "CellStorage.h"
#include "commands/ColumnStyleCommand.h"
#include "commands/DataManipulators.h"
#include "commands/DeleteCommand.h"
#include "commands/RowColumnManipulators.h"
#include "commands/RowStyleCommand.h"
#include "DependencyManager.h"
#include "Map.h"
#include "RowColumnFormat.h"
#include "Sheet.h"

// TODO
// - Extract the pasting code from Cell.
// - Get plain text pasting right.


using namespace KSpread;

class PasteCellCommand : public AbstractRegionCommand
{
public:
    PasteCellCommand(QUndoCommand *parent = 0)
            : AbstractRegionCommand(parent)
            , m_pasteMode(Paste::Normal)
            , m_pasteOperation(Paste::OverWrite)
            , m_pasteFC(false) {
    }
    virtual ~PasteCellCommand() {}

    void addXmlElement(const Cell &cell, const KoXmlElement &element) {
        add(cell.cellPosition(), m_sheet);
        m_elements.insert(cell, element);
    }

    Paste::Mode         m_pasteMode;
    Paste::Operation    m_pasteOperation;
    bool                m_pasteFC; // FIXME What's that? ForceConditions?

protected:
    bool process(Element *element) {
        // Destination cell:
        Cell cell(m_sheet, element->rect().topLeft());
        const int xOffset = cell.column() - m_elements[cell].attribute("column").toInt();
        const int yOffset = cell.row() - m_elements[cell].attribute("row").toInt();
        return cell.load(m_elements[cell], xOffset, yOffset,
                         m_pasteMode, m_pasteOperation, m_pasteFC);
    }

    bool preProcessing() {
        if (m_firstrun) {
            m_sheet->cellStorage()->startUndoRecording();
        }
        return true;
    }

    bool mainProcessing() {
        if (m_reverse) {
            QUndoCommand::undo(); // undo child commands
            return true;
        }
        return AbstractRegionCommand::mainProcessing();
    }

    bool postProcessing() {
        if (m_firstrun) {
            m_sheet->cellStorage()->stopUndoRecording(this);
        }
        return true;
    }

private:
    QHash<Cell, KoXmlElement> m_elements;
};



PasteCommand::PasteCommand(QUndoCommand *parent)
        : AbstractRegionCommand(parent)
        , m_mimeData(0)
        , m_xmlDocument(0)
        , m_insertMode(NoInsertion)
        , m_pasteMode(Paste::Normal)
        , m_operation(Paste::OverWrite)
        , m_pasteFC(false)
{
}

PasteCommand::~PasteCommand()
{
    delete m_xmlDocument;
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

bool PasteCommand::isApproved() const
{
    if (supports(m_mimeData)) {
        return AbstractRegionCommand::isApproved();
    }
    kWarning() << "Unrecognized MIME type(s):" << m_mimeData->formats().join(", ");
    return false;
}

// static
bool PasteCommand::supports(const QMimeData *mimeData)
{
    if (mimeData->hasFormat("application/x-kspread-snippet")) {
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
    if (!mimeData) {
        return false;
    }

    QByteArray byteArray;

    if (mimeData->hasFormat("application/x-kspread-snippet")) {
        byteArray = mimeData->data("application/x-kspread-snippet");
    } else {
        return false;
    }

    QString errorMsg;
    int errorLine;
    int errorColumn;
    KoXmlDocument d;
    if (!d.setContent(byteArray, false, &errorMsg, &errorLine, &errorColumn)) {
        // an error occurred
        kDebug() << "An error occurred."
        << "line:" << errorLine << "col:" << errorColumn << errorMsg;
        return false;
    }

    KoXmlElement e = d.documentElement();
    if (!e.namedItem("columns").toElement().isNull()) {
        return false;
    }

    if (!e.namedItem("rows").toElement().isNull()) {
        return false;
    }

    KoXmlElement c = e.firstChild().toElement();
    for (; !c.isNull(); c = c.nextSibling().toElement()) {
        if (c.tagName() == "cell") {
            return true;
        }
    }
    return false;
}

bool PasteCommand::preProcessing()
{
    return AbstractRegionCommand::preProcessing();
}

bool PasteCommand::mainProcessing()
{
    if (!m_reverse) { // apply/redo
        if (m_firstrun) { // apply
            // First, prepare the data ONCE for all region elements.
            if (m_mimeData->hasFormat("application/x-kspread-snippet")) {
                m_xmlDocument = new KoXmlDocument();
                const QByteArray data = m_mimeData->data("application/x-kspread-snippet");
                kDebug(36005) << "Parsing" << data.size() << "bytes";
                QString errorMsg;
                int errorLine;
                int errorColumn;
                if (!m_xmlDocument->setContent(data, false, &errorMsg, &errorLine, &errorColumn)) {
                    // an error occurred
                    kDebug(36005) << "An error occurred." << "line:" << errorLine
                    << "col:" << errorColumn << errorMsg;
                    return false;
                }
            } else if (m_mimeData->hasText()) {
                // TODO Maybe prepare the string list here?!
            }

            // Iterate over all region elements and build the sub-commands.
            const QList<Element *> elements = cells();
            const int begin = m_reverse ? elements.count() - 1 : 0;
            const int end = m_reverse ? -1 : elements.count();
            for (int i = begin; i != end; m_reverse ? --i : ++i) {
                if (m_mimeData->hasFormat("application/x-kspread-snippet")) {
                    processXmlData(elements[i], m_xmlDocument);
                } else if (m_mimeData->hasText()) {
                    processTextPlain(elements[i]);
                }
            }
        }
        QUndoCommand::redo(); // redo the child commands
    } else { // undo
        QUndoCommand::undo(); // undo the child commands
    }
    return true;
}

bool PasteCommand::postProcessing()
{
    return AbstractRegionCommand::postProcessing();
}

bool PasteCommand::processXmlData(Element *element, KoXmlDocument *data)
{
    const QRect pasteArea = element->rect();
    Sheet *const sheet = element->sheet();
    Q_ASSERT(sheet == m_sheet);
    Map *const map = sheet->map();

    const KoXmlElement root = data->documentElement(); // "spreadsheet-snippet"
    if (root.hasAttribute("cut")) {
        const Region cutRegion(root.attribute("cut"), map, sheet);
        if (cutRegion.isValid()) {
            const Cell destination(sheet, pasteArea.topLeft());
            map->dependencyManager()->regionMoved(cutRegion, destination);
        }
    }

    const int sourceHeight = root.attribute("rows").toInt();
    const int sourceWidth  = root.attribute("columns").toInt();

    // Find size of rectangle that we want to paste to (either clipboard size or current selection)
    const bool noRowsInClipboard    = root.namedItem("rows").toElement().isNull();
    const bool noColumnsInClipboard = root.namedItem("columns").toElement().isNull();
    const bool noRowsSelected       = !Region::Range(pasteArea).isRow();
    const bool noColumnsSelected    = !Region::Range(pasteArea).isColumn();
    const bool biggerSelectedWidth  = pasteArea.width()  >= sourceWidth;
    const bool biggerSelectedHeight = pasteArea.height() >= sourceHeight;

    const int pasteWidth  = biggerSelectedWidth && noRowsSelected && noRowsInClipboard
                            ? pasteArea.width() : sourceWidth;
    const int pasteHeight = biggerSelectedHeight && noColumnsSelected && noColumnsInClipboard
                            ? pasteArea.height() : sourceHeight;

    const int xOffset = noRowsInClipboard ? pasteArea.left() - 1 : 0;
    const int yOffset = noColumnsInClipboard ? pasteArea.top() - 1 : 0;

    kDebug(36005) << "selected size (col x row):" << pasteArea.width() << 'x' << pasteArea.height();
    kDebug(36005) << "source size (col x row):" << sourceWidth << 'x' << sourceHeight;
    kDebug(36005) << "paste area size (col x row):" << pasteWidth << 'x' << pasteHeight;
    kDebug(36005) << "xOffset:" << xOffset << "yOffset:" << yOffset;

    // Determine the shift direction, if needed.
    if (m_insertMode == ShiftCells) {
        if (!noColumnsInClipboard && !noRowsInClipboard) {
            // There are columns and rows in the source data.
            m_insertMode = ShiftCellsRight; // faster than down
        } else if (!noColumnsInClipboard) {
            // There are columns in the source data.
            m_insertMode = ShiftCellsRight;
        } else if (!noRowsInClipboard) {
            // There are rows in the source data.
            m_insertMode = ShiftCellsDown;
        } else {
            // Should not happen.
            // ShiftCells should only be set, if the data contains columns/rows.
            Q_ASSERT(false);
            m_insertMode = ShiftCellsRight; // faster than down
        }
    }

    const bool noColumns = noColumnsInClipboard && noColumnsSelected;
    const bool noRows = noRowsInClipboard && noRowsSelected;

    // Shift cells down.
    if (m_insertMode == ShiftCellsDown) {
        // Cases:
        // 1. Columns AND rows are contained in either source or selection
        // 1.a Columns in source and rows in selection
        //      I.e. yOffset=0
        //      Clear everything.
        //      Taking the column data/style and fill all columns.
        // 1.b Columns and rows in source, but not in selection
        //      I.e. xOffset=0,yOffset=0
        //      Leave everything as is? No, data from different sheet is possible!
        //      Clear everything.
        //      Fill with the source column/row data/style,
        //      i.e. the sheet data becomes equal to the source data.
        //      Same procedure as in 1.e
        // 1.c Columns and rows in selection, but not in source
        //      Clear everything.
        //      Fill with the source data. Tiling -> HUGE task!
        // 1.d Rows in source and columns in selection
        //      I.e. xOffset=0
        //      Clear everything.
        //      Taking the row data/style and fill all rows.
        // 1.e Columns AND rows in both
        //      I.e. xOffset=0,yOffset=0
        //      Leave everything as is? No, data from different sheet is possible!
        //      Clear everything.
        //      Fill with the source column/row data/style,
        //      i.e. the sheet data becomes equal to the source data.
        //      Same procedure as in 1.b
        // 2. Columns are present in either source or selection, but no rows
        // 2a Columns in source
        //      I.e. yOffset=0
        //      Clear the appropriate columns in the paste area.
        //      Fill them with the source data.
        // 2b Columns in selection
        //      Clear the selected columns.
        //      Fill them with the source data. Tiling -> HUGE task!
        // 2c Columns in both
        //      I.e. yOffset=0
        //      Clear the selected columns.
        //      Fill them with the source column data/style.
        // 3. Rows are present in either source or selection, but no columns
        // 3a Rows in source
        //      I.e. xOffset=0
        //      Insert rows.
        //      Fill in data.
        // 3b Rows in selection
        //      Insert rows.
        //      Fill in data. Tiling -> HUGE task!
        // 3c Rows in both
        //      I.e. xOffset=0
        //      Insert rows.
        //      Fill in data/style from source rows.
        // 4. Neither column, nor rows are present
        //      Shift the cell down.
        //      Fill in data.
        if ((!noColumns && !noRows) || (!noColumns && noRows)) {
            // Everything or only columns present.
            DeleteCommand *const command = new DeleteCommand(this);
            command->setSheet(m_sheet);
            command->add(Region(pasteArea.x(), pasteArea.y(), pasteWidth, pasteHeight, sheet));
            command->setMode(DeleteCommand::OnlyCells);
        } else if (noColumns && !noRows) {
            // Rows present.
            InsertDeleteRowManipulator *const command = new InsertDeleteRowManipulator(this);
            command->setSheet(sheet);
            command->add(Region(pasteArea.x(), pasteArea.y(), pasteWidth, pasteHeight, sheet));
        } else {
            // Neither columns, nor rows present.
            ShiftManipulator *const command = new ShiftManipulator(this);
            command->setSheet(sheet);
            command->add(Region(pasteArea.x(), pasteArea.y(), pasteWidth, pasteHeight, sheet));
            command->setDirection(ShiftManipulator::ShiftBottom);
        }
    }
    // Shift cells right.
    if (m_insertMode == ShiftCellsRight) {
        // Cases:
        // Same as for ShiftCellsDown,
        // except that clearing and inserting are exchanged for cases 2 and 3.
        // Shifting a column to the right is the same as column insertion.
        // Shifting a row to the right is the same as clearing the row.
        if ((!noColumns && !noRows) || (noColumns && !noRows)) {
            // Everything or only rows present.
            DeleteCommand *const command = new DeleteCommand(this);
            command->setSheet(m_sheet);
            command->add(Region(pasteArea.x(), pasteArea.y(), pasteWidth, pasteHeight, sheet));
            command->setMode(DeleteCommand::OnlyCells);
        } else if (!noColumns && noRows) {
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

    // This command will collect as many cell loads as possible in the iteration.
    PasteCellCommand *pasteCellCommand = 0;

    KoXmlElement e = root.firstChild().toElement(); // "columns", "rows" or "cell"
    for (; !e.isNull(); e = e.nextSibling().toElement()) {
        // If the element is not a cell, unset the pasteCellCommand pointer.
        // If existing, it is attached as child commnand, so no leaking here.
        if (e.tagName() != "cell") {
            pasteCellCommand = 0;
        }

        // entire columns given
        if (e.tagName() == "columns" && !sheet->isProtected()) {
            const int number = e.attribute("count").toInt();
            if (m_insertMode == NoInsertion) {
                // Clear the existing content; not the column style.
                DeleteCommand *const command = new DeleteCommand(this);
                command->setSheet(m_sheet);
                const int col = e.attribute("column").toInt();
                const int cols = qMax(pasteArea.width(), number);
                const Region region(col + xOffset, 1, cols, KS_rowMax, m_sheet);
                command->add(region);
                command->setMode(DeleteCommand::OnlyCells);
            }

            // Set the column style.
            ColumnFormat columnFormat;
            columnFormat.setSheet(sheet);
            KoXmlElement c = e.firstChild().toElement();
            for (; !c.isNull(); c = c.nextSibling().toElement()) {
                if (c.tagName() != "column") {
                    continue;
                }
                if (columnFormat.load(c, xOffset, m_pasteMode)) {
                    const int col = columnFormat.column();
                    const int cols = qMax(pasteArea.width(), number);
                    for (int coff = 0; col - xOffset + coff <= cols; coff += number) {
                        ColumnStyleCommand *const command = new ColumnStyleCommand(this);
                        command->setSheet(m_sheet);
                        command->add(Region(col + coff, 1, 1, 1, m_sheet));
                        command->setTemplate(columnFormat);
                    }
                }
            }
        }

        // entire rows given
        if (e.tagName() == "rows" && !sheet->isProtected()) {
            const int number = e.attribute("count").toInt();
            if (m_insertMode == NoInsertion) {
                // Clear the existing content; not the row style.
                DeleteCommand *const command = new DeleteCommand(this);
                command->setSheet(m_sheet);
                const int row = e.attribute("row").toInt();
                const int rows = qMax(pasteArea.height(), number);
                const Region region(1, row + yOffset, KS_colMax, rows, m_sheet);
                command->add(region);
                command->setMode(DeleteCommand::OnlyCells);
            }

            // Set the row style.
            RowFormat rowFormat;
            rowFormat.setSheet(sheet);
            KoXmlElement c = e.firstChild().toElement();
            for (; !c.isNull(); c = c.nextSibling().toElement()) {
                if (c.tagName() != "row") {
                    continue;
                }
                if (rowFormat.load(c, yOffset, m_pasteMode)) {
                    const int row = rowFormat.row();
                    const int rows = qMax(pasteArea.height(), number);
                    for (int roff = 0; row - yOffset + roff <= rows; roff += number) {
                        RowStyleCommand *const command = new RowStyleCommand(this);
                        command->setSheet(m_sheet);
                        command->add(Region(1, rowFormat.row(), 1, 1, m_sheet));
                        command->setTemplate(rowFormat);
                    }
                }
            }
        }

        if (e.tagName() == "cell") {
            // Create a new PasteCellCommand, if necessary.
            if (!pasteCellCommand) {
                pasteCellCommand = new PasteCellCommand(this);
                pasteCellCommand->setSheet(m_sheet);
                pasteCellCommand->m_pasteMode = m_pasteMode;
                pasteCellCommand->m_pasteOperation = m_operation;
                pasteCellCommand->m_pasteFC = m_pasteFC;
            }

            // Source cell location:
            const int row = e.attribute("row").toInt();
            const int col = e.attribute("column").toInt();

            // tile the selection with the clipboard contents
            for (int roff = 0; row + roff <= pasteHeight; roff += sourceHeight) {
                for (int coff = 0; col + coff <= pasteWidth; coff += sourceWidth) {
                    kDebug(36005) << "cell at" << (col + xOffset + coff) << ',' << (row + yOffset + roff)
                    << " with roff,coff=" << roff << ',' << coff
                    << ", xOffset:" << xOffset << ", yOffset:" << yOffset << endl;

                    // Destination cell:
                    const Cell cell(sheet, col + xOffset + coff, row + yOffset + roff);
                    // Do nothing, if the sheet and the cell are protected.
                    if (sheet->isProtected() && !cell.style().notProtected()) {
                        continue;
                    }
                    // Add the destination cell and the XML element itself.
                    pasteCellCommand->addXmlElement(cell, e);
                }
            }
        }
    }
    return true;
}

bool PasteCommand::processTextPlain(Element *element)
{
    const QString text = m_mimeData->text();
    if (text.isEmpty()) {
        return false;
    }

    // Split the text into lines.
    const QStringList list = text.split('\n');

    const int mx = 1; // always one column
    const int my = list.count();

    // Put the lines into an array value.
    Value value(Value::Array);
    for (int i = 0; i < list.count(); ++i) {
        value.setElement(0, i, Value(list[i]));
    }

    // FIXME Determine and tile the destination area.
    Region range(mx, my, 1, list.size());

    // create a command, configure it and execute it
    DataManipulator *command = new DataManipulator(this);
    command->setSheet(m_sheet);
    command->setParsing(false);
    command->setValue(value);
    command->add(element->rect(), m_sheet);
    return true;
}
