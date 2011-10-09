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
#include "frame_layer.h"


class AnimatorModel : public QAbstractTableModel
{
    Q_OBJECT
    
public:
    AnimatorModel(QObject* parent = 0);
    virtual ~AnimatorModel();
    
public:
    void init();
    
public:
    // ok
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    // ok
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    
    // ok
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;
    // ok
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

public:
    virtual int getMajor(const KisNode* meta_node);
    virtual int getMajor();
    
    virtual int getMinor(const KisNode* meta_node);
    virtual int getMinor();
    
    virtual bool isActive();
    
public slots:
    // ok, +review
    virtual void updateImage();

    // ok, +review
    virtual void loadLayers();
    
    void lightTableUpdate();
    
    virtual void convertLayers();
    virtual void convertLayer(KisNode* node);
    virtual void setEnabled(bool val);
    
    // ok
    virtual void forceFramesNumber(int num);
    // ok
    virtual void setVisible(bool val);
    // ok, +todo
    virtual void goNext();
    // ok
    virtual void goFirst();
    // ok, +review
    virtual void goFrame(int num);

    // ok
    virtual bool isLast();
    
    
    virtual void enableLayer(int num, bool value);
    virtual void visibleLayer(int num, bool value);
    

protected:
    // ok
    int previousFrame();
   
    // ok
    virtual bool isKey(const QModelIndex& index) const;
    // ok
    virtual bool isKey(int l, int f) const;

    
    virtual void setFramesNumber(int frames);
    virtual void setFramesNumber(int frames, bool up);
    
    
    // later
    virtual void visibleAll(bool v);
    
protected:
    KisNode* findMetaLayer();
    void setVersion(int major, int minor);

// public slots:
//     void testSlot();
    
public slots:
    void setCanvas(KisCanvas2* canvas);
    void setImage(KisImageWSP image);
    
    void setLightTable(AnimatorLightTable* table);
    void toggleExtLTable(bool val);
    
    void nodeDestroyed(QObject* node);

    void activateLayer(const QModelIndex &index);
    
    void setNodeManager(KisNodeManager* nodeman);
    
//    void updateCanvas();

    // Create layers at current frame
    void createFrame(QModelIndex index, char* layer_type);
    void copyFramePrevious(QModelIndex index);
    void copyFrameNext(QModelIndex index);

    void clonePrevious(QModelIndex index);

    void clearFrames(QModelIndexList frames);
    void clearFrame(QModelIndex index);
    
    QModelIndex createLayer();
    void deleteLayer();
    
    void layerUp();
    void layerDown();
    
    QString& getLayerName(QModelIndex index);
    QString& getLayerName(int l_num);
    
    void setLayerName(QModelIndex index, QString& name);
    void setLayerName(int l_num, QString& name);
    
    // Working with frames
    void frameMoveTo(QModelIndex& index, QModelIndex& moveto);
    void frameMoveTo(int l, int f, int tl, int tf);
    
    void framesMove(unsigned int src, int n, unsigned int dst);
    void framesMove(QModelIndexList fl, int move);
    
    void framesInsert(int n, unsigned int dst);
    void framesInsert(int n);
    void framesDelete(unsigned int src, int n);
    void framesDelete(int n);
    
    void framesClear(unsigned int src, int n);
    
    void frameRight();
    void frameLeft();

signals:
    void frameChanged(int frame_new);
    void framesNumberChanged(int frames);
    
    void statusChanged(bool ns);
    
public:
    KisNodeModel* sourceModel() const;
    void setSourceModel(KisNodeModel* model);
    
//     /**
//      * Differs from nodeFromIndex: just gives node from index, no any search for other ways
//      */
//     const KisNode* nodeAtIndex(const QModelIndex& index) const;
    
    /**
     * Returns node from given index or interpolates previous nodes in this row
     */
    const KisNode* nodeFromIndex(const QModelIndex& index) const;
    
    const KisNode* previousFrame(const QModelIndex& index) const;
    const KisNode* nextFrame(const QModelIndex& index) const;

public:
    void setOnionNext(int n);
    void setOnionPrevious(int n);
    void setOnionOpacity(quint8 op);
    
    void enableOnion();
    void disableOnion();
    
public:
    AnimatedLayer* getAnimatedLayer(quint32 num) const;
    AnimatedLayer* getAnimatedLayerByChild(const KisNode* node) const;
    
    AnimatedLayer* getActiveAnimatedLayer() const;
    
    FrameLayer* getCachedFrame(quint32 l, quint32 f) const;
    FrameLayer* getCachedFrame(const QModelIndex& index) const;
    
    FrameLayer* getUpdatedFrame(const QModelIndex& index);
    FrameLayer* getUpdatedFrame(quint32 l, quint32 f);
    
protected:
//     /**
//      * This functions gets info about layers and update info about animation layers
//      */
//     void realUpdate();
//     void afterUpdate();
    
    bool activateAtIndex(QModelIndex index);
    bool activateBeforeIndex(QModelIndex index);
    bool activateAfterIndex(QModelIndex index);
    
private:
//     virtual QModelIndex parent(const QModelIndex& child) const;

private:
    bool m_enabled;
    
    bool m_visible;
    
    bool m_updating;
    
    
//     QList< KisNode* > m_main_layers;                            // old
//     QList< QList < KisNode* > > m_frames_layers;                // old
    
    QList< AnimatedLayer* > m_layers;                           // new
    
    const QModelIndex& indexFromNode(KisNode* node) const;
    
    qint32 m_frame;
    qint32 m_previous_frame;
    quint32 m_frame_num;
    quint32 m_forced_frame_num;
    
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
