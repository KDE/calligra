/* This file is part of the KDE project
 * Copyright (c) 2013 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KoColorPopupButton.h"

#include <QResizeEvent>
#include <QStyle>
#include <QStyleOptionToolButton>

#include "kdebug.h"

KoColorPopupButton::KoColorPopupButton(QWidget* parent)
:QToolButton(parent)
{
    m_iconSize = QSize(16,16);
}

KoColorPopupButton::~KoColorPopupButton()
{
}

void KoColorPopupButton::resizeEvent(QResizeEvent* e)
{
    QStyleOptionToolButton opt;
    initStyleOption(&opt);

    kDebug(30006) << "Size" << e->size();
    kDebug(30006) << "iconSize" << style()->sizeFromContents(QStyle::CT_ToolButton, &opt, iconSize(), this);

    QSize rect = style()->sizeFromContents(QStyle::CT_ToolButton, &opt, iconSize(), this);
    int iconWidth = m_iconSize.width() + rect.width() - e->size().width();

    if (iconWidth != m_iconSize.width()) {
        m_iconSize = QSize(iconWidth, 16);
        setIconSize(m_iconSize);
    }
}

#include <KoColorPopupButton.moc>
