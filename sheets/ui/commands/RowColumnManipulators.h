/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2005 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ROW_COLUMN_MANIPULATORS
#define CALLIGRA_SHEETS_ROW_COLUMN_MANIPULATORS

#include <QSizeF>

#include "AbstractRegionCommand.h"

namespace Calligra
{
namespace Sheets
{
class Cell;
class ColFormat;
class RowFormat;
class Style;

/**
 * \class ResizeColumnManipulator
 * \ingroup Commands
 * \brief Resizes a column.
 */
class CALLIGRA_SHEETS_UI_EXPORT ResizeColumnManipulator : public AbstractRegionCommand
{
public:
    explicit ResizeColumnManipulator(KUndo2Command *parent = nullptr);
    ~ResizeColumnManipulator() override;

    void setSize(double size)
    {
        m_newSize = size;
    }

protected:
    bool performNonCommandActions() override;
    bool undoNonCommandActions() override;

private:
    double m_newSize;
    QHash<int, double> m_oldSizes;
};

/**
 * \class ResizeRowManipulator
 * \ingroup Commands
 * \brief Resizes a row.
 */
class CALLIGRA_SHEETS_UI_EXPORT ResizeRowManipulator : public AbstractRegionCommand
{
public:
    explicit ResizeRowManipulator(KUndo2Command *parent = nullptr);
    ~ResizeRowManipulator() override;

    void setSize(double size)
    {
        m_newSize = size;
    }

protected:
    bool performNonCommandActions() override;
    bool undoNonCommandActions() override;

private:
    double m_newSize;
    QHash<int, double> m_oldSizes;
};

/**
 * \class AdjustColumnRowManipulator
 * \ingroup Commands
 * \brief Optimizes the height and the width of rows and columns, respectively.
 */
class CALLIGRA_SHEETS_UI_EXPORT AdjustColumnRowManipulator : public AbstractRegionCommand
{
public:
    explicit AdjustColumnRowManipulator(KUndo2Command *parent = nullptr);
    ~AdjustColumnRowManipulator() override;

    void setAdjustColumn(bool state)
    {
        m_adjustColumn = state;
    }
    void setAdjustRow(bool state)
    {
        m_adjustRow = state;
    }

protected:
    bool performNonCommandActions() override;
    bool undoNonCommandActions() override;

    void setHeights(int from, int to, QMap<int, double> &heights);
    void setWidths(int from, int to, QMap<int, double> &widths);

    KUndo2MagicString name() const;

    QSizeF textSize(const QString &text, const Style &style) const;
    double idealColumnWidth(int col);
    double idealRowHeight(int row);
    double adjustColumnHelper(const Cell &cell);
    double adjustRowHelper(const Cell &cell);

private:
    bool m_adjustColumn : 1;
    bool m_adjustRow : 1;
    QMap<int, double> m_newWidths;
    QMap<int, double> m_oldWidths;
    QMap<int, double> m_newHeights;
    QMap<int, double> m_oldHeights;
};

/**
 * \class HideShowManipulator
 * \ingroup Commands
 * \brief Hides/Shows columns and/or rows.
 */
class CALLIGRA_SHEETS_UI_EXPORT HideShowManipulator : public AbstractRegionCommand
{
public:
    explicit HideShowManipulator(KUndo2Command *parent = nullptr);
    ~HideShowManipulator() override;

    void setHide(bool hide);

    bool preProcess() override;

    void setManipulateColumns(bool state)
    {
        m_manipulateColumns = state;
    }
    void setManipulateRows(bool state)
    {
        m_manipulateRows = state;
    }

protected:
    void performActions(bool hide);
    void undoActions();

    bool performNonCommandActions() override;
    bool undoNonCommandActions() override;

    KUndo2MagicString name(bool hide) const;

private:
    bool m_manipulateColumns : 1;
    bool m_manipulateRows : 1;
    bool m_hide : 1;
    QMap<int, bool> oldHidden;
};

/**
 * \class InsertDeleteColumnManipulator
 * \ingroup Commands
 * \brief Inserts/Removes columns.
 */
class CALLIGRA_SHEETS_UI_EXPORT InsertDeleteColumnManipulator : public AbstractRegionCommand
{
public:
    explicit InsertDeleteColumnManipulator(KUndo2Command *parent = nullptr);
    ~InsertDeleteColumnManipulator() override;

    void setTemplate(const ColFormat &columnFormat);
    void setDelete(bool deletion);

protected:
    bool process(Element *) override;
    bool preProcess() override;
    bool undoNonCommandActions() override;

private:
    enum Mode { Insert, Delete };
    Mode m_mode;
    ColFormat *m_template;
};

/**
 * \class InsertDeleteRowManipulator
 * \ingroup Commands
 * \brief Inserts/Removes rows.
 */
class CALLIGRA_SHEETS_UI_EXPORT InsertDeleteRowManipulator : public AbstractRegionCommand
{
public:
    explicit InsertDeleteRowManipulator(KUndo2Command *parent = nullptr);
    ~InsertDeleteRowManipulator() override;

    void setTemplate(const RowFormat &rowFormat);
    void setDelete(bool deletion);

protected:
    bool process(Element *) override;
    bool preProcess() override;
    bool undoNonCommandActions() override;

private:
    enum Mode { Insert, Delete };
    Mode m_mode;
    RowFormat *m_template;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ROW_COLUMN_MANIPULATORS
