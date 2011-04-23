/* This file is part of the KDE project
   Copyright (C) 2005-2008 Jarosław Staniek <staniek@kde.org>

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

#include "KexiCommandLinkButton.h"

#include <QStyle>
#include <QPaintEvent>
#include <QPainter>
#include <QStyleOptionButton>

class KexiCommandLinkButton::Private
{
public:
    Private() {}
    
};

KexiCommandLinkButton::KexiCommandLinkButton(QWidget * parent)
 : QCommandLinkButton(parent)
 , d(new Private)
{
}

KexiCommandLinkButton::KexiCommandLinkButton(const QString & text,
                                             QWidget * parent)
 : QCommandLinkButton(text, parent)
 , d(new Private)
{
}

KexiCommandLinkButton::KexiCommandLinkButton(const QString & text,
                                             const QString & description,
                                             QWidget * parent)
 : QCommandLinkButton(text, description, parent)
 , d(new Private)
{
}

KexiCommandLinkButton::~KexiCommandLinkButton()
{
    delete d;
}

void KexiCommandLinkButton::paintEvent(QPaintEvent* event)
{
    QCommandLinkButton::paintEvent(event);
    QPainter p(this);
    QStyleOptionButton option;
    option.initFrom(this);
    
    int margin = style()->pixelMetric(QStyle::PM_ButtonMargin, &option, this);
    option.rect.setX(option.rect.width() - margin * 2 - 10);
    style()->drawPrimitive(QStyle::PE_IndicatorArrowRight, &option, &p, this);
}

#include "KexiCommandLinkButton.moc"
