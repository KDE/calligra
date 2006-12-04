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

#include "KivioAbstractPage.h"

#include <KLocale>

#include <KoShape.h>

#include "KivioDocument.h"
#include "KivioLayer.h"

KivioAbstractPage::KivioAbstractPage(KivioDocument* doc, const QString& title)
{
  m_document = doc;
  setTitle(title);

  // Add a default layer
  addLayer(new KivioLayer(i18n("Layer 1"), this));
}

KivioAbstractPage::~KivioAbstractPage()
{
  qDeleteAll(m_layerList);
  m_layerList.clear();
}

void KivioAbstractPage::setTitle(const QString& newTitle)
{
  if(newTitle.isEmpty()) return;

  m_title = newTitle;
}

QString KivioAbstractPage::title() const
{
  return m_title;
}

QList<KivioLayer*> KivioAbstractPage::layers() const
{
  return m_layerList;
}

QList<KoShape*> KivioAbstractPage::shapes() const
{
    QList<KoShape*> shapes;

    foreach(KivioLayer* layer, m_layerList) {
        shapes += layer->shapes();
    }

    return shapes;
}

void KivioAbstractPage::addLayer(KivioLayer* layer)
{
    if((layer == 0) || m_layerList.contains(layer))
        return;

    m_layerList.append(layer);
}

void KivioAbstractPage::removeLayer(KivioLayer* layer)
{
    if(layer == 0)
        return;

    m_layerList.removeAll(layer);
}
