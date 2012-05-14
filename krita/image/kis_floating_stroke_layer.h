/*
 *  Copyright (c) 2004 Bart Coppens <kde@bartcoppens.be>
 *  Copyright (c) 2010 Dmitry Kazakov <dimula73@gmail.com>
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

#ifndef KIS_FLOATING_STROKE_LAYER_H_
#define KIS_FLOATING_STROKE_LAYER_H_

#include "krita_export.h"
#include "kis_types.h"
#include "kis_node.h"

class QBitArray;
class KisUndoAdapter;
class KisPostExecutionUndoAdapter;

/**
 * KisFloatingStrokeLayer is an invisible paint target that
 * strokes are painted on until the stroke is done; then the stroke
 * result is merged with the paint layer. A KisPaintLayer is a
 * KisFloatingStrokeLayer.
 *
 * This is done in WASH and FRINGE painting mode -- see KisPaintActionTypeOption.
 *
 * The process by which we decide whether or not to use indirect painting is
 * convoluted. For examplle for the plain brush operation:
 *
 * It starts with:
 * KisPaintActionTypeOption:  UNSUPPORTED, BUILDUP, WASH, FRINGED
 *  KisBrushopSettingsWidget: owns KisPaintActionTypeOption
 *  KisBrushBasedPaintopSettings: created by KisBrushopSettingsWidget
 *    bool KisPaintOpSettings::paintIncremental()
 *
 *   KisBrushOp::settings: returns KisBrushBasedPaintopSettings
 *
 *  KisResourcesSnapshot::needsIndirectPainting()
 *      calls KisPaintOpSettings::paintIncremental()
 *
 *  KisToolFreehandHelper::initPaint:
 *
 *   calls KisResourcesSnapshot::needsIndirectPainting()
 *      creates FreehandStrokeStrategy(indirectPainting,...)
 *	 calls setNeedsIndirectPainting(needsIndirectPainting);
 *
 *
 *   KisPainterBasedStrokeStrategy::initStrokeCallback()
 *     casts current node to KisFloatingStrokeLayer
 */
class KRITAIMAGE_EXPORT KisFloatingStrokeLayer
{
    KisFloatingStrokeLayer(const KisFloatingStrokeLayer&);
    KisFloatingStrokeLayer& operator=(const KisFloatingStrokeLayer&);
public:

    KisFloatingStrokeLayer();
    virtual ~KisFloatingStrokeLayer();

    bool hasTemporaryTarget() const;

    void setTemporaryTarget(KisPaintDeviceSP t);
    void setTemporaryCompositeOp(const KoCompositeOp* c);
    void setTemporaryOpacity(quint8 o);
    void setTemporaryChannelFlags(const QBitArray& channelFlags);

    /**
     * @brief setMask
     * Sets a mask on the indirect painting layer. This mask is autoextended
     * over the size of the stroke. The mask will "texture" the stroke before
     * combining it.
     *
     * @param mask the selection that describes the mask.
     * @param xOffset the x offset from the origin of the node
     * @param yOffset the y offset from the origin of the node
     * @param maskBounds the exact bounds of the mask. May be different from the actual
     *    exactBounds of the selection mask since we might want to repeat a mask with
     *    empty borders.
     */
    void setMask(KisPixelSelectionSP mask, int xOffset, int yOffset, QRect &maskBounds);

    void setDirty(const QRect &rect);

    /**
     * Writes the temporary target into the paint device of the layer when the stroke is finished.
     * This is called by the stroke sthread
     * This action will lock the temporary target itself.
     */
    void mergeToLayer(KisLayerSP layer, KisUndoAdapter *undoAdapter, const QString &transactionText);
    void mergeToLayer(KisLayerSP layer, KisPostExecutionUndoAdapter *undoAdapter, const QString &transactionText);

protected:

    /**
     * Lock the temporary target.
     * It should be done for guarding every access to
     * temporaryTarget() or original()
     * NOTE: well, not "every", but...
     */
    void lockTemporaryTarget() const;

    /**
     * Unlock the temporary target
     * 
     * \see lockTemporaryTarget()
     */
    void unlockTemporaryTarget() const;

    KisPaintDeviceSP temporaryTarget();
    const KisPaintDeviceSP temporaryTarget() const;
    const KoCompositeOp* temporaryCompositeOp() const;
    quint8 temporaryOpacity() const;
    const QBitArray& temporaryChannelFlags() const;

private:
    /**
     * Adds a dirty rect to the list of rects those should be
     * merged to the layer after the indirect painting is finished
     * WARNING: should be called with the lock held
     *
     * \see lockTemporaryTarget()
     */
    void addIndirectlyDirtyRect(const QRect &rect);
    QRegion indirectlyDirtyRegion();

    template<class UndoAdapter>
        void mergeToLayerImpl(KisLayerSP layer,
                              UndoAdapter *undoAdapter,
                              const QString &transactionText);
private:
    struct Private;
    Private* const d;
};


#endif /* KIS_INDIRECT_PAINTING_SUPPORT_H_ */
