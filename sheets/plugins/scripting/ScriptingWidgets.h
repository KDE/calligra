// This file is part of KSpread
// SPDX-FileCopyrightText: 2007 Sebastian Sauer <mail@dipe.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

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
    ~ScriptingSheetsListView() override;

    /**
    * Enumeration of selection types.
    */
    enum SelectionType { SingleSelect, MultiSelect };
    Q_ENUM(SelectionType);

    /**
    * Enumeration of editor types.
    */
    enum EditorType { Disabled, Cell, Range };
    Q_ENUM(EditorType);

public Q_SLOTS:

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

    void showEvent(QShowEvent* event) override;
};

#endif
