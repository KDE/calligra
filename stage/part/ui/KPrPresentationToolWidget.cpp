/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jim Courtiau <jeremy.courtiau@gmail.com>
 * SPDX-FileCopyrightText: 2009 Johann Hingue <yoan1703@hotmail.fr>
 * SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KPrPresentationToolWidget.h"

#include <KoIcon.h>

#include <QPaintEvent>
#include <QPainter>

KPrPresentationToolWidget::KPrPresentationToolWidget(QWidget *parent)
    : QWidget(parent)
{
    // This QWidget will be the setup User Interface
    m_uiWidget.setupUi(this);

    // Set the size of the icon
    QSize size = QSize(40, 40);
    m_uiWidget.penButton->setIconSize(size);
    m_uiWidget.highLightButton->setIconSize(size);
    m_uiWidget.blackButton->setIconSize(size);
    // Set the buttons Icon
    // TODO add tooltip
    m_uiWidget.penButton->setIcon(koIcon("draw-freehand"));
    m_uiWidget.highLightButton->setIcon(koIcon("highlight-pointer-spot"));
    m_uiWidget.blackButton->setIcon(koIcon("pause-to-black-screen"));
}

void KPrPresentationToolWidget::paintEvent(QPaintEvent *event)
{
    // For the transparent background...
    QPainter painter(this);
    painter.fillRect(event->rect(), Qt::transparent);
}

Ui::KPrPresentationTool KPrPresentationToolWidget::presentationToolUi()
{
    return m_uiWidget;
}
