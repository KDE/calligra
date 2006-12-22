/* This file is part of the KDE project
 *   Copyright (C)  2006 Peter Simonsson <peter.simonsson@gmail.com>
 * 
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Library General Public
 *   License as published by the Free Software Foundation; either
 *   version 2 of the License, or (at your option) any later version.
 * 
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Library General Public License for more details.
 * 
 *   You should have received a copy of the GNU Library General Public License
 *   along with this library; see the file COPYING.LIB.  If not, write to
 *   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *   Boston, MA 02110-1301, USA.
 */

#include "KivioShapeGeometry.h"

#include <QSizeF>
#include <QPointF>
#include <QDockWidget>

#include <klocale.h>

#include <KoShape.h>
#include <KoSelection.h>
#include <KoShapeMoveCommand.h>
#include <KoShapeSizeCommand.h>
#include <KoShapeLockCommand.h>
#include <KoShapeRotateCommand.h>

#include "KivioDocument.h"

KivioShapeGeometry::KivioShapeGeometry(KivioDocument* doc)
    : QDockWidget(i18n("Geometry")), Ui::KivioShapeGeometry(), m_doc(doc)
{
    m_lockedForUpdate = false;
    m_selection = 0;

    setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    m_mainWidget = new QWidget(this);
    m_mainWidget->setEnabled(false);
    setWidget(m_mainWidget);

    setupUi(m_mainWidget);

    connect(m_xInput, SIGNAL(valueChangedPt(double)), this, SLOT(positionChanged()));
    connect(m_yInput, SIGNAL(valueChangedPt(double)), this, SLOT(positionChanged()));
    connect(m_widthInput, SIGNAL(valueChangedPt(double)), this, SLOT(sizeChanged()));
    connect(m_heightInput, SIGNAL(valueChangedPt(double)), this, SLOT(sizeChanged()));
    connect(m_protectSizeCheck, SIGNAL(toggled(bool)), this, SLOT(protectSizeChanged(bool)));
    connect(m_rotationInput, SIGNAL(valueChanged(double)), this, SLOT(rotationChanged()));
}

void KivioShapeGeometry::setSelection(KoSelection* selection)
{
    m_selection = selection;

    if(!m_selection) {
        m_mainWidget->setEnabled(false);
        return;
    }

    bool selected = m_selection->count() > 0;
    m_mainWidget->setEnabled(selected);

    update();
}

void KivioShapeGeometry::update()
{
    if(!m_selection || m_selection->count() <= 0) {
        return;
    }

    m_lockedForUpdate = true;
    KoShape* firstShape = *(m_selection->selectedShapes(KoFlake::StrippedSelection).begin());
    QPointF position = firstShape->position();
    QSizeF size = firstShape->size();
    m_xInput->changeValue(position.x());
    m_yInput->changeValue(position.y());
    m_widthInput->changeValue(size.width());
    m_heightInput->changeValue(size.height());
    m_rotationInput->setValue(firstShape->rotation());

    m_protectSizeCheck->setChecked(firstShape->isLocked());
    protectSizeChanged(firstShape->isLocked()); // set the other widgets enabled/disabled
    m_lockedForUpdate = false;
}

void KivioShapeGeometry::positionChanged()
{
    if(m_lockedForUpdate)
        return;

    KoSelectionSet selectionSet = m_selection->selectedShapes(KoFlake::StrippedSelection);
    KoShape* firstShape = *(selectionSet.begin());
    QPointF position = firstShape->position();
    double dx = m_xInput->value() - position.x();
    double dy = m_yInput->value() - position.y();

    QList<QPointF> prevPos;
    QList<QPointF> newPos;
    QList<KoShape*> shapes;

    foreach(KoShape* shape, selectionSet) {
        if(shape->isLocked())
            continue;

        shapes.append(shape);
        QPointF p = shape->position();
        prevPos.append(p);
        p.setX(p.x() + dx);
        p.setY(p.y() + dy);
        newPos.append(p);
    }

    KoShapeMoveCommand* moveCommand = new KoShapeMoveCommand(shapes, prevPos, newPos);
    m_doc->addCommand(moveCommand);
}

