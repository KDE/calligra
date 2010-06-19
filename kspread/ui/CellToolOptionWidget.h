/* This file is part of the KDE project
   Copyright 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

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

#ifndef KSPREAD_CELL_TOOL_OPTION_WIDGET
#define KSPREAD_CELL_TOOL_OPTION_WIDGET

#include <QWidget>

class QToolButton;

namespace KSpread
{
class CellToolBase;
class ExternalEditor;
class LocationComboBox;

/**
 * \ingroup UI
 * \brief The cell tool options widget.
 * The option widget has a width-dependent layout, that moves the last layout
 * item, the external editor, either to the end of the first row or into a
 * second row. For the latter case, the item will span all columns of the first
 * row.
 */
class CellToolOptionWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * Creates the cell tool option widget.
     * \param parent the parent widget
     */
    CellToolOptionWidget(CellToolBase *parent = 0);

    /** Destroys the widget. */
    virtual ~CellToolOptionWidget();

    /** \return the location combobox */
    LocationComboBox *locationComboBox() const;

    /** \return the button for invoking the formula dialog */
    QToolButton *formulaButton() const;

    /** \return the button for applying the user input */
    QToolButton *applyButton() const;

    /** \return the button for discarding the user input */
    QToolButton *cancelButton() const;

    /** \return the widget for editing the user input */
    ExternalEditor *editor() const;

protected: // reimplementations
    virtual void resizeEvent(QResizeEvent *event);

private Q_SLOTS:
    /** Updates the location address on selection changes. */
    void updateLocationComboBox();

private:
    class Private;
    Private *const d;
};

} // namespace KSpread

#endif // KSPREAD_CELL_TOOL_OPTION_WIDGET
