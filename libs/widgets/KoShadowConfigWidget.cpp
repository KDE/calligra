/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2012 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoShadowConfigWidget.h"
#include "ui_KoShadowConfigWidget.h"

#include <KoCanvasBase.h>
#include <KoCanvasResourceManager.h>
#include <KoColorPopupAction.h>
#include <KoIcon.h>
#include <KoSelection.h>
#include <KoShapeManager.h>
#include <KoShapeShadow.h>
#include <KoShapeShadowCommand.h>
#include <KoUnit.h>

#include <KLocalizedString>

#include <QCheckBox>

#include <math.h>

class Q_DECL_HIDDEN KoShadowConfigWidget::Private
{
public:
    Private() = default;
    Ui_KoShadowConfigWidget widget;
    KoColorPopupAction *actionShadowColor;
    KoCanvasBase *canvas;
};

KoShadowConfigWidget::KoShadowConfigWidget(QWidget *parent)
    : QWidget(parent)
    , d(new Private())
{
    d->widget.setupUi(this);
    d->widget.shadowOffset->setValue(8.0);
    d->widget.shadowBlur->setValue(8.0);
    d->widget.shadowBlur->setMinimum(0.0);
    d->widget.shadowAngle->setValue(315.0);
    d->widget.shadowAngle->setMinimum(0.0);
    d->widget.shadowAngle->setMaximum(360.0);
    d->widget.shadowVisible->setChecked(false);
    visibilityChanged();

    d->actionShadowColor = new KoColorPopupAction(this);
    d->actionShadowColor->setCurrentColor(QColor(0, 0, 0, 192)); // some reasonable default for shadow
    d->actionShadowColor->setIcon(koIcon("format-stroke-color"));
    d->actionShadowColor->setToolTip(i18n("Change the color of the shadow"));
    d->widget.shadowColor->setDefaultAction(d->actionShadowColor);

    connect(d->widget.shadowVisible, &QAbstractButton::toggled, this, &KoShadowConfigWidget::applyChanges);
    connect(d->widget.shadowVisible, &QAbstractButton::toggled, this, &KoShadowConfigWidget::visibilityChanged);
    connect(d->actionShadowColor, &KoColorPopupAction::colorChanged, this, &KoShadowConfigWidget::applyChanges);
    connect(d->widget.shadowAngle, &QAbstractSlider::valueChanged, this, &KoShadowConfigWidget::applyChanges);
    connect(d->widget.shadowOffset, &KoUnitDoubleSpinBox::valueChangedPt, this, &KoShadowConfigWidget::applyChanges);
    connect(d->widget.shadowBlur, &KoUnitDoubleSpinBox::valueChangedPt, this, &KoShadowConfigWidget::applyChanges);
}

KoShadowConfigWidget::~KoShadowConfigWidget()
{
    delete d;
}

void KoShadowConfigWidget::setShadowColor(const QColor &color)
{
    d->widget.shadowColor->blockSignals(true);
    d->actionShadowColor->blockSignals(true);

    d->actionShadowColor->setCurrentColor(color);

    d->actionShadowColor->blockSignals(false);
    d->widget.shadowColor->blockSignals(false);
}

QColor KoShadowConfigWidget::shadowColor() const
{
    return d->actionShadowColor->currentColor();
}

void KoShadowConfigWidget::setShadowOffset(const QPointF &offset)
{
    qreal length = sqrt(offset.x() * offset.x() + offset.y() * offset.y());
    qreal angle = atan2(-offset.y(), offset.x());
    if (angle < 0.0) {
        angle += 2 * M_PI;
    }

    d->widget.shadowAngle->blockSignals(true);
    d->widget.shadowAngle->setValue(-90 - angle * 180.0 / M_PI);
    d->widget.shadowAngle->blockSignals(false);

    d->widget.shadowOffset->blockSignals(true);
    d->widget.shadowOffset->changeValue(length);
    d->widget.shadowOffset->blockSignals(false);
}

