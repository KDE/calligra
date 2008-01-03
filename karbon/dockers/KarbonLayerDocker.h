/* This file is part of the KDE project
 * Copyright (C) 2006-2007 Jan Hambrecht <jaham@gmx.net>
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
#ifndef KARBONLAYERDOCKER_H
#define KARBONLAYERDOCKER_H

#include <QDockWidget>
#include <KoDockFactory.h>

class VDocument;
class KoDocumentSectionView;
class KoShapeControllerBase;
class KoShape;
class KoShapeLayer;
class KoShapeGroup;
class KarbonLayerModel;
class KarbonPart;
class QModelIndex;

namespace KParts
{
    class Part;
}

class KarbonLayerDockerFactory : public KoDockFactory
{
public:
    KarbonLayerDockerFactory();

    virtual QString id() const;
    virtual QDockWidget* createDockWidget();
};

class KarbonLayerDocker : public QDockWidget
{
Q_OBJECT

public:
    KarbonLayerDocker();
    virtual ~KarbonLayerDocker();
public slots:
    void updateView();
    void setPart( KParts::Part * part );
private slots:
    void slotButtonClicked( int buttonId );
    void addLayer();
    void deleteItem();
    void raiseItem();
    void lowerItem();
    void itemClicked( const QModelIndex &index );
private:
    void extractSelectedLayersAndShapes( QList<KoShapeLayer*> &layers, QList<KoShape*> &shapes, bool addChilds = false );
    void addChildsRecursive( KoShapeGroup * parent, QList<KoShape*> &shapes );
    KarbonPart * m_part;
    KarbonLayerModel * m_model;
    KoDocumentSectionView * m_layerView;
};

#endif // KARBONLAYERDOCKER_H

// kate: replace-tabs on; space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
