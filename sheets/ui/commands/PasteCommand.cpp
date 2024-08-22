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
#include "PageBreakCommand.h"
#include "RowColumnManipulators.h"

#include "core/CellStorage.h"
#include "core/ColFormatStorage.h"
#include "core/RowFormatStorage.h"
#include "core/Sheet.h"
#include "core/ksp/SheetsKsp.h"
#include "engine/Damages.h"
#include "engine/DependencyManager.h"
#include "engine/MapBase.h"

using namespace Calligra::Sheets;

PasteCommand::PasteCommand(KUndo2Command *parent)
    : AbstractRegionCommand(parent)
    , m_haveSource(false)
    , m_haveText(false)
    , m_pasteMode(Paste::Normal)
    , m_operation(Paste::OverWrite)
    , m_pasteFC(false)
    , m_sameApp(true)
    , m_isCut(false)
{
}

PasteCommand::~PasteCommand() = default;

bool PasteCommand::setMimeData(const QMimeData *mimeData, bool sameApp)
{
    if (!mimeData)
        return false;
    m_sameApp = sameApp;

    if (mimeData->hasFormat("application/x-calligra-sheets-snippet") && m_sameApp)
        setSourceRegion(parseSnippet(mimeData, &m_isCut));
    else {
        QString data = mimeData->text();
        m_text = data.split('\n');
        const int lines = m_text.count();
        setSourceRegion(Region(QRect(1, 1, 1, lines)));
        m_haveText = true;
    }

    return true;
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

void PasteCommand::setSourceRegion(const Region &region)
{
    m_sourceRegion = region;
    adjustTargetRegion();
}

Region PasteCommand::sourceRegion() const
{
    return m_sourceRegion;
}

void PasteCommand::setCutMode(bool cut)
{
    m_isCut = cut;
}

bool PasteCommand::isApproved() const
{
    if (m_haveSource || m_haveText)
        return AbstractRegionCommand::isApproved();
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

bool PasteCommand::unknownShiftDirection()
{
    if (m_sourceRegion.isEmpty())
        return false;
    if (m_sourceRegion.isColumnOrRowSelected())
        return false;
    return true;
}

Region PasteCommand::parseSnippet(const QMimeData *mimeData, bool *isCut)
{
    Region res;
    *isCut = false;

    bool isSnippet = mimeData->hasFormat("application/x-calligra-sheets-snippet");
    if (!isSnippet)
        return res;

    m_haveSource = true;
    QByteArray byteArray = mimeData->data("application/x-calligra-sheets-snippet");
    QString data = QString::fromUtf8(byteArray);
    QStringList list = data.split('\n');
    if (!list.size())
        return res; // nothing to do?
    // cut or copy?
    if (list.size() && (list[0] == "CUT")) {
        *isCut = true;
        list.removeFirst();
    }

    for (QString e : list) {
        QStringList parts = e.split(' ');
        if (parts.size() < 6)
            continue;
        bool ok;
        int x1 = parts.at(1).toUInt(&ok);
        if (!ok)
            continue;
        int y1 = parts.at(2).toUInt(&ok);
        if (!ok)
            continue;
        int x2 = parts.at(3).toUInt(&ok);
        if (!ok)
            continue;
        int y2 = parts.at(4).toUInt(&ok);
        if (!ok)
            continue;
        QString sheetName;
        for (int i = 5; i < parts.size(); ++i) {
            if (i > 5)
                sheetName += ' ';
            sheetName += parts.at(i);
        }
        QRect rect = QRect(QPoint(x1, y1), QPoint(x2, y2));
        SheetBase *sheet = m_sheet->map()->findSheet(sheetName);
        if (!sheet)
            sheet = m_sheet;

        res.add(rect, sheet);
    }

    return res;
}

bool PasteCommand::performCommands()
{
    const QList<Element *> elements = cells();

    if (!m_haveSource) {
        for (int i = 0; i < elements.count(); ++i)
            processTextPlain(elements[i], m_text);
        return true;
    }

    int areas = 0;
    QVector<QRect> rects;
    QVector<SheetBase *> sheets;
    for (Region::ConstIterator it = m_sourceRegion.constBegin(); it != m_sourceRegion.constEnd(); ++it) {
        rects.append((*it)->rect());
        sheets.append((*it)->sheet());
        areas++;
    }

    // Iterate over all region elements and build the sub-commands.
    int datapos = 0;
    for (int i = 0; i < elements.count(); ++i) {
        processSnippetData(elements[i], rects[datapos], sheets[datapos], m_isCut);
        datapos = (datapos + 1) % areas;
    }
    return true;
}

// static
QRect PasteCommand::adjustPasteArea(QRect sourceRect, QRect pasteArea)
{
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

    pasteArea.setWidth(pasteWidth);
    pasteArea.setHeight(pasteHeight);

    return pasteArea;
}

void PasteCommand::adjustTargetRegion()
{
    int areas = 0;
    QVector<QRect> rects;
    QVector<SheetBase *> sheets;
    for (Region::ConstIterator it = m_sourceRegion.constBegin(); it != m_sourceRegion.constEnd(); ++it) {
        rects.append((*it)->rect());
        sheets.append((*it)->sheet());
        areas++;
    }

    Region newTarget;
    const QList<Element *> elements = cells();
    int datapos = 0;
    for (int i = 0; i < elements.count(); ++i) {
        Element *e = elements[i];

        QRect tg = adjustPasteArea(rects[datapos], e->rect());
        newTarget.add(tg, e->sheet());
        datapos = (datapos + 1) % areas;
    }

    clear();
    for (Region::ConstIterator it = newTarget.constBegin(); it != newTarget.constEnd(); ++it) {
        add((*it)->rect(), (*it)->sheet());
    }
}

bool PasteCommand::processSnippetData(Element *element, QRect sourceRect, SheetBase *sourceSheet, bool isCut)
{
    const QRect pasteArea = element->rect();
    Sheet *sheet = dynamic_cast<Sheet *>(element->sheet());
    Q_ASSERT(sheet == m_sheet);

    const int pasteWidth = pasteArea.width();
    const int pasteHeight = pasteArea.height();

    const int sourceWidth = sourceRect.width();
    const int sourceHeight = sourceRect.height();

    Sheet *fullSourceSheet = dynamic_cast<Sheet *>(sourceSheet);

    // If the areas partially overlap, we need to make sure that we don't overwrite data that hasn't been copied yet
    // This will set reverse mode even if it isn't needed due to no overlap, but that's fine.
    bool reverseRows = false, reverseCols = false;
    if (sheet == fullSourceSheet) {
        if (sourceRect.top() <= pasteArea.top())
            reverseRows = true;
        if (sourceRect.left() <= pasteArea.left())
            reverseCols = true;
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
                ResizeRowManipulator *const resize = new ResizeRowManipulator(nullptr);
                resize->setSheet(sheet);
                resize->add(region);
                resize->setSize(rowFormats->rowHeight(srcRow));
                resize->redo();
                if (m_firstrun)
                    m_nestedCommands.append(resize);
                else
                    delete resize;

                HideShowManipulator *const hideShow = new HideShowManipulator(nullptr);
                hideShow->setManipulateColumns(false);
                hideShow->setSheet(sheet);
                hideShow->add(region);
                hideShow->setHide(!rowFormats->isHidden(srcRow));
                hideShow->redo();
                if (m_firstrun)
                    m_nestedCommands.append(hideShow);
                else
                    delete hideShow;

                PageBreakCommand *const pageBreak = new PageBreakCommand(nullptr);
                pageBreak->setMode(PageBreakCommand::BreakBeforeRow);
                pageBreak->setSheet(sheet);
                pageBreak->add(region);
                pageBreak->setBreak(rowFormats->hasPageBreak(srcRow));
                pageBreak->redo();
                if (m_firstrun)
                    m_nestedCommands.append(pageBreak);
                else
                    delete pageBreak;
            }
        }
        if (sourceHeight == KS_rowMax) {
            ColFormatStorage *columnFormats = fullSourceSheet->columnFormats();
            for (int xx = 0; xx < pasteWidth; xx++) {
                int x = reverseCols ? (pasteWidth - 1 - xx) : xx;
                int srcCol = sourceRect.left() + x;
                int tgCol = pasteArea.left() + x;

                Region region = Region(tgCol, 1, 1, 1, sheet);
                ResizeColumnManipulator *const resize = new ResizeColumnManipulator(nullptr);
                resize->setSheet(sheet);
                resize->add(region);
                resize->setSize(columnFormats->colWidth(srcCol));
                m_nestedCommands.append(resize);
                resize->redo();
                if (m_firstrun)
                    m_nestedCommands.append(resize);
                else
                    delete resize;

                HideShowManipulator *const hideShow = new HideShowManipulator(nullptr);
                hideShow->setManipulateColumns(true);
                hideShow->setSheet(sheet);
                hideShow->add(region);
                hideShow->setHide(!columnFormats->isHidden(srcCol));
                hideShow->redo();
                if (m_firstrun)
                    m_nestedCommands.append(hideShow);
                else
                    delete hideShow;

                PageBreakCommand *const pageBreak = new PageBreakCommand(nullptr);
                pageBreak->setMode(PageBreakCommand::BreakBeforeColumn);
                pageBreak->setSheet(sheet);
                pageBreak->add(region);
                pageBreak->setBreak(columnFormats->hasPageBreak(srcCol));
                pageBreak->redo();
                if (m_firstrun)
                    m_nestedCommands.append(pageBreak);
                else
                    delete pageBreak;
            }
        }
    }

    // Now copy the actual cells.
    CellStorage *cs = fullSourceSheet->fullCellStorage();
    int sourceMaxX = cs->columns();
    int sourceMaxY = cs->rows();
    for (int yy = 0; yy < pasteHeight; yy++) {
        int y = reverseRows ? (pasteHeight - 1 - yy) : yy;
        for (int xx = 0; xx < pasteWidth; xx++) {
            int x = reverseCols ? (pasteWidth - 1 - xx) : xx;
            int srcX = sourceRect.left() + x;
            int srcY = sourceRect.top() + y;
            int tgX = pasteArea.left() + x;
            int tgY = pasteArea.top() + y;

            // do not copy empty cells
            if (srcX > sourceMaxX)
                continue;
            if (srcY > sourceMaxY)
                continue;
            if (tgX > KS_colMax)
                continue;
            if (tgY > KS_rowMax)
                continue;

            // The coordinates are good.
            Cell srcCell = Cell(fullSourceSheet, srcX, srcY);
            Cell tgCell = Cell(sheet, tgX, tgY);
            tgCell.copyAll(srcCell, m_pasteMode, m_operation);

            // Clear the original? But only if it's not a part of the target.
            if (isCut && (!pasteArea.contains(srcX, srcY)))
                cs->take(srcX, srcY);
        }
    }
    if (isCut)
        m_sheet->map()->addDamage(new CellDamage(sourceSheet, Region(sourceRect), CellDamage::Appearance));

    return true;
}

bool PasteCommand::processTextPlain(Element *element, const QStringList &list)
{
    //    const int mx = 1; // always one column
    const int my = list.count();
    if (!my)
        return false;

    // Put the lines into an array value.
    Value value(Value::Array);
    for (int i = 0; i < my; ++i) {
        value.setElement(0, i, Value(list[i]));
    }

    // FIXME Determine and tile the destination area.
    //     Region range(mx, my, 1, list.size());

    // create a command, configure it and execute it
    DataManipulator *command = new DataManipulator(nullptr);
    command->setSheet(m_sheet);
    command->setParsing(false);
    command->setValue(value);
    command->add(element->rect(), m_sheet);
    command->redo();
    delete command;

    return true;
}

bool PasteCommand::postProcess()
{
    if (!m_firstrun)
        return true;
    // Nested commands should appear on the undo stack after the recorded ones, so we add them here.
    for (AbstractRegionCommand *command : m_nestedCommands)
        addCommand(command);
    return true;
}
