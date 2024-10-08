/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Carlos Licea <carlos.licea@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrIrisWipeEffectStrategyBase.h"

#include <QPainter>
#include <QWidget>

KPrIrisWipeEffectStrategyBase::KPrIrisWipeEffectStrategyBase(QPainterPath shape, int subType, const char *smilType, const char *smilSubType, bool reverse)
    : KPrPageEffectStrategy(subType, smilType, smilSubType, reverse)
    , m_shape(shape)
{
}

KPrIrisWipeEffectStrategyBase::~KPrIrisWipeEffectStrategyBase() = default;

int KPrIrisWipeEffectStrategyBase::findMaxScaling(const KPrPageEffect::Data &data)
{
    const int width = data.m_widget->width();
    const int height = data.m_widget->height();
    QPainterPath widget;
    widget.addRect(0, 0, width, height);

    int pathMaxMeasure;
    int maxMeasure;
    // We find whether the screen is taller or wider so that we can start searching
    // from a closer point
    if (width > height) {
        pathMaxMeasure = m_shape.boundingRect().width();
        maxMeasure = width;
    } else {
        pathMaxMeasure = m_shape.boundingRect().height();
        maxMeasure = height;
    }

    // We now search from the previous point and increasing over and over till the shape fills
    // the widget given
    int halfWidth = width / 2;
    int halfHeight = height / 2;
    QPainterPath path;
    while (!path.contains(widget)) {
        QTransform matrix;
        matrix.translate(halfWidth, halfHeight);
        double maxScaling = (double)maxMeasure / (double)pathMaxMeasure;
        matrix.scale(maxScaling, maxScaling);
        path = matrix.map(m_shape);
        maxMeasure += 5; // we don't need to be very precise
    }

    return maxMeasure;
}

void KPrIrisWipeEffectStrategyBase::setup(const KPrPageEffect::Data &data, QTimeLine &timeLine)
{
    timeLine.setFrameRange(0, findMaxScaling(data));
}

void KPrIrisWipeEffectStrategyBase::paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data)
{
    const int width = data.m_widget->width();
    const int height = data.m_widget->height();
    qreal scaleStep;
    qreal fullScale = findMaxScaling(data);
    if (width > height) {
        scaleStep = 1 / m_shape.boundingRect().width();
    } else {
        scaleStep = 1 / m_shape.boundingRect().height();
    }

    QPoint pointZero(0, 0);
    if (!reverse()) {
        QRect rect(0, 0, width, height);
        p.drawPixmap(pointZero, data.m_oldPage, rect);

        QTransform matrix;
        matrix.translate(width / 2, height / 2);
        matrix.scale(currPos * scaleStep, currPos * scaleStep);

        p.setClipPath(matrix.map(m_shape));
        p.drawPixmap(pointZero, data.m_newPage, rect);
    } else {
        QRect rect(0, 0, width, height);
        p.drawPixmap(pointZero, data.m_newPage, rect);

        QTransform matrix;
        matrix.translate(width / 2, height / 2);
        matrix.scale((fullScale - currPos) * scaleStep, (fullScale - currPos) * scaleStep);

        p.setClipPath(matrix.map(m_shape));
        p.drawPixmap(pointZero, data.m_oldPage, rect);
    }
}

void KPrIrisWipeEffectStrategyBase::next(const KPrPageEffect::Data &data)
{
    const int width = data.m_widget->width();
    const int height = data.m_widget->height();
    const int currPos = data.m_timeLine.frameForTime(data.m_currentTime);
    const int lastPos = data.m_timeLine.frameForTime(data.m_lastTime);

    qreal scaleStep;
    qreal fullScale = findMaxScaling(data);
    if (width > height) {
        scaleStep = 1 / m_shape.boundingRect().width();
    } else {
        scaleStep = 1 / m_shape.boundingRect().height();
    }

    QPainterPath newPath;
    if (!reverse()) {
        QTransform matrix;
        matrix.translate(width / 2, height / 2);
        matrix.scale(currPos * scaleStep, currPos * scaleStep);
        newPath = matrix.map(m_shape);
    } else {
        QTransform matrix;
        matrix.translate(width / 2, height / 2);
        matrix.scale((fullScale - lastPos - 3) * scaleStep, (fullScale - lastPos - 3) * scaleStep);
        // NOTE: i have no idea why I can't just update the old rectangle and have to do this small hack (update a bigger rectangle) instead, we think that
        // might be something floating-point related, but i think this will work just fine
        newPath = matrix.map(m_shape);
    }

    data.m_widget->update(newPath.boundingRect().toRect().adjusted(-2, -2, 2, 2));
}
