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

#include "kis_tool_freehand_helper.h"

#include <QTimer>
#include <QVector>

#include <KLocale>

#include <KoPointerEvent.h>
#include <KoCanvasResourceManager.h>

#include "kis_distance_information.h"
#include "kis_painting_information_builder.h"
#include "kis_recording_adapter.h"
#include "kis_image.h"
#include "kis_painter.h"


struct KisToolFreehandHelper::Private
{
    // Mostly stateless class that creates KisPaintInformation objects
    // by compensating for the tool's assistant magnetism and the
    // global pressure curve
    KisPaintingInformationBuilder *infoBuilder;

    KisRecordingAdapter *recordingAdapter;
    KisStrokesFacade *strokesFacade;

    bool hasPaintAtLeastOnce;

    QTime strokeTime;
    QTimer strokeTimeoutTimer;

    KisResourcesSnapshotSP resources;
    KisStrokeId strokeId;

    // The list of places where we'll paint. One for every touchpoint,
    // or for every transform (in the case of multihand painting).
    QVector<PainterInfo*> painterInfos;

    // The list of start positions for the stroke. Used by the
    // KisPaintingInformationBuilder class to create correct KisPaintInformation
    // objects by asking the tool for assistant magnetism corrections
    // We only keep a list of startpoints equal in size to the list of
    // painterinfos when painting using multitouch: multihand paint like
    // stylus/mouse painting has only _one_ startpoint.
    QVector<QPointF> startPoints;

    // lists of previous and one before previous KisPaintInformation
    // objects, one for every touch point (if any). If there are no
    // touch points active, there is only one item in these vectors.
    QVector<KisPaintInformation> previousPaintInformations;
    QVector<KisPaintInformation> olderPaintInformations;

    // A list of all the tangents associated with the various touch points
    QVector<bool> hasTangents;
    QVector<QPointF> previousTangents;

    bool smooth;
    qreal smoothness;

    QTimer airbrushingTimer;

    // For the automatic multihand tool: the number of transformations to be
    // applied to the painting postition so we paint at all the positions
    // calculated using these transforms from the original point of painting.
    QVector<QTransform> transformations;
};


KisToolFreehandHelper::KisToolFreehandHelper(KisPaintingInformationBuilder *infoBuilder,
                                             KisRecordingAdapter *recordingAdapter)
    : m_d(new Private)
{
    m_d->infoBuilder = infoBuilder;
    m_d->recordingAdapter = recordingAdapter;

    m_d->smooth = true;
    m_d->smoothness = 1.0;

    m_d->strokeTimeoutTimer.setSingleShot(true);
    connect(&m_d->strokeTimeoutTimer, SIGNAL(timeout()), SLOT(finishStroke()));

    connect(&m_d->airbrushingTimer, SIGNAL(timeout()), SLOT(doAirbrushing()));
}

KisToolFreehandHelper::~KisToolFreehandHelper()
{
    delete m_d;
}

void KisToolFreehandHelper::setSmoothness(bool smooth, qreal smoothness)
{
    m_d->smooth = smooth;
    m_d->smoothness = smoothness;
}

void KisToolFreehandHelper::setupTransformations(const QVector<QTransform> &transformations)
{
    m_d->transformations = transformations;
}

void KisToolFreehandHelper::initPaint(KoPointerEvent *event,
                                      KoCanvasResourceManager *resourceManager,
                                      KisImageWSP image,
                                      KisStrokesFacade *strokesFacade,
                                      KisPostExecutionUndoAdapter *undoAdapter,
                                      KisNodeSP overrideNode)
{
    Q_UNUSED(overrideNode);

    m_d->strokesFacade = strokesFacade;
    m_d->hasPaintAtLeastOnce = false;
    m_d->strokeTime.start();

    m_d->previousPaintInformations.clear();
    m_d->olderPaintInformations.clear();
    m_d->previousTangents.clear();
    m_d->hasTangents.clear();
    m_d->painterInfos.clear();

    // auto-multihand and true multihand do not go together, for now. Mirroring should be no problem,
    // since that is implemented in a different way from multihand
    if (event->touchPoints.size() > 0 && m_d->transformations.isEmpty()) {
        for (int i = 0; i < event->touchPoints.size(); ++i) {
            m_d->painterInfos << new PainterInfo(new KisPainter(), new KisDistanceInformation());
        }
    }
    else if (!m_d->transformations.isEmpty()) {
        for (int i = 0; i < m_d->transformations.size(); i++) {
            m_d->painterInfos << new PainterInfo(new KisPainter(), new KisDistanceInformation());
        }

    }
    else {
        m_d->painterInfos << new PainterInfo(new KisPainter(), new KisDistanceInformation());
    }

    m_d->resources = new KisResourcesSnapshot(image,
                                              undoAdapter,
                                              resourceManager);

    if (overrideNode) {
        m_d->resources->setCurrentNode(overrideNode);
    }

    bool indirectPainting = m_d->resources->needsIndirectPainting();

    if (m_d->recordingAdapter) {
        m_d->recordingAdapter->startStroke(image, m_d->resources);
    }

    KisStrokeStrategy *stroke =
        new FreehandStrokeStrategy(indirectPainting,
                                   m_d->resources, m_d->painterInfos, i18n("Freehand Stroke"));

    m_d->strokeId = m_d->strokesFacade->startStroke(stroke);

    m_d->previousPaintInformations.clear();
    // We only keep startpoints for the touchpoints, not in the case we have more paintinfo's
    // because transformations are set.
    if (event->touchPoints.size() > 0 && m_d->transformations.isEmpty()) {
        for (int i = 0; i < event->touchPoints.size(); ++i) {
            // XXX: check for touchpoint state?
            m_d->previousPaintInformations << m_d->infoBuilder->startStroke(event, m_d->strokeTime.elapsed(), i);
            m_d->olderPaintInformations << KisPaintInformation();
            m_d->startPoints << event->touchPoints[i].point;
            m_d->previousTangents << QPointF();
            m_d->hasTangents << false;
        }
    }
    else {
        m_d->previousPaintInformations << m_d->infoBuilder->startStroke(event, m_d->strokeTime.elapsed());
        m_d->olderPaintInformations << KisPaintInformation();
        m_d->startPoints << event->point;
        m_d->previousTangents << QPointF();
        m_d->hasTangents << false;
    }

    if (m_d->resources->needsAirbrushing()) {
        m_d->airbrushingTimer.setInterval(m_d->resources->airbrushingRate());
        m_d->airbrushingTimer.start();
    }
}

