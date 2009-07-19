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

#include "FilterEffectEditWidget.h"
#include "KoGenericRegistryModel.h"
#include "KoFilterEffectRegistry.h"
#include "KoFilterEffect.h"
#include "KoShape.h"

#include <QtGui/QGraphicsRectItem>

const QSizeF ConnectorSize = QSize(10,10);
const qreal ItemWidth = 15 * ConnectorSize.height();
const qreal ItemSpacing = 10.0;
const qreal FontSize = 8.0;
const qreal ConnectionDistance = ConnectorSize.height();

class EffectItemBase : public QGraphicsRectItem
{
public:
    EffectItemBase()
    : QGraphicsRectItem(0)
    {
        setZValue(1);
        setFlags(QGraphicsItem::ItemIsSelectable);
    }
    
    void createText(const QString &text)
    {
        QGraphicsSimpleTextItem * textItem = new QGraphicsSimpleTextItem(text, this);
        QFont font = textItem->font();
        font.setPointSize(FontSize);
        textItem->setFont(font);
        QRectF textBox = textItem->boundingRect();
        QPointF offset = rect().center()-textBox.center();
        textItem->translate(offset.x(), offset.y());
    }
    
    void createConnector(const QPointF &position, const QColor &color)
    {
        QRectF circle(QPointF(), ConnectorSize);
        circle.moveCenter(position);
        QGraphicsEllipseItem * connector = new QGraphicsEllipseItem(circle, this);
        connector->setBrush(QBrush(color));
        connector->setCursor(Qt::OpenHandCursor);
    }
    
    void createOutput(const QPointF &position, const QString &name)
    {
        createConnector(position, Qt::red);
        m_outputPosition = position;
        m_outputName = name;
    }
    
    void createInput(const QPointF &position)
    {
        createConnector(position, Qt::green);
        m_inputPositions.append(position);
    }
    
    QPointF outputPosition() const
    {
        return m_outputPosition;
    }
    
    QPointF inputPosition(int index) const
    {
        if (index < 0 || index >= m_inputPositions.count())
            return QPointF();
        return m_inputPositions[index];
    }
    
    QString outputName() const
    {
        return m_outputName;
    }
private:
    QPointF m_outputPosition;
    QString m_outputName;
    QList<QPointF> m_inputPositions;
};

class DefaultInputItem : public EffectItemBase
{
public:
    DefaultInputItem(const QString &inputName)
    : EffectItemBase(), m_inputName(inputName)
    {
        setRect(0, 0, ItemWidth, 20);

        createOutput(QPointF(ItemWidth, 0.5*rect().height()), inputName);
        createText(inputName);
        
        QLinearGradient g(QPointF(0,0), QPointF(1,1));
        g.setCoordinateMode(QGradient::ObjectBoundingMode);
        g.setColorAt(0, Qt::white);
        g.setColorAt(1, QColor(255,168,88));
        setBrush(QBrush(g));
    }
private:
    QString m_inputName;
};

class EffectItem : public EffectItemBase
{
public:
    EffectItem(KoFilterEffect *effect)
    : EffectItemBase(), m_effect(effect)
    {
        Q_ASSERT(effect);
        QRectF circle(QPointF(), ConnectorSize);
                
        QPointF position(ItemWidth, ConnectorSize.height());
        
        // create input connectors
        int requiredInputCount = effect->requiredInputCount();
        for (int i = 0; i < requiredInputCount; ++i) {
            createInput(position);
            position.ry() += 1.5*ConnectorSize.height();
        }
        // create a new input connector when maximal input count in not reached yet
        if (requiredInputCount < effect->maximalInputCount()) {
            createInput(position);
            position.ry() += 1.5*ConnectorSize.height();
        }
        // create output connector
        position.ry() += 0.5*ConnectorSize.height();
        createOutput(position, effect->output());
        
        setRect(0, 0, ItemWidth, position.y()+ConnectorSize.height());

        createText(effect->id());
        
        QLinearGradient g(QPointF(0,0), QPointF(1,1));
        g.setCoordinateMode(QGradient::ObjectBoundingMode);
        g.setColorAt(0, Qt::white);
        g.setColorAt(1, QColor(0,192,192));
        setBrush(QBrush(g));
    }
    
