// SPDX-FileCopyrightText: 2012 C. Boemann <cbo@boemann.dk>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "SimpleHeaderFooterWidget.h"

#include "KWView.h"

SimpleHeaderFooterWidget::SimpleHeaderFooterWidget(KWView *view, QWidget *parent)
    : QWidget(parent)
{
    widget.setupUi(this);

    widget.insertFooter->setDefaultAction(view->action("insert_footer"));
    widget.insertHeader->setDefaultAction(view->action("insert_header"));
}

SimpleHeaderFooterWidget::~SimpleHeaderFooterWidget() = default;
