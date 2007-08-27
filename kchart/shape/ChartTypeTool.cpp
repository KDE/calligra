/* This file is part of the KDE project
 * Copyright (C) 2007      Inge Wallin <inge@lysator.liu.se>
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


#include "ChartTypeTool.h"

// Qt
#include <QAction>
#include <QGridLayout>
#include <QToolButton>
#include <QCheckBox>

// KDE
#include <KLocale>
#include <KIcon>

// KOffice
#include <KoCanvasBase.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoPointerEvent.h>

// ChartShape
#include "ChartTypeConfigWidget.h"


using namespace KChart;


ChartTypeTool::ChartTypeTool(KoCanvasBase *canvas)
    : KoTool(canvas),
      m_currentShape(0)
{
#if 0
    QActionGroup *group = new QActionGroup(this);
    m_topLeftOrientation  = new QAction(KIcon("golden-ratio-topleft"), i18n("Top Left"), this);
    m_topLeftOrientation->setCheckable(true);
    group->addAction(m_topLeftOrientation);
    connect( m_topLeftOrientation, SIGNAL(toggled(bool)), this, SLOT(topLeftOrientationToggled(bool)) );

    m_topRightOrientation  = new QAction(KIcon("golden-ratio-topright"), i18n("Top Right"), this);
    m_topRightOrientation->setCheckable(true);
    group->addAction(m_topRightOrientation);
    connect( m_topRightOrientation, SIGNAL(toggled(bool)), this, SLOT(topRightOrientationToggled(bool)) );

    m_bottomRightOrientation  = new QAction(KIcon("golden-ratio-bottomleft"), i18n("Bottom Right"), this);
    m_bottomRightOrientation->setCheckable(true);
    group->addAction(m_bottomRightOrientation);
    connect( m_bottomRightOrientation, SIGNAL(toggled(bool)), this, SLOT(bottomRightOrientationToggled(bool)) );

    m_bottomLeftOrientation  = new QAction(KIcon("golden-ratio-bottomright"), i18n("Bottom Left"), this);
    m_bottomLeftOrientation->setCheckable(true);
    group->addAction(m_bottomLeftOrientation);
    connect( m_bottomLeftOrientation, SIGNAL(toggled(bool)), this, SLOT(bottomLeftOrientationToggled(bool)) );
#endif
}

ChartTypeTool::~ChartTypeTool()
{
}


void ChartTypeTool::paint( QPainter &painter, const KoViewConverter &converter)
{
}

void ChartTypeTool::mousePressEvent( KoPointerEvent *event )
{
    event->ignore();
}

void ChartTypeTool::mouseMoveEvent( KoPointerEvent *event )
{
    event->ignore();
}

void ChartTypeTool::mouseReleaseEvent( KoPointerEvent *event )
{
    event->ignore();
}

void ChartTypeTool::activate (bool)
{
    KoSelection *selection = m_canvas->shapeManager()->selection();
    foreach (KoShape *shape, selection->selectedShapes()) {
        m_currentShape = dynamic_cast<ChartShape*>(shape);
        if (m_currentShape)
            break;
    }
    if (!m_currentShape) { // none found
        emit done();
        return;
    }

    //useCursor() // lets keep the forbidden cursor for now; as this tool doesn't really allow mouse interaction anyway

    updateActions();
}

void ChartTypeTool::deactivate()
{
    m_currentShape = 0;
}

void ChartTypeTool::updateActions()
{
#if 0
    switch(m_currentShape->orientation()) {
        case ChartTypeShape::BottomRight: m_bottomRightOrientation->setChecked(true); break;
        case ChartTypeShape::BottomLeft: m_bottomLeftOrientation->setChecked(true); break;
        case ChartTypeShape::TopRight: m_topRightOrientation->setChecked(true); break;
        case ChartTypeShape::TopLeft: m_topLeftOrientation->setChecked(true); break;
    }
#endif
}

#if 0
void ChartTypeTool::topLeftOrientationToggled(bool on)
{
    if(on && m_currentShape)
        m_currentShape->setOrientation(ChartTypeShape::TopLeft);
}

void ChartTypeTool::topRightOrientationToggled(bool on) {
    if(on && m_currentShape)
        m_currentShape->setOrientation(ChartTypeShape::TopRight);
}

void ChartTypeTool::bottomLeftOrientationToggled(bool on) {
    if(on && m_currentShape)
        m_currentShape->setOrientation(ChartTypeShape::BottomLeft);
}

void ChartTypeTool::bottomRightOrientationToggled(bool on) {
    if(on && m_currentShape)
        m_currentShape->setOrientation(ChartTypeShape::BottomRight);
}


void ChartTypeTool::setPrintable(bool on)
{
    if (m_currentShape)
        m_currentShape->setPrintable(on);
}
#endif


QWidget *ChartTypeTool::createOptionWidget()
{
#if 0
    QWidget *widget = new QWidget();
    QGridLayout *layout = new QGridLayout(widget);
    QToolButton *tlButton = new QToolButton(widget);
    tlButton->setDefaultAction(m_topLeftOrientation);
    layout->addWidget(tlButton, 0, 0);
    QToolButton *trButton = new QToolButton(widget);
    trButton->setDefaultAction(m_topRightOrientation);
    layout->addWidget(trButton, 0, 1);
    QToolButton *blButton = new QToolButton(widget);
    blButton->setDefaultAction(m_bottomLeftOrientation);
    layout->addWidget(blButton, 1, 0);
    QToolButton *brButton = new QToolButton(widget);
    brButton->setDefaultAction(m_bottomRightOrientation);
    layout->addWidget(brButton, 1, 1);
    QCheckBox *cb = new QCheckBox(i18n("Print the help lines"), widget);
    layout->addWidget(cb, 2, 0, 1, 3);
    connect( cb, SIGNAL(toggled(bool)), this, SLOT(setPrintable(bool)) );

    layout->setSpacing(0);
    layout->setMargin(6);
    layout->setRowStretch(3, 1);
    layout->setColumnStretch(2, 1);
#else
    QWidget *widget = new ChartTypeConfigWidget();
#endif
    return widget;
}

#include "ChartTypeTool.moc"
