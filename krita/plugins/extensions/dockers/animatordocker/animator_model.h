/*
 *  Model
 *  Copyright (C) 2011 Torio Mlshi <mlshi@lavabit.com>
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
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */


#ifndef ANIMATOR_MODEL_H
#define ANIMATOR_MODEL_H

#include <QModelIndex>
#include <QAbstractTableModel>
#include <QAbstractItemModel>

#include "kis_layer.h"
#include "kis_node_model.h"
#include "kis_node_manager.h"
#include "kis_canvas2.h"
#include "kis_image.h"
#include "animator_light_table.h"

#include "simple_animated_layer.h"


class AnimatorModel : public QAbstractTableModel
{
    Q_OBJECT
    
public:
    AnimatorModel(QObject* parent = 0);
    virtual ~AnimatorModel();
    
public:
    void init();
    
public:
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

public slots:
    void testSlot();
    
public slots:
    void setCanvas(KisCanvas2* canvas);
    void setImage(KisImageWSP image);
    void setLightTable(AnimatorLightTable* table);
    
    void toogleExtLTable(bool val);
    
    void update();
    
    /**
     * This functions gets info about layers and update info about animation layers
     */
    void realUpdate();
    void afterUpdate();
    
    void nodeDestroyed(QObject* node);

    void setFrame(int frame);
    void setFramesNumber(int frames, bool up = true);
    
    void activateLayer(QModelIndex index);
    
    void frameUpdate();
    void lightTableUpdate();
    
    void setNodeManager(KisNodeManager* nodeman);
    
    void updateCanvas();

    // Create layers at current frame
    void createFrame(QModelIndex index, char* layer_type);
    void copyFramePrevious(QModelIndex index);
    void copyFrameNext(QModelIndex index);
    void clearFrame(QModelIndex index);
    
    void createLayer();
    void deleteLayer();
    
    void layerUp();
    void layerDown();
    
    void renameLayer(QModelIndex index, QString& name);
    void renameLayer(int l_num, QString& name);
    
//     void visibleAll();
    
    void setEnabled(bool en);
    
    // Working with frames
    void framesInsert(int n, unsigned int dst);
    void framesMove(unsigned int src, int n, unsigned int dst);
    void framesDelete(unsigned int src, int n);
    
    void framesClear(unsigned int src, int n);
    
    void frameRight();
    void frameLeft();

signals:
    void frameChanged(int frame_new);
    void framesNumberChanged(int frames);
    
public:
    KisNodeModel* sourceModel() const;
    void setSourceModel(KisNodeModel* model);
    
    /**
     * Differs from nodeFromIndex: just gives node from index, no any search for other ways
     */
    const KisNode* nodeAtIndex(const QModelIndex& index) const;
    
    /**
     * Returns node from given index or extrapolates previous nodes in this row
     * TODO: real extropolation
     */
    const KisNode* nodeFromIndex(const QModelIndex& index) const;
    
    const KisNode* previousFrame(const QModelIndex& index) const;
    const KisNode* nextFrame(const QModelIndex& index) const;

public:
    void goNext();
    void goFirst();
    
    bool isLast();
    
public:
    void setOnionNext(int n);
    void setOnionPrevious(int n);
    void setOnionOpacity(quint8 op);
    
    void enableOnion();
    void disableOnion();
    
    
protected:
    KisNode* getAnimatedLayerByChild(const KisNode* node);
    
    /**
     * Make all frames of layer unvisible
     */
    void unvisibleLayer(int n);
    
    bool activateAtIndex(QModelIndex index);
    bool activateBeforeIndex(QModelIndex index);
    bool activateAfterIndex(QModelIndex index);
    
private:
//     virtual QModelIndex parent(const QModelIndex& child) const;

private:
    
    bool m_enabled;
    
    bool m_updating;
    
    
//     QList< KisNode* > m_main_layers;                            // old
//     QList< QList < KisNode* > > m_frames_layers;                // old
    
    QList< AnimatedLayer* > m_layers;                           // new
    
    const QModelIndex& indexFromNode(KisNode* node) const;
     
    qint32 m_frame;
    qint32 m_frame_num;
    
    KisNodeModel* m_source;
    KisNodeManager* m_nodeman;  // Is it necessary?
    KisCanvas2* m_canvas;
    KisImage* m_image;
    
    // Onion skin
    int m_onion_prv;
    int m_onion_nxt;
    
    bool m_onion_en;
    bool m_ext_lighttable;
    
    AnimatorLightTable* m_light_table;
    
    quint8 m_onion_opacity;
    
//     m_can
};

#endif // ANIMATOR_MODEL_H
