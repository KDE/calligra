/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Marijn Kruisselbrink <mkruisselbrink@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KPrMatrixWipeStrategy.h"

#include <QPainter>
#include <QWidget>

static const int squaresPerRow = 15;
static const int squaresPerCol = 11;
static const int framesPerSquare = 16;

KPrMatrixWipeStrategy::KPrMatrixWipeStrategy(int subType, const char *smilType, const char *smilSubType, bool reverse, bool smooth)
    : KPrPageEffectStrategy(subType, smilType, smilSubType, reverse)
    , m_smooth(smooth)
    , m_squaresPerRow(squaresPerRow)
    , m_squaresPerCol(squaresPerCol)
{
}

KPrMatrixWipeStrategy::~KPrMatrixWipeStrategy() = default;

void KPrMatrixWipeStrategy::setNeedEvenSquares(bool hor, bool vert)
{
    if (hor)
        m_squaresPerRow++;
    if (vert)
        m_squaresPerCol++;
}

void KPrMatrixWipeStrategy::setup(const KPrPageEffect::Data &data, QTimeLine &timeLine)
{
    Q_UNUSED(data);
    timeLine.setFrameRange(0, (m_smooth ? framesPerSquare : 1) * maxIndex(m_squaresPerRow, m_squaresPerCol));
}

// TODO: check if these can be replaced by math function 1:1, named floor and ceil before
static inline int intFloor(double d)
{
    return (int)(d + 1e-5);
}
static inline int intCeil(double d)
{
    return (int)(d + 1 - 1e-5);
}

static QRect tileRect(KPrMatrixWipeStrategy::Direction direction, int step, const QRect &base)
{
    switch (direction) {
    case KPrMatrixWipeStrategy::TopToBottom:
        return QRect(base.topLeft(), QSize(base.width(), base.height() * step / framesPerSquare));
    case KPrMatrixWipeStrategy::BottomToTop:
        return QRect(QPoint(base.left(), base.bottom() - base.height() * step / framesPerSquare), base.bottomRight());
    case KPrMatrixWipeStrategy::LeftToRight:
        return QRect(base.topLeft(), QSize(base.width() * step / framesPerSquare, base.height()));
    case KPrMatrixWipeStrategy::RightToLeft:
        return QRect(QPoint(base.right() - base.width() * step / framesPerSquare, base.top()), base.bottomRight());
    default:
        return base;
    }
}

void KPrMatrixWipeStrategy::paintStep(QPainter &p, int currPos, const KPrPageEffect::Data &data)
{
    int width = data.m_widget->width();
    int height = data.m_widget->height();

    int curSquare = currPos / (m_smooth ? framesPerSquare : 1);

    for (int i = 0; i < m_squaresPerRow; ++i) {
        for (int j = 0; j < m_squaresPerCol; ++j) {
            QRect rect(intFloor(qreal(width) / m_squaresPerRow * i),
                       intFloor(qreal(height) / m_squaresPerCol * j),
                       intCeil(qreal(width) / m_squaresPerRow),
                       intCeil(qreal(height) / m_squaresPerCol));
            int square = squareIndex(i, j, m_squaresPerRow, m_squaresPerCol);
            if (square <= curSquare) {
                if (square == curSquare && m_smooth) {
                    int squarePos = currPos % framesPerSquare;
                    p.drawPixmap(rect.topLeft(), data.m_oldPage, rect);
                    rect = tileRect(squareDirection(i, j, m_squaresPerRow, m_squaresPerCol), squarePos, rect);
                    if (rect.width() > 0 && rect.height() > 0) {
                        p.drawPixmap(rect.topLeft(), data.m_newPage, rect);
                    }
                } else {
                    p.drawPixmap(rect.topLeft(), data.m_newPage, rect);
                }
            } else {
                p.drawPixmap(rect.topLeft(), data.m_oldPage, rect);
            }
        }
    }
}

void KPrMatrixWipeStrategy::next(const KPrPageEffect::Data &data)
{
    int lastPos = data.m_timeLine.frameForTime(data.m_lastTime);
    int currPos = data.m_timeLine.frameForTime(data.m_currentTime);

    int width = data.m_widget->width();
    int height = data.m_widget->height();

    int curSquare = currPos / (m_smooth ? framesPerSquare : 1);
    int lastSquare = lastPos / (m_smooth ? framesPerSquare : 1);

    for (int i = 0; i < m_squaresPerRow; ++i) {
        for (int j = 0; j < m_squaresPerCol; ++j) {
            QRect rect(intFloor(qreal(width) / m_squaresPerRow * i),
                       intFloor(qreal(height) / m_squaresPerCol * j),
                       intCeil(qreal(width) / m_squaresPerRow),
                       intCeil(qreal(height) / m_squaresPerCol));
            int square = squareIndex(i, j, m_squaresPerRow, m_squaresPerCol);
            if (square <= curSquare && square >= lastSquare) {
                data.m_widget->update(rect);
            }
        }
    }
}

KPrMatrixWipeStrategy::Direction KPrMatrixWipeStrategy::squareDirection(int x, int y, int columns, int rows)
{
    Q_UNUSED(x);
    Q_UNUSED(y);
    Q_UNUSED(columns);
    Q_UNUSED(rows);
    return NotSmooth;
}
