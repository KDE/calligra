/*  This file is part of the KDE libraries
    SPDX-FileCopyrightText: 2004 Ariya Hidayat <ariya@kde.org>
    SPDX-FileCopyrightText: 2006 Peter Simonsson <peter.simonsson@gmail.com>
    SPDX-FileCopyrightText: 2006-2007 C. Boemann <cbo@boemann.dk>

    SPDX-License-Identifier: LGPL-2.0-only
*/
#include "KoZoomAction.h"
#include "KoZoomMode.h"
#include "KoZoomWidget.h"

#include <KoIcon.h>

#include <QButtonGroup>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QLocale>
#include <QMenu>
#include <QSlider>
#include <QStatusBar>
#include <QString>
#include <QStringList>
#include <QToolButton>

#include <KLocalizedString>
#include <WidgetsDebug.h>

#include <algorithm>
#include <math.h>

class Q_DECL_HIDDEN KoZoomAction::Private
{
public:
    Private(KoZoomAction *_parent)
        : parent(_parent)
        , minimumZoomValue(-1)
        , maximumZoomValue(-1)
    {
    }

    KoZoomAction *parent;

    KoZoomMode::Modes zoomModes;
    QList<qreal> sliderLookup;

    qreal effectiveZoom;

    KoZoomAction::SpecialButtons specialButtons;

    QList<qreal> generateSliderZoomLevels() const;
    QList<qreal> filterMenuZoomLevels(const QList<qreal> &zoomLevels) const;

    qreal minimumZoomValue;
    qreal maximumZoomValue;
};

QList<qreal> KoZoomAction::Private::generateSliderZoomLevels() const
{
    QList<qreal> zoomLevels;

    qreal defaultZoomStep = sqrt(2.0);

    zoomLevels << 0.25 / 2.0;
    zoomLevels << 0.25 / 1.5;
    zoomLevels << 0.25;
    zoomLevels << 1.0 / 3.0;
    zoomLevels << 0.5;
    zoomLevels << 2.0 / 3.0;
    zoomLevels << 1.0;

    for (qreal zoom = zoomLevels.first() / defaultZoomStep; zoom > parent->minimumZoom(); zoom /= defaultZoomStep) {
        zoomLevels.prepend(zoom);
    }

    for (qreal zoom = zoomLevels.last() * defaultZoomStep; zoom < parent->maximumZoom(); zoom *= defaultZoomStep) {
        zoomLevels.append(zoom);
    }

    return zoomLevels;
}

QList<qreal> KoZoomAction::Private::filterMenuZoomLevels(const QList<qreal> &zoomLevels) const
{
    QList<qreal> filteredZoomLevels;

    foreach (qreal zoom, zoomLevels) {
        if (zoom >= 0.2 && zoom <= 10) {
            filteredZoomLevels << zoom;
        }
    }

    return filteredZoomLevels;
}

KoZoomAction::KoZoomAction(KoZoomMode::Modes zoomModes, const QString &text, QObject *parent)
    : KSelectAction(text, parent)
    , d(new Private(this))
{
    d->zoomModes = zoomModes;
    d->specialButtons = {};
    setIcon(koIcon("zoom-original"));
    setEditable(true);
    setMaxComboViewCount(15);

    d->sliderLookup = d->generateSliderZoomLevels();

    d->effectiveZoom = 1.0;
    regenerateItems(d->effectiveZoom, true);

    connect(this, SIGNAL(triggered(QString)), SLOT(triggered(QString)));
}

KoZoomAction::~KoZoomAction()
{
    delete d;
}

qreal KoZoomAction::effectiveZoom() const
{
    return d->effectiveZoom;
}

void KoZoomAction::setZoom(qreal zoom)
{
    setEffectiveZoom(zoom);
    regenerateItems(d->effectiveZoom, true);
}

