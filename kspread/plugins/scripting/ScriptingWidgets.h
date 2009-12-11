/*
 * This file is part of KSpread
 *
 * Copyright (c) 2007 Sebastian Sauer <mail@dipe.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef SCRIPTINGWIDGETS_H
#define SCRIPTINGWIDGETS_H

#include <QVariant>
#include <QWidget>

class QTreeView;
class ScriptingModule;

/**
 * The ScriptingSheetsListView provides a listview-widget that displays
 * all sheets and lets the user choose 0..n of them plus specify
 * cell-ranges for all of them.
 */
class ScriptingSheetsListView : public QWidget
{
    Q_OBJECT

    /**
    * Enumeration of selection types.
    */
    Q_ENUMS(SelectionType)

    /**
    * Enumeration of editor types.
    */
    Q_ENUMS(EditorType)

public:

    /**
    * Constructor.
    * \param module The \a ScriptingModule instance that should be used
    * as datasource for what should be displayed within the listview.
    * \param parent The parent QWidget this widget should be displayed in.
    */
    ScriptingSheetsListView(ScriptingModule* module, QWidget* parent);

    /**
    * Destructor.
    */
    virtual ~ScriptingSheetsListView();

    enum SelectionType { SingleSelect, MultiSelect };
    enum EditorType { Disabled, Cell, Range };

public slots:

    /**
    * Set the selection type to \p selectiontype. This could be either
    * "SingleSelect" or "MultiSelect".
    */
    void setSelectionType(const QString& selectiontype);

    /**
    * Set the editor type to \p editortype. This could be either
    * "Disabled", "Cell" or "Range".
    */
    void setEditorType(const QString& editortype);

    /**
    * Return the name of the sheet. This makes only sense of
    * "SingleSelect" was defined as selection type else, if
    * "MultiSelect" was defined, use the sheets() function.
    */
    QString sheet();

    /**
    * Return the content of the editor. This makes only sense of
    * "SingleSelect" was defined as selection type else, if
    * "MultiSelect" was defined, use the sheets() function.
    */
    QString editor();

    /**
    * Return the list of all selected sheets. Selected are those sheets the
    * user enabled the checkbutton for and the resulting list contains for
    * each such sheetitem a list of the name and the range rectangle.
    *
    * For example in python following structure got returned if there
    * exist 3 sheets where Sheet1 and Sheet2 got selected. Sheet1 also does
    * define the range A1:B2.
    * [['Sheet1', 1, [1, 1, 2, 2]], ['Sheet2', 1], ['Sheet3', 0]]
    *
    * Each sheet contains a tuple of
    * \li sheetname
    * \li 1=enabled or 0=disabled
    * \li optional range tuple [from column, from row, to column, to row]
    */
    QVariantList sheets();

private:
    ScriptingModule* m_module;
    QTreeView* m_view;
    bool m_initialized;
    QVariantList m_prevlist;

    SelectionType m_selectiontype;
    EditorType m_editortype;

    void initialize();
    void finalize();

    virtual void showEvent(QShowEvent* event);
};

#endif
