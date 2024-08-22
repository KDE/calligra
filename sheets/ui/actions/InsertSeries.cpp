/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "InsertSeries.h"
#include "./dialogs/SeriesDialog.h"
#include "Actions.h"

#include "core/CellStorage.h"
#include "core/Sheet.h"
#include "engine/ValueCalc.h"
#include "ui/Selection.h"

#include <float.h> // for DBL_EPSILON

#include <QApplication>
#include <QKeyEvent>

#include <KLocalizedString>

using namespace Calligra::Sheets;

InsertSeries::InsertSeries(Actions *actions)
    : DialogCellAction(actions, "insertSeries", i18n("&Series..."), koIcon("series"), i18n("Insert a series"))
{
    m_closeEditor = true;
}

InsertSeries::~InsertSeries() = default;

ActionDialog *InsertSeries::createDialog(QWidget *canvasWidget)
{
    SeriesDialog *dlg = new SeriesDialog(canvasWidget);
    connect(dlg, &SeriesDialog::insertSeries, this, &InsertSeries::insertSeries);
    return dlg;
}

void InsertSeries::insertSeries(double start, double end, double step, bool isColumn, bool isLinear)
{
    SeriesManipulator *manipulator = new SeriesManipulator;
    manipulator->setSheet(m_selection->activeSheet());
    manipulator->setupSeries(m_selection->cursor(),
                             start,
                             end,
                             step,
                             isColumn ? SeriesManipulator::Column : SeriesManipulator::Row,
                             isLinear ? SeriesManipulator::Linear : SeriesManipulator::Geometric);

    // setupSeries also called add(), so we can call execute directly
    manipulator->execute(m_selection->canvas());
}

// The actual command.

SeriesManipulator::SeriesManipulator()
{
    setText(kundo2_i18n("Insert Series"));

    m_type = Linear;
    m_last = -2;
}

SeriesManipulator::~SeriesManipulator() = default;

void SeriesManipulator::setupSeries(const QPoint &_marker, double start, double end, double step, Series mode, Series type)
{
    m_type = type;
    m_start = Value(start);
    m_step = Value(step);
    // compute cell count
    int numberOfCells = 1;
    if (type == Linear)
        numberOfCells = (int)((end - start) / step + 1);
    if (type == Geometric)
        /* basically, A(n) = start * step ^ n
         * so when is end >= start * step ^ n ??
         * when n = ln(end/start) / ln(step)
         */
        // DBL_EPSILON is added to prevent rounding errors
        numberOfCells = (int)(::log(end / start) / ::log(step) + DBL_EPSILON) + 1;

    // with this, generate range information
    Region range(_marker.x(), _marker.y(), (mode == Column) ? 1 : numberOfCells, (mode == Row) ? 1 : numberOfCells);

    // and add the range to the manipulator
    add(range);
}

Value SeriesManipulator::newValue(Element *element, int col, int row, bool *parse, Format::Type *)
{
    *parse = false;
    ValueCalc *calc = m_sheet->map()->calc();

    // either colidx or rowidx is always zero
    QRect range = element->rect();
    int colidx = col - range.left();
    int rowidx = row - range.top();
    int which = (colidx > 0) ? colidx : rowidx;
    Value val;
    if (which == m_last + 1) {
        // if we are requesting next item in the series, which should almost always
        // be the case, we can use the pre-computed value to speed up the process
        if (m_type == Linear)
            val = calc->add(m_prev, m_step);
        if (m_type == Geometric)
            val = calc->mul(m_prev, m_step);
    } else {
        // otherwise compute from scratch
        val = m_start;
        for (int i = 0; i < which; ++i) {
            if (m_type == Linear)
                val = calc->add(val, m_step);
            if (m_type == Geometric)
                val = calc->mul(val, m_step);
        }
    }
    // store last value
    m_prev = val;
    m_last = which;

    // return the computed value
    return val;
}
