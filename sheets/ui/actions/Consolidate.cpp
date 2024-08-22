/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Consolidate.h"
#include "Actions.h"

#include "core/Sheet.h"
#include "engine/MapBase.h"
#include "engine/Value.h"
#include "engine/ValueConverter.h"
#include "ui/actions/dialogs/ConsolidateDialog.h"

#include <KLocalizedString>

using namespace Calligra::Sheets;

Consolidate::Consolidate(Actions *actions)
    : CellAction(actions, "consolidate", i18n("&Consolidate..."), QIcon(), i18n("Create a region of summary data from a group of similar regions"))
    , m_dlg(nullptr)
{
    m_closeEditor = true;
}

Consolidate::~Consolidate()
{
    if (m_dlg)
        delete m_dlg;
}

void Consolidate::dialogClosed()
{
    m_dlg->deleteLater();
    m_dlg = nullptr;
}

void Consolidate::execute(Selection *selection, Sheet *, QWidget *canvasWidget)
{
    m_selection = selection;
    m_dlg = new ConsolidateDialog(canvasWidget, selection);
    connect(m_dlg, &ConsolidateDialog::finished, this, &Consolidate::dialogClosed);
    connect(m_dlg, &ConsolidateDialog::applyChange, this, &Consolidate::applyChange);
    m_dlg->show();
}

void Consolidate::applyChange()
{
    ConsolidateManipulator *manipulator = new ConsolidateManipulator;

    manipulator->setRanges(m_dlg->sourceRegions());
    manipulator->setRowHeaders(m_dlg->rowHeaders());
    manipulator->setColHeaders(m_dlg->colHeaders());
    manipulator->setFunction(m_dlg->function());

    Region tg = m_dlg->targetRegion();
    manipulator->setSheet(m_selection->activeSheet());
    manipulator->add(tg);
    manipulator->execute(m_selection->canvas());
}

ConsolidateManipulator::ConsolidateManipulator()
    : m_rowHeaders(false)
    , m_colHeaders(false)
{
    setText(kundo2_i18n("Consolidate"));
}

ConsolidateManipulator::~ConsolidateManipulator() = default;

bool ConsolidateManipulator::preProcess()
{
    if (!m_firstrun)
        return true;
    // Determine all the headers, fill in m_rowHeaderList and m_colHeaderList
    if (m_colHeaders) {
        for (Region &r : m_ranges) {
            QRect rect = r.firstRange();
            SheetBase *sheet = r.firstSheet();
            for (int col = rect.left(); col <= rect.right(); ++col) {
                if ((col == rect.left()) && (m_rowHeaders))
                    continue; // do not include the header
                CellBase c = CellBase(sheet, col, rect.top());
                QString header = c.value().asString();
                if (!m_colHeaderList.contains(header))
                    m_colHeaderList.append(header);
            }
        }
    }
    if (m_rowHeaders) {
        for (Region &r : m_ranges) {
            QRect rect = r.firstRange();
            SheetBase *sheet = r.firstSheet();
            for (int row = rect.top(); row <= rect.bottom(); ++row) {
                if ((row == rect.top()) && (m_colHeaders))
                    continue; // do not include the header
                CellBase c = CellBase(sheet, rect.left(), row);
                QString header = c.value().asString();
                if (!m_rowHeaderList.contains(header))
                    m_rowHeaderList.append(header);
            }
        }
    }

    // set the target area
    QRect range = firstRange();
    SheetBase *sheet = firstSheet();
    clear();
    int cols = 0, rows = 0;
    if (m_colHeaders) {
        cols = m_colHeaderList.length();
        if (m_rowHeaders)
            cols++;
    } else {
        for (Region &r : m_ranges) {
            int c = r.firstRange().width();
            if (c > cols)
                cols = c;
        }
    }
    if (m_rowHeaders) {
        rows = m_rowHeaderList.length();
        if (m_colHeaders)
            rows++;
    } else {
        for (Region &r : m_ranges) {
            int rc = r.firstRange().height();
            if (rc > rows)
                rows = rc;
        }
    }

    range.setWidth(cols);
    range.setHeight(rows);
    add(range, sheet);

    return true;
}

