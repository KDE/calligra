/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Martin Pfeiffer <hubipete@gmx.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef FormulaToolEditWidget_H
#define FormulaToolEditWidget_H

#include "ui_FormulaToolEditWidget.h"
#include <QMenu>
#include <QTabWidget>

class KoFormulaTool;
class QTableWidgetItem;

/**
 * @short A widget providing options for the FormulaTool
 *
 * The FormulaToolOptions widget provides the combobox - listwidget combination
 * which is used to select templates for inserting in the formula. Further the
 * widget provides two buttons - save and load formula. For saving and loading it
 * uses the loadOdf() and saveOdf() methods of KoFormulaShape.
 */
class FormulaToolEditWidget : public QWidget, Ui::FormulaToolEditWidget
{
    Q_OBJECT
public:
    /// Standard constructor
    explicit FormulaToolEditWidget(KoFormulaTool *tool, QWidget *parent = nullptr);

    /// Standard destructor
    ~FormulaToolEditWidget() override;

    /// Set the KoFormulaTool @p tool this options widget belongs to
    void setFormulaTool(KoFormulaTool *tool);

public Q_SLOTS:
    void insertSymbol(QTableWidgetItem *item);

private:
    /// The KoFormulaTool this options widget belongs to
    KoFormulaTool *m_tool;

    QMenu m_scriptsMenu;
    QMenu m_fractionMenu;
    QMenu m_tableMenu;
    QMenu m_fenceMenu;
    QMenu m_rootMenu;
    QMenu m_arrowMenu;
    QMenu m_greekMenu;
    QMenu m_miscMenu;
    QMenu m_relationMenu;
    QMenu m_operatorMenu;
    QMenu m_alterTableMenu;
    QMenu m_rowMenu;
};

#endif // FormulaToolEditWidget_H
