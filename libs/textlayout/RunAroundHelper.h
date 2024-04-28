/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2006-2007, 2010 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2010 Ko Gmbh <cbo@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef RUNAROUNDHELPER_H
#define RUNAROUNDHELPER_H

#include <QList>
#include <QRectF>
#include <QTextLine>

class KoTextLayoutArea;
class KoTextLayoutObstruction;

class RunAroundHelper
{
public:
    RunAroundHelper();
    void setLine(KoTextLayoutArea *area, const QTextLine &l);
    void setObstructions(const QList<KoTextLayoutObstruction *> &obstructions);
    bool stayOnBaseline() const;
    void updateObstruction(KoTextLayoutObstruction *obstruction);
    bool fit(bool resetHorizontalPosition, bool isRightToLeft, const QPointF &position);
    QTextLine line;

private:
    KoTextLayoutArea *m_area;
    QList<KoTextLayoutObstruction *> m_obstructions;
    QList<KoTextLayoutObstruction *> m_validObstructions;
    QVector<QRectF> m_lineParts;
    QRectF m_lineRect;
    qreal m_horizontalPosition;
    bool m_updateValidObstructions;
    bool m_stayOnBaseline;
    qreal m_textWidth;
    void validateObstructions();
    void validateObstruction(KoTextLayoutObstruction *obstruction);
    void createLineParts();
    QRectF minimizeHeightToLeastNeeded(const QRectF &lineRect);
    void updateLineParts(const QRectF &lineRect);
    QRectF getLineRectPart();
    void setMaxTextWidth(const QRectF &minLineRectPart, const qreal leftIndent, const qreal maxNaturalTextWidth);
    QRectF getLineRect(const QRectF &lineRect, const qreal maxNaturalTextWidth);
    void checkEndOfLine(const QRectF &lineRectPart, const qreal maxNaturalTextWidth);
};

#endif
