/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "SimpleCaptionsWidget.h"
#include "TextTool.h"

#include <QAction>
#include <QDebug>

#include <QWidget>

SimpleCaptionsWidget::SimpleCaptionsWidget(QWidget *parent)
    : QWidget(parent)
    , m_blockSignals(false)
{
    widget.setupUi(this);
    //    widget.splitCells->setDefaultAction(tool->action("split_tablecells"));

    //    connect(widget.addRowAbove, SIGNAL(clicked(bool)), this, SIGNAL(doneWithFocus()));
}

void SimpleCaptionsWidget::setStyleManager(KoStyleManager *sm)
{
    m_styleManager = sm;
}
