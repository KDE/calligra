/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Martin Pfeiffer <hubipete@gmx.net>
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2008 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "DefaultToolTransformWidget.h"

#include "SelectionDecorator.h"
#include <KoCanvasBase.h>
#include <KoCanvasResourceManager.h>
#include <KoInteractionTool.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoUnit.h>
#include <commands/KoShapeMoveCommand.h>
#include <commands/KoShapeSizeCommand.h>
#include <commands/KoShapeTransformCommand.h>

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QList>
#include <QRadioButton>
#include <QSize>
#include <QTransform>

DefaultToolTransformWidget::DefaultToolTransformWidget(KoInteractionTool *tool, QWidget *parent)
    : QMenu(parent)
{
    m_tool = tool;

    setupUi(this);

    setUnit(m_tool->canvas()->unit());

    connect(m_tool->canvas()->resourceManager(), &KoCanvasResourceManager::canvasResourceChanged, this, &DefaultToolTransformWidget::resourceChanged);

    connect(rotateButton, &QAbstractButton::clicked, this, &DefaultToolTransformWidget::rotationChanged);
    connect(shearXButton, &QAbstractButton::clicked, this, &DefaultToolTransformWidget::shearXChanged);
    connect(shearYButton, &QAbstractButton::clicked, this, &DefaultToolTransformWidget::shearYChanged);
    connect(scaleXButton, &QAbstractButton::clicked, this, &DefaultToolTransformWidget::scaleXChanged);
    connect(scaleYButton, &QAbstractButton::clicked, this, &DefaultToolTransformWidget::scaleYChanged);
    connect(scaleAspectCheckBox, &QAbstractButton::toggled, scaleYSpinBox, &QWidget::setDisabled);
    connect(scaleAspectCheckBox, &QAbstractButton::toggled, scaleYButton, &QWidget::setDisabled);
    connect(resetButton, &QAbstractButton::clicked, this, &DefaultToolTransformWidget::resetTransformations);
}

void DefaultToolTransformWidget::setUnit(const KoUnit &unit)
{
    shearXSpinBox->setUnit(unit);
    shearYSpinBox->setUnit(unit);
}

void DefaultToolTransformWidget::resourceChanged(int key, const QVariant &res)
{
    if (key == KoCanvasResourceManager::Unit)
        setUnit(res.value<KoUnit>());
}

void DefaultToolTransformWidget::rotationChanged()
{
    QList<KoShape *> selectedShapes = m_tool->canvas()->shapeManager()->selection()->selectedShapes(KoFlake::TopLevelSelection);
    const int selectedShapesCount = selectedShapes.count();

    QVector<QTransform> oldTransforms;
    oldTransforms.reserve(selectedShapesCount);

    foreach (KoShape *shape, selectedShapes)
        oldTransforms << shape->transformation();

    qreal angle = rotateSpinBox->value();
    QPointF rotationCenter = m_tool->canvas()->shapeManager()->selection()->absolutePosition(SelectionDecorator::hotPosition());
    QTransform matrix;
    matrix.translate(rotationCenter.x(), rotationCenter.y());
    matrix.rotate(angle);
    matrix.translate(-rotationCenter.x(), -rotationCenter.y());

    foreach (KoShape *shape, selectedShapes) {
        shape->update();
        shape->applyAbsoluteTransformation(matrix);
        shape->update();
    }

    m_tool->canvas()->shapeManager()->selection()->applyAbsoluteTransformation(matrix);

    QVector<QTransform> newTransforms;
    newTransforms.reserve(selectedShapesCount);

    foreach (KoShape *shape, selectedShapes)
        newTransforms << shape->transformation();

    KoShapeTransformCommand *cmd = new KoShapeTransformCommand(selectedShapes, oldTransforms, newTransforms);
    cmd->setText(kundo2_i18n("Rotate"));
    m_tool->canvas()->addCommand(cmd);
}

void DefaultToolTransformWidget::shearXChanged()
{
    KoSelection *selection = m_tool->canvas()->shapeManager()->selection();
    QList<KoShape *> selectedShapes = selection->selectedShapes(KoFlake::TopLevelSelection);
    const int selectedShapesCount = selectedShapes.count();

    QVector<QTransform> oldTransforms;
    oldTransforms.reserve(selectedShapesCount);

    foreach (KoShape *shape, selectedShapes)
        oldTransforms << shape->transformation();

    qreal shearX = shearXSpinBox->value() / selection->size().height();
    QPointF basePoint = selection->absolutePosition(SelectionDecorator::hotPosition());
    QTransform matrix;
    matrix.translate(basePoint.x(), basePoint.y());
    matrix.shear(shearX, 0.0);
    matrix.translate(-basePoint.x(), -basePoint.y());

    foreach (KoShape *shape, selectedShapes) {
        shape->update();
        shape->applyAbsoluteTransformation(matrix);
        shape->update();
    }

    selection->applyAbsoluteTransformation(matrix);

    QVector<QTransform> newTransforms;
    newTransforms.reserve(selectedShapesCount);

    foreach (KoShape *shape, selectedShapes)
        newTransforms << shape->transformation();

    KoShapeTransformCommand *cmd = new KoShapeTransformCommand(selectedShapes, oldTransforms, newTransforms);
    cmd->setText(kundo2_i18n("Shear X"));
    m_tool->canvas()->addCommand(cmd);
}

