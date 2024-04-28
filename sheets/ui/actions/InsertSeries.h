/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_INSERT_SERIES
#define CALLIGRA_SHEETS_ACTION_INSERT_SERIES

#include "DialogCellAction.h"
#include "ui/commands/DataManipulators.h"
#include "ui/sheets_ui_export.h"

namespace Calligra
{
namespace Sheets
{
class SeriesDialog;

class InsertSeries : public DialogCellAction
{
    Q_OBJECT
public:
    InsertSeries(Actions *actions);
    virtual ~InsertSeries();

protected Q_SLOTS:
    void insertSeries(double start, double end, double step, bool isColumn, bool isLinear);

protected:
    virtual ActionDialog *createDialog(QWidget *canvasWidget) override;
};

/**
 * \ingroup Commands
 * \brief Fills a value series into a cell range.
 */
class CALLIGRA_SHEETS_UI_EXPORT SeriesManipulator : public AbstractDataManipulator
{
public:
    enum Series { Column, Row, Linear, Geometric };

    SeriesManipulator();
    ~SeriesManipulator() override;

    /** Setup the series. This sets the necessary parameters, and also the
    correct range. */
    void setupSeries(const QPoint &_marker, double start, double end, double step, Series mode, Series type);

protected:
    Value newValue(Element *element, int col, int row, bool *, Format::Type *) override;

    Series m_type;
    Value m_start, m_step, m_prev;
    int m_last;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_INSERT_SERIES
