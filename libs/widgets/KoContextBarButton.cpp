// vim: set tabstop=4 shiftwidth=4 noexpandtab:
/* This file is part of the KDE project
SPDX-FileCopyrightText: 2011 Aurélien Gâteau <agateau@kde.org>
SPDX-FileCopyrightText: 2011 Paul Mendez <paulestebanms@gmail.com>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KoContextBarButton.h"

// Qt
#include <QApplication>
#include <QIcon>
#include <QPainterPath>
#include <QStyleOptionToolButton>
#include <QStylePainter>

/** How lighter is the border of context bar buttons */
const int CONTEXTBAR_BORDER_LIGHTNESS = 140;

/** How darker is the background of context bar buttons */
const int CONTEXTBAR_BACKGROUND_DARKNESS = 80;

/** How lighter are context bar buttons when under mouse */
const int CONTEXTBAR_MOUSEOVER_LIGHTNESS = 120;

/** Radius of ContextBarButtons */
const int CONTEXTBAR_RADIUS = 50;

KoContextBarButton::KoContextBarButton(const QString &iconName, QWidget *parent)
    : QToolButton(parent)
    , m_isHovered(false)
    , m_fadingValue(0)
    , m_fadingTimeLine(nullptr)
{
    const int size = QApplication::style()->pixelMetric(QStyle::PM_ButtonIconSize);
    setIconSize(QSize(size, size));
    setAutoRaise(true);
    setIcon(QIcon::fromTheme(iconName));
}

void KoContextBarButton::paintEvent(QPaintEvent *)
{
    QStylePainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);

    const QColor bgColor = palette().color(QPalette::Highlight);
    QColor color = bgColor.darker(CONTEXTBAR_BACKGROUND_DARKNESS);
    QColor borderColor = bgColor.lighter(CONTEXTBAR_BORDER_LIGHTNESS);

    if (opt.state & QStyle::State_MouseOver && opt.state & QStyle::State_Enabled) {
        color = color.lighter(CONTEXTBAR_MOUSEOVER_LIGHTNESS);
        borderColor = borderColor.lighter(CONTEXTBAR_MOUSEOVER_LIGHTNESS);
    }

    const QRectF rectF = QRectF(opt.rect).adjusted(0.5, 0.5, -0.5, -0.5);
    QPainterPath path;
    path.addRoundedRect(rectF, CONTEXTBAR_RADIUS, CONTEXTBAR_RADIUS, Qt::RelativeSize);

    if (m_fadingValue < 255) {
        color.setAlpha(m_fadingValue);
    }

    // Background
    painter.fillPath(path, color);

    if (opt.state & QStyle::State_Raised && opt.state & QStyle::State_Enabled) {
        // Bottom shadow
        QLinearGradient gradient(rectF.bottomLeft(), rectF.bottomLeft() - QPoint(0, 5));
        gradient.setColorAt(0, QColor::fromHsvF(0, 0, 0, .3));
        gradient.setColorAt(1, Qt::transparent);
        painter.fillPath(path, gradient);

        // Left shadow
        gradient.setFinalStop(rectF.bottomLeft() + QPoint(3, 0));
        painter.fillPath(path, gradient);
    } else {
        // Top shadow
        QLinearGradient gradient(rectF.topLeft(), rectF.topLeft() + QPoint(0, 5));
        gradient.setColorAt(0, QColor::fromHsvF(0, 0, 0, .3));
        gradient.setColorAt(1, Qt::transparent);
        painter.fillPath(path, gradient);

        // Left shadow
        gradient.setFinalStop(rectF.topLeft() + QPoint(5, 0));
        painter.fillPath(path, gradient);
    }

    // Border
    painter.setPen(QPen(borderColor, 0));
    painter.drawPath(path);

    // Content
    painter.drawControl(QStyle::CE_ToolButtonLabel, opt);
}

void KoContextBarButton::startFading()
{
    Q_ASSERT(!m_fadingTimeLine);

    const int duration = 300;

    m_fadingTimeLine = new QTimeLine(duration, this);
    connect(m_fadingTimeLine, &QTimeLine::frameChanged, this, &KoContextBarButton::setFadingValue);
    m_fadingTimeLine->setFrameRange(0, 255);
    m_fadingTimeLine->start();
    m_fadingValue = 0;
}

void KoContextBarButton::stopFading()
{
    if (m_fadingTimeLine) {
        m_fadingTimeLine->stop();
        delete m_fadingTimeLine;
        m_fadingTimeLine = nullptr;
    }
    m_fadingValue = 0;
}

void KoContextBarButton::enterEvent(QEnterEvent *event)
{
    QToolButton::enterEvent(event);

    // if the mouse cursor is above the selection toggle, display
    // it immediately without fading timer
    m_isHovered = true;
    if (m_fadingTimeLine) {
        m_fadingTimeLine->stop();
    }
    m_fadingValue = 255;
    update();
}

void KoContextBarButton::leaveEvent(QEvent *event)
{
    QToolButton::leaveEvent(event);

    m_isHovered = false;
    update();
}

void KoContextBarButton::setFadingValue(int value)
{
    m_fadingValue = value;
    if (m_fadingValue >= 255) {
        Q_ASSERT(m_fadingTimeLine);
        m_fadingTimeLine->stop();
    }
    update();
}

void KoContextBarButton::showEvent(QShowEvent *event)
{
    stopFading();
    startFading();
    QToolButton::showEvent(event);
}

void KoContextBarButton::hideEvent(QHideEvent *event)
{
    stopFading();
    QToolButton::hideEvent(event);
}
