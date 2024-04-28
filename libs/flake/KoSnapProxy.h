/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008-2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef KOSNAPPROXY_H
#define KOSNAPPROXY_H

class KoSnapGuide;
class KoShape;
class KoPathSegment;
class KoCanvasBase;
#include "flake_export.h"
#include <QList>
#include <QVector>

class QPointF;
class QRectF;
/**
 * This class provides access to different shape related snap targets to snap strategies.
 */
class FLAKE_EXPORT KoSnapProxy
{
public:
    explicit KoSnapProxy(KoSnapGuide *snapGuide);

    /// returns list of points in given rectangle in document coordinates
    QVector<QPointF> pointsInRect(const QRectF &rect) const;

    /// returns list of shape in given rectangle in document coordinates
    QList<KoShape *> shapesInRect(const QRectF &rect, bool omitEditedShape = false) const;

    /// returns list of points from given shape
    QVector<QPointF> pointsFromShape(KoShape *shape) const;

    /// returns list of points in given rectangle in document coordinates
    QList<KoPathSegment> segmentsInRect(const QRectF &rect) const;

    /// returns list of all shapes
    QList<KoShape *> shapes(bool omitEditedShape = false) const;

    /// returns canvas we are working on
    KoCanvasBase *canvas() const;

private:
    KoSnapGuide *m_snapGuide;
};

#endif
