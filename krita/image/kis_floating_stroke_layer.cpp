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

#include "kis_floating_stroke_layer.h"

#include <QMutex>
#include <QMutexLocker>
#include <QReadWriteLock>

#include <KoCompositeOp.h>
#include "kis_layer.h"
#include "kis_paint_layer.h"
#include "kis_paint_device.h"
#include "kis_painter.h"


struct KisFloatingStrokeLayer::Private {
    // To simulate the indirect painting
    KisPaintDeviceSP temporaryTarget;
    const KoCompositeOp* compositeOp;
    quint8 compositeOpacity;
    QBitArray channelFlags;
    QReadWriteLock lock;

    QMutex dirtyRegionMutex;
    QRegion dirtyRegion;
};


KisFloatingStrokeLayer::KisFloatingStrokeLayer()
    : d(new Private)
{
    d->compositeOp = 0;
}

KisFloatingStrokeLayer::~KisFloatingStrokeLayer()
{
    delete d;
}

void KisFloatingStrokeLayer::setTemporaryTarget(KisPaintDeviceSP t)
{
    d->dirtyRegion = QRegion();
    d->temporaryTarget = t;
}

void KisFloatingStrokeLayer::setTemporaryCompositeOp(const KoCompositeOp* c)
{
    d->compositeOp = c;
}

void KisFloatingStrokeLayer::setTemporaryOpacity(quint8 o)
{
    d->compositeOpacity = o;
}

void KisFloatingStrokeLayer::setTemporaryChannelFlags(const QBitArray& channelFlags)
{
    d->channelFlags = channelFlags;
}

void KisFloatingStrokeLayer::lockTemporaryTarget() const
{
    d->lock.lockForRead();
}

void KisFloatingStrokeLayer::unlockTemporaryTarget() const
{
    d->lock.unlock();
}

KisPaintDeviceSP KisFloatingStrokeLayer::temporaryTarget()
{
    return d->temporaryTarget;
}

const KisPaintDeviceSP KisFloatingStrokeLayer::temporaryTarget() const
{
    return d->temporaryTarget;
}

const KoCompositeOp* KisFloatingStrokeLayer::temporaryCompositeOp() const
{
    return d->compositeOp;
}

quint8 KisFloatingStrokeLayer::temporaryOpacity() const
{
    return d->compositeOpacity;
}

const QBitArray& KisFloatingStrokeLayer::temporaryChannelFlags() const
{
    return d->channelFlags;
}

bool KisFloatingStrokeLayer::hasTemporaryTarget() const
{
    return d->temporaryTarget;
}

void KisFloatingStrokeLayer::setDirty(const QRect &rect)
{
    lockTemporaryTarget();
    if(hasTemporaryTarget()) {
        addIndirectlyDirtyRect(rect);
    }
    unlockTemporaryTarget();
}

void KisFloatingStrokeLayer::addIndirectlyDirtyRect(const QRect &rect)
{
    QMutexLocker locker(&d->dirtyRegionMutex);
    d->dirtyRegion += rect;
}

QRegion KisFloatingStrokeLayer::indirectlyDirtyRegion()
{
    QMutexLocker locker(&d->dirtyRegionMutex);
    return d->dirtyRegion;
}

void KisFloatingStrokeLayer::mergeToLayer(KisLayerSP layer, KisUndoAdapter *undoAdapter, const QString &transactionText)
{
    mergeToLayerImpl(layer, undoAdapter, transactionText);
}

void KisFloatingStrokeLayer::mergeToLayer(KisLayerSP layer, KisPostExecutionUndoAdapter *undoAdapter, const QString &transactionText)
{
    mergeToLayerImpl(layer, undoAdapter, transactionText);
}

template<class UndoAdapter>
void KisFloatingStrokeLayer::mergeToLayerImpl(KisLayerSP layer,
                                                  UndoAdapter *undoAdapter,
                                                  const QString &transactionText)
{
    /**
     * We do not apply selection here, because it has already
     * been taken into account in a tool code
     */
    KisPainter gc(layer->paintDevice());
    gc.setCompositeOp(d->compositeOp);
    gc.setOpacity(d->compositeOpacity);
    gc.setChannelFlags(d->channelFlags);

    d->lock.lockForWrite();

    /**
     * Scratchpad may not have an undo adapter
     */
    if(undoAdapter) {
        gc.beginTransaction(transactionText);
    }

    QRegion dirtyRegion = indirectlyDirtyRegion();
    foreach(const QRect& rc, dirtyRegion.rects()) {
        gc.bitBlt(rc.topLeft(), d->temporaryTarget, rc);
    }
    d->temporaryTarget = 0;

    if(undoAdapter) {
        gc.endTransaction(undoAdapter);
    }

    d->lock.unlock();
}
