/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Martin Pfeiffer <hubipete@gmx.net>
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
   SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2010 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "DefaultToolWidget.h"
#include "DefaultTool.h"

#include "DefaultToolTransformWidget.h"
#include "SelectionDecorator.h"
#include <KoCanvasBase.h>
#include <KoCanvasResourceManager.h>
#include <KoInteractionTool.h>
#include <KoPositionSelector.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoUnit.h>
#include <commands/KoShapeMoveCommand.h>
#include <commands/KoShapeSizeCommand.h>
#include <commands/KoShapeTransformCommand.h>

#include <QAction>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QList>
#include <QRadioButton>
#include <QSize>
#include <QTransform>

DefaultToolWidget::DefaultToolWidget(KoInteractionTool *tool, QWidget *parent)
    : QWidget(parent)
    , m_tool(tool)
    , m_blockSignals(false)
{
    setupUi(this);

    setUnit(m_tool->canvas()->unit());

    aspectButton->setKeepAspectRatio(false);
    updatePosition();
    updateSize();

    connect(positionSelector, &KoPositionSelector::positionSelected, this, &DefaultToolWidget::positionSelected);

    connect(positionXSpinBox, &QAbstractSpinBox::editingFinished, this, &DefaultToolWidget::positionHasChanged);
    connect(positionYSpinBox, &QAbstractSpinBox::editingFinished, this, &DefaultToolWidget::positionHasChanged);

    connect(widthSpinBox, &QAbstractSpinBox::editingFinished, this, &DefaultToolWidget::sizeHasChanged);
    connect(heightSpinBox, &QAbstractSpinBox::editingFinished, this, &DefaultToolWidget::sizeHasChanged);

    KoSelection *selection = m_tool->canvas()->shapeManager()->selection();
    connect(selection, &KoSelection::selectionChanged, this, &DefaultToolWidget::updatePosition);
    connect(selection, &KoSelection::selectionChanged, this, &DefaultToolWidget::updateSize);
    KoShapeManager *manager = m_tool->canvas()->shapeManager();
    connect(manager, &KoShapeManager::selectionContentChanged, this, &DefaultToolWidget::updatePosition);
    connect(manager, &KoShapeManager::selectionContentChanged, this, &DefaultToolWidget::updateSize);

    connect(m_tool->canvas()->resourceManager(), &KoCanvasResourceManager::canvasResourceChanged, this, &DefaultToolWidget::resourceChanged);

    connect(aspectButton, &KoAspectButton::keepAspectRatioChanged, this, &DefaultToolWidget::aspectButtonToggled);
}

void DefaultToolWidget::positionSelected(KoFlake::Position position)
{
    m_tool->canvas()->resourceManager()->setResource(DefaultTool::HotPosition, QVariant(position));
    updatePosition();
}

void DefaultToolWidget::updatePosition()
{
    QPointF selPosition(0, 0);
    KoFlake::Position position = positionSelector->position();

    KoSelection *selection = m_tool->canvas()->shapeManager()->selection();
    if (selection->count())
        selPosition = selection->absolutePosition(position);

    positionXSpinBox->setEnabled(selection->count());
    positionYSpinBox->setEnabled(selection->count());

    if (m_blockSignals)
        return;
    m_blockSignals = true;
    positionXSpinBox->changeValue(selPosition.x());
    positionYSpinBox->changeValue(selPosition.y());

    QList<KoShape *> selectedShapes = selection->selectedShapes(KoFlake::TopLevelSelection);
    bool aspectLocked = false;
    foreach (KoShape *shape, selectedShapes)
        aspectLocked = aspectLocked | shape->keepAspectRatio();
    aspectButton->setKeepAspectRatio(aspectLocked);
    m_blockSignals = false;
}

void DefaultToolWidget::positionHasChanged()
{
    KoSelection *selection = m_tool->canvas()->shapeManager()->selection();
    if (!selection->count())
        return;

    KoFlake::Position position = positionSelector->position();
    QPointF newPos(positionXSpinBox->value(), positionYSpinBox->value());
    QPointF oldPos = selection->absolutePosition(position);
    if (oldPos == newPos)
        return;

    QList<KoShape *> selectedShapes = selection->selectedShapes(KoFlake::TopLevelSelection);
    QPointF moveBy = newPos - oldPos;
    QVector<QPointF> oldPositions;
    QVector<QPointF> newPositions;
    const int selectedShapesCount = selectedShapes.count();
    oldPositions.reserve(selectedShapesCount);
    newPositions.reserve(selectedShapesCount);
    foreach (KoShape *shape, selectedShapes) {
        oldPositions.append(shape->position());
        newPositions.append(shape->position() + moveBy);
    }
    selection->setPosition(selection->position() + moveBy);
    m_tool->canvas()->addCommand(new KoShapeMoveCommand(selectedShapes, oldPositions, newPositions));
    updatePosition();
}