    KoFilterEffect * effect() const
    {
        return m_effect;
    }
    
private:
    KoFilterEffect * m_effect;
};

class ConnectionItem : public QGraphicsPathItem
{
public:
    ConnectionItem(EffectItemBase *source, EffectItemBase * target, int targetInput)
    : QGraphicsPathItem(0), m_source(source), m_target(target), m_targetInput(targetInput)
    {
        setPen(QPen(Qt::black));
    }
    
    EffectItemBase * sourceItem() const
    {
        return m_source;
    }
    
    EffectItemBase * targetItem() const
    {
        return m_target;
    }
    
    int targetInput() const
    {
        return m_targetInput;
    }
    
    void setSourceItem(EffectItemBase * source)
    {
        m_source = source;
    }
    
    void setTargetItem(EffectItemBase * target, int targetInput)
    {
        m_target = target;
        m_targetInput = targetInput;
    }
    
private:
    EffectItemBase * m_source;
    EffectItemBase * m_target;
    int m_targetInput;
};

FilterEffectEditWidget::FilterEffectEditWidget(QWidget *parent)
: QWidget(parent), m_scene(new QGraphicsScene(this)), m_shape(0)
{
    setupUi( this );
    
    KoGenericRegistryModel<KoFilterEffectFactory*> * filterEffectModel = new KoGenericRegistryModel<KoFilterEffectFactory*>(KoFilterEffectRegistry::instance());
    
    effectSelector->setModel(filterEffectModel);
    removeEffect->setIcon(KIcon("list-remove"));
    connect(removeEffect, SIGNAL(clicked()), this, SLOT(removeSelectedItem()));
    addEffect->setIcon(KIcon("list-add"));
    connect(addEffect, SIGNAL(clicked()), this, SLOT(addSelectedEffect()));
    
    raiseEffect->setIcon(KIcon("arrow-up"));
    lowerEffect->setIcon(KIcon("arrow-down"));
    addPreset->setIcon(KIcon("list-add"));
    removePreset->setIcon(KIcon("list-remove"));
    copyPreset->setIcon(KIcon("edit-copy"));
    
    canvas->setScene(m_scene);
    canvas->setRenderHint(QPainter::Antialiasing, true);
    canvas->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    
    m_defaultInputs << "SourceGraphic" << "SourceAlpha";
    m_defaultInputs << "BackgroundImage" << "BackgroundAlpha";
    m_defaultInputs << "FillPaint" << "StrokePaint";
    
    connect(m_scene, SIGNAL(selectionChanged()), this, SLOT(sceneSelectionChanged()));
}

void FilterEffectEditWidget::editShape(KoShape *shape)
{
    if (!m_shape) {
        qDeleteAll(m_effects);
        m_effects.clear();
    }
    
    m_shape = shape;
    
    if (m_shape) {
        m_effects = m_shape->filterEffectStack();
    }
    
    initScene();
}

void FilterEffectEditWidget::addItem(QGraphicsItem *item)
{
    m_scene->addItem(item);
    EffectItemBase * effectItem = dynamic_cast<EffectItemBase*>(item);
    if (effectItem) { 
        m_items.append(effectItem);
    } else {
        ConnectionItem * connectionItem = dynamic_cast<ConnectionItem*>(item);
        if (connectionItem)
            m_connectionItems.append(connectionItem);
    }
}

void FilterEffectEditWidget::initScene()
{
    m_items.clear();
    m_connectionItems.clear();
    m_outputs.clear();
    m_scene->clear();
    
    if (!m_effects.count())
        return;
    
    foreach(KoFilterEffect *effect, m_effects) {
        createEffectItems(effect);
    }
    
    layoutEffects();
    layoutConnections();
    fitScene();
}

