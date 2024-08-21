/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Alexia Allanic <alexia_allanic@yahoo.fr>
 * SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#include "KPrPresentationStrategyBase.h"

#include "KPrPresentationStrategy.h"
#include "KPrPresentationTool.h"
#include "KPrPresentationToolEventForwarder.h"
#include "KPrViewModePresentation.h"
#include "ui/KPrPresentationToolWidget.h"

KPrPresentationStrategyBase::KPrPresentationStrategyBase(KPrPresentationTool *tool)
    : m_tool(tool)
    , m_widget(nullptr)
{
}

KPrPresentationStrategyBase::~KPrPresentationStrategyBase()
{
    delete m_widget;
}

void KPrPresentationStrategyBase::setToolWidgetParent(QWidget *widget)
{
    return m_tool->m_presentationToolWidget->setParent(widget);
}

KoPACanvasBase *KPrPresentationStrategyBase::canvas()
{
    return m_tool->m_viewMode.canvas();
}

void KPrPresentationStrategyBase::activateDefaultStrategy()
{
    m_tool->switchStrategy(new KPrPresentationStrategy(m_tool));
}

KPrPresentationToolEventForwarder *KPrPresentationStrategyBase::widget()
{
    return m_widget;
}
