/* This file is part of the KDE project
 *
 * SPDX-FileCopyrightText: 2009 Jean-Nicolas Artaud <jeannicolasartaud@gmail.com>
 * SPDX-FileCopyrightText: 2011 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KO_CONNECTION_TOOL_H
#define KO_CONNECTION_TOOL_H

#define ConnectionTool_ID "ConnectionTool"

#include <KoCanvasBase.h>
#include <KoConnectionShape.h>
#include <KoSnapGuide.h>
#include <KoToolBase.h>
#include <QActionGroup>
#include <QCursor>

class QAction;
class QActionGroup;
class KoShapeConfigWidgetBase;
class KoInteractionStrategy;

class ConnectionTool : public KoToolBase
{
    Q_OBJECT
public:
    /**
     * @brief Constructor
     */
    explicit ConnectionTool(KoCanvasBase *canvas);
    /**
     * @brief Destructor
     */
    ~ConnectionTool() override;

    /// reimplemented from superclass
    void paint(QPainter &painter, const KoViewConverter &converter) override;
    /// reimplemented from superclass
    void repaintDecorations() override;

    /// reimplemented from superclass
    void mousePressEvent(KoPointerEvent *event) override;
    /// reimplemented from superclass
    void mouseMoveEvent(KoPointerEvent *event) override;
    /// reimplemented from superclass
    void mouseReleaseEvent(KoPointerEvent *event) override;
    /// reimplemented from superclass
    void mouseDoubleClickEvent(KoPointerEvent *event) override;
    /// reimplemented from superclass
    void keyPressEvent(QKeyEvent *event) override;
    /// reimplemented from superclass
    void activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes) override;
    /// reimplemented from superclass
    void deactivate() override;
    /// reimplemented from superclass
    void deleteSelection() override;

Q_SIGNALS:
    void connectionPointEnabled(bool enabled);
    void sendConnectionType(int type);
    void sendConnectionPointEditState(bool enabled);

public Q_SLOTS:
    void toggleConnectionPointEditMode(int state);

private Q_SLOTS:
    void horizontalAlignChanged();
    void verticalAlignChanged();
    void relativeAlignChanged();
    void escapeDirectionChanged();
    void connectionChanged();
    void getConnectionType(int type);
    void slotShapeRemoved(KoShape *);

private:
    /// reimplemented from superclass
    QList<QPointer<QWidget>> createOptionWidgets() override;

    /**
     * @brief Return the square of the absolute distance between p1 and p2
     *
     * @param p1 The first point
     * @param p2 The second point
     * @return The float which is the square of the distance
     */
    qreal squareDistance(const QPointF &p1, const QPointF &p2) const;

    /// Returns nearest connection handle or nearest connection point id of shape
    int handleAtPoint(KoShape *shape, const QPointF &mousePoint) const;

    enum EditMode {
        Idle, ///< in idle mode we can only start a connector creation, manipulation to existing connectors and connection points not allowed
        CreateConnection, ///< we are creating a new connection
        EditConnection, ///< we are editing a connection
        EditConnectionPoint ///< we are editing connection points
    };

    /// Sets the edit mode, current shape and active handle
    void setEditMode(EditMode mode, KoShape *currentShape, int handle);

    /// Resets the current edit mode to Idle, standard connector type
    void resetEditMode();

    /// Returns the nearest connection shape within handle grab sensitivity distance
    KoConnectionShape *nearestConnectionShape(const QList<KoShape *> &shapes, const QPointF &mousePos) const;

    /// Updates status text depending on edit mode
    void updateStatusText();

    /// Updates current shape and edit mode dependent on position
    KoShape *findShapeAtPosition(const QPointF &position) const;

    /// Updates current shape and edit mode dependent on position excluding connection shapes
    KoShape *findNonConnectionShapeAtPosition(const QPointF &position) const;

    /// Updates actions
    void updateActions();

    /// Updates currently selected connection point
    void updateConnectionPoint();

    EditMode m_editMode; ///< the current edit mode
    KoConnectionShape::Type m_connectionType;
    KoShape *m_currentShape; ///< the current shape we are working on
    int m_activeHandle; ///< the currently active connection point/connection handle
    KoInteractionStrategy *m_currentStrategy; ///< the current editing strategy
    KoSnapGuide::Strategies m_oldSnapStrategies; ///< the previously enables snap strategies
    bool m_resetPaint; ///< whether in initial paint mode
    QCursor m_connectCursor;

    QActionGroup *m_alignVertical;
    QActionGroup *m_alignHorizontal;
    QActionGroup *m_alignRelative;
    QActionGroup *m_escapeDirections;

    QAction *m_editConnectionPoint;

    QAction *m_alignPercent;
    QAction *m_alignLeft;
    QAction *m_alignCenterH;
    QAction *m_alignRight;
    QAction *m_alignTop;
    QAction *m_alignCenterV;
    QAction *m_alignBottom;

    QAction *m_escapeAll;
    QAction *m_escapeHorizontal;
    QAction *m_escapeVertical;
    QAction *m_escapeUp;
    QAction *m_escapeLeft;
    QAction *m_escapeDown;
    QAction *m_escapeRight;

    QList<KoShapeConfigWidgetBase *> m_connectionShapeWidgets;
};

#endif // KO_CONNECTION_TOOL_H