void FilterEffectEditWidget::createEffectItems(KoFilterEffect *effect)
{
    QString defaultInput = m_items.count() ? m_items.last()->outputName() : "SourceGraphic";
    
    QList<QString> inputs = effect->inputs();
    for (int i = inputs.count(); i < effect->requiredInputCount(); ++i) {
        inputs.append(defaultInput);
    }
    
    QSet<QString> defaultItems;
    foreach(const QString &input, inputs) {
        if (m_defaultInputs.contains(input) && ! defaultItems.contains(input)) {
            DefaultInputItem * item = new DefaultInputItem(input);
            addItem(item);
            m_outputs.insert(item->outputName(), item);
            defaultItems.insert(input);
        }
    }
    
    EffectItem * effectItem = new EffectItem(effect);
    
    // create connections
    int index = 0;
    foreach(const QString &input, inputs) {
        EffectItemBase * outputItem = m_outputs.value(input, 0);
        if (outputItem) {
            ConnectionItem * connectionItem = new ConnectionItem(outputItem, effectItem, index);
            addItem(connectionItem);
        }
        index++;
    }
    
    addItem(effectItem);
    
    defaultInput = effectItem->outputName();
    m_outputs.insert(defaultInput, effectItem);
}

void FilterEffectEditWidget::layoutEffects()
{
    QPointF position(25,25);
    foreach(EffectItemBase * item, m_items) {
        item->setPos(position);
        position.ry() += item->rect().height() + ItemSpacing;
    }
}

void FilterEffectEditWidget::layoutConnections()
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
        path.moveTo(sourcePos+QPointF(0.5*ConnectorSize.width(),0));
        path.lineTo(sourcePos+QPointF(distance,0));
        path.lineTo(targetPos+QPointF(distance,0));
        path.lineTo(targetPos+QPointF(0.5*ConnectorSize.width(),0));
        connectionItem->setPath(path);
    }
}

void FilterEffectEditWidget::fitScene()
{
    QRectF bbox = m_scene->itemsBoundingRect();
    m_scene->setSceneRect(bbox);
    bbox.adjust(-25,-25,25,25);
    canvas->centerOn(bbox.center());
    canvas->fitInView(bbox, Qt::KeepAspectRatio);
}

void FilterEffectEditWidget::resizeEvent( QResizeEvent * event )
{
    fitScene();
}

void FilterEffectEditWidget::showEvent( QShowEvent * event )
{
    fitScene();
}

void FilterEffectEditWidget::addSelectedEffect()
{
    KoFilterEffectRegistry * registry = KoFilterEffectRegistry::instance();
    KoFilterEffectFactory * factory = registry->values()[effectSelector->currentIndex()];
    if( ! factory )
        return;

    KoFilterEffect * effect = factory->createFilterEffect();
    
    QRectF bbox(0,0,1,1);
    if (m_shape) {
        bbox = QRectF(QPointF(), m_shape->size());
    }
    
    QRectF clipRect(-0.1*bbox.width(), -0.1*bbox.height(), 1.2*bbox.width(), 1.2*bbox.height());
    effect->setClipRect(clipRect);
    effect->setFilterRect(clipRect);
    
    if (m_shape) {
        int shapeFilterCount = m_shape->filterEffectStack().count();
        m_shape->update();
        m_shape->insertFilterEffect(shapeFilterCount, effect);
        m_shape->update();
        m_effects = m_shape->filterEffectStack();
    } else {
        m_effects.append(effect);
    }
    
    createEffectItems(effect);
    layoutEffects();
    layoutConnections();
    fitScene();
}

