/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "ArtisticTextToolSelection.h"
#include "ArtisticTextShape.h"

#include <KoCanvasBase.h>
#include <KoViewConverter.h>

#include <QDebug>

#include <QFontMetrics>
#include <QPainter>

ArtisticTextToolSelection::ArtisticTextToolSelection(KoCanvasBase *canvas, QObject *parent)
    : KoToolSelection(parent)
    , m_canvas(canvas)
    , m_currentShape(nullptr)
    , m_selectionStart(-1)
    , m_selectionCount(0)
{
    Q_ASSERT(m_canvas);
}

ArtisticTextToolSelection::~ArtisticTextToolSelection() = default;

bool ArtisticTextToolSelection::hasSelection()
{
    return m_currentShape && m_selectionCount > 0;
}

void ArtisticTextToolSelection::setSelectedShape(ArtisticTextShape *textShape)
{
    if (textShape == m_currentShape)
        return;
    clear();
    m_currentShape = textShape;
}

ArtisticTextShape *ArtisticTextToolSelection::selectedShape() const
{
    return m_currentShape;
}

void ArtisticTextToolSelection::selectText(int from, int to)
{
    if (!m_currentShape)
        return;

    repaintDecoration();

    const int textCount = m_currentShape->plainText().count();
    m_selectionStart = qBound(0, from, textCount - 1);
    m_selectionCount = qBound(from, to, textCount) - m_selectionStart;

    repaintDecoration();
}

int ArtisticTextToolSelection::selectionStart() const
{
    return m_selectionStart;
}

int ArtisticTextToolSelection::selectionCount() const
{
    return m_selectionCount;
}

void ArtisticTextToolSelection::clear()
{
    repaintDecoration();
    m_selectionStart = -1;
    m_selectionCount = 0;
}

void ArtisticTextToolSelection::paint(QPainter &painter, const KoViewConverter &converter)
{
    if (!hasSelection())
        return;

    m_currentShape->applyConversion(painter, converter);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 255, 127));
    painter.drawPath(outline());
}

QPainterPath ArtisticTextToolSelection::outline()
{
    if (!hasSelection())
        return QPainterPath();

    CharIndex charPos = m_currentShape->indexOfChar(m_selectionStart);
    if (charPos.first < 0)
        return QPainterPath();

    QPainterPath outline;

    QPolygonF polygon;

    QList<ArtisticTextRange> ranges = m_currentShape->text();
    if (ranges.size() == 0)
        return outline;

    int globalCharIndex = m_selectionStart;
    int remainingChars = m_selectionCount;
    while (remainingChars && ranges.size() > charPos.first) {
        const ArtisticTextRange &currentRange = ranges[charPos.first];

        int currentTextLength = currentRange.text().length();
        while (charPos.second < currentTextLength && remainingChars > 0) {
            const QPointF pos = m_currentShape->charPositionAt(globalCharIndex);
            const qreal angle = m_currentShape->charAngleAt(globalCharIndex);

            QTransform charTransform;
            charTransform.translate(pos.x() - 1, pos.y());
            charTransform.rotate(360. - angle);

            QFontMetricsF metrics(currentRange.font());

            polygon.prepend(charTransform.map(QPointF(0.0, -metrics.ascent())));
            polygon.append(charTransform.map(QPointF(0.0, metrics.descent())));

            // advance to next character
            charPos.second++;
            globalCharIndex++;
            remainingChars--;

            // next character has y-offset or we are at the end of this text range
            const bool hasYOffset = currentRange.hasYOffset(charPos.second);
            const bool atRangeEnd = charPos.second == currentTextLength;
            const bool atSelectionEnd = remainingChars == 0;
            if (hasYOffset || atRangeEnd || atSelectionEnd) {
                if (hasYOffset || atRangeEnd) {
                    const QChar c = currentRange.text().at(charPos.second - 1);
                    const qreal w = metrics.boundingRect(c).width();
                    polygon.prepend(charTransform.map(QPointF(w, -metrics.ascent())));
                    polygon.append(charTransform.map(QPointF(w, metrics.descent())));
                } else {
                    const QPointF pos = m_currentShape->charPositionAt(globalCharIndex);
                    const qreal angle = m_currentShape->charAngleAt(globalCharIndex);
                    charTransform.reset();
                    charTransform.translate(pos.x() - 1, pos.y());
                    charTransform.rotate(360. - angle);
                    polygon.prepend(charTransform.map(QPointF(0.0, -metrics.ascent())));
                    polygon.append(charTransform.map(QPointF(0.0, metrics.descent())));
                }
                QPainterPath p;
                p.addPolygon(polygon);
                outline = outline.united(p);
                polygon.clear();
            }
        }

        // go to first character of next text range
        charPos.first++;
        charPos.second = 0;
    }

    // transform to document coordinates
    return m_currentShape->absoluteTransformation(nullptr).map(outline);
}

void ArtisticTextToolSelection::repaintDecoration()
{
    if (hasSelection())
        m_canvas->updateCanvas(outline().boundingRect());
}
