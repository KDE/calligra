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

#ifndef FORMULATOOLOPTIONS_H
#define FORMULATOOLOPTIONS_H

#include <QWidget>

class KoFormulaTool;
class KoXmlElement;
class QPushButton;
class QComboBox;
class QListWidget;
class QListWidgetItem;
class QGridLayout;

/**
 * @short A widget providing options for the FormulaTool
 *
 * The FormulaToolOptions widget provides the combobox - listwidget combination
 * which is used to select templates for inserting in the formula. Further the
 * widget provides two buttons - save and load formula. For saving and loading it
 * uses the loadOdf() and saveOdf() methods of KoFormulaShape.
 */
class FormulaToolOptions : public QWidget {
Q_OBJECT
public:
    /// Standart constructor
    explicit FormulaToolOptions( QWidget* parent = 0, Qt::WindowFlags f = 0 );

    /// Standart destructor
    ~FormulaToolOptions();

    /// Set the KoFormulaTool @p tool this options widget belongs to
    void setFormulaTool( KoFormulaTool* tool );

private slots:
    /// Slot connected to click of load formula button - initiates loading of a formula
    void slotLoadFormula();

    /// Slot connected to click of save formula button - initiates saving of a formula
    void slotSaveFormula();

    /// Called whenever the currently selected template class changes in the combo
    void slotTemplateComboIndexChange( int index );

private:
    /// Load the templates from path @p p into the @p list
    void loadTemplates( QList<QListWidgetItem*>* list, const QString& p );

    QListWidgetItem* createListItem( const KoXmlElement& xml );

    /// The KoFormulaTool this options widget belongs to
    KoFormulaTool* m_tool;

    /// The list widget which displays the templates for the user to insert
    QListWidget* m_templateList;

    /// The layout that is used for this widget
    QGridLayout* m_layout;

    /// The combo box to switch between the template lists
    QComboBox* m_templateCombo;

    /// The push button which is clicked for loading a formula
    QPushButton* m_loadFormula;

    /// The push button which is clicked for saving a formula 
    QPushButton* m_saveFormula;

    QList<QListWidgetItem*> m_operators;

    QList<QListWidgetItem*> m_functions;

    QList<QListWidgetItem*> m_general;

    QList<QListWidgetItem*> m_custom;
};

#endif // FORMULATOOLOPTIONS_H
