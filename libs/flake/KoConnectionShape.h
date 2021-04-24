/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2007 Boudewijn Rempt <boud@kde.org>
 * SPDX-FileCopyrightText: 2007 Thorsten Zachmann <zachmann@kde.org>
 * SPDX-FileCopyrightText: 2007 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2009 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KO_CONNECTION_SHAPE_H
#define KO_CONNECTION_SHAPE_H

#include "KoParameterShape.h"

#include "flake_export.h"

#define KOCONNECTIONSHAPEID "KoConnectionShape"

class KoConnectionShapePrivate;

/// API docs go here
class FLAKE_EXPORT KoConnectionShape : public KoParameterShape
{
public:
    enum Type {
        Standard, ///< escapes connected shapes with straight lines, connects with perpendicular lines
        Lines,    ///< escapes connected shapes with straight lines, connects with straight line
        Straight, ///< one straight line between connected shapes
        Curve     ///< a single curved line between connected shapes
    };

    // IDs of the connecting handles
    enum HandleId {
        StartHandle,
        EndHandle,
        ControlHandle_1,
        ControlHandle_2,
        ControlHandle_3
    };

    KoConnectionShape();
    ~KoConnectionShape() override;

    // reimplemented
    void saveOdf(KoShapeSavingContext &context) const override;

    // reimplemented
    bool loadOdf(const KoXmlElement &element, KoShapeLoadingContext &context) override;

    // reimplemented
    QString pathShapeId() const override;

    /**
     * Sets the first shape this connector is connected to
     *
     * Passing a null pointer as the first parameter will sever the connection.
     *
     * @param shape the shape to connect to or null to reset the connection
     * @param connectionPointId the id of the connection point to connect to
     * @return true if connection could be established, otherwise false
     */
    bool connectFirst(KoShape *shape, int connectionPointId);

    /**
    * Sets the second shape the connector is connected to
    *
    * Passing a null pointer as the first parameter will sever the connection.
    *
    * @param shape the shape to connect to or null to reset the connection
    * @param connectionPointId the id of the connection point to connect to
    * @return true if connection could be established, otherwise false
    */
    bool connectSecond(KoShape *shape, int connectionPointId);

    /**
     * Return the first shape this connection is attached to, or null if none.
     */
    KoShape *firstShape() const;

    /**
     * Return the connection point id of the first shape we are connected to.
     * In case we are not connected to a first shape the return value is undefined.
     * @see firstShape(), KoShape::connectionPoints()
     */
    int firstConnectionId() const;

    /**
     * Return the second shape this connection is attached to, or null if none.
     */
    KoShape *secondShape() const;

    /**
     * Return the connection point id of the second shape we are connected to.
     * In case we are not connected to a second shape the return value is undefined.
     * @see firstShape(), KoShape::connectionPoints()
     */
    int secondConnectionId() const;

    /**
     * Finishes the loading of a connection.
     */
    void finishLoadingConnection();

    /// Returns connection type
    Type type() const;

    /// Sets the connection type
    void setType(Type connectionType);

    /// Updates connections to shapes
    void updateConnections();

protected:
    /// reimplemented
    void moveHandleAction(int handleId, const QPointF &point, Qt::KeyboardModifiers modifiers = Qt::NoModifier) override;

    /// reimplemented
    void updatePath(const QSizeF &size) override;

    /// reimplemented
    void shapeChanged(ChangeType type, KoShape *shape) override;

private:
    Q_DECLARE_PRIVATE(KoConnectionShape)
};

#endif
