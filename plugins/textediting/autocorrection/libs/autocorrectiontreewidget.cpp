/*
  SPDX-FileCopyrightText: 2012-2022 Laurent Montel <montel@kde.org>

  SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "autocorrectiontreewidget.h"

#include <QKeyEvent>

AutoCorrectionTreeWidget::AutoCorrectionTreeWidget(QWidget *parent)
    : QTreeWidget(parent)
{
}

AutoCorrectionTreeWidget::~AutoCorrectionTreeWidget() = default;

void AutoCorrectionTreeWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete) {
        Q_EMIT deleteSelectedItems();
    }
    QTreeWidget::keyPressEvent(event);
}
