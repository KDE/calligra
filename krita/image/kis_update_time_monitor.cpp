/*
 *  Copyright (c) 2011 Dmitry Kazakov <dimula73@gmail.com>
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

#include "kis_update_time_monitor.h"
#include "kis_update_time_monitor_p.h"


#include <kglobal.h>
#include <QQueue>
#include <QMutex>
#include <QMutexLocker>
#include <QThreadStorage>


struct KisUpdateTimeMonitor::Private
{
    Private() : strokeStarted(false) {}

    QQueue<StrokeInfo*> strokesQueue;
    QThreadStorage<void**> registeredUpdateKey;


    QMutex mutex;

    QRect bounds;
    bool strokeStarted;
};


KisUpdateTimeMonitor::KisUpdateTimeMonitor()
    : m_d(new Private)
{
}

KisUpdateTimeMonitor::~KisUpdateTimeMonitor()
{
    delete m_d;
}

KisUpdateTimeMonitor* KisUpdateTimeMonitor::instance()
{
    K_GLOBAL_STATIC(KisUpdateTimeMonitor, s_instance);
    return s_instance;
}

void KisUpdateTimeMonitor::startStroke(const QString &name)
{
    QMutexLocker(&m_d->mutex);
    m_d->strokesQueue.enqueue(new StrokeInfo(name));
}

void KisUpdateTimeMonitor::endStroke()
{
    QMutexLocker(&m_d->mutex);
    Q_ASSERT(!m_d->strokesQueue.isEmpty());
    m_d->strokesQueue.last()->end();
}

void KisUpdateTimeMonitor::mouseEvent(const QPointF &pos)
{
    QMutexLocker(&m_d->mutex);
    if(!m_d->strokesQueue.isEmpty()) {
        m_d->strokesQueue.last()->registerMouseEvent(pos);
    }
}

void KisUpdateTimeMonitor::jobAdded(void *key)
{
    if(!key) return;

    QMutexLocker(&m_d->mutex);
    Q_ASSERT(!m_d->strokesQueue.isEmpty());
    m_d->strokesQueue.last()->jobAdded(key);
}

void KisUpdateTimeMonitor::updateRequested(void *key, const QVector<QRect> &rects)
{
    QMutexLocker(&m_d->mutex);
    if(!m_d->strokesQueue.isEmpty()) {
        StrokeInfo *currentInfo = 0;
        foreach(StrokeInfo *info, m_d->strokesQueue) {
            if(!info->jobsFinished()) {
                currentInfo = info;
                break;
            }
        }
        if(currentInfo) {
            currentInfo->updateRequested(key, rects, m_d->bounds);
        }
    }
}

void KisUpdateTimeMonitor::updateFinished(const QRect &rect)
{
    QMutexLocker(&m_d->mutex);

    foreach(StrokeInfo *info, m_d->strokesQueue) {
        info->updateFinished(rect);

        if(info->isFinished()) {
            info->printValues();
            m_d->strokesQueue.removeOne(info);
            delete info;
        }
    }
}

void KisUpdateTimeMonitor::setBounds(const QRect &bounds)
{
    m_d->bounds = bounds;
}

void KisUpdateTimeMonitor::jobThreadStarted(void *key)
{
    if(!key) return;

    m_d->registeredUpdateKey.setLocalData(new (void*)(key));
}

void KisUpdateTimeMonitor::jobThreadFinished()
{
    if(!m_d->registeredUpdateKey.hasLocalData()) return;

    void *key = *m_d->registeredUpdateKey.localData();

    QMutexLocker(&m_d->mutex);

        StrokeInfo *currentInfo = 0;
        foreach(StrokeInfo *info, m_d->strokesQueue) {
            if(!info->jobsFinished()) {
                currentInfo = info;
                break;
            }
        }
        if(currentInfo) {
            currentInfo->jobFinished(key);
        }

    m_d->registeredUpdateKey.setLocalData(0);
}

void KisUpdateTimeMonitor::jobThreadUpdateRequested(const QVector<QRect> &rects)
{
    if(m_d->registeredUpdateKey.hasLocalData()) {
        void *key = *m_d->registeredUpdateKey.localData();
        updateRequested(key, rects);
    }
}
