/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 * SPDX-FileCopyrightText: 2009 Carlos Licea <carlos.licea@kdemail.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef GUIDESTOOL_H
#define GUIDESTOOL_H

#include <KoToolBase.h>

#include <QPair>
#include <QString>

class KoCanvasBase;
class GuidesTransaction;
class InsertGuidesToolOptionWidget;
class GuidesToolOptionWidget;

class GuidesTool : public KoToolBase
{
    Q_OBJECT

public:
    explicit GuidesTool(KoCanvasBase *canvas);
    ~GuidesTool() override;
    /// reimplemented form KoToolBase
    void paint(QPainter &painter, const KoViewConverter &converter) override;
    /// reimplemented form KoToolBase
    void mousePressEvent(KoPointerEvent *event) override;
    /// reimplemented form KoToolBase
    void mouseMoveEvent(KoPointerEvent *event) override;
    /// reimplemented form KoToolBase
    void mouseReleaseEvent(KoPointerEvent *event) override;
    /// reimplemented form KoToolBase
    void mouseDoubleClickEvent(KoPointerEvent *event) override;
    /// reimplemented form KoToolBase
    void repaintDecorations() override;
    /// reimplemented form KoToolBase
    void activate(ToolActivation toolActivation, const QSet<KoShape *> &shapes) override;
    /// reimplemented form KoToolBase
    void deactivate() override;

    /// Sets up tool state to move the specified guide line
    void moveGuideLine(Qt::Orientation orientation, int index);

    /// Sets up tool state to edit the specified guide line
    void editGuideLine(Qt::Orientation orientation, int index);

public Q_SLOTS:
    /// Sets up tool state to create a new guide line and activates the tool
    void createGuideLine(Qt::Orientation orientation, qreal position);

protected:
    /// reimplemented form KoToolBase
    QList<QPointer<QWidget>> createOptionWidgets() override;

private Q_SLOTS:
    void updateGuidePosition(qreal position);
    void guideLineSelected(Qt::Orientation orientation, int index);
    void guideLinesChanged(Qt::Orientation orientation);
    /// reimplemented from KoToolBase
    void canvasResourceChanged(int key, const QVariant &res) override;

    void insertorCreateGuidesSlot(GuidesTransaction *result);

private:
    typedef QPair<Qt::Orientation, int> GuideLine;
    GuideLine guideLineAtPosition(const QPointF &position);

    /// Calculates update rectangle for specified guide line
    QRectF updateRectFromGuideLine(qreal position, Qt::Orientation orientation);

    enum EditMode { AddGuide, MoveGuide, EditGuide };
    Qt::Orientation m_orientation;
    int m_index;
    qreal m_position;
    EditMode m_mode;
    GuidesToolOptionWidget *m_options;
    InsertGuidesToolOptionWidget *m_insert;
    bool m_isMoving;
};

#endif // GUIDESTOOL_H
