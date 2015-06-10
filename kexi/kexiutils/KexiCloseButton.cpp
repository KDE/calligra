/* This file is part of the KDE project
   Copyright (C) 2014 Jarosław Staniek <staniek@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KexiCloseButton.h"
#include "utils.h"

#include <KIcon>
#include <KStandardGuiItem>

#include <QApplication>
#include <QStyle>
#include <QStyleOptionButton>
#include <QPainter>
#include <QPaintEvent>

class KexiCloseButton::Private
{
public:
    Private()
    {
    }
    bool dummy;
};

KexiCloseButton::KexiCloseButton(QWidget* parent)
 : QToolButton(parent), d(new Private)
{
    init();
}

KexiCloseButton::~KexiCloseButton()
{
    delete d;
}

void KexiCloseButton::init()
{
    setToolTip(KStandardGuiItem::close().plainText());
    setAutoRaise(true);
    setText(QString());
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFocusPolicy(Qt::NoFocus);
    setMarginEnabled(true);
}

void KexiCloseButton::setMarginEnabled(bool set)
{
    QStyleOptionButton option;
    option.initFrom(this);
    int m = set ? style()->pixelMetric(QStyle::PM_ButtonMargin, &option, this) : 0;
    const int iconSize = style()->pixelMetric(QStyle::PM_SmallIconSize, &option, this);
    setFixedSize(QSize(iconSize, iconSize) + QSize(m*2, m*2));
    update();
}

void KexiCloseButton::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    if (style()->objectName() != "breeze" && QApplication::style()->objectName() != "breeze") {
        // Draw frames. Breeze's close button has no frames.
        QToolButton::paintEvent(e);
    }
    QStyleOptionButton option;
    option.initFrom(this);
    KIcon icon(style()->standardIcon(QStyle::SP_TitleBarCloseButton, &option, this));
    QPainter p(this);
    const int metric = style()->pixelMetric(QStyle::PM_SmallIconSize, &option, this);
    QSize iconSize(metric, metric);
    const QSize margin = (size() - iconSize) / 2;
    QRect iconRect(QPoint(margin.width(), margin.height()), iconSize);
    const bool enabled(option.state & QStyle::State_Enabled);
    QIcon::Mode mode;
    if (option.state & QStyle::State_MouseOver) {
        mode = QIcon::Active;
    }
    else if (enabled) {
        mode = QIcon::Normal;
    }
    else {
        mode = QIcon::Disabled;
    }
    const QIcon::State iconState((isDown() & Qt::LeftButton) ? QIcon::On : QIcon::Off);
    const QPixmap pixmap = icon.pixmap(iconSize, mode, iconState);
    style()->drawItemPixmap(&p, iconRect, Qt::AlignCenter, pixmap);
}

#include "KexiCloseButton.moc"
