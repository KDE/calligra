/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "Subtotals.h"
#include "Actions.h"
#include "ui/actions/dialogs/SubtotalDialog.h"

#include <KLocalizedString>

#include "core/Cell.h"
#include "core/Sheet.h"
#include "ui/Selection.h"

using namespace Calligra::Sheets;

Subtotals::Subtotals(Actions *actions)
    : CellAction(actions, "subtotals", i18n("&Subtotals..."), QIcon(), i18n("Create different kind of subtotals to a list or database"))
    , m_dlg(nullptr)
{
    m_closeEditor = true;
}

Subtotals::~Subtotals()
{
    if (m_dlg)
        delete m_dlg;
}

bool Subtotals::enabledForSelection(Selection *selection, const Cell &)
{
    if (!selection->isContiguous())
        return false;
    // don't allow full rows/columns
    if (selection->isRowSelected())
        return false;
    if (selection->isColumnSelected())
        return false;
    QRect range = selection->lastRange();
    if ((range.width() < 2) || (range.height() < 2))
        return false;
    return true;
}

void Subtotals::execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget)
{
    m_selection = selection;

    m_dlg = new SubtotalDialog(canvasWidget, selection);
    if (m_dlg->exec()) {
        SubtotalsManipulator *cmd = new SubtotalsManipulator;
        cmd->setRange(*selection);
        cmd->setSheet(sheet);

        int primary = m_dlg->primaryColumn();
        if (m_dlg->removeExisting()) {
            cmd->setPrimaryColumn(primary); // not really needed, but we should have something
            QList<int> cols({primary});
            cmd->setColumns(cols);
            cmd->setSummaryBelow(false);
            cmd->setSummaryOnly(true);
        } else {
            cmd->setPrimaryColumn(primary);
            cmd->setColumns(m_dlg->columns());
            cmd->setSummaryBelow(m_dlg->summaryBelow());
            cmd->setSummaryOnly(m_dlg->summaryOnly());
        }
        cmd->setFuncCode(m_dlg->funcCode());
        cmd->setIgnoreEmptyColumns(m_dlg->ignoreEmpty());

        cmd->add(*selection);
        cmd->execute(selection->canvas());
        selection->initialize(cmd->lastRange(), dynamic_cast<Sheet *>(cmd->lastSheet()));
    }
    delete m_dlg;
    m_dlg = nullptr;
}

// Info about one row
struct SubtotalsRowInfo {
    bool isSubtotalRow;
    int fromRow, toRow; // if isSubtotalRow is true

    // if isSubtotalRow is false: the mappings are per columns
    QMap<int, Style> cellStyle;
    QMap<int, QString> userInput;
    QMap<int, bool> isFormula;
};

class SubtotalsManipulator::Private
{
public:
    Private();
    Region m_range;
    bool m_ignoreEmptyColumns;
    bool m_summaryBelow, m_summaryOnly;
    QList<int> m_columns;
    int m_primaryColumn;
    int m_funcCode;
    QMap<int, SubtotalsRowInfo> rows;
};

SubtotalsManipulator::Private::Private()
    : m_ignoreEmptyColumns(false)
    , m_summaryBelow(true)
    , m_summaryOnly(false)
    , m_primaryColumn(0)
    , m_funcCode(0)
{
}

SubtotalsManipulator::SubtotalsManipulator()
{
    d = new Private;
    setText(kundo2_i18n("Subtotals"));
}

SubtotalsManipulator::~SubtotalsManipulator()
{
    delete d;
}

void SubtotalsManipulator::setRange(const Region &range)
{
    d->m_range = range;
}

void SubtotalsManipulator::setColumns(QList<int> columns)
{
    d->m_columns = columns;
}

void SubtotalsManipulator::setPrimaryColumn(int col)
{
    d->m_primaryColumn = col;
}

void SubtotalsManipulator::setFuncCode(int code)
{
    d->m_funcCode = code;
}

void SubtotalsManipulator::setIgnoreEmptyColumns(bool ignore)
{
    d->m_ignoreEmptyColumns = ignore;
}

void SubtotalsManipulator::setSummaryBelow(bool summary)
{
    d->m_summaryBelow = summary;
}

void SubtotalsManipulator::setSummaryOnly(bool summary)
{
    d->m_summaryOnly = summary;
}

