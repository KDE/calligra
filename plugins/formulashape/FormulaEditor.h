/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2001 Andrea Rizzi <rizzi@kde.org>
                      Ulrich Kuettler <ulrich.kuettler@mailbox.tu-dresden.de>
                 2006 Martin Pfeiffer <hubipete@gmx.net>
                 2009 Jeremias Epperlein <jeeree@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef FORMULAEDITOR_H
#define FORMULAEDITOR_H

#include "FormulaCursor.h"
#include "FormulaData.h"
#include "koformula_export.h"
#include <QPair>
#include <QString>

class BasicElement;
class QString;
class QPainter;

/**
 * @short The Class responsible for manipulating a formula
 *
 * Every manipulation of a formula is done by the tool through this class. It holds
 * the current selection and provides methods to change the content of the formula
 * according to this selection. The m_data is used to notify the shape and tool of
 * changes in the formula.
 *
 */

class KOFORMULA_EXPORT FormulaEditor
{
public:
    /// The constructor - set the FormulaCursor at the position specified in @p cursor
    explicit FormulaEditor(FormulaCursor cursor, FormulaData *data);

    /// The constructor - set the FormulaCursor to the left of the main element of @p data
    explicit FormulaEditor(FormulaData *data);

    /**
     * Draw the cursor to the given QPainter
     * only for convenience
     * @param painter The QPainter the cursor draws itself to
     */
    void paint(QPainter &painter) const;

    /**
     * Insert text content at the current cursor position
     * @param text The text to insert
     */
    FormulaCommand *insertText(const QString &text);

    /**
     * Insert an element at the current cursor position
     * @param element The element to be inserted
     */
    FormulaCommand *insertElement(BasicElement *element);

    /// Insert the elements encoded in MathML in data
    FormulaCommand *insertMathML(const QString &data);

    /// Manipulate the rows/columns of a table
    FormulaCommand *changeTable(bool insert, bool rows);

    /**
     * Remove an element from the formula
     * @param elementBeforePosition Indicates removal of element before or after cursor
     */
    FormulaCommand *remove(bool elementBeforePosition);

    /// @return The FormulaData which is navigated by this cursor
    FormulaData *formulaData() const;

    /// set the FormulaData which is navigated by this cursor
    void setData(FormulaData *data);

    /// @return the buffer with the last user input
    QString inputBuffer() const;

    /// set the cursor that holds the current position and selection
    void setCursor(FormulaCursor &cursor);

    /// @return the cursor that contains the current selection and position
    FormulaCursor &cursor();

private:
    /// @return the mathml token element, that should be used for this character
    QString tokenType(const QChar &character) const;

private:
    /// the cursor that contains the current selection and position
    FormulaCursor m_cursor;

    /// The formulaData
    FormulaData *m_data;

    /// Buffer for the user input
    QString m_inputBuffer;
};

#endif // FORMULAEDITOR_H
