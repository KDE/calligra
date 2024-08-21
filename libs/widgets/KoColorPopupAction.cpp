/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2007 Fredy Yanardi <fyanardi@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "KoColorPopupAction.h"

#include "KoCheckerBoardPainter.h"
#include "KoColorSetWidget.h"
#include "KoColorSlider.h"
#include "KoResourceServer.h"
#include "KoResourceServerProvider.h"
#include "KoTriangleColorSelector.h"
#include <KoColor.h>
#include <KoColorSpaceRegistry.h>

#include <KLocalizedString>
#include <WidgetsDebug.h>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QMenu>
#include <QPainter>
#include <QToolButton>
#include <QWidgetAction>

class KoColorPopupAction::KoColorPopupActionPrivate
{
public:
    KoColorPopupActionPrivate()
        : colorSetWidget(nullptr)
        , colorChooser(nullptr)
        , opacitySlider(nullptr)
        , menu(nullptr)
        , checkerPainter(4)
        , showFilter(true)
        , applyMode(true)
        , firstTime(true)
    {
    }

    ~KoColorPopupActionPrivate()
    {
        delete colorSetWidget;
        delete colorChooser;
        delete opacitySlider;
        delete menu;
    }

    KoColor currentColor;
    KoColor buddyColor;

    KoColorSetWidget *colorSetWidget;
    KoTriangleColorSelector *colorChooser;
    KoColorSlider *opacitySlider;
    QMenu *menu;
    KoCheckerBoardPainter checkerPainter;
    bool showFilter;
    bool applyMode;

    bool firstTime;
};

KoColorPopupAction::KoColorPopupAction(QObject *parent)
    : QAction(parent)
    , d(new KoColorPopupActionPrivate())
{
    d->menu = new QMenu();
    QWidget *widget = new QWidget(d->menu);
    QWidgetAction *wdgAction = new QWidgetAction(d->menu);
    d->colorSetWidget = new KoColorSetWidget(widget);

    d->colorChooser = new KoTriangleColorSelector(widget);
    // prevent mouse release on color selector from closing popup
    d->colorChooser->setAttribute(Qt::WA_NoMousePropagation);
    d->opacitySlider = new KoColorSlider(Qt::Vertical, widget);
    d->opacitySlider->setFixedWidth(25);
    d->opacitySlider->setRange(0, 255);
    d->opacitySlider->setValue(255);
    d->opacitySlider->setToolTip(i18n("Opacity"));

    QGridLayout *layout = new QGridLayout(widget);
    layout->addWidget(d->colorSetWidget, 0, 0, 1, -1);
    layout->addWidget(d->colorChooser, 1, 0);
    layout->addWidget(d->opacitySlider, 1, 1);
    layout->setContentsMargins(4, 4, 4, 4);

    wdgAction->setDefaultWidget(widget);
    d->menu->addAction(wdgAction);
    setMenu(d->menu);
    new QHBoxLayout(d->menu);
    d->menu->layout()->addWidget(widget);
    d->menu->layout()->setContentsMargins({});

    connect(this, &QAction::triggered, this, &KoColorPopupAction::emitColorChanged);

    connect(d->colorSetWidget, &KoColorSetWidget::colorChanged, this, &KoColorPopupAction::colorWasSelected);

    connect(d->colorChooser, &KoTriangleColorSelector::colorChanged, this, &KoColorPopupAction::colorWasEdited);
    connect(d->opacitySlider, &QAbstractSlider::valueChanged, this, &KoColorPopupAction::opacityWasChanged);
}

KoColorPopupAction::~KoColorPopupAction()
{
    delete d;
}

void KoColorPopupAction::setCurrentColor(const KoColor &color)
{
    d->colorChooser->setColor(color);

    KoColor minColor(color);
    d->currentColor = minColor;

    KoColor maxColor(color);
    minColor.setOpacity(OPACITY_TRANSPARENT_U8);
    maxColor.setOpacity(OPACITY_OPAQUE_U8);
    d->opacitySlider->blockSignals(true);
    d->opacitySlider->setColors(minColor, maxColor);
    d->opacitySlider->setValue(color.opacityU8());
    d->opacitySlider->blockSignals(false);

    updateIcon();
}

void KoColorPopupAction::setCurrentColor(const QColor &_color)
{
#ifndef NDEBUG
    if (!_color.isValid()) {
        warnWidgets << "Invalid color given, defaulting to black";
    }
#endif
    const QColor color(_color.isValid() ? _color : QColor(0, 0, 0, 255));
    setCurrentColor(KoColor(color, KoColorSpaceRegistry::instance()->rgb8()));
}

QColor KoColorPopupAction::currentColor() const
{
    return d->currentColor.toQColor();
}

KoColor KoColorPopupAction::currentKoColor() const
{
    return d->currentColor;
}

void KoColorPopupAction::updateIcon()
{
    QSize iconSize;
    QToolButton *toolButton = dynamic_cast<QToolButton *>(parentWidget());
    if (toolButton) {
        iconSize = QSize(toolButton->iconSize());
    } else {
        iconSize = QSize(16, 16);
    }

    // This must be a QImage, as drawing to a QPixmap outside the
    // UI thread will cause sporadic crashes.
    QImage pm;

    if (icon().isNull()) {
        d->applyMode = false;
    }

    if (d->applyMode) {
        pm = icon().pixmap(iconSize).toImage();
        if (pm.isNull()) {
            pm = QImage(iconSize, QImage::Format_ARGB32_Premultiplied);
            pm.fill(Qt::transparent);
        }
        QPainter p(&pm);
        p.fillRect(0, iconSize.height() - 4, iconSize.width(), 4, d->currentColor.toQColor());
        p.end();
    } else {
        pm = QImage(iconSize, QImage::Format_ARGB32_Premultiplied);
        pm.fill(Qt::transparent);
        QPainter p(&pm);
        d->checkerPainter.paint(p, QRect(QPoint(), iconSize));
        p.fillRect(0, 0, iconSize.width(), iconSize.height(), d->currentColor.toQColor());
        p.end();
    }
    setIcon(QIcon(QPixmap::fromImage(pm)));
}

void KoColorPopupAction::emitColorChanged()
{
    Q_EMIT colorChanged(d->currentColor);
}

void KoColorPopupAction::colorWasSelected(const KoColor &color, bool final)
{
    d->currentColor = color;
    if (final) {
        menu()->hide();
        emitColorChanged();
    }
    updateIcon();
}

void KoColorPopupAction::colorWasEdited(const KoColor &color)
{
    d->currentColor = color;
    quint8 opacity = d->opacitySlider->value();
    d->currentColor.setOpacity(opacity);

    KoColor minColor = d->currentColor;
    minColor.setOpacity(OPACITY_TRANSPARENT_U8);
    KoColor maxColor = minColor;
    maxColor.setOpacity(OPACITY_OPAQUE_U8);

    d->opacitySlider->setColors(minColor, maxColor);

    emitColorChanged();

    updateIcon();
}

void KoColorPopupAction::opacityWasChanged(int opacity)
{
    d->currentColor.setOpacity(quint8(opacity));

    emitColorChanged();
}

void KoColorPopupAction::slotTriggered(bool)
{
    if (d->firstTime) {
        KoResourceServer<KoColorSet> *srv = KoResourceServerProvider::instance()->paletteServer(false);
        QList<KoColorSet *> palettes = srv->resources();
        if (!palettes.empty()) {
            d->colorSetWidget->setColorSet(palettes.first());
        }
        d->firstTime = false;
    }
}
