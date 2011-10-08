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

#ifndef __KIS_UPDATE_TIME_MONITOR_INTERFACE_H
#define __KIS_UPDATE_TIME_MONITOR_INTERFACE_H

#if 1

#include "kis_update_time_monitor.h"
#include "kis_stroke_strategy.h"

#define UTM_START_STROKE(name)                          \
    KisUpdateTimeMonitor::instance()->startStroke(name)
#define UTM_END_STROKE()                                \
    KisUpdateTimeMonitor::instance()->endStroke()
#define UTM_JOB_ADDED(key)                              \
    KisUpdateTimeMonitor::instance()->jobAdded(key)
#define UTM_THREAD_STARTED(key)                                 \
    KisUpdateTimeMonitor::instance()->jobThreadStarted(key)
#define UTM_THREAD_FINISHED()                                   \
    KisUpdateTimeMonitor::instance()->jobThreadFinished()
#define UTM_THREAD_UPDATE_REQUESTED(rects)                              \
    KisUpdateTimeMonitor::instance()->jobThreadUpdateRequested(QVector<QRect>() << (rects))
#define UTM_SET_BOUNDS(bounds)                          \
    KisUpdateTimeMonitor::instance()->setBounds(bounds)
#define UTM_UPDATE_FINISHED(rect)                               \
    KisUpdateTimeMonitor::instance()->updateFinished(rect)

#else

#define UTM_START_STROKE(name)
#define UTM_END_STROKE()
#define UTM_JOB_ADDED(key)
#define UTM_THREAD_STARTED(key)
#define UTM_THREAD_FINISHED()
#define UTM_THREAD_UPDATE_REQUESTED(rects)
#define UTM_SET_BOUNDS(bounds)
#define UTM_UPDATE_FINISHED(rect)

#endif

#endif /* __KIS_UPDATE_TIME_MONITOR_INTERFACE_H */
