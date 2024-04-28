/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010-2011 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2014 Denis Kuplyakov <dener.kup@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "SimpleInsertWidget.h"
#include "TextTool.h"

#include <QAction>
#include <QDebug>

#include <QWidget>

SimpleInsertWidget::SimpleInsertWidget(TextTool *tool, QWidget *parent)
    : QWidget(parent)
    , m_blockSignals(false)
    , m_tool(tool)
{
    widget.setupUi(this);
    widget.insertVariable->setDefaultAction(tool->action("insert_variable"));
    widget.insertVariable->setPopupMode(QToolButton::InstantPopup); // because action overrode ui file
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