void KoZoomAction::triggered(const QString &text)
{
    QString zoomString = text;
    zoomString = zoomString.remove('&');

    KoZoomMode::Mode mode = KoZoomMode::toMode(zoomString);
    int zoom = 0;

    if (mode == KoZoomMode::ZOOM_CONSTANT) {
        bool ok;
        QRegularExpression regexp(".*(\\d+).*"); // "Captured" non-empty sequence of digits
        QRegularExpressionMatch match;
        int pos = zoomString.indexOf(regexp, 0, &match);

        if (pos > -1) {
            zoom = match.captured(1).toInt(&ok);

            if (!ok) {
                zoom = 0;
            }
        }
    }

    Q_EMIT zoomChanged(mode, zoom / 100.0);
}

void KoZoomAction::setZoomModes(KoZoomMode::Modes zoomModes)
{
    d->zoomModes = zoomModes;
    regenerateItems(d->effectiveZoom);
}

void KoZoomAction::regenerateItems(const qreal zoom, bool asCurrent)
{
    QList<qreal> zoomLevels = d->filterMenuZoomLevels(d->sliderLookup);

    if (!zoomLevels.contains(zoom))
        zoomLevels << zoom;

    std::sort(zoomLevels.begin(), zoomLevels.end());

    // update items with new sorted zoom values
    QStringList values;
    if (d->zoomModes & KoZoomMode::ZOOM_WIDTH) {
        values << KoZoomMode::toString(KoZoomMode::ZOOM_WIDTH);
    }
    if (d->zoomModes & KoZoomMode::ZOOM_TEXT) {
        values << KoZoomMode::toString(KoZoomMode::ZOOM_TEXT);
    }
    if (d->zoomModes & KoZoomMode::ZOOM_PAGE) {
        values << KoZoomMode::toString(KoZoomMode::ZOOM_PAGE);
    }

    foreach (qreal value, zoomLevels) {
        const qreal valueInPercent = value * 100;
        const int precision = (value > 10.0) ? 0 : 1;

        values << i18n("%1%", QLocale().toString(valueInPercent, 'f', precision));
    }

    setItems(values);

    Q_EMIT zoomLevelsChanged(values);

    if (asCurrent) {
        const qreal zoomInPercent = zoom * 100;
        // TODO: why zoomInPercent and not zoom here? different from above
        const int precision = (zoomInPercent > 10.0) ? 0 : 1;

        const QString valueString = i18n("%1%", QLocale().toString(zoomInPercent, 'f', precision));

        setCurrentAction(valueString);

        Q_EMIT currentZoomLevelChanged(valueString);
    }
}

void KoZoomAction::sliderValueChanged(int value)
{
    setZoom(d->sliderLookup[value]);

    Q_EMIT zoomChanged(KoZoomMode::ZOOM_CONSTANT, d->sliderLookup[value]);
}

qreal KoZoomAction::nextZoomLevel() const
{
    const qreal eps = 1e-5;
    int i = 0;
    while (d->effectiveZoom > d->sliderLookup[i] - eps && i < d->sliderLookup.size() - 1)
        i++;

    return qMax(d->effectiveZoom, d->sliderLookup[i]);
}

qreal KoZoomAction::prevZoomLevel() const
{
    const qreal eps = 1e-5;
    int i = d->sliderLookup.size() - 1;
    while (d->effectiveZoom < d->sliderLookup[i] + eps && i > 0)
        i--;

    return qMin(d->effectiveZoom, d->sliderLookup[i]);
}

void KoZoomAction::zoomIn()
{
    qreal zoom = nextZoomLevel();

    if (zoom > d->effectiveZoom) {
        setZoom(zoom);
        Q_EMIT zoomChanged(KoZoomMode::ZOOM_CONSTANT, d->effectiveZoom);
    }
}

void KoZoomAction::zoomOut()
{
    qreal zoom = prevZoomLevel();

    if (zoom < d->effectiveZoom) {
        setZoom(zoom);
        Q_EMIT zoomChanged(KoZoomMode::ZOOM_CONSTANT, d->effectiveZoom);
    }
}

