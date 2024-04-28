/* This file is part of the KDE project
   SPDX-FileCopyrightText: 1998-2022 The Calligra Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 2022 Tomas Mecir <mecirt@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ACTION_CONSOLIDATE
#define CALLIGRA_SHEETS_ACTION_CONSOLIDATE

#include "CellAction.h"

#include "ui/commands/DataManipulators.h"

namespace Calligra
{
namespace Sheets
{

class ConsolidateDialog;
class Selection;

class Consolidate : public CellAction
{
    Q_OBJECT
public:
    Consolidate(Actions *actions);
    virtual ~Consolidate();

protected Q_SLOTS:
    void applyChange();
    void dialogClosed();

protected:
    virtual void execute(Selection *selection, Sheet *sheet, QWidget *canvasWidget) override;

    Selection *m_selection;
    ConsolidateDialog *m_dlg;
};

class ConsolidateManipulator : public AbstractDataManipulator
{
public:
    ConsolidateManipulator();
    virtual ~ConsolidateManipulator() override;

    void setRanges(const QList<Region> &ranges);
    void setRowHeaders(bool headers)
    {
        m_rowHeaders = headers;
    }
    void setColHeaders(bool headers)
    {
        m_colHeaders = headers;
    }
    void setFunction(const QString &fun)
    {
        m_function = fun;
    }

protected:
    bool preProcess() override;
    bool postProcess() override;
    Value newValue(Element *element, int col, int row, bool *parse, Format::Type *fmtType) override;

    bool matchingRow(const QRect &source, SheetBase *sourceSheet, int sourceRow, int targetRowIndex, const QString &targetHeader);
    bool matchingCol(const QRect &source, SheetBase *sourceSheet, int sourceCol, int targetColIndex, const QString &targetHeader);

    QString colHeader(int which);
    QString rowHeader(int which);

    QList<Region> m_ranges;
    bool m_rowHeaders, m_colHeaders;
    QList<QString> m_rowHeaderList, m_colHeaderList;
    QString m_function;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ACTION_CONSOLIDATE
