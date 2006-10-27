/* This file is part of the KDE project
 * Copyright (C) 2006 Jan Hambrecht <jaham@gmx.net>
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
#ifndef V_LAYER_DOCKER
#define V_LAYER_DOCKER

#include <QWidget>
#include <KoDocumentSectionModel.h>

class KarbonView;
class VDocument;
class KoDocumentSectionView;
class KarbonCanvas;
class KoShapeManager;
class KoShape;
class KoLayerShape;
class QAbstractItemModel;
class VDocumentModel;
class KoViewConverter;

class VLayerDocker : public QWidget
{
Q_OBJECT

public:
    VLayerDocker( KarbonView *view, VDocument *doc );
    virtual ~VLayerDocker();
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
    void extractSelectedLayersAndShapes( QList<KoLayerShape*> &layers, QList<KoShape*> &shapes );
    KarbonView *m_view;
    VDocument *m_document;
    KarbonCanvas *m_canvas;
    KoDocumentSectionView *m_layerView;
    VDocumentModel *m_model;
};

class VDocumentModel : public KoDocumentSectionModel
{
public:
    VDocumentModel( VDocument *document, KoShapeManager *shapeManager );
    void update();
    // from QAbstractItemModel
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent( const QModelIndex &child ) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);
private:
    PropertyList properties( KoShape* shape ) const;
    void setProperties( KoShape* shape, const PropertyList &properties );
    QImage createThumbnail( KoShape* shape, const QSize &thumbSize ) const;
    QRectF transformedShapeBox( KoShape *shape, const QMatrix &shapeMatrix ) const;
    void paintShape( KoShape *shape, QPainter &painter, const KoViewConverter &converter ) const;
    VDocument *m_document;
    KoShapeManager *m_shapeManager;
    KoShape *m_shape;
};

#endif // V_LAYER_DOCKER

// kate: replace-tabs on; space-indent on; indent-width 4; mixedindent off; indent-mode cstyle;
