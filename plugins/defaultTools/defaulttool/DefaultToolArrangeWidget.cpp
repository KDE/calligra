/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Martin Pfeiffer <hubipete@gmx.net>
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "DefaultToolArrangeWidget.h"

#include <KoInteractionTool.h>
#include <QAction>

DefaultToolArrangeWidget::DefaultToolArrangeWidget(KoInteractionTool *tool, QWidget *parent)
    : QWidget(parent)
{
    m_tool = tool;

    setupUi(this);

    bringToFront->setDefaultAction(m_tool->action("object_order_front"));
    raiseLevel->setDefaultAction(m_tool->action("object_order_raise"));
    lowerLevel->setDefaultAction(m_tool->action("object_order_lower"));
    sendBack->setDefaultAction(m_tool->action("object_order_back"));

    leftAlign->setDefaultAction(m_tool->action("object_align_horizontal_left"));
    hCenterAlign->setDefaultAction(m_tool->action("object_align_horizontal_center"));
    rightAlign->setDefaultAction(m_tool->action("object_align_horizontal_right"));
    topAlign->setDefaultAction(m_tool->action("object_align_vertical_top"));
    vCenterAlign->setDefaultAction(m_tool->action("object_align_vertical_center"));
    bottomAlign->setDefaultAction(m_tool->action("object_align_vertical_bottom"));

    group->setDefaultAction(m_tool->action("object_group"));
    ungroup->setDefaultAction(m_tool->action("object_ungroup"));
}
