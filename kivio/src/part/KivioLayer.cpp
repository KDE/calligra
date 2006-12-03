/* This file is part of the KDE project
   Copyright (C)  2006 Peter Simonsson <peter.simonsson@gmail.com>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#include "KivioLayer.h"

#include <KoShape.h>

#include "KivioAbstractPage.h"
#include "KivioDocument.h"

KivioLayer::KivioLayer(const QString& title, KivioAbstractPage* page)
    : KoShapeContainer()
{
    setTitle(title);
    m_page = page;
}

KivioLayer::~KivioLayer()
{
}

void KivioLayer::setTitle(const QString& newTitle)
{
    m_title = newTitle;
}

QString KivioLayer::title() const
{
    return m_title;
}

void KivioLayer::paintComponent(QPainter& painter, const KoViewConverter& converter)
{
    Q_UNUSED(painter);
    Q_UNUSED(converter);
}

void KivioLayer::addShape(KoShape* shape)
{
    KoShapeContainer::addChild(shape);
    m_page->document()->addShapeToViews(m_page, shape);
}

void KivioLayer::removeShape(KoShape* shape)
{
    KoShapeContainer::removeChild(shape);
    m_page->document()->removeShapeFromViews(m_page, shape);
}
