/* This file is part of the KDE project
   Copyright (C) 2009 Jarosław Staniek <staniek@kde.org>

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

#include "KexiDockableWidget.h"
#include <kexiutils/utils.h>
#include <QVBoxLayout>


class KexiDockableWidget::Private
{
public:
    Private() {}

    QPointer<QWidget> widget;
    QSize hint;
};


KexiDockableWidget::KexiDockableWidget(QWidget* parent)
        : QWidget(parent)
	, d(new Private)
{
}

KexiDockableWidget::~KexiDockableWidget()
{
    delete d;
}

QSize KexiDockableWidget::sizeHint() const
{
    return d->hint.isValid() ? d->hint : QWidget::sizeHint();
}

void KexiDockableWidget::setSizeHint(const QSize& size)
{
    d->hint = size;
}

QWidget* KexiDockableWidget::widget() const
{
    return d->widget;
}

void KexiDockableWidget::setWidget(QWidget* widget)
{
    if (d->widget || widget == this)
        return;
    d->widget = widget;
    d->widget->setParent(this);
    QVBoxLayout *lyr = new QVBoxLayout(this);
    KexiUtils::setMargins(lyr, 0);
    lyr->addWidget(d->widget);
}
