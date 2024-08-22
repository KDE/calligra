/*
    SPDX-FileCopyrightText: 2004 Ariya Hidayat <ariya@kde.org>
    SPDX-FileCopyrightText: 2006 Peter Simonsson <peter.simonsson@gmail.com>
    SPDX-FileCopyrightText: 2006-2007 C. Boemann <cbo@boemann.dk>
    SPDX-FileCopyrightText: 2014 Sven Langkamp <sven.langkamp@gmail.com>

    SPDX-License-Identifier: LGPL-2.0-only
*/

#include "KoZoomWidget.h"

#include <QBoxLayout>
#include <QSlider>
#include <QToolButton>

#include <KLocalizedString>

#include "KoIcon.h"
#include "KoZoomInput.h"

class KoZoomWidget::Private
{
public:
    Private()
        : slider(nullptr)
        , input(nullptr)
        , aspectButton(nullptr)
    {
    }

    QSlider *slider;
    KoZoomInput *input;
    QToolButton *aspectButton;

    qreal effectiveZoom;
};

KoZoomWidget::KoZoomWidget(QWidget *parent, KoZoomAction::SpecialButtons specialButtons, int maxZoom)
    : QWidget(parent)
    , d(new Private)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    // layout->setSizeConstraint(QLayout::SetFixedSize);
    layout->setContentsMargins({});
    layout->setSpacing(0);

    d->input = new KoZoomInput(this);
    connect(d->input, &KoZoomInput::zoomLevelChanged, this, &KoZoomWidget::zoomLevelChanged);
    layout->addWidget(d->input);

    d->slider = new QSlider(Qt::Horizontal);
    d->slider->setToolTip(i18n("Zoom"));
    d->slider->setMinimum(0);
    d->slider->setMaximum(maxZoom);
    d->slider->setValue(0);
    d->slider->setSingleStep(1);
    d->slider->setPageStep(1);
    d->slider->setMinimumWidth(80);
    layout->addWidget(d->slider);
    layout->setStretch(1, 1);

    if (specialButtons & KoZoomAction::AspectMode) {
        d->aspectButton = new QToolButton(this);
        d->aspectButton->setIcon(koIcon("zoom-pixels"));
        d->aspectButton->setIconSize(QSize(16, 16));
        d->aspectButton->setCheckable(true);
        d->aspectButton->setChecked(true);
        d->aspectButton->setAutoRaise(true);
        d->aspectButton->setToolTip(i18n("Use same aspect as pixels"));
        connect(d->aspectButton, &QAbstractButton::toggled, this, &KoZoomWidget::aspectModeChanged);
        layout->addWidget(d->aspectButton);
    }
    if (specialButtons & KoZoomAction::ZoomToSelection) {
        QToolButton *zoomToSelectionButton = new QToolButton(this);
        zoomToSelectionButton->setIcon(koIcon("zoom-select"));
        zoomToSelectionButton->setIconSize(QSize(16, 16));
        zoomToSelectionButton->setAutoRaise(true);
        zoomToSelectionButton->setToolTip(i18n("Zoom to Selection"));
        connect(zoomToSelectionButton, &QAbstractButton::clicked, this, &KoZoomWidget::zoomedToSelection);
        layout->addWidget(zoomToSelectionButton);
    }
    if (specialButtons & KoZoomAction::ZoomToAll) {
        QToolButton *zoomToAllButton = new QToolButton(this);
        zoomToAllButton->setIcon(koIcon("zoom-draw"));
        zoomToAllButton->setIconSize(QSize(16, 16));
        zoomToAllButton->setAutoRaise(true);
        zoomToAllButton->setToolTip(i18n("Zoom to All"));
        connect(zoomToAllButton, &QAbstractButton::clicked, this, &KoZoomWidget::zoomedToAll);
        layout->addWidget(zoomToAllButton);
    }
    connect(d->slider, &QAbstractSlider::valueChanged, this, &KoZoomWidget::sliderValueChanged);
}

KoZoomWidget::~KoZoomWidget() = default;

void KoZoomWidget::setZoomLevels(const QStringList &values)
{
    d->input->setZoomLevels(values);
}

void KoZoomWidget::setCurrentZoomLevel(const QString &valueString)
{
    d->input->setCurrentZoomLevel(valueString);
}

void KoZoomWidget::setSliderValue(int value)
{
    d->slider->blockSignals(true);
    d->slider->setValue(value);
    d->slider->blockSignals(false);
}

void KoZoomWidget::setAspectMode(bool status)
{
    if (d->aspectButton && d->aspectButton->isChecked() != status) {
        d->aspectButton->blockSignals(true);
        d->aspectButton->setChecked(status);
        d->aspectButton->blockSignals(false);
    }
}
