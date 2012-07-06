/*
 *  Copyright (c) 2005 C. Boemann <cbo@boemann.dk>
 *  Copyright (c) 2006 Bart Coppens <kde@bartcoppens.be>
 *  Copyright (c) 2007 Boudewijn Rempt <boud@valdyas.org>
 *  Copyright (c) 2009 Dmitry Kazakov <dimula73@gmail.com>
 *  Copyright (c) 2012 Francisco Fernandes <francisco.fernandes.j@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
#ifndef KIS_ITEM_LAYER_H_
#define KIS_ITEM_LAYER_H_

#include "kis_types.h"
#include "kis_layer.h"
#include "kis_indirect_painting_support.h"

#include <QVector>  //for now i will use this data structure. After I see the physics is doing good, get a better one.

class KoColorSpace;
class Particle; //

class KRITAIMAGE_EXPORT KisItemLayer : public KisLayer, public KisIndirectPaintingSupport
{

    Q_OBJECT

public:

    KisItemLayer(KisImageWSP image, const QString& name, quint8 opacity, KisPaintDeviceSP dev);

    KisItemLayer(KisImageWSP image, const QString& name, quint8 opacity);

    KisItemLayer(KisImageWSP image, const QString& name, quint8 opacity, const KoColorSpace * colorSpace);

    KisItemLayer(const KisItemLayer& rhs);
    virtual ~KisItemLayer();

    KisNodeSP clone() const {
        return KisNodeSP(new KisItemLayer(*this));
    }

    bool allowAsChild(KisNodeSP) const;

    const KoColorSpace * colorSpace() const;

    bool needProjection() const;

    void copyOriginalToProjection(const KisPaintDeviceSP original,
                                  KisPaintDeviceSP projection,
                                  const QRect& rect) const;

    using KisLayer::setDirty;
    void setDirty(const QRect & rect);

    QIcon icon() const;
    void setImage(KisImageWSP image);

    KoDocumentSectionModel::PropertyList sectionModelProperties() const;
    void setSectionModelProperties(const KoDocumentSectionModel::PropertyList &properties);

public:

    QRect extent() const;
    QRect exactBounds() const;

    bool accept(KisNodeVisitor &v);
    void accept(KisProcessingVisitor &visitor, KisUndoAdapter *undoAdapter);

    void setChannelLockFlags(const QBitArray& channelFlags);

    const QBitArray& channelLockFlags() const;

    KisPaintDeviceSP paintDevice() const;

    KisPaintDeviceSP original() const;

    bool alphaLocked() const;

    void setAlphaLocked(bool lock);

    void paintScene(const QRect & rect); //

    void addGrains(QVector<Particle> grains);

public slots:

    // KisIndirectPaintingSupport
    KisLayer* layer() {
        return this;
    }

    


private:

    struct Private;
    Private * const m_d;

    /*
     * Since a layer already have an inheritance with QObject (from KisBaseNode)
     * I can't let KisItemLayer be a subclass of QGraphicsScene. My solution for now
     * its to have an QGraphicsScene atribute and create a method to update this layer
     * with the QGraphicsScene current state
     */
    QVector<Particle> * scene;
};

typedef KisSharedPtr<KisItemLayer> KisItemLayerSP;

#endif // KIS_ITEM_LAYER_H_