QWidget *KoZoomAction::createWidget(QWidget *parent)
{
    KoZoomWidget *zoomWidget = new KoZoomWidget(parent, d->specialButtons, d->sliderLookup.size() - 1);
    connect(this, &KoZoomAction::zoomLevelsChanged, zoomWidget, &KoZoomWidget::setZoomLevels);
    connect(this, &KoZoomAction::currentZoomLevelChanged, zoomWidget, &KoZoomWidget::setCurrentZoomLevel);
    connect(this, &KoZoomAction::sliderChanged, zoomWidget, &KoZoomWidget::setSliderValue);
    connect(this, &KoZoomAction::aspectModeChanged, zoomWidget, &KoZoomWidget::setAspectMode);
    connect(zoomWidget, &KoZoomWidget::sliderValueChanged, this, &KoZoomAction::sliderValueChanged);
    connect(zoomWidget, SIGNAL(zoomLevelChanged(QString)), this, SLOT(triggered(QString)));
    connect(zoomWidget, &KoZoomWidget::aspectModeChanged, this, &KoZoomAction::aspectModeChanged);
    connect(zoomWidget, &KoZoomWidget::zoomedToSelection, this, &KoZoomAction::zoomedToSelection);
    connect(zoomWidget, &KoZoomWidget::zoomedToAll, this, &KoZoomAction::zoomedToAll);
    regenerateItems(d->effectiveZoom, true);
    syncSliderWithZoom();
    return zoomWidget;
}

void KoZoomAction::setEffectiveZoom(qreal zoom)
{
    if (d->effectiveZoom == zoom)
        return;

    zoom = clampZoom(zoom);
    d->effectiveZoom = zoom;
    syncSliderWithZoom();
}

void KoZoomAction::setSelectedZoomMode(KoZoomMode::Mode mode)
{
    QString modeString(KoZoomMode::toString(mode));
    setCurrentAction(modeString);

    Q_EMIT currentZoomLevelChanged(modeString);
}

void KoZoomAction::setSpecialButtons(SpecialButtons buttons)
{
    d->specialButtons = buttons;
}

void KoZoomAction::setAspectMode(bool status)
{
    Q_EMIT aspectModeChanged(status);
}

void KoZoomAction::syncSliderWithZoom()
{
    const qreal eps = 1e-5;
    int i = d->sliderLookup.size() - 1;
    while (d->effectiveZoom < d->sliderLookup[i] + eps && i > 0)
        i--;

    Q_EMIT sliderChanged(i);
}

qreal KoZoomAction::minimumZoom()
{
    if (d->minimumZoomValue < 0) {
        return KoZoomMode::minimumZoom();
    }
    return d->minimumZoomValue;
}

qreal KoZoomAction::maximumZoom()
{
    if (d->maximumZoomValue < 0) {
        return KoZoomMode::maximumZoom();
    }
    return d->maximumZoomValue;
}

qreal KoZoomAction::clampZoom(qreal zoom)
{
    return qMin(maximumZoom(), qMax(minimumZoom(), zoom));
}

void KoZoomAction::setMinimumZoom(qreal zoom)
{
    Q_ASSERT(zoom > 0.0f);
    KoZoomMode::setMinimumZoom(zoom);
    d->minimumZoomValue = zoom;
    d->generateSliderZoomLevels();
    d->sliderLookup = d->generateSliderZoomLevels();
    regenerateItems(d->effectiveZoom, true);
    syncSliderWithZoom();
}

void KoZoomAction::setMaximumZoom(qreal zoom)
{
    Q_ASSERT(zoom > 0.0f);
    KoZoomMode::setMaximumZoom(zoom);
    d->maximumZoomValue = zoom;
    d->sliderLookup = d->generateSliderZoomLevels();
    regenerateItems(d->effectiveZoom, true);
    syncSliderWithZoom();
}
