/* This file is part of the KDE project
 * Copyright (C) 2010-2011 C. Boemann <cbo@boemann.dk>
 * Copyright (C) 2014 Denis Kuplyakov <dener.kup@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */
#include "SimpleInsertWidget.h"
#include "TextTool.h"

#include <QAction>
#include <QDebug>

#include <QWidget>

SimpleInsertWidget::SimpleInsertWidget(TextTool *tool, QWidget *parent)
        : QWidget(parent),
        m_blockSignals(false),
        m_tool(tool)
{
    widget.setupUi(this);
    widget.insertVariable->setDefaultAction(tool->action("insert_variable"));
    widget.insertVariable->setPopupMode(QToolButton::InstantPopup); //because action overrode ui file
    widget.insertSpecialChar->setDefaultAction(tool->action("insert_specialchar"));
    widget.quickTable->addAction(tool->action("insert_table"));
    widget.insertSection->setDefaultAction(tool->action("insert_section"));
    widget.configureSection->setDefaultAction(tool->action("configure_section"));
    widget.insertPageBreak->setDefaultAction(tool->action("insert_framebreak"));
    widget.splitSections->setDefaultAction(tool->action("split_sections"));

    connect(widget.insertVariable, &QAbstractButton::clicked, this, &SimpleInsertWidget::doneWithFocus);
    connect(widget.insertSpecialChar, &QAbstractButton::clicked, this, &SimpleInsertWidget::doneWithFocus);
    connect(widget.insertPageBreak, &QAbstractButton::clicked, this, &SimpleInsertWidget::doneWithFocus);
    connect(widget.insertSection, &QAbstractButton::clicked, this, &SimpleInsertWidget::doneWithFocus);
    connect(widget.configureSection, &QAbstractButton::clicked, this, &SimpleInsertWidget::doneWithFocus);
    connect(widget.splitSections, &QAbstractButton::clicked, this, &SimpleInsertWidget::doneWithFocus);

    connect(widget.quickTable, QOverload<int, int>::of(&QuickTableButton::create), this, &SimpleInsertWidget::insertTableQuick);
    connect(widget.quickTable, QOverload<int, int>::of(&QuickTableButton::create), this, &SimpleInsertWidget::doneWithFocus);
}

void SimpleInsertWidget::setStyleManager(KoStyleManager *sm)
{
    m_styleManager = sm;
}