void KivioShapeGeometry::sizeChanged()
{
    if(m_lockedForUpdate)
        return;

    m_lockedForUpdate = true;

    KoSelectionSet selectionSet = m_selection->selectedShapes(KoFlake::StrippedSelection);
    KoShape* firstShape = *(selectionSet.begin());
    QSizeF size = firstShape->size();
    double dx = m_widthInput->value() - size.width();
    double dy = m_heightInput->value() - size.height();

    QList<QSizeF> prevSize;
    QList<QSizeF> newSize;
    QList<KoShape*> shapes;

    foreach(KoShape* shape, selectionSet) {
        if(shape->isLocked())
            continue;

        shapes.append(shape);
        QSizeF s = shape->size();
        prevSize.append(s);

        if(m_aspectRatioCheck->isChecked()) {
            double ratio = s.width() / s.height();

            if(dx != 0.0) {
                dy = dx / ratio;
            } else if(dy != 0.0) {
                dx = dy * ratio;
            }
        }

        s.setWidth(s.width() + dx);
        s.setHeight(s.height() + dy);
        newSize.append(s);
    }

    KoShapeSizeCommand* sizeCommand = new KoShapeSizeCommand(shapes, prevSize, newSize);
    m_doc->addCommand(sizeCommand);

    // Make sure we show the correct values (needed for keep ratio)
    size = firstShape->size();
    m_widthInput->changeValue(size.width());
    m_heightInput->changeValue(size.height());

    m_lockedForUpdate = false;
}

void KivioShapeGeometry::protectSizeChanged(bool protect)
{
    m_xInput->setDisabled(protect);
    m_yInput->setDisabled(protect);
    m_widthInput->setDisabled(protect);
    m_heightInput->setDisabled(protect);
    m_rotationInput->setDisabled(protect);
    m_aspectRatioCheck->setDisabled(protect);

    if(m_lockedForUpdate)
        return;

    KoSelectionSet selectionSet = m_selection->selectedShapes(KoFlake::StrippedSelection);
    QList<bool> oldLock;
    QList<bool> newLock;

    foreach(KoShape* shape, selectionSet) {
        oldLock.append(shape->isLocked());
        newLock.append(protect);
    }

    KoShapeLockCommand* lockCommand = new KoShapeLockCommand(selectionSet, oldLock, newLock);
    m_doc->addCommand(lockCommand);
}

void KivioShapeGeometry::rotationChanged()
{
    if(m_lockedForUpdate)
        return;

    KoSelectionSet selectionSet = m_selection->selectedShapes(KoFlake::StrippedSelection);
    KoShape* firstShape = *(selectionSet.begin());
    double rotation = firstShape->rotation();
    double dr = m_rotationInput->value() - rotation;

    QList<double> prevRotation;
    QList<double> newRotation;
    QList<KoShape*> shapes;

    foreach(KoShape* shape, selectionSet) {
        if(shape->isLocked())
            continue;

        shapes.append(shape);
        double r = shape->rotation();
        prevRotation.append(r);
        newRotation.append(r + dr);
    }

    KoShapeRotateCommand* rotateCommand = new KoShapeRotateCommand(shapes, prevRotation, newRotation);
    m_doc->addCommand(rotateCommand);
}

void KivioShapeGeometry::setUnit(KoUnit unit)
{
    m_xInput->setUnit(unit);
    m_yInput->setUnit(unit);
    m_widthInput->setUnit(unit);
    m_heightInput->setUnit(unit);
}


//
// KivioShapeGeometryFactory
//

KivioShapeGeometryFactory::KivioShapeGeometryFactory(KivioDocument* doc)
    : KoDockFactory()
{
    m_doc = doc;
}

QString KivioShapeGeometryFactory::dockId() const
{
    return QString("KivioShapeGeometry");
}

Qt::DockWidgetArea KivioShapeGeometryFactory::defaultDockWidgetArea() const
{
    return Qt::RightDockWidgetArea;
}

QDockWidget* KivioShapeGeometryFactory::createDockWidget()
{
    KivioShapeGeometry* dockWidget = new KivioShapeGeometry(m_doc);
    dockWidget->setObjectName(dockId());
    dockWidget->setUnit(m_doc->unit());

    QObject::connect(m_doc, SIGNAL(unitChanged(KoUnit)), dockWidget, SLOT(setUnit(KoUnit)));

    return dockWidget;
}

#include "KivioShapeGeometry.moc"
