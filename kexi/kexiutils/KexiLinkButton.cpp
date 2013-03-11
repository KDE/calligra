/* This file is part of the KDE project
   Copyright (C) 2012 Jarosław Staniek <staniek@kde.org>

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

#include "KexiLinkButton.h"
#include "utils.h"

#include <QCursor>
#include <QStyle>
#include <QStyleOptionButton>
#include <QEvent>
 
class KexiLinkButton::Private
{
public:
    Private()
     : usesForegroundColor(false)
    {
    }
    bool usesForegroundColor;
    //! Used to remember the orig icon so setUsesForegroundColor(false) is possible
    KIcon origIcon;
};

KexiLinkButton::KexiLinkButton(QWidget* parent)
 : KPushButton(parent), d(new Private)
{
    init();
}

KexiLinkButton::KexiLinkButton(const KIcon &icon, QWidget* parent)
 : KPushButton(icon, QString(), parent), d(new Private)
{
    init();
}

KexiLinkButton::KexiLinkButton(const QPixmap &pixmap, QWidget* parent)
 : KPushButton(KIcon(pixmap), QString(), parent), d(new Private)
{
    init();
}

KexiLinkButton::KexiLinkButton(const KGuiItem &item, QWidget *parent)
 : KPushButton(item, parent), d(new Private)
{
    init();
}

KexiLinkButton::~KexiLinkButton()
{
    delete d;
}

void KexiLinkButton::init()
{
    setFlat(true);
    setText(QString());
    setCursor(QCursor(Qt::PointingHandCursor));
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setForegroundRole(QPalette::Text);
    QStyleOptionButton option;
    option.initFrom(this);
    int m = style()->pixelMetric(QStyle::PM_ButtonMargin, &option, this);
    setFixedSize(iconSize() + QSize(m*2, m*2));
    d->origIcon = KIcon(icon());
}

void KexiLinkButton::setUsesForegroundColor(bool set)
{
    if (d->usesForegroundColor == set)
        return;
    d->usesForegroundColor = set;
    setIcon(d->origIcon);
}

bool KexiLinkButton::usesForegroundColor() const
{
    return d->usesForegroundColor;
}

void KexiLinkButton::changeEvent(QEvent* event)
{
    switch (event->type()) {
    case QEvent::EnabledChange:
    case QEvent::PaletteChange:
        updateIcon(KIcon(icon()));
        break;
    default:;
    }
    KPushButton::changeEvent(event);
}

void KexiLinkButton::updateIcon(const KIcon &icon)
{
    if (!d->usesForegroundColor)
        return;
    QColor c(palette().color(foregroundRole()));
    QPixmap pixmap(icon.pixmap(iconSize()));
    KexiUtils::replaceColors(&pixmap, c);
    KPushButton::setIcon(KIcon(pixmap));
}

void KexiLinkButton::setIcon(const KIcon &icon)
{
    d->origIcon = icon;
    if (d->usesForegroundColor) {
        updateIcon(d->origIcon);
    }
    else {
        KPushButton::setIcon(d->origIcon);
    }
}
