/* This file is part of the KDE project
   Copyright (C) 2005 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#ifndef KSPREAD_ROW_COLUMN_MANIPULATORS
#define KSPREAD_ROW_COLUMN_MANIPULATORS

#include <QSizeF>

#include <Style.h>

#include "AbstractRegionCommand.h"

namespace KSpread
{
class ColumnFormat;
class RowFormat;

/**
   * \class ResizeColumnManipulator
   * \brief Resize column operation.
 */
class ResizeColumnManipulator : public AbstractRegionCommand
{
public:
    ResizeColumnManipulator(QUndoCommand* parent = 0);
    ~ResizeColumnManipulator();

    void setSize(double size) {
        m_newSize = size;
    }

protected:
    virtual bool process(Element*);

private:
    double m_newSize;
    QHash<int, double> m_oldSizes;
};



/**
   * \class ResizeRowManipulator
   * \brief Resize row operation.
 */
class ResizeRowManipulator : public AbstractRegionCommand
{
public:
    ResizeRowManipulator(QUndoCommand* parent = 0);
    ~ResizeRowManipulator();

    void setSize(double size) {
        m_newSize = size;
    }

protected:
    virtual bool process(Element*);

private:
    double m_newSize;
    QHash<int, double> m_oldSizes;
};


/**
 * \class AdjustColumnRowManipulator
 * \brief Optimizes the height and the width of rows and columns, respectively.
 */
class AdjustColumnRowManipulator : public AbstractRegionCommand
{
public:
    AdjustColumnRowManipulator(QUndoCommand* parent = 0);
    virtual ~AdjustColumnRowManipulator();

    virtual bool process(Element*);
    virtual bool preProcessing();

    void setAdjustColumn(bool state) {
        m_adjustColumn = state;
    }
    void setAdjustRow(bool state) {
        m_adjustRow = state;
    }

protected:
    QString name() const;

    QSizeF textSize(const QString& text, const Style& style) const;
    double adjustColumnHelper(const Cell& cell);
    double adjustRowHelper(const Cell& cell);

private:
bool m_adjustColumn : 1;
bool m_adjustRow    : 1;
    QMap<int, double> m_newWidths;
    QMap<int, double> m_oldWidths;
    QMap<int, double> m_newHeights;
    QMap<int, double> m_oldHeights;
};



/**
 * \class HideShowManipulator
 * \brief Hides and shows columns and rows.
 */
class HideShowManipulator : public AbstractRegionCommand
{
public:
    HideShowManipulator();
    virtual ~HideShowManipulator();

    virtual bool process(Element*);
    virtual bool preProcessing();
    virtual bool postProcessing();

    void setManipulateColumns(bool state) {
        m_manipulateColumns = state;
    }
    void setManipulateRows(bool state) {
        m_manipulateRows = state;
    }

protected:
    QString name() const;

private:
bool m_manipulateColumns : 1;
bool m_manipulateRows    : 1;
};



/**
 * \class InsertDeleteColumnManipulator
 * \brief Inserts and deletes rows.
 */
class InsertDeleteColumnManipulator : public AbstractRegionCommand
{
public:
    InsertDeleteColumnManipulator(QUndoCommand *parent = 0);
    virtual ~InsertDeleteColumnManipulator();

    void setTemplate(const ColumnFormat &columnFormat);
    virtual void setReverse(bool reverse);

protected:
    virtual bool process(Element*);
    virtual bool preProcessing();
    virtual bool mainProcessing();
    virtual bool postProcessing();

private:
    enum Mode { Insert, Delete };
    Mode m_mode;
    ColumnFormat *m_template;
};



/**
 * \class InsertDeleteRowManipulator
 * \brief Inserts and deletes rows.
 */
class InsertDeleteRowManipulator : public AbstractRegionCommand
{
public:
    InsertDeleteRowManipulator(QUndoCommand *parent = 0);
    virtual ~InsertDeleteRowManipulator();

    void setTemplate(const RowFormat &rowFormat);
    virtual void setReverse(bool reverse);

protected:
    virtual bool process(Element*);
    virtual bool preProcessing();
    virtual bool mainProcessing();
    virtual bool postProcessing();

private:
    enum Mode { Insert, Delete };
    Mode m_mode;
    RowFormat *m_template;
};

}  // namespace KSpread

#endif // KSPREAD_ROW_COLUMN_MANIPULATORS
