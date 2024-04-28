/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Thomas Zander <zander@kde.org>
 * SPDX-FileCopyrightText: 2010 Boudewijn Rempt <boud@kogmbh.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOSHAPEPRIVATE_H
#define KOSHAPEPRIVATE_H

#include "KoShape.h"

#include <QPaintDevice>
#include <QPoint>
#include <QTransform>

#include <KoCanvasBase.h>

class KoBorder;

class KoShapePrivate
{
public:
    explicit KoShapePrivate(KoShape *shape);
    virtual ~KoShapePrivate();
    /**
     * Notify the shape that a change was done. To be used by inheriting shapes.
     * @param type the change type
     */
    void shapeChanged(KoShape::ChangeType type);

    void addShapeManager(KoShapeManager *manager);
    void removeShapeManager(KoShapeManager *manager);

    /**
     * Fills the style stack and returns the value of the given style property (e.g fill, stroke).
     */
    static QString getStyleProperty(const char *property, KoShapeLoadingContext &context);

    /// Loads the shadow style
    KoShapeShadow *loadOdfShadow(KoShapeLoadingContext &context) const;

    // Loads the border style.
    KoBorder *loadOdfBorder(KoShapeLoadingContext &context) const;

    /// calls update on the shape where the stroke is.
    void updateStroke();

    // Members

    KoShape *q_ptr; // Points the shape that owns this class.

    mutable QSizeF size; // size in pt
    QString shapeId;
    QString name; ///< the shapes names

    QTransform localMatrix; ///< the shapes local transformation matrix

    KoConnectionPoints connectors; ///< glue point id to data mapping

    KoShapeContainer *parent;
    QSet<KoShapeManager *> shapeManagers;
    QSet<KoShape *> toolDelegates;
    KoShapeUserData *userData;
    KoShapeApplicationData *appData;
    KoShapeStrokeModel *stroke; ///< points to a stroke, or 0 if there is no stroke
    QSharedPointer<KoShapeBackground> fill; ///< Stands for the background color / fill etc.
    QList<KoShape *> dependees; ///< list of shape dependent on this shape
    KoShapeShadow *shadow; ///< the current shape shadow
    KoBorder *border; ///< the current shape border
    KoClipPath *clipPath; ///< the current clip path
    QMap<QString, QString> additionalAttributes;
    QMap<QByteArray, QString> additionalStyleAttributes;
    QSet<KoEventAction *> eventActions; ///< list of event actions the shape has
    KoFilterEffectStack *filterEffectStack; ///< stack of filter effects applied to the shape
    qreal transparency; ///< the shapes transparency
    QString hyperLink; // hyperlink for this shape

    static const int MaxZIndex = 32767;
    int zIndex : 16; // keep maxZIndex in sync!
    int runThrough : 16;
    int visible : 1;
    int printable : 1;
    int keepAspect : 1;
    int detectCollision : 1;

    KoShape::TextRunAroundSide textRunAroundSide;
    qreal textRunAroundDistanceLeft;
    qreal textRunAroundDistanceTop;
    qreal textRunAroundDistanceRight;
    qreal textRunAroundDistanceBottom;
    qreal textRunAroundThreshold;
    KoShape::TextRunAroundContour textRunAroundContour;
    KoShapeAnchor *anchor;
    qreal minimumHeight;

    /// Convert connection point position from shape coordinates, taking alignment into account
    void convertFromShapeCoordinates(KoConnectionPoint &point, const QSizeF &shapeSize) const;

    /// Convert connection point position to shape coordinates, taking alignment into account
    void convertToShapeCoordinates(KoConnectionPoint &point, const QSizeF &shapeSize) const;

    KoShape::AllowedInteractions allowedInteractions;

    Q_DECLARE_PUBLIC(KoShape)
};

#endif
