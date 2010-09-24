/* This file is part of the KDE project
   Copyright (C) 2006-2010 Jarosław Staniek <staniek@kde.org>

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

#include "styleproxy.h"

#include <QStyleFactory>
#include <QApplication>

using namespace KexiUtils;

StyleProxy::StyleProxy(QStyle* parentStyle)
        : QStyle()
        , m_method(new int(0))
{
    setParentStyle(parentStyle);
}

StyleProxy::~StyleProxy()
{
    delete m_style;
    delete m_method;
}

void StyleProxy::setParentStyle(QStyle* style)
{
    m_style = QStyleFactory::create(style->objectName());
    if (!m_style)
        m_style = style;
}

QStyle* StyleProxy::parentStyle() const
{
    if (m_style)
        return m_style;
    return QApplication::style();
}

QStyle* StyleProxy::parentStyle(int methodIndex) const
{
    if (!m_style || *m_method == methodIndex /* avoid infinite recursion */)
        return QApplication::style();
    *m_method = methodIndex;
    return m_style;
}
