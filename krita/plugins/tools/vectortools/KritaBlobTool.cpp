/* This file is part of the KDE project
 * Copyright (C) 2012 José Luis Vergara <pentalis@gmail.com>
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

#include "KritaBlobTool.h"

#include <KoPathShape.h>
#include <KoParameterShape.h>
#include <KoShapeStroke.h>
#include <KoPointerEvent.h>
#include <KoCanvasBase.h>
#include <KoShapeController.h>
#include <KoShapeManager.h>
#include <KoSelection.h>
#include <KoCanvasResourceManager.h>
#include <KoColor.h>
#include <KoPathPoint.h>
#include <KoPathPointData.h>
#include <KoPathPointMergeCommand.h>
#include <KoShapePaintingContext.h>

#include <knuminput.h>
#include <klocale.h>
#include <kcombobox.h>

#include <QStackedWidget>
#include <QGroupBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QPainter>
#include <QLabel>

#include <math.h>

KritaBlobTool::KritaBlobTool(KoCanvasBase *canvas)  :  KoToolBase(canvas)
{
    m_shape = 0;
}

KritaBlobTool::~KritaBlobTool()
{
}

void KritaBlobTool::paint(QPainter &painter, const KoViewConverter &converter)
{
    if (m_shape) {
        painter.save();
        m_shape->stroke()->paint(m_shape, painter, converter);
        painter.restore();
    }
}

void KritaBlobTool::repaintDecorations()
{
}

void KritaBlobTool::mousePressEvent(KoPointerEvent *event)
{
    if (!m_shape) {
        m_shape = new KoPathShape();
        m_shape->setShapeId(KoPathShapeId);
        m_shape->setStroke(0);
    }
}

void KritaBlobTool::mouseMoveEvent(KoPointerEvent *event)
{
    qDebug() << "Mouse moved to ";
    qDebug() << event->pos();
}

void KritaBlobTool::mouseReleaseEvent(KoPointerEvent *event)
{
    qDebug() << "Mouse has been released";
}

void KritaBlobTool::keyPressEvent(QKeyEvent *event)
{
}

void KritaBlobTool::activate(ToolActivation, const QSet<KoShape*> &)
{

}

void KritaBlobTool::deactivate()
{

}

QWidget *KritaBlobTool::createOptionWidget()
{
    QWidget *optionWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(optionWidget);

    QHBoxLayout *modeLayout = new QHBoxLayout;
    modeLayout->setSpacing(3);
    QLabel *modeLabel = new QLabel(i18n("THIS WILL BE A CONFIG WIDGET"), optionWidget);
    modeLayout->addWidget(modeLabel);
    layout->addLayout(modeLayout);

    return optionWidget;
}


#include "KritaBlobTool.moc"
