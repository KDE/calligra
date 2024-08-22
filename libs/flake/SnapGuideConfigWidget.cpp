/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "SnapGuideConfigWidget.h"
#include "KoSnapGuide.h"

#include <KoIcon.h>

SnapGuideConfigWidget::SnapGuideConfigWidget(KoSnapGuide *snapGuide, QWidget *parent)
    : QWidget(parent)
    , m_snapGuide(snapGuide)
{
    widget.setupUi(this);

    widget.orthogonalSnapGuide->setIcon(koIcon("snap-orthogonal"));
    widget.nodeSnapGuide->setIcon(koIcon("snap-node"));
    widget.extensionSnapGuide->setIcon(koIcon("snap-extension"));
    widget.intersectionSnapGuide->setIcon(koIcon("snap-intersection"));
    widget.boundingBoxSnapGuide->setIcon(koIcon("snap-bounding-box"));
    widget.lineGuideSnapGuide->setIcon(koIcon("snap-guideline"));

    updateControls();

    connect(widget.useSnapGuides, &QAbstractButton::toggled, this, &SnapGuideConfigWidget::snappingEnabled);
    connect(widget.orthogonalSnapGuide, &QAbstractButton::toggled, this, &SnapGuideConfigWidget::strategyChanged);
    connect(widget.nodeSnapGuide, &QAbstractButton::toggled, this, &SnapGuideConfigWidget::strategyChanged);
    connect(widget.extensionSnapGuide, &QAbstractButton::toggled, this, &SnapGuideConfigWidget::strategyChanged);
    connect(widget.intersectionSnapGuide, &QAbstractButton::toggled, this, &SnapGuideConfigWidget::strategyChanged);
    connect(widget.boundingBoxSnapGuide, &QAbstractButton::toggled, this, &SnapGuideConfigWidget::strategyChanged);
    connect(widget.lineGuideSnapGuide, &QAbstractButton::toggled, this, &SnapGuideConfigWidget::strategyChanged);
    connect(widget.snapDistance, QOverload<int>::of(&QSpinBox::valueChanged), this, &SnapGuideConfigWidget::distanceChanged);

    widget.useSnapGuides->setChecked(snapGuide->isSnapping());
}

SnapGuideConfigWidget::~SnapGuideConfigWidget() = default;

void SnapGuideConfigWidget::snappingEnabled(bool isEnabled)
{
    widget.orthogonalSnapGuide->setEnabled(isEnabled);
    widget.nodeSnapGuide->setEnabled(isEnabled);
    widget.extensionSnapGuide->setEnabled(isEnabled);
    widget.intersectionSnapGuide->setEnabled(isEnabled);
    widget.boundingBoxSnapGuide->setEnabled(isEnabled);
    widget.lineGuideSnapGuide->setEnabled(isEnabled);
    widget.snapDistance->setEnabled(isEnabled);

    m_snapGuide->enableSnapping(isEnabled);
}

void SnapGuideConfigWidget::strategyChanged()
{
    KoSnapGuide::Strategies strategies;
    if (widget.orthogonalSnapGuide->isChecked())
        strategies |= KoSnapGuide::OrthogonalSnapping;
    if (widget.nodeSnapGuide->isChecked())
        strategies |= KoSnapGuide::NodeSnapping;
    if (widget.extensionSnapGuide->isChecked())
        strategies |= KoSnapGuide::ExtensionSnapping;
    if (widget.intersectionSnapGuide->isChecked())
        strategies |= KoSnapGuide::IntersectionSnapping;
    if (widget.boundingBoxSnapGuide->isChecked())
        strategies |= KoSnapGuide::BoundingBoxSnapping;
    if (widget.lineGuideSnapGuide->isChecked())
        strategies |= KoSnapGuide::GuideLineSnapping;

    m_snapGuide->enableSnapStrategies(strategies);
}

void SnapGuideConfigWidget::distanceChanged(int distance)
{
    m_snapGuide->setSnapDistance(distance);
}

void SnapGuideConfigWidget::updateControls()
{
    const KoSnapGuide::Strategies enabledSnapStrategies = m_snapGuide->enabledSnapStrategies();

    widget.orthogonalSnapGuide->setChecked(enabledSnapStrategies & KoSnapGuide::OrthogonalSnapping);
    widget.nodeSnapGuide->setChecked(enabledSnapStrategies & KoSnapGuide::NodeSnapping);
    widget.extensionSnapGuide->setChecked(enabledSnapStrategies & KoSnapGuide::ExtensionSnapping);
    widget.intersectionSnapGuide->setChecked(enabledSnapStrategies & KoSnapGuide::IntersectionSnapping);
    widget.boundingBoxSnapGuide->setChecked(enabledSnapStrategies & KoSnapGuide::BoundingBoxSnapping);
    widget.lineGuideSnapGuide->setChecked(enabledSnapStrategies & KoSnapGuide::GuideLineSnapping);

    widget.snapDistance->setValue(m_snapGuide->snapDistance());
}

void SnapGuideConfigWidget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    updateControls();
}