void KisToolFreehandHelper::paint(KoPointerEvent *event)
{
    for (int i = 0; i < m_d->painterInfos.size(); ++i) {

        QPointF startPoint;
        KisPaintInformation previousPaintInformation;
        KisPaintInformation olderPaintInformation;
        bool hasTangent = false;

        if (i < m_d->startPoints.size()) startPoint = m_d->startPoints[i];
        if (i < m_d->previousPaintInformations.size()) previousPaintInformation = m_d->previousPaintInformations[i];
        if (i < m_d->olderPaintInformations.size()) olderPaintInformation = m_d->olderPaintInformations[i];
        if (i < m_d->hasTangents.size()) hasTangent = m_d->hasTangents[i];

        KisPaintInformation info =
                m_d->infoBuilder->continueStroke(event,
                                                 previousPaintInformation.pos(),
                                                 startPoint,
                                                 m_d->strokeTime.elapsed(),
                                                 i);

        // the position in the indexes for tangent, but also previous and
        // older paint info, either 0 if single-hand or multihand,
        // or i, if multitouch.
        int pos = event->touchPoints.size() > 0 ? i : 0;

        if (m_d->smooth) {
            QPointF previousTangent;
            if (hasTangent) {
                previousTangent = (info.pos() - previousPaintInformation.pos()) * m_d->smoothness / (3.0 * (info.currentTime() - previousPaintInformation.currentTime()));
                m_d->hasTangents[pos] = true;
                m_d->previousTangents[pos] = previousTangent;
            }
            else {
                QPointF newTangent = (info.pos() - olderPaintInformation.pos()) * m_d->smoothness /
                        (3.0 * (info.currentTime() - olderPaintInformation.currentTime()));
                qreal scaleFactor = (previousPaintInformation.currentTime() - olderPaintInformation.currentTime());
                QPointF control1 = olderPaintInformation.pos() + previousTangent * scaleFactor;
                QPointF control2 = previousPaintInformation.pos() - newTangent * scaleFactor;

                paintBezierCurve(i, olderPaintInformation, control1, control2, previousPaintInformation);

                m_d->previousTangents[pos] = newTangent;
            }
            m_d->olderPaintInformations[pos] = previousPaintInformation;
            m_d->strokeTimeoutTimer.start(100);
        }
        else {
            paintLine(i, previousPaintInformation, info);
        }

        m_d->previousPaintInformations[i] = info;

        if (m_d->airbrushingTimer.isActive()) {
            m_d->airbrushingTimer.start();
        }
    }
}

void KisToolFreehandHelper::endPaint()
{
    if (!m_d->hasPaintAtLeastOnce) {
        for (int i = 0; i < m_d->painterInfos.size(); ++i) {
            paintAt(i, m_d->previousPaintInformations[i]);
        }
    }
    else if (m_d->smooth) {
        finishStroke();
    }
    m_d->strokeTimeoutTimer.stop();

    if (m_d->airbrushingTimer.isActive()) {
        m_d->airbrushingTimer.stop();
    }

    /**
     * There might be some timer events still pending, so
     * we should cancel them. Use this flag for the purpose.
     * Please note that we are not in MT here, so no mutex
     * is needed
     */
    m_d->painterInfos.clear();
    m_d->previousPaintInformations.clear();
    m_d->olderPaintInformations.clear();
    m_d->startPoints.clear();

    m_d->strokesFacade->endStroke(m_d->strokeId);

    if (m_d->recordingAdapter) {
        m_d->recordingAdapter->endStroke();
    }
}