void DefaultToolWidget::updateSize()
{
    QSizeF selSize(0, 0);
    KoSelection *selection = m_tool->canvas()->shapeManager()->selection();
    uint selectionCount = selection->count();
    if (selectionCount)
        selSize = selection->boundingRect().size();

    widthSpinBox->setEnabled(selectionCount);
    heightSpinBox->setEnabled(selectionCount);

    if (m_blockSignals)
        return;
    m_blockSignals = true;
    widthSpinBox->changeValue(selSize.width());
    heightSpinBox->changeValue(selSize.height());
    m_blockSignals = false;
}

void DefaultToolWidget::sizeHasChanged()
{
    if (aspectButton->hasFocus())
        return;
    if (m_blockSignals)
        return;

    QSizeF newSize(widthSpinBox->value(), heightSpinBox->value());

    KoSelection *selection = m_tool->canvas()->shapeManager()->selection();
    QRectF rect = selection->boundingRect();

    if (aspectButton->keepAspectRatio()) {
        qreal aspect = rect.width() / rect.height();
        if (rect.width() != newSize.width())
            newSize.setHeight(newSize.width() / aspect);
        else if (rect.height() != newSize.height())
            newSize.setWidth(newSize.height() * aspect);
    }

    if (rect.width() != newSize.width() || rect.height() != newSize.height()) {
        // get the scale/resize center from the position selector
        QPointF scaleCenter = selection->absolutePosition(positionSelector->position());

        QTransform resizeMatrix;
        resizeMatrix.translate(scaleCenter.x(), scaleCenter.y());
        // make sure not to divide by 0 in case the selection is a line and has no width. In this case just scale by 1.
        resizeMatrix.scale(rect.width() ? newSize.width() / rect.width() : 1, rect.height() ? newSize.height() / rect.height() : 1);
        resizeMatrix.translate(-scaleCenter.x(), -scaleCenter.y());

        QList<KoShape *> selectedShapes = selection->selectedShapes(KoFlake::StrippedSelection);
        QVector<QSizeF> oldSizes, newSizes;
        QVector<QTransform> oldState;
        QVector<QTransform> newState;
        const int selectedShapesCount = selectedShapes.count();

        oldSizes.reserve(selectedShapesCount);
        newSizes.reserve(selectedShapesCount);
        oldState.reserve(selectedShapesCount);
        newState.reserve(selectedShapesCount);

        foreach (KoShape *shape, selectedShapes) {
            shape->update();
            QSizeF oldSize = shape->size();
            oldState << shape->transformation();
            QTransform shapeMatrix = shape->absoluteTransformation(nullptr);

            // calculate the matrix we would apply to the local shape matrix
            // that tells us the effective scale values we have to use for the resizing
            QTransform localMatrix = shapeMatrix * resizeMatrix * shapeMatrix.inverted();
            // save the effective scale values, without any mirroring portion
            const qreal scaleX = qAbs(localMatrix.m11());
            const qreal scaleY = qAbs(localMatrix.m22());

            // calculate the scale matrix which is equivalent to our resizing above
            QTransform scaleMatrix = (QTransform().scale(scaleX, scaleY));
            scaleMatrix = shapeMatrix.inverted() * scaleMatrix * shapeMatrix;

            // calculate the new size of the shape, using the effective scale values
            oldSizes << oldSize;
            QSizeF newSize = QSizeF(scaleX * oldSize.width(), scaleY * oldSize.height());
            newSizes << newSize;
            shape->setSize(newSize);

            // apply the rest of the transformation without the resizing part
            shape->applyAbsoluteTransformation(scaleMatrix.inverted() * resizeMatrix);
            newState << shape->transformation();
        }
        m_tool->repaintDecorations();
        selection->applyAbsoluteTransformation(resizeMatrix);
        KUndo2Command *cmd = new KUndo2Command(kundo2_i18n("Resize"));
        new KoShapeSizeCommand(selectedShapes, oldSizes, newSizes, cmd);
        new KoShapeTransformCommand(selectedShapes, oldState, newState, cmd);
        m_tool->canvas()->addCommand(cmd);
        updateSize();
        updatePosition();
    }
}

void DefaultToolWidget::setUnit(const KoUnit &unit)
{
    m_blockSignals = true;
    positionXSpinBox->setUnit(unit);
    positionYSpinBox->setUnit(unit);
    widthSpinBox->setUnit(unit);
    heightSpinBox->setUnit(unit);
    m_blockSignals = false;

    updatePosition();
    updateSize();
}

void DefaultToolWidget::resourceChanged(int key, const QVariant &res)
{
    if (key == KoCanvasResourceManager::Unit)
        setUnit(res.value<KoUnit>());
    else if (key == DefaultTool::HotPosition) {
        if (res.toInt() != positionSelector->position()) {
            positionSelector->setPosition(static_cast<KoFlake::Position>(res.toInt()));
            updatePosition();
        }
    }
}

void DefaultToolWidget::aspectButtonToggled(bool keepAspect)
{
    if (m_blockSignals)
        return;
    KoSelection *selection = m_tool->canvas()->shapeManager()->selection();
    foreach (KoShape *shape, selection->selectedShapes(KoFlake::TopLevelSelection)) {
        shape->setKeepAspectRatio(keepAspect);
    }
}
