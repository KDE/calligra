/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Martin Pfeiffer <hubipete@gmx.net>
                 2009 Jeremias Epperlein <jeeree@web.de>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "FormulaToolEditWidget.h"

#include "BasicElement.h"
#include "ElementFactory.h"
#include "FormulaCursor.h"
#include "FormulaDebug.h"
#include "KoFormulaShape.h"
#include "KoFormulaTool.h"

#include <QAction>
#include <QHeaderView>
#include <QMenu>
#include <QTableWidget>
#include <QWidgetAction>

FormulaToolEditWidget::FormulaToolEditWidget(KoFormulaTool *tool, QWidget *parent)
    : QWidget(parent)
    , m_tool(tool)
{
    setupUi(this);
    setWindowTitle(i18nc("@title:group", "Edit"));
    // setup the element insert menus
    m_fractionMenu.addAction(m_tool->action("insert_fraction"));
    m_fractionMenu.addAction(m_tool->action("insert_bevelled_fraction"));

    m_fenceMenu.addAction(m_tool->action("insert_fence"));
    m_fenceMenu.addAction(m_tool->action("insert_enclosed"));

    m_tableMenu.addAction(m_tool->action("insert_33table"));
    m_tableMenu.addAction(m_tool->action("insert_21table"));

    m_rootMenu.addAction(m_tool->action("insert_root"));
    m_rootMenu.addAction(m_tool->action("insert_sqrt"));

    m_scriptsMenu.addAction(m_tool->action("insert_subscript"));
    m_scriptsMenu.addAction(m_tool->action("insert_supscript"));
    m_scriptsMenu.addAction(m_tool->action("insert_subsupscript"));
    m_scriptsMenu.addAction(m_tool->action("insert_underscript"));
    m_scriptsMenu.addAction(m_tool->action("insert_overscript"));
    m_scriptsMenu.addAction(m_tool->action("insert_underoverscript"));

    m_alterTableMenu.addAction(m_tool->action("insert_row"));
    m_alterTableMenu.addAction(m_tool->action("insert_column"));
    m_alterTableMenu.addAction(m_tool->action("remove_row"));
    m_alterTableMenu.addAction(m_tool->action("remove_column"));
}

FormulaToolEditWidget::~FormulaToolEditWidget() = default;

void FormulaToolEditWidget::setFormulaTool(KoFormulaTool *tool)
{
    m_tool = tool;
}

void FormulaToolEditWidget::insertSymbol(QTableWidgetItem *item)
{
    m_tool->insertSymbol(item->text());
}