const KisPaintOp* KisToolFreehandHelper::currentPaintOp() const
{
    return !m_d->painterInfos.isEmpty() ? m_d->painterInfos.first()->painter->paintOp() : 0;
}

void KisToolFreehandHelper::finishStroke()
{
    for (int i = 0; i < m_d->painterInfos.size(); ++i) {

        // there is only one tangent if we are painting multihand, not multitouch
        bool hasTangent = false;
        if (i < m_d->hasTangents.size()) {
            hasTangent = m_d->hasTangents[i];
            m_d->hasTangents[i] = false;
        }
        if (hasTangent) {
            // and there's only one previous and older paintinfo object in that case
            KisPaintInformation older;
            KisPaintInformation previous;
            QPointF previousTangent;

            if (i < m_d->previousPaintInformations.size()) previous = m_d->previousPaintInformations[i];
            if (i < m_d->olderPaintInformations.size()) older = m_d->olderPaintInformations[i];
            if (i < m_d->previousTangents.size()) previousTangent = m_d->previousTangents[i];

            QPointF newTangent  = (previous.pos() - older.pos()) * m_d->smoothness / 3.0;
            qreal   scaleFactor = (previous.currentTime() - older.currentTime());
            QPointF control1    = older.pos() + previousTangent * scaleFactor;
            QPointF control2    = previous.pos() - newTangent;

            paintBezierCurve(i, older, control1, control2, previous);
        }
    }
}

void KisToolFreehandHelper::doAirbrushing()
{
    if (!m_d->painterInfos.isEmpty()) {
        for (int i = 0; i < m_d->painterInfos.size(); ++i) {
            paintAt(i, m_d->previousPaintInformations[i]);
        }

    }
}

void KisToolFreehandHelper::paintAt(int painterInfoIndex,
                                    const KisPaintInformation &pi)
{
    PainterInfo *painterInfo = m_d->painterInfos[painterInfoIndex];
    KisPaintInformation __pi = pi;

    if (m_d->transformations.size() > painterInfoIndex) {
        const QTransform &transform = m_d->transformations[painterInfoIndex];
        __pi.setPos(transform.map(__pi.pos()));
    }

    m_d->hasPaintAtLeastOnce = true;
    m_d->strokesFacade->addJob(m_d->strokeId,
        new FreehandStrokeStrategy::Data(m_d->resources->currentNode(),
                                         painterInfo, __pi));

    if (m_d->recordingAdapter) {
        m_d->recordingAdapter->addPoint(__pi);
    }
}

void KisToolFreehandHelper::paintLine(int painterInfoIndex,
                                      const KisPaintInformation &pi1,
                                      const KisPaintInformation &pi2)
{
    m_d->hasPaintAtLeastOnce = true;

    PainterInfo *painterInfo = m_d->painterInfos[painterInfoIndex];
    KisPaintInformation __pi1 = pi1;
    KisPaintInformation __pi2 = pi2;

    if (m_d->transformations.size() > painterInfoIndex) {
        const QTransform &transform = m_d->transformations[painterInfoIndex];

        __pi1.setPos(transform.map(__pi1.pos()));
        __pi2.setPos(transform.map(__pi2.pos()));
    }

    m_d->strokesFacade->addJob(m_d->strokeId,
        new FreehandStrokeStrategy::Data(m_d->resources->currentNode(),
                                         painterInfo, __pi1, __pi2));

    if (m_d->recordingAdapter) {
        m_d->recordingAdapter->addLine(__pi1, __pi2);
    }
}

void KisToolFreehandHelper::paintBezierCurve(int painterInfoIndex,
                                             const KisPaintInformation &pi1,
                                             const QPointF &control1,
                                             const QPointF &control2,
                                             const KisPaintInformation &pi2)
{
    m_d->hasPaintAtLeastOnce = true;

    PainterInfo *painterInfo = m_d->painterInfos[painterInfoIndex];
    KisPaintInformation __pi1 = pi1;
    KisPaintInformation __pi2 = pi2;
    QPointF __control1 = control1;
    QPointF __control2 = control2;

    if (m_d->transformations.size() > painterInfoIndex) {
        const QTransform &transform = m_d->transformations[painterInfoIndex];

        __pi1.setPos(transform.map(__pi1.pos()));
        __pi2.setPos(transform.map(__pi2.pos()));

        __control1 = transform.map(control1);
        __control2 = transform.map(control2);
    }

    m_d->strokesFacade->addJob(m_d->strokeId,
        new FreehandStrokeStrategy::Data(m_d->resources->currentNode(),
                                         painterInfo,
                                         __pi1, __control1, __control2, __pi2));

    if (m_d->recordingAdapter) {
        m_d->recordingAdapter->addCurve(__pi1, __control1, __control2, __pi2);
    }
}
