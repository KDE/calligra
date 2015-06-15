/*  This file is part of the KDE project
    Copyright (C) 2008 Matthias Kretz <kretz@kde.org>
    Copyright (C) 2008 Rafael Fernández López <ereslibre@kde.org>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) version 3.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.

*/

#include "KexiFadeWidgetEffect.h"
#include "KexiFadeWidgetEffect_p.h"

#include <QtCore/QEvent>
#include <QPaintEngine>
#include <QPainter>
#include <QStyle>

KexiFadeWidgetEffectPrivate::KexiFadeWidgetEffectPrivate(QWidget *_destWidget)
    : destWidget(_destWidget), disabled(false)
{
}

// Code from KFileItemDelegate (Author: Frederik Höglund)
// Fast transitions. Read:
// http://techbase.kde.org/Development/Tutorials/Graphics/Performance
// for further information on why not use setOpacity.
QPixmap KexiFadeWidgetEffectPrivate::transition(const QPixmap &from, const QPixmap &to, qreal amount) const
{
    const int value = int(0xff * amount);

    if (value == 0) {
        return from;
    }

    if (value == 1) {
        return to;
    }

    QColor color;
    color.setAlphaF(amount);

    // If the native paint engine supports Porter/Duff compositing and CompositionMode_Plus
    if (from.paintEngine()->hasFeature(QPaintEngine::PorterDuff) &&
            from.paintEngine()->hasFeature(QPaintEngine::BlendModes)) {
        QPixmap under = from;
        QPixmap over  = to;

        QPainter p;
        p.begin(&over);
        p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        p.fillRect(over.rect(), color);
        p.end();

        p.begin(&under);
        p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
        p.fillRect(under.rect(), color);
        p.setCompositionMode(QPainter::CompositionMode_Plus);
        p.drawPixmap(0, 0, over);
        p.end();

        return under;
    } else {
        // Fall back to using QRasterPaintEngine to do the transition.
        QImage under = from.toImage();
        QImage over  = to.toImage();

        QPainter p;
        p.begin(&over);
        p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
        p.fillRect(over.rect(), color);
        p.end();

        p.begin(&under);
        p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
        p.fillRect(under.rect(), color);
        p.setCompositionMode(QPainter::CompositionMode_Plus);
        p.drawImage(0, 0, over);
        p.end();

        return QPixmap::fromImage(under);
    }
}

KexiFadeWidgetEffect::KexiFadeWidgetEffect(QWidget *destWidget)
    : QWidget(destWidget ? destWidget->parentWidget() : 0),
      d(new KexiFadeWidgetEffectPrivate(destWidget))
{
    d->q = this;
    Q_ASSERT(destWidget && destWidget->parentWidget());
    if (!destWidget || !destWidget->parentWidget() || !destWidget->isVisible() ||
            !style()->styleHint(QStyle::SH_Widget_Animate, 0, this)) {
        d->disabled = true;
        hide();
        return;
    }
    setGeometry(QRect(destWidget->mapTo(parentWidget(), QPoint(0, 0)), destWidget->size()));
    d->oldPixmap = QPixmap::grabWidget(destWidget);
    d->timeLine.setFrameRange(0, 255);
    d->timeLine.setCurveShape(QTimeLine::EaseOutCurve);
    connect(&d->timeLine, SIGNAL(finished()), SLOT(finished()));
    connect(&d->timeLine, SIGNAL(frameChanged(int)), SLOT(repaint()));
    show();
}

KexiFadeWidgetEffect::~KexiFadeWidgetEffect()
{
    delete d;
}

void KexiFadeWidgetEffect::finished()
{
    d->destWidget->setUpdatesEnabled(false);
    hide();
    deleteLater();
    d->destWidget->setUpdatesEnabled(true);
}

void KexiFadeWidgetEffect::start(int duration)
{
    if (d->disabled) {
        deleteLater();
        return;
    }
    d->newPixmap = QPixmap::grabWidget(d->destWidget);
    d->timeLine.setDuration(duration);
    d->timeLine.start();
}

void KexiFadeWidgetEffect::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.drawPixmap(rect(), d->transition(d->oldPixmap, d->newPixmap, d->timeLine.currentValue()));
    p.end();
}