void DefaultToolTransformWidget::shearYChanged()
{
    KoSelection *selection = m_tool->canvas()->shapeManager()->selection();
    QList<KoShape *> selectedShapes = selection->selectedShapes(KoFlake::TopLevelSelection);
    const int selectedShapesCount = selectedShapes.count();

    QVector<QTransform> oldTransforms;
    oldTransforms.reserve(selectedShapesCount);

    foreach (KoShape *shape, selectedShapes)
        oldTransforms << shape->transformation();

    qreal shearY = shearYSpinBox->value() / selection->size().width();
    QPointF basePoint = selection->absolutePosition(SelectionDecorator::hotPosition());
    QTransform matrix;
    matrix.translate(basePoint.x(), basePoint.y());
    matrix.shear(0.0, shearY);
    matrix.translate(-basePoint.x(), -basePoint.y());

    foreach (KoShape *shape, selectedShapes) {
        shape->update();
        shape->applyAbsoluteTransformation(matrix);
        shape->update();
    }

    selection->applyAbsoluteTransformation(matrix);

    QVector<QTransform> newTransforms;
    newTransforms.reserve(selectedShapesCount);

    foreach (KoShape *shape, selectedShapes)
        newTransforms << shape->transformation();

    KoShapeTransformCommand *cmd = new KoShapeTransformCommand(selectedShapes, oldTransforms, newTransforms);
    cmd->setText(kundo2_i18n("Shear Y"));
    m_tool->canvas()->addCommand(cmd);
}

void DefaultToolTransformWidget::scaleXChanged()
{
    QList<KoShape *> selectedShapes = m_tool->canvas()->shapeManager()->selection()->selectedShapes(KoFlake::TopLevelSelection);
    const int selectedShapesCount = selectedShapes.count();

    QVector<QTransform> oldTransforms;
    oldTransforms.reserve(selectedShapesCount);

    foreach (KoShape *shape, selectedShapes)
        oldTransforms << shape->transformation();

    qreal scale = scaleXSpinBox->value() * 0.01; // Input is in per cent
    QPointF basePoint = m_tool->canvas()->shapeManager()->selection()->absolutePosition(SelectionDecorator::hotPosition());
    QTransform matrix;
    matrix.translate(basePoint.x(), basePoint.y());

    if (scaleAspectCheckBox->isChecked())
        matrix.scale(scale, scale);
    else
        matrix.scale(scale, 1.0);

    matrix.translate(-basePoint.x(), -basePoint.y());

    foreach (KoShape *shape, selectedShapes) {
        shape->update();
        shape->applyAbsoluteTransformation(matrix);
        shape->update();
    }

    m_tool->canvas()->shapeManager()->selection()->applyAbsoluteTransformation(matrix);

    QVector<QTransform> newTransforms;
    newTransforms.reserve(selectedShapesCount);

    foreach (KoShape *shape, selectedShapes)
        newTransforms << shape->transformation();

    KoShapeTransformCommand *cmd = new KoShapeTransformCommand(selectedShapes, oldTransforms, newTransforms);
    cmd->setText(kundo2_i18n("Scale"));
    m_tool->canvas()->addCommand(cmd);
}

void DefaultToolTransformWidget::scaleYChanged()
{
    QList<KoShape *> selectedShapes = m_tool->canvas()->shapeManager()->selection()->selectedShapes(KoFlake::TopLevelSelection);
    const int selectedShapesCount = selectedShapes.count();

    QVector<QTransform> oldTransforms;
    oldTransforms.reserve(selectedShapesCount);

    foreach (KoShape *shape, selectedShapes)
        oldTransforms << shape->transformation();

    qreal scale = scaleYSpinBox->value() * 0.01; // Input is in per cent
    QPointF basePoint = m_tool->canvas()->shapeManager()->selection()->absolutePosition(SelectionDecorator::hotPosition());
    QTransform matrix;
    matrix.translate(basePoint.x(), basePoint.y());
    matrix.scale(1.0, scale);
    matrix.translate(-basePoint.x(), -basePoint.y());

    foreach (KoShape *shape, selectedShapes) {
        shape->update();
        shape->applyAbsoluteTransformation(matrix);
        shape->update();
    }

    m_tool->canvas()->shapeManager()->selection()->applyAbsoluteTransformation(matrix);

    QVector<QTransform> newTransforms;
    newTransforms.reserve(selectedShapesCount);

    foreach (KoShape *shape, selectedShapes)
        newTransforms << shape->transformation();

    KoShapeTransformCommand *cmd = new KoShapeTransformCommand(selectedShapes, oldTransforms, newTransforms);
    cmd->setText(kundo2_i18n("Scale"));
    m_tool->canvas()->addCommand(cmd);
}

void DefaultToolTransformWidget::resetTransformations()
{
    QList<KoShape *> selectedShapes = m_tool->canvas()->shapeManager()->selection()->selectedShapes(KoFlake::TopLevelSelection);
    const int selectedShapesCount = selectedShapes.count();

    QVector<QTransform> oldTransforms;
    oldTransforms.reserve(selectedShapesCount);

    foreach (KoShape *shape, selectedShapes)
        oldTransforms << shape->transformation();

    QTransform matrix;

    foreach (KoShape *shape, selectedShapes) {
        shape->update();
        shape->setTransformation(matrix);
        shape->update();
    }

    m_tool->canvas()->shapeManager()->selection()->applyAbsoluteTransformation(matrix);

    QVector<QTransform> newTransforms;
    newTransforms.reserve(selectedShapesCount);

    foreach (KoShape *shape, selectedShapes)
        newTransforms << shape->transformation();

    KoShapeTransformCommand *cmd = new KoShapeTransformCommand(selectedShapes, oldTransforms, newTransforms);
    cmd->setText(kundo2_i18n("Reset Transformations"));
    m_tool->canvas()->addCommand(cmd);
}