bool ConsolidateManipulator::postProcess()
{
    return true;
}

// indexes go from 0
bool ConsolidateManipulator::matchingRow(const QRect &source, SheetBase *sourceSheet, int sourceRow, int targetRowIndex, const QString &targetHeader)
{
    if (m_colHeaders && (sourceRow == source.top()))
        return false; // do not match headers
    if (m_rowHeaders) {
        // The first column contains row headers
        CellBase c(sourceSheet, source.left(), sourceRow);
        Value v = c.value();
        if (v.asString() == targetHeader)
            return true;
        return false;
    }
    // No row headers, we go by indexes
    int sourceRowIndex = sourceRow - source.top();
    if (m_colHeaders)
        sourceRowIndex--;
    return (sourceRowIndex == targetRowIndex);
}

bool ConsolidateManipulator::matchingCol(const QRect &source, SheetBase *sourceSheet, int sourceCol, int targetColIndex, const QString &targetHeader)
{
    if (m_rowHeaders && (sourceCol == source.left()))
        return false; // do not match headers
    if (m_colHeaders) {
        // The first row contains column headers
        CellBase c(sourceSheet, sourceCol, source.top());
        Value v = c.value();
        if (v.asString() == targetHeader)
            return true;
        return false;
    }
    // No column headers, we go by indexes
    int sourceColIndex = sourceCol - source.left();
    if (m_rowHeaders)
        sourceColIndex--;
    return (sourceColIndex == targetColIndex);
}

QString ConsolidateManipulator::rowHeader(int which)
{
    if (!m_rowHeaders)
        return QString();
    if (m_colHeaders)
        which--;
    if (which < 0)
        return QString();
    if (which >= m_rowHeaderList.length())
        return QString(); // out of range
    return m_rowHeaderList[which];
}

QString ConsolidateManipulator::colHeader(int which)
{
    if (!m_colHeaders)
        return QString();
    if (m_rowHeaders)
        which--;
    if (which < 0)
        return QString();
    if (which >= m_colHeaderList.length())
        return QString(); // out of range
    return m_colHeaderList[which];
}

Value ConsolidateManipulator::newValue(Element *element, int col, int row, bool *parse, Format::Type *)
{
    QRect range = element->rect();
    int colidx = col - range.left();
    int rowidx = row - range.top();

    if ((colidx == 0) && m_rowHeaders)
        return Value(rowHeader(rowidx));
    if ((rowidx == 0) && m_colHeaders)
        return Value(colHeader(colidx));

    int count = 0;
    QString formula = '=' + m_function + '(';
    for (Region &r : m_ranges) {
        QRect rect = r.firstRange();
        SheetBase *sheet = r.firstSheet();
        // This is going to be a bit slow, but we don't do it very often, so it should be fine.
        for (int row = rect.top(); row <= rect.bottom(); ++row) {
            QString targetHeader = rowHeader(rowidx);
            if (!matchingRow(rect, sheet, row, rowidx, targetHeader))
                continue;
            for (int col = rect.left(); col <= rect.right(); ++col) {
                targetHeader = colHeader(colidx);
                if (!matchingCol(rect, sheet, col, colidx, targetHeader))
                    continue;

                if (count)
                    formula += ";";
                bool fullName = (sheet != element->sheet());
                CellBase c = CellBase(sheet, col, row);
                formula += fullName ? c.fullName() : c.name();
                count++;
            }
        }
    }
    if (!count)
        return Value();
    formula += ")";
    *parse = true;
    return Value(formula);
}

void ConsolidateManipulator::setRanges(const QList<Region> &ranges)
{
    m_ranges = ranges;
}
