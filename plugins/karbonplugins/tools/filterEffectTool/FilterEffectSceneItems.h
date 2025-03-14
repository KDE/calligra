/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef FILTEREFFECTSCENEITEMS_H
#define FILTEREFFECTSCENEITEMS_H

#include <QGraphicsRectItem>
#include <QGraphicsSceneMouseEvent>
#include <QMimeData>

class KoFilterEffect;

/// Graphics item representing a connector (input/output)
class ConnectorItem : public QGraphicsEllipseItem
{
public:
    enum ConnectorType {
        Input,
        Output
    };

    ConnectorItem(ConnectorType type, int index, QGraphicsItem *parent);
    void setCenter(const QPointF &position);
    ConnectorType connectorType();
    int connectorIndex() const;
    KoFilterEffect *effect() const;

private:
    ConnectorType m_type;
    int m_index;
};

/// Custom mime data for connector drag and drop
class ConnectorMimeData : public QMimeData
{
public:
    explicit ConnectorMimeData(ConnectorItem *connector);
    ConnectorItem *connector() const;

private:
    ConnectorItem *m_connector;
};

/// Base class for effect items
class EffectItemBase : public QGraphicsRectItem
{
public:
    explicit EffectItemBase(KoFilterEffect *effect);

    /// Returns the position of the output connector
    QPointF outputPosition() const;

    /// Returns the position of the specified input connector
    QPointF inputPosition(int index) const;

    /// Returns the name of the output
    QString outputName() const;

    /// Returns the size of the connectors
    QSizeF connectorSize() const;

    /// Returns the corresponding filter effect
    KoFilterEffect *effect() const;

protected:
    void createText(const QString &text);
    void createOutput(const QPointF &position, const QString &name);
    void createInput(const QPointF &position);

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;

    ConnectorItem *connectorAtPosition(const QPointF &scenePosition);

private:
    QPointF m_outputPosition;
    QString m_outputName;
    QVector<QPointF> m_inputPositions;
    KoFilterEffect *m_effect;
};

/// Graphics item representing a predefined input image
class DefaultInputItem : public EffectItemBase
{
public:
    DefaultInputItem(const QString &name, KoFilterEffect *effect);

private:
    QString m_name;
};

/// Graphics item representing a effect primitive
class EffectItem : public EffectItemBase
{
public:
    explicit EffectItem(KoFilterEffect *effect);

private:
};

/// Graphics item representing an connection between an output and input
class ConnectionItem : public QGraphicsPathItem
{
public:
    ConnectionItem(EffectItemBase *source, EffectItemBase *target, int targetInput);

    /// Returns the source item of the connection
    EffectItemBase *sourceItem() const;
    /// Returns the target item of the connection
    EffectItemBase *targetItem() const;
    /// Returns the input index of the target item
    int targetInput() const;

    /// Sets the source item
    void setSourceItem(EffectItemBase *source);
    /// Set the target item and the corresponding input index
    void setTargetItem(EffectItemBase *target, int targetInput);

private:
    EffectItemBase *m_source;
    EffectItemBase *m_target;
    int m_targetInput;
};

#endif // FILTEREFFECTSCENEITEMS_H