bool SubtotalsManipulator::preProcess()
{
    if (!m_firstrun)
        return true; // ran already

    if (!d->m_primaryColumn)
        return false;
    if (!d->m_columns.size())
        return false;

    QRect rect = d->m_range.lastRange();
    if (rect.isEmpty())
        return false;
    SheetBase *bsheet = d->m_range.lastSheet();
    Sheet *sheet = dynamic_cast<Sheet *>(bsheet);

    // Iterate through rows, determine what to put in each
    int from = rect.top();
    int to = rect.bottom();
    QRect used = sheet->usedArea();
    if (used.bottom() < to)
        to = used.bottom();
    if (used.right() < rect.right())
        rect.setRight(used.right());
    int xfrom = rect.left();
    int xto = rect.right();
    if (d->m_primaryColumn < xfrom)
        d->m_primaryColumn = xfrom;
    if (d->m_primaryColumn > xto)
        d->m_primaryColumn = xto;

    QString lastInput;
    bool gotLastInput = false;
    int lastInputSince = -1;
    int rows = 0;
    for (int y = from; y <= to; ++y) {
        // Check if the row contains an existing subtotal, if so, it's to be replaced
        bool containsSubtotal = false;
        for (int x = xfrom; x <= xto; ++x) {
            CellBase cell = CellBase(sheet, x, y);
            if (!cell.isFormula())
                continue;

            QString text = cell.userInput().toLower();
            if (text.indexOf("subtotal") != -1) {
                containsSubtotal = true;
                break;
            }
        }
        if (containsSubtotal)
            continue;

        // Now go by the primary column
        CellBase cell = CellBase(sheet, d->m_primaryColumn, y);
        QString curInput = cell.userInput();

        // If we're ignoring empty columns, do that
        if (d->m_ignoreEmptyColumns && (!curInput.length()))
            continue;

        if (!gotLastInput) {
            lastInput = curInput;
            gotLastInput = true;
            lastInputSince = rows;
        }

        // Check if the value differs from the previous one - if not, store all the values so we can copy the row to its new position
        if (lastInput != curInput) {
            // If the value differs, and the option is set, we'll need to add a new subtotal
            if (!d->m_summaryOnly) {
                SubtotalsRowInfo i;
                i.isSubtotalRow = true;
                i.fromRow = from + lastInputSince;
                i.toRow = from + rows - 1;
                d->rows[from + rows] = i;
                rows++;
            }
            lastInput = curInput;
            lastInputSince = rows;
        }

        SubtotalsRowInfo i;
        i.isSubtotalRow = false;

        for (int col = xfrom; col <= xto; ++col) {
            Cell cell = Cell(sheet, col, y); // need the full cell as we're grabbing styles too
            i.cellStyle[col] = cell.style();
            i.isFormula[col] = cell.isFormula();
            // For formulas, need to store the encoded formula and decode it when moving it - this is so that the references get updated
            if (i.isFormula[col])
                i.userInput[col] = cell.encodeFormula();
            else
                i.userInput[col] = cell.userInput();
        }
        d->rows[from + rows] = i;
        rows++;
    }

    // Last one
    if (!d->m_summaryOnly) {
        SubtotalsRowInfo i;
        i.isSubtotalRow = true;
        i.fromRow = from + lastInputSince;
        i.toRow = from + rows - 1;
        d->rows[from + rows] = i;
        rows++;
    }

    // Finally, add a summary to the end if needed
    if (d->m_summaryBelow) {
        SubtotalsRowInfo i;
        i.isSubtotalRow = true;
        i.fromRow = from;
        i.toRow = from + rows - 1;
        d->rows[from + rows] = i;
        rows++;
    }

    int origRows = to - from + 1;
    if (rows < origRows)
        rows = origRows;
    rect.setHeight(rows);
    add(rect, sheet);

    return true;
}

bool SubtotalsManipulator::postProcess()
{
    return true;
}

Value SubtotalsManipulator::newValue(Element *element, int col, int row, bool *parse, Format::Type *)
{
    if (!d->rows.count(row))
        return Value();
    *parse = true;
    SubtotalsRowInfo i = d->rows[row];
    if (i.isSubtotalRow) {
        if (!d->m_columns.contains(col))
            return Value();
        return Value("=SUBTOTAL(" + QString::number(d->m_funcCode) + ";" + CellBase::name(col, i.fromRow) + ":" + CellBase::name(col, i.toRow) + ")");
    }

    if (!i.cellStyle.count(col))
        return Value();
    QString res = i.userInput[col];
    if (i.isFormula[col]) {
        CellBase cell = CellBase(element->sheet(), col, row);
        res = cell.decodeFormula(res);
    }
    return Value(res);
}

Style SubtotalsManipulator::newFormat(Element *, int col, int row)
{
    if (!d->rows.count(row))
        return Style();
    SubtotalsRowInfo i = d->rows[row];
    if (i.isSubtotalRow) {
        Style s;
        s.setDefault();
        s.setFontBold(true);
        s.setFontItalic(true);
        s.setFontUnderline(true);
        return s;
    }
    if (!i.cellStyle.count(col))
        return Style();
    return i.cellStyle[col];
}
