/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2006-2007 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2006-2008 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2007, 2009 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2012 Inge Wallin <inge@lysator.liu.se>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOSHAPESTROKE_H
#define KOSHAPESTROKE_H

#include "KoInsets.h"
#include "KoShapeStrokeModel.h"

#include "flake_export.h"

#include <QColor>
#include <QMetaType>

class KoShape;
class QPainter;
class QBrush;
class KoViewConverter;

/**
 * A border for shapes that draws a single line around the object.
 */
class FLAKE_EXPORT KoShapeStroke : public KoShapeStrokeModel
{
public:
    /// Constructor for a thin line in black
    KoShapeStroke();

    /// Copy constructor
    KoShapeStroke(const KoShapeStroke &other);

    /**
     * Constructor for a Stroke
     * @param lineWidth the width, in pt
     * @param color the color we draw the outline in.
     */
    explicit KoShapeStroke(qreal lineWidth, const QColor &color = Qt::black);
    ~KoShapeStroke() override;

    /// Assignment operator
    KoShapeStroke &operator=(const KoShapeStroke &rhs);

    /// Sets the lines cap style
    void setCapStyle(Qt::PenCapStyle style);
    /// Returns the lines cap style
    Qt::PenCapStyle capStyle() const;
    /// Sets the lines join style
    void setJoinStyle(Qt::PenJoinStyle style);
    /// Returns the lines join style
    Qt::PenJoinStyle joinStyle() const;
    /// Sets the line width
    void setLineWidth(qreal lineWidth);
    /// Returns the line width
    qreal lineWidth() const;
    /// Sets the miter limit
    void setMiterLimit(qreal miterLimit);
    /// Returns the miter limit
    qreal miterLimit() const;
    /// Sets the line style
    void setLineStyle(Qt::PenStyle style, const QVector<qreal> &dashes);
    /// Returns the line style
    Qt::PenStyle lineStyle() const;
    /// Returns the line dashes
    QVector<qreal> lineDashes() const;
    /// Sets the dash offset
    void setDashOffset(qreal dashOffset);
    /// Returns the dash offset
    qreal dashOffset() const;

    /// Returns the color
    QColor color() const;
    /// Sets the color
    void setColor(const QColor &color);

    /// Sets the strokes brush used to fill strokes of this border
    void setLineBrush(const QBrush &brush);
    /// Returns the strokes brush
    QBrush lineBrush() const;

    // pure virtuals from KoShapeStrokeModel implemented here.
    void fillStyle(KoGenStyle &style, KoShapeSavingContext &context) const override;
    void strokeInsets(const KoShape *shape, KoInsets &insets) const override;
    bool hasTransparency() const override;
    void paint(KoShape *shape, QPainter &painter, const KoViewConverter &converter) override;
    void paint(KoShape *shape, QPainter &painter, const KoViewConverter &converter, const QColor &color) override;

private:
    class Private;
    Private *const d;
};

Q_DECLARE_METATYPE(KoShapeStroke)

#endif // KOSHAPESTROKE_H
