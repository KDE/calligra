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
#include "PresentationViewPortTool.h"
#include "PresentationViewPortShape.h"
#include "PresentationViewPortConfigWidget.h"

#include <QToolButton>

#include <KoCanvasBase.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoPointerEvent.h>

PresentationViewPortTool::PresentationViewPortTool( KoCanvasBase* canvas )
   : KoToolBase(canvas) , m_shape(0)
{
}

void PresentationViewPortTool::activate(ToolActivation toolActivation, const QSet<KoShape*> &shapes)
{
    Q_UNUSED(toolActivation);

    foreach (KoShape *shape, shapes) {
        m_shape = dynamic_cast<PresentationViewPortShape*>( shape );
        if (m_shape)
            break;
    }
    if (!m_shape){
        emit done();
        return;
    }
    useCursor(Qt::ArrowCursor);
}

void PresentationViewPortTool::deactivate()
{
    m_shape = 0;
}

QWidget * PresentationViewPortTool::createOptionWidget()
{
    PresentationViewPortConfigWidget* configWidget = new PresentationViewPortConfigWidget(this);
 
    m_widget = configWidget;//TODO Not a good fix. shapeSelected signal should be connected to configWidget->updateWidget instead
  
    connect(this, SIGNAL(sequenceChanged(int)), this, SLOT(setSequence(int)));
    connect(this, SIGNAL(zoomChanged(int)), this, SLOT(setZoom(int)));
    connect(this, SIGNAL(durationChanged(int)), this, SLOT(setDuration(int)));
    connect(this, SIGNAL(transitionProfileChanged(QString)), this, SLOT(setTransitionProfile(const QString)));
       
    KoShapeManager* manager = canvas()->shapeManager();
    connect(manager, SIGNAL(selectionChanged()), this, SLOT(setCurrentShape()));
    
    m_widget->updateWidget();
    return configWidget;
}

QList< QWidget* > PresentationViewPortTool::createOptionWidgets()
{
    QList< QWidget* > ow;
    ow.append(createOptionWidget());
    
    
    return ow;    
}

void PresentationViewPortTool::setCurrentShape()
{
    KoSelection *selection = canvas()->shapeManager()->selection();
    if (selection->isSelected(m_shape))
        return;

    foreach (KoShape *shape, selection->selectedShapes()) {
        PresentationViewPortShape* viewport = dynamic_cast<PresentationViewPortShape*>(shape);
        if(viewport) {
            m_shape = viewport;
	    qDebug() << "Current shape changed.";
	    m_widget->updateWidget();    
	    
	    break;
        }
    }
}

PresentationViewPortShape* PresentationViewPortTool::currentShape()
{
    return m_shape;
}

void PresentationViewPortTool::setChangedProperty(const QString attrName, QString attrValue)
{
    m_shape->setAttribute(attrName, attrValue);
}

void PresentationViewPortTool::setSequence(int newSeq)
{
    setChangedProperty("sequence", QString("%1").arg(newSeq));
}

void PresentationViewPortTool::setZoom(int newZoom)
{
    setChangedProperty("transition-zoom-percent", QString("%1").arg(newZoom));
}

void PresentationViewPortTool::setDuration(int newDuration)
{
    setChangedProperty("transition-duration-ms", QString("%1").arg(newDuration));
}

void PresentationViewPortTool::setTransitionProfile(const QString profile)
{
    setChangedProperty("transition-profile", profile.toLower());
}

void PresentationViewPortTool::mousePressEvent(KoPointerEvent* event)
{
        KoSelection *selection = canvas()->shapeManager()->selection();
            selection->select(m_shape);
}

#include <PresentationViewPortTool.moc>