QPointF KoShadowConfigWidget::shadowOffset() const
{
    QPointF offset(d->widget.shadowOffset->value(), 0);
    QTransform m;
    m.rotate(d->widget.shadowAngle->value() + 90);
    return m.map(offset);
}

void KoShadowConfigWidget::setShadowBlur(const qreal &blur)
{
    d->widget.shadowBlur->blockSignals(true);
    d->widget.shadowBlur->changeValue(blur);
    d->widget.shadowBlur->blockSignals(false);
}

qreal KoShadowConfigWidget::shadowBlur() const
{
    return d->widget.shadowBlur->value();
}

void KoShadowConfigWidget::setShadowVisible(bool visible)
{
    d->widget.shadowVisible->blockSignals(true);
    d->widget.shadowVisible->setChecked(visible);
    d->widget.shadowVisible->blockSignals(false);
    visibilityChanged();
}

bool KoShadowConfigWidget::shadowVisible() const
{
    return d->widget.shadowVisible->isChecked();
}

void KoShadowConfigWidget::visibilityChanged()
{
    d->widget.shadowAngle->setEnabled(d->widget.shadowVisible->isChecked());
    d->widget.shadowBlur->setEnabled(d->widget.shadowVisible->isChecked());
    d->widget.shadowColor->setEnabled(d->widget.shadowVisible->isChecked());
    d->widget.shadowOffset->setEnabled(d->widget.shadowVisible->isChecked());
}

void KoShadowConfigWidget::applyChanges()
{
    if (d->canvas) {
        KoSelection *selection = d->canvas->shapeManager()->selection();
        KoShape *shape = selection->firstSelectedShape(KoFlake::TopLevelSelection);
        if (!shape) {
            return;
        }

        KoShapeShadow *newShadow = new KoShapeShadow();
        newShadow->setVisible(shadowVisible());
        newShadow->setColor(shadowColor());
        newShadow->setOffset(shadowOffset());
        newShadow->setBlur(shadowBlur());
        d->canvas->addCommand(new KoShapeShadowCommand(selection->selectedShapes(KoFlake::TopLevelSelection), newShadow));
    }
}

void KoShadowConfigWidget::selectionChanged()
{
    if (!d->canvas) {
        return;
    }

    KoSelection *selection = d->canvas->shapeManager()->selection();
    KoShape *shape = selection->firstSelectedShape(KoFlake::TopLevelSelection);

    setEnabled(shape != nullptr);

    if (!shape) {
        setShadowVisible(false);
        return;
    }

    KoShapeShadow *shadow = shape->shadow();
    if (!shadow) {
        setShadowVisible(false);
        return;
    }

    setShadowVisible(shadow->isVisible());
    setShadowOffset(shadow->offset());
    setShadowColor(shadow->color());
    setShadowBlur(shadow->blur());
}

void KoShadowConfigWidget::setCanvas(KoCanvasBase *canvas)
{
    d->canvas = canvas;
    connect(canvas->shapeManager(), &KoShapeManager::selectionChanged, this, &KoShadowConfigWidget::selectionChanged);
    connect(canvas->shapeManager(), &KoShapeManager::selectionContentChanged, this, &KoShadowConfigWidget::selectionChanged);

    setUnit(canvas->unit());

    connect(d->canvas->resourceManager(), &KoCanvasResourceManager::canvasResourceChanged, this, &KoShadowConfigWidget::resourceChanged);
}

void KoShadowConfigWidget::setUnit(const KoUnit &unit)
{
    d->widget.shadowOffset->blockSignals(true);
    d->widget.shadowBlur->blockSignals(true);
    d->widget.shadowOffset->setUnit(unit);
    d->widget.shadowBlur->setUnit(unit);
    d->widget.shadowOffset->blockSignals(false);
    d->widget.shadowBlur->blockSignals(false);
}

void KoShadowConfigWidget::resourceChanged(int key, const QVariant &res)
{
    if (key == KoCanvasResourceManager::Unit) {
        setUnit(res.value<KoUnit>());
    }
}
