/* This file is part of the KDE project
   Copyright (C) 2007 Martin Pfeiffer <hubipete@gmx.net>

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

#ifndef FORMULATOOLWIDGET_H
#define FORMULATOOLWIDGET_H

#include <QTabWidget>
#include "ui_FormulaToolWidget.h"

class KoFormulaTool;

/**
 * @short A widget providing options for the FormulaTool
 *
 * The FormulaToolOptions widget provides the combobox - listwidget combination
 * which is used to select templates for inserting in the formula. Further the
 * widget provides two buttons - save and load formula. For saving and loading it
 * uses the loadOdf() and saveOdf() methods of KoFormulaShape.
 */
class FormulaToolWidget : public QTabWidget, Ui::mainTabWidget {
Q_OBJECT
public:
    /// Standart constructor
    explicit FormulaToolWidget( QWidget* parent = 0 );

    /// Standart destructor
    ~FormulaToolWidget();

    /// Set the KoFormulaTool @p tool this options widget belongs to
    void setFormulaTool( KoFormulaTool* tool );

private slots:
    /// Slot connected to click of load formula button - initiates loading of a formula
    void slotLoadFormula();

    /// Slot connected to click of save formula button - initiates saving of a formula
    void slotSaveFormula();

private:
    /// The KoFormulaTool this options widget belongs to
    KoFormulaTool* m_tool;
};

#endif // FORMULATOOLWIDGET_H
