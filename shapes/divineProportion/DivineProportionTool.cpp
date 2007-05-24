/* This file is part of the KDE project
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
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

#include "DivineProportionTool.h"

#include <KoCanvasBase.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoPointerEvent.h>

#include <KLocale>
#include <KIcon>
#include <QAction>
#include <QGridLayout>
#include <QToolButton>

DivineProportionTool::DivineProportionTool(KoCanvasBase *canvas)
    : KoTool(canvas),
    m_currentShape(0)
{
    QActionGroup *group = new QActionGroup(this);
    m_topLeftOrientation  = new QAction(KIcon("golden-ratio-topleft"), i18n("TopLeft"), this);
    m_topLeftOrientation->setCheckable(true);
    group->addAction(m_topLeftOrientation);
    connect( m_topLeftOrientation, SIGNAL(toggled(bool)), this, SLOT(topLeftOrientationToggled(bool)) );

    m_topRightOrientation  = new QAction(KIcon("golden-ratio-topleft"), i18n("TopLeft"), this);
    m_topRightOrientation->setCheckable(true);
    group->addAction(m_topRightOrientation);
    connect( m_topRightOrientation, SIGNAL(toggled(bool)), this, SLOT(topRightOrientationToggled(bool)) );

    m_bottomRightOrientation  = new QAction(KIcon("golden-ratio-topleft"), i18n("TopLeft"), this);
    m_bottomRightOrientation->setCheckable(true);
    group->addAction(m_bottomRightOrientation);
    connect( m_bottomRightOrientation, SIGNAL(toggled(bool)), this, SLOT(bottomLeftOrientationToggled(bool)) );

    m_bottomLeftOrientation  = new QAction(KIcon("golden-ratio-topleft"), i18n("TopLeft"), this);
    m_bottomLeftOrientation->setCheckable(true);
    group->addAction(m_bottomLeftOrientation);
    connect( m_bottomLeftOrientation, SIGNAL(toggled(bool)), this, SLOT(bottomRightOrientationToggled(bool)) );
}

DivineProportionTool::~DivineProportionTool() {
}

void DivineProportionTool::paint( QPainter &painter, KoViewConverter &converter) {
}

void DivineProportionTool::mousePressEvent( KoPointerEvent *event ) {
    event->ignore();
}

void DivineProportionTool::mouseMoveEvent( KoPointerEvent *event ) {
    event->ignore();
}

void DivineProportionTool::mouseReleaseEvent( KoPointerEvent *event ) {
    event->ignore();
}

void DivineProportionTool::activate (bool) {
    KoSelection *selection = m_canvas->shapeManager()->selection();
    foreach(KoShape *shape, selection->selectedShapes()) {
        m_currentShape = dynamic_cast<DivineProportionShape*> (shape);
        if(m_currentShape)
            break;
    }
    if(m_currentShape == 0) { // none found
        emit sigDone();
        return;
    }
    //useCursor() // lets keep the forbidden cursor for now; as this tool doesn't really allow mouse interaction anyway

    updateActions();
}

void DivineProportionTool::deactivate() {
    m_currentShape = 0;
}

void DivineProportionTool::updateActions() {
    m_topLeftOrientation->setChecked(m_currentShape->orientation() == DivineProportionShape::TopLeft);
    m_topRightOrientation->setChecked(m_currentShape->orientation() == DivineProportionShape::TopRight);
    m_bottomLeftOrientation->setChecked(m_currentShape->orientation() == DivineProportionShape::BottomLeft);
    m_bottomRightOrientation->setChecked(m_currentShape->orientation() == DivineProportionShape::BottomRight);
}

void DivineProportionTool::topLeftOrientationToggled(bool on) {
    if(on && m_currentShape)
        m_currentShape->setOrientation(DivineProportionShape::TopLeft);
}

void DivineProportionTool::topRightOrientationToggled(bool on) {
    if(on && m_currentShape)
        m_currentShape->setOrientation(DivineProportionShape::TopRight);
}

void DivineProportionTool::bottomLeftOrientationToggled(bool on) {
    if(on && m_currentShape)
        m_currentShape->setOrientation(DivineProportionShape::BottomLeft);
}

void DivineProportionTool::bottomRightOrientationToggled(bool on) {
    if(on && m_currentShape)
        m_currentShape->setOrientation(DivineProportionShape::BottomRight);
}

QWidget *DivineProportionTool::createOptionWidget() {
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

    layout->setSpacing(0);
    layout->setMargin(6);
    layout->setRowStretch(2, 1);
    layout->setColumnStretch(2, 1);
    return widget;
}

#include "DivineProportionTool.moc"