void FilterEffectEditWidget::removeSelectedItem()
{
    QList<QGraphicsItem*> selectedItems = m_scene->selectedItems();
    if (!selectedItems.count())
        return;
    if (!m_items.count())
        return;
    
    QList<KoFilterEffect*> effectsToDelete;
    
    foreach(QGraphicsItem * item, selectedItems) {
        // we cannot remove the first predefined input item
        if (item == m_items.first())
            continue;
        int itemIndex = m_items.indexOf(dynamic_cast<EffectItemBase*>(item));
        if (itemIndex < 0)
            continue;
        
        EffectItem * effectItem = dynamic_cast<EffectItem*>(item);
        if (effectItem) {
            // remove effect item and all predefined input items before
            KoFilterEffect * effect = effectItem->effect();
            int effectIndex = m_effects.indexOf(effect);
            // adjust inputs of all following effects in the stack
            for (int i = effectIndex+1; i < m_effects.count(); ++i) {
                KoFilterEffect * nextEffect = m_effects[i];
                QList<QString> inputs = nextEffect->inputs();
                int inputIndex = 0;
                foreach(const QString &input, inputs) {
                    if( input == effect->output()) {
                        nextEffect->removeInput(inputIndex);
                        nextEffect->insertInput(inputIndex, "");
                    }
                }
                // if one of the next effects has the same output name we stop
                if (nextEffect->output() == effect->output())
                    break;
            }
            // remove the effect from the stack
            if (m_shape) {
                m_shape->update();
                m_shape->removeFilterEffect(effectIndex);
                m_effects = m_shape->filterEffectStack();
                m_shape->update();
            } else {
                m_effects.removeAt(effectIndex);
            }
            initScene();
            
        } else {
            DefaultInputItem * inputItem = dynamic_cast<DefaultInputItem*>(item);
            removeDefaultInputItem(inputItem);
            layoutEffects();
            layoutConnections();
            fitScene();
        }
    }
}

void FilterEffectEditWidget::removeDefaultInputItem(DefaultInputItem * item)
{
    int itemIndex = m_items.indexOf(item);
    if (itemIndex < 0)
        return;
    
    // remove predefined input item and adjust input of effect item below
    EffectItem * nextEffectItem = nextEffectItemFromIndex(itemIndex);
    EffectItem * prevEffectItem = prevEffectItemFromIndex(itemIndex);
    Q_ASSERT(nextEffectItem);
    Q_ASSERT(prevEffectItem);
    QList<QString> inputs = nextEffectItem->effect()->inputs();
    int inputIndex = 0;
    foreach(const QString &input, inputs) {
        if (input == item->outputName()) {
            nextEffectItem->effect()->removeInput(inputIndex);
            nextEffectItem->effect()->insertInput(inputIndex, "");
            // adjust connection items
            foreach(ConnectionItem * connectionItem, m_connectionItems) {
                if (connectionItem->sourceItem() != item)
                    continue;
                if (connectionItem->targetItem() != nextEffectItem)
                    continue;
                if (connectionItem->targetInput() != inputIndex)
                    continue;
                connectionItem->setSourceItem(prevEffectItem);
            }
        }
        inputIndex++;
    }
    // finally delete the item from the scene
    m_items.removeAt(itemIndex);
    m_scene->removeItem(item);
    delete item;
}

EffectItem * FilterEffectEditWidget::nextEffectItemFromIndex(int index)
{
    if(index < 0 || index >= m_items.count())
        return 0;
    
    for (int i = index+1; i < m_items.count(); ++i) {
        EffectItem * effectItem = dynamic_cast<EffectItem*>(m_items[i]);
        if (effectItem)
            return effectItem;
    }
    return 0;
}

EffectItem * FilterEffectEditWidget::prevEffectItemFromIndex(int index)
{
    if(index < 0 || index >= m_items.count())
        return 0;
    
    for (int i = index-1; i > 0; --i) {
        EffectItem * effectItem = dynamic_cast<EffectItem*>(m_items[i]);
        if (effectItem)
            return effectItem;
    }
    return 0;
}

void FilterEffectEditWidget::sceneSelectionChanged()
{
    QList<QGraphicsItem*> selectedItems = m_scene->selectedItems();
    if(selectedItems.count()) {
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

#include "FilterEffectEditWidget.moc"
