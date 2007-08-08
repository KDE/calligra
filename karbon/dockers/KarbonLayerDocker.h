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
#include <KoDocumentSectionModel.h>
#include <KoDockFactory.h>

class VDocument;
class KoDocumentSectionView;
class KoShapeControllerBase;
class KoShape;
class KoShapeContainer;
class KoShapeLayer;
class QAbstractItemModel;
class KarbonDocumentModel;
class KoViewConverter;

class KarbonLayerDockerFactory : public KoDockFactory
{
public:
    KarbonLayerDockerFactory( KoShapeControllerBase *shapeController, VDocument *document );

    virtual QString id() const;
    virtual QDockWidget* createDockWidget();
private:
    KoShapeControllerBase * m_shapeController;
    VDocument *m_document;
};

class KarbonLayerDocker : public QDockWidget
{
Q_OBJECT

public:
    KarbonLayerDocker( KoShapeControllerBase *shapeController, VDocument *document );
    virtual ~KarbonLayerDocker();
public slots:
    void updateView();
private slots:
    void slotButtonClicked( int buttonId );
    void addLayer();
    void deleteItem();
    void raiseItem();
    void lowerItem();
    void itemClicked( const QModelIndex &index );
private:
    void extractSelectedLayersAndShapes( QList<KoShapeLayer*> &layers, QList<KoShape*> &shapes );
    KoShapeControllerBase *m_shapeController;
    VDocument *m_document;
    KoDocumentSectionView *m_layerView;
    KarbonDocumentModel *m_model;
};

class KarbonDocumentModel : public KoDocumentSectionModel
{
public:
    KarbonDocumentModel( VDocument *document );
    void update();
    // from QAbstractItemModel
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent( const QModelIndex &child ) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
    virtual Qt::DropActions supportedDropActions() const;
    virtual QStringList mimeTypes() const;
    virtual QMimeData * mimeData( const QModelIndexList & indexes ) const;
    virtual bool dropMimeData( const QMimeData * data, Qt::DropAction action, int row, int column, const QModelIndex & parent );
private:
    /// Returns properties of the given shape
    PropertyList properties( KoShape* shape ) const;
    /// Sets the properties on the given shape
    void setProperties( KoShape* shape, const PropertyList &properties );
    /// Creates a thumbnail image with the specified size from the given shape
    QImage createThumbnail( KoShape* shape, const QSize &thumbSize ) const;
    QRectF transformedShapeBox( KoShape *shape ) const;
    /// Paints shape using the given painter and view converter
    void paintShape( KoShape *shape, QPainter &painter, const KoViewConverter &converter, bool isSingleShape ) const;
    /// Returns the child shape with the given index from the parent shape
    KoShape * childFromIndex( KoShapeContainer *parent, int row ) const;
    /// Returns the zero based index of a child shape within its parent shape
    int indexFromChild( KoShapeContainer *parent, KoShape *child ) const;
    /// Returns the parent model index from the given child shape
    QModelIndex parentIndexFromShape( const KoShape * child );

    VDocument *m_document; ///< the undelying data structure
    KoShape *m_shape;
    mutable QList<KoShape*> m_childs;
    mutable KoShapeContainer *m_lastContainer;
};

#endif // KARBONLAYERDOCKER_H

// kate: replace-tabs on; space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
