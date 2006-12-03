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

#ifndef KIVIOABSTRACTPAGE_H
#define KIVIOABSTRACTPAGE_H

#include <QString>
#include <QList>

#include <KoShapeControllerBase.h>

class KoPageLayout;
class KoShape;

class KivioDocument;
class KivioLayer;

/** Base class for KivioPage and KivioMasterPage */
class KivioAbstractPage
{
  public:
    KivioAbstractPage(KivioDocument* doc, const QString& title);
    virtual ~KivioAbstractPage();

    /// Change the page title
    void setTitle(const QString& newTitle);
    /// Get page title
    QString title() const;

    /// The page's document
    KivioDocument* document() const { return m_document; }

    virtual KoPageLayout pageLayout() const = 0;

    /// Add @p layer to the page's layer list
    void addLayer(KivioLayer* layer);
    /// Remove @p layer from the page's layer list
    void removeLayer(KivioLayer* layer);

    /// @return list of layers
    QList<KivioLayer*> layers() const;

    /// @return the layers as a list of KoShapes to be used when KoShapes is expected
    QList<KoShape*> layerShapes() const;

  private:
    KivioDocument* m_document;
    QString m_title;
    QList<KivioLayer*> m_layerList;
};

#endif
