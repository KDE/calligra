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

#include "KivioDocument.h"

KivioDocument::KivioDocument(QWidget* parentWidget, const char* widgetName, QObject* parent,
              const char* name, bool singleViewMode)
  : KoDocument(parentWidget, widgetName, parent, name, singleViewMode)
{
}

KivioDocument::~KivioDocument()
{
}

void KivioDocument::paintContent(QPainter &painter, const QRect &rect, bool transparent,
                            double zoomX, double zoomY)
{
}

bool KivioDocument::loadXML(QIODevice* device, const QDomDocument& doc)
{
}

bool KivioDocument::loadOasis(const QDomDocument& doc, KoOasisStyles& oasisStyles,
                        const QDomDocument& settings, KoStore* store)
{
}

bool KivioDocument::saveOasis(KoStore* store, KoXmlWriter* manifestWriter)
{
}

KoView* KivioDocument::createViewInstance(QWidget* parent, const char* name)
{
}

#include "KivioDocument.moc"
