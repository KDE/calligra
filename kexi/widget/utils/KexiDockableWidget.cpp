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

KexiDockableWidget::KexiDockableWidget(QWidget* parent)
        : QWidget(parent)
{
}

KexiDockableWidget::~KexiDockableWidget()
{
}

QSize KexiDockableWidget::sizeHint() const
{
    return m_hint.isValid() ? m_hint : QWidget::sizeHint();
}

void KexiDockableWidget::setSizeHint(const QSize& size)
{
    m_hint = size;
}

QWidget* KexiDockableWidget::widget() const
{
    return m_widget;
}

void KexiDockableWidget::setWidget(QWidget* widget)
{
    if (m_widget || widget == this)
        return;
    m_widget = widget;
    m_widget->setParent(this);
    QVBoxLayout *lyr = new QVBoxLayout(this);
    KexiUtils::setMargins(lyr, 0);
    lyr->addWidget(m_widget);
}
