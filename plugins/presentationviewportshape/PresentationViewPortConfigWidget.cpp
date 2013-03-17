/* This file is part of the KDE project
 * Copyright (C) 2011 Aakriti Gupta <aakriti.a.gupta@gmail.com>
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
#include "PresentationViewPortConfigWidget.h"
#include "ui_PresentationViewPortConfigWidget.h"

#include <QLayout>
#include "PresentationViewPortShape.h"
#include <KoToolSelection.h>

PresentationViewPortConfigWidget::PresentationViewPortConfigWidget(PresentationViewPortTool * tool, QWidget* parent)
:m_tool(tool)
{
  Q_ASSERT(m_tool);
  
  m_widget.setupUi(this);
  
  connect(m_widget.sequence, SIGNAL(valueChanged(int)), m_tool, SIGNAL(sequenceChanged(int)));
  connect(m_widget.duration, SIGNAL(valueChanged(int)), m_tool, SIGNAL(durationChanged(int)));
  connect(m_widget.zoom, SIGNAL(valueChanged(int)), m_tool, SIGNAL(zoomChanged(int)));
  connect(m_widget.transitionProfile, SIGNAL(currentIndexChanged(QString)), m_tool, SIGNAL(transitionProfileChanged(QString)));
  connect(m_widget.timeoutenable, SIGNAL(stateChanged(int)), m_tool, SIGNAL(timeoutenableChanged(int)));
  connect(m_widget.clip, SIGNAL(stateChanged(int)), m_tool, SIGNAL(clipChanged(bool)));
  connect(m_widget.title, SIGNAL(textChanged(QString)), m_tool, SIGNAL(titleChanged(QString)));
  connect(m_widget.timeoutms, SIGNAL(valueChanged(int)), m_tool, SIGNAL(timeoutMsChanged(int)));
}

PresentationViewPortConfigWidget::~PresentationViewPortConfigWidget()
{

}

void PresentationViewPortConfigWidget::updateWidget()
{
    PresentationViewPortShape* currShape = m_tool->currentShape();
   
    QString attributeValue(currShape->attribute(PresentationViewPortShape::sequence));
    m_widget.sequence->setValue(attributeValue.toInt());
    
    attributeValue = currShape->attribute(PresentationViewPortShape::transitionDurationMs);
    m_widget.duration->setValue(attributeValue.toInt());
    
    attributeValue = currShape->attribute(PresentationViewPortShape::transitionZoomPercent);
    m_widget.zoom->setValue(attributeValue.toInt());
    
    attributeValue = currShape->attribute(PresentationViewPortShape::transitionProfile);
    int index = currShape->transitionProfileIndex(attributeValue);
    m_widget.transitionProfile->setCurrentIndex(index);
    
    if(currShape->attribute("clip") == "true")
      m_widget.clip->setCheckState(Qt::Checked);
    else
      m_widget.clip->setCheckState(Qt::Unchecked);
    
    
    if(currShape->attribute("timeout-enable") == "true")
      m_widget.timeoutenable->setCheckState(Qt::Checked);
    else
      m_widget.timeoutenable->setCheckState(Qt::Unchecked);
    
    attributeValue = currShape->attribute(PresentationViewPortShape::timeoutMs);
    m_widget.timeoutms->setValue(attributeValue.toInt());
    
    m_widget.title->setText(currShape->attribute("title"));
}