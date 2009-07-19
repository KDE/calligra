/* This file is part of the KDE project
 * Copyright (c) 2009 Jan Hambrecht <jaham@gmx.net>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "FilterEffectScene.h"
#include "FilterEffectSceneItems.h"
#include "KoShape.h"
#include "KoFilterEffect.h"

const qreal ItemSpacing = 10.0;
const qreal ConnectionDistance = 10.0;

SceneItem::SceneItem()
: m_effect(0), m_type(EffectItem)
{
}

SceneItem::SceneItem(KoFilterEffect *effect, Type type)
: m_effect(effect), m_type(type)
{
}

SceneItem::Type SceneItem::type() const
{
    return m_type;
}

KoFilterEffect * SceneItem::effect() const
{
    return m_effect;
}

SceneItem::Type SceneItem::typeFromString(const QString &str)
{
    if (str == "SourceGraphic")
        return SourceGraphic;
    else if (str == "SourceAlpha")
        return SourceAlpha;
    else if (str == "BackgroundImage")
        return BackgroundImage;
    else if (str == "BackgroundAlpha")
        return BackgroundAlpha;
    else if (str == "FillPaint")
        return FillPaint;
    else if (str == "StrokePaint")
        return StrokePaint;
    else
        return EffectItem;
}

QString SceneItem::typeToString(Type type)
{
    if (type == SourceGraphic)
        return "SourceGraphic";
    else if (type == SourceAlpha)
        return "SourceAlpha";
    else if (type == BackgroundImage)
        return "BackgroundImage";
    else if (type == BackgroundAlpha)
        return "BackgroundAlpha";
    else if (type == FillPaint)
        return "FillPaint";
    else if (type == StrokePaint)
        return "StrokePaint";
    else
        return "";
}

FilterEffectScene::FilterEffectScene(QObject *parent)
: QGraphicsScene(parent)
{
    m_defaultInputs << "SourceGraphic" << "SourceAlpha";
    m_defaultInputs << "BackgroundImage" << "BackgroundAlpha";
    m_defaultInputs << "FillPaint" << "StrokePaint";
    
    connect(this, SIGNAL(selectionChanged()), this, SLOT(selectionChanged()));
}

void FilterEffectScene::initialize(const QList<KoFilterEffect*> &effects)
{
    m_items.clear();
    m_connectionItems.clear();
    m_outputs.clear();
    clear();
    
    m_effects = effects;

    if (!m_effects.count())
        return;
    
    foreach(KoFilterEffect *effect, m_effects) {
        createEffectItems(effect);
    }
    
    layoutEffects();
    layoutConnections();
}

void FilterEffectScene::createEffectItems(KoFilterEffect *effect)
{
    const bool isFirstItem = m_items.count() == 0;
    QString defaultInput = isFirstItem ? "SourceGraphic" : m_items.last()->outputName();
    
    QList<QString> inputs = effect->inputs();
    for (int i = inputs.count(); i < effect->requiredInputCount(); ++i) {
        inputs.append(defaultInput);
    }
    
    QSet<QString> defaultItems;
    foreach(QString input, inputs) {
        if (input.isEmpty())
            input = defaultInput;
        if (m_defaultInputs.contains(input) && ! defaultItems.contains(input)) {
            DefaultInputItem * item = new DefaultInputItem(input, effect);
            addSceneItem(item);
            m_outputs.insert(item->outputName(), item);
            defaultItems.insert(input);
        }
    }
    
    EffectItem * effectItem = new EffectItem(effect);
    
    // create connections
    int index = 0;
    foreach(QString input, inputs) {
        if (input.isEmpty())
            input = defaultInput;
        EffectItemBase * outputItem = m_outputs.value(input, 0);
        if (outputItem) {
            ConnectionItem * connectionItem = new ConnectionItem(outputItem, effectItem, index);
            addSceneItem(connectionItem);
        }
        index++;
    }
    
    addSceneItem(effectItem);
    
    m_outputs.insert(effectItem->outputName(), effectItem);
}

void FilterEffectScene::addSceneItem(QGraphicsItem *item)                                 
{                                                                                         
    addItem(item);
    EffectItemBase * effectItem = dynamic_cast<EffectItemBase*>(item);
    if (effectItem) {
        m_items.append(effectItem);
    } else {
        ConnectionItem * connectionItem = dynamic_cast<ConnectionItem*>(item);
        if (connectionItem)
            m_connectionItems.append(connectionItem);
    }
}
        
void FilterEffectScene::layoutEffects()
{
    QPointF position(25,25);
    foreach(EffectItemBase * item, m_items) {
        item->setPos(position);
        position.ry() += item->rect().height() + ItemSpacing;
    }
}

void FilterEffectScene::layoutConnections()
{
    QList<QPair<int,int> > sortedConnections;
    
    // calculate connection sizes from item distances
    int connectionIndex = 0;
    foreach(ConnectionItem *item, m_connectionItems) {
        int sourceIndex = m_items.indexOf(item->sourceItem());
        int targetIndex = m_items.indexOf(item->targetItem());
        sortedConnections.append(QPair<int,int>(targetIndex-sourceIndex, connectionIndex));
        connectionIndex++;
    }
    
    qSort(sortedConnections);
    qreal distance = ConnectionDistance;
    int lastSize = -1;
    int connectionCount = sortedConnections.count();
    for (int i = 0; i < connectionCount; ++i) {
        const QPair<int, int> &connection = sortedConnections[i];
        
        int size = connection.first;
        if (size > lastSize) {
            lastSize = size;
            distance += ConnectionDistance;
        }
        
        ConnectionItem * connectionItem = m_connectionItems[connection.second];
        if (!connectionItem)
            continue;
        EffectItemBase * sourceItem = connectionItem->sourceItem();
        EffectItemBase * targetItem = connectionItem->targetItem();
        if (!sourceItem || ! targetItem)
            continue;
        
        int targetInput = connectionItem->targetInput();
        QPointF sourcePos = sourceItem->mapToScene(sourceItem->outputPosition());
        QPointF targetPos = targetItem->mapToScene(targetItem->inputPosition(targetInput));
        QPainterPath path;
        path.moveTo(sourcePos+QPointF(0.5*sourceItem->connectorSize().width(),0));
        path.lineTo(sourcePos+QPointF(distance,0));
        path.lineTo(targetPos+QPointF(distance,0));
        path.lineTo(targetPos+QPointF(0.5*targetItem->connectorSize().width(),0));
        connectionItem->setPath(path);
    }
}

void FilterEffectScene::selectionChanged()
{
    if(selectedItems().count()) {
        foreach(EffectItemBase* item, m_items) {
            if (item->isSelected())
                item->setOpacity(1.0);
            else
                item->setOpacity(0.25);
        }
    } else {
        foreach(EffectItemBase* item, m_items) {
            item->setOpacity(1);
        }
    }
}

QList<SceneItem> FilterEffectScene::selectedEffectItems() const
{
    QList<SceneItem> effectItems;
    
    QList<QGraphicsItem*> selectedGraphicsItems = selectedItems();
    if (!selectedGraphicsItems.count())
        return effectItems;
    if (!m_items.count())
        return effectItems;
    
    foreach(QGraphicsItem * item, selectedGraphicsItems) {
        // we cannot remove the first predefined input item
        if (item == m_items.first())
            continue;
        
        EffectItemBase * effectItem = dynamic_cast<EffectItemBase*>(item);
        if (!item)
            continue;

        SceneItem::Type type = SceneItem::EffectItem;
        
        KoFilterEffect * effect = effectItem->effect();
        if (dynamic_cast<DefaultInputItem*>(item)) {
            type = SceneItem::typeFromString(effectItem->outputName());
        }
        
        effectItems.append(SceneItem(effect, type));
    }
    
    return effectItems;
}

void FilterEffectScene::dropEvent(QGraphicsSceneDragDropEvent * event)
{
    ConnectorItem * targetItem = 0;
    QList<QGraphicsItem*> itemsAtPositon = items(event->scenePos());
    foreach(QGraphicsItem* item, itemsAtPositon) {
        targetItem = dynamic_cast<ConnectorItem*>(item);
        if (targetItem)
            break;
    }
    if (!targetItem)
        return;
    
    const ConnectorMimeData * data = dynamic_cast<const ConnectorMimeData*>(event->mimeData());
    if (!data)
        return;
    
    ConnectorItem * sourceItem = data->connector();
    if (!sourceItem)
        return;

    KoFilterEffect * targetEffect = targetItem->effect();
    KoFilterEffect * sourceEffect = sourceItem->effect();
    
    EffectItemBase * sourceParentItem = dynamic_cast<EffectItemBase*>(sourceItem->parentItem());
    EffectItemBase * targetParentItem = dynamic_cast<EffectItemBase*>(targetItem->parentItem());
    
    SceneConnection connection;
    
    if (targetItem->connectorType() == ConnectorItem::Input) {
        // dropped output onto an input
        SceneItem::Type targetType = SceneItem::EffectItem;
        SceneItem::Type sourceType = SceneItem::EffectItem;
        if (m_defaultInputs.contains(sourceParentItem->outputName())) {
            sourceType = SceneItem::typeFromString(sourceParentItem->outputName());
        }
        connection.source = SceneItem(sourceEffect, sourceType);
        connection.target = SceneItem(targetEffect, targetType);
        connection.targetIndex = targetItem->connectorIndex();
    } else {
        // dropped input onto an output
        SceneItem::Type sourceType = SceneItem::EffectItem;
        SceneItem::Type targetType = SceneItem::EffectItem;
        if (m_defaultInputs.contains(targetParentItem->outputName())) {
            targetType = SceneItem::typeFromString(targetParentItem->outputName());
        }
        connection.source = SceneItem(targetEffect, targetType);
        connection.target = SceneItem(sourceEffect, sourceType);
        connection.targetIndex = sourceItem->connectorIndex();
    }
    
    emit connectionCreated(connection);
}

#include "FilterEffectScene.moc"
