/*
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

#ifndef ANIMATOR_MANAGER_H
#define ANIMATOR_MANAGER_H

#include <QObject>

#include <KoCanvasObserverBase.h>

#include <kis_image.h>


class KisNodeManager;
class KisNodeCommandsAdapter;

class AnimatorFrameManager;
class AnimatorSwitcher;
class AnimatorUpdater;
class AnimatorLoader;
class AnimatorPlayer;
class AnimatorExporter;
class AnimatorImporter;

#include "simple_frame_layer.h"
#include "framed_animated_layer.h"
#include "animator_meta_info.h"
#include "filtered_frame_layer.h"

class AnimatorManager : public QObject, KoCanvasObserverBase {
    Q_OBJECT

public:
    AnimatorManager(KisImageWSP image);
    virtual ~AnimatorManager();

public:
    virtual void setCanvas(KoCanvasBase* canvas);
    virtual void unsetCanvas();

    virtual bool ready() const;

public:
    virtual AnimatorFrameManager* getFrameManager() const;
    virtual AnimatorSwitcher* getSwitcher() const;
    virtual AnimatorUpdater* getUpdater() const;
    virtual AnimatorLoader* getLoader() const;
    virtual AnimatorPlayer* getPlayer() const;
    virtual AnimatorExporter* getExporter() const;
    virtual AnimatorImporter* getImporter() const;

    virtual KisImageWSP image() const;

public:
    virtual void layerFramesNumberChange(AnimatedLayerSP layer, int number);

signals:
    void layerFramesNumberChanged(AnimatedLayerSP layer, int number) const;
    void framesNumberChanged(int number) const;

    void animatedLayerActivated(KisNodeSP layer) const;

protected slots:
    virtual void framesNumberCheck(AnimatedLayerSP layer, int number);

public:
    virtual void setKraMetaInfo(AnimatorMetaInfo* info);
    virtual void setKraMetaInfo();
    virtual AnimatorMetaInfo* kraMetaInfo();
    virtual AnimatorMetaInfo* metaInfo();

public:
    virtual void initLayers();

public:
    template <class CustomAnimatedLayer> void createAnimatedLayer();
    virtual void createNormalLayer();
    virtual void createControlLayer();
    virtual void convertToViewLayer(int from, int to);
    virtual void removeLayer();
    virtual void renameLayer(KisNodeSP layer, const QString& name);
    virtual void renameLayer(const QString& name);

    virtual void calculateActiveLayer();
    virtual void calculateLayer(AnimatedLayerSP layer);

    virtual KisNodeSP getAnimatedLayerByChild(KisNodeSP child) const;
    virtual KisNodeSP activeLayer() const;

    virtual void createFrame(AnimatedLayerSP layer, int frameNumber, const QString& ftype, bool iskey = true);
    virtual void createFrame(AnimatedLayerSP layer, const QString& ftype, bool iskey = true);
    virtual void createFrame(const QString& ftype, bool iskey = true);
    virtual void interpolate();

    virtual void createLoopFrame(int target, int repeat);

public:
    virtual void setFrameContent(SimpleFrameLayerSP frame, KisNodeSP content);
    virtual void setFrameFilter(FilteredFrameLayerSP frame, KisAdjustmentLayerSP filter);
    virtual void putNodeAt(KisNodeSP node, KisNodeSP parent, int index);
    virtual void removeNode(KisNodeSP node);

    virtual void insertLayer(AnimatedLayerSP layer, KisNodeSP parent, int index);
    virtual void removeLayer(KisNodeSP layer);

    virtual KisNodeSP createLayerAt(const QString &layerType, KisNodeSP parent, int index);

    // for legacy loader
    virtual void createGroupLayer(KisNodeSP parent);

public:
    virtual int framesNumber() const;

public:
    virtual void activate(int frameNumber, KisNodeSP node) const;
    virtual void activateKeyFrame(AnimatedLayerSP alayer, int frameNumber);

public:
    virtual QList<AnimatedLayerSP> layers();

protected:
    virtual void calculateFramesNumber();

protected:
    virtual void layerAdded(KisNodeSP layer);
    virtual void layerRemoved(KisNodeSP layer);

private:
    KisImageWSP m_image;
    KisNodeManager* m_nodeManager;
    KisNodeCommandsAdapter *m_nodeAdapter;

    AnimatorFrameManager* m_frameManager;
    AnimatorSwitcher* m_switcher;
    AnimatorUpdater* m_updater;
    AnimatorLoader* m_loader;
    AnimatorPlayer* m_player;
    AnimatorExporter* m_exporter;
    AnimatorImporter* m_importer;
    AnimatorMetaInfo* m_info;

    QList<AnimatedLayerSP> m_layers;
    KisNodeSP m_maxFrameLayer;

    int m_framesNumber;
};

#endif

