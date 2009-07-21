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
#include "FilterEffectScene.h"
#include "KoGenericRegistryModel.h"
#include "KoFilterEffectRegistry.h"
#include "KoFilterEffect.h"
#include "KoShape.h"

#include <KDebug>

#include <QtGui/QGraphicsItem>
#include <QtCore/QSet>

FilterEffectEditWidget::FilterEffectEditWidget(QWidget *parent)
: QWidget(parent), m_scene(new FilterEffectScene(this)), m_shape(0)
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
    
    connect(m_scene, SIGNAL(connectionCreated(ConnectionSource, ConnectionTarget)),
             this, SLOT(connectionCreated(ConnectionSource, ConnectionTarget)));
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
    
    m_scene->initialize(m_effects);
    fitScene();
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
    Q_UNUSED(event);
    fitScene();
}

void FilterEffectEditWidget::showEvent( QShowEvent * event )
{
    Q_UNUSED(event);
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
    
    m_scene->initialize(m_effects);
    fitScene();
}

void FilterEffectEditWidget::removeSelectedItem()
{
    QList<ConnectionSource> selectedItems = m_scene->selectedEffectItems();
    if (!selectedItems.count())
        return;
    
    foreach(const ConnectionSource &item, selectedItems) {
        KoFilterEffect * effect = item.effect();
        if (item.type() == ConnectionSource::Effect) {
            int effectIndex = m_effects.indexOf(effect);
            // adjust inputs of all following effects in the stack
            for (int i = effectIndex+1; i < m_effects.count(); ++i) {
                KoFilterEffect * nextEffect = m_effects[i];
                QList<QString> inputs = nextEffect->inputs();
                int inputIndex = 0;
                foreach(const QString &input, inputs) {
                    if(input == effect->output()) {
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
                m_shape->update();
                m_effects = m_shape->filterEffectStack();
            } else {
                m_effects.removeAt(effectIndex);
            }
        } else {
            QString outputName = ConnectionSource::typeToString(item.type());
            QList<QString> inputs = effect->inputs();
            int inputIndex = 0;
            foreach(const QString &input, inputs) {
                if (input == outputName) {
                    effect->removeInput(inputIndex);
                    effect->insertInput(inputIndex, "");
                }
                inputIndex++;
            }
        }
    }
    
    m_scene->initialize(m_effects);
    fitScene();
}

void FilterEffectEditWidget::connectionCreated(ConnectionSource source, ConnectionTarget target)
{
    int targetEffectIndex = m_effects.indexOf(target.effect());
    if (targetEffectIndex < 0)
        return;
    
    if (source.type() == ConnectionSource::Effect) {
        QString sourceName = source.effect()->output();
        int sourceEffectIndex = m_effects.indexOf(source.effect());
        if (targetEffectIndex-sourceEffectIndex > 1) {
            // there are effects between source effect and target effect
            // so we have to take extra care
            bool renameOutput = false;
            if (sourceName.isEmpty()) {
                // output is not named so we have to rename the source output
                // and adjust the next effect in case it uses this output
                renameOutput = true;
            } else {
                // output is named but if there is an effect with the same
                // output name, we have to rename the source output
                for (int i = sourceEffectIndex+1; i < targetEffectIndex; ++i) {
                    KoFilterEffect * effect = m_effects[i];
                    if (effect->output() == sourceName) {
                        renameOutput = true;
                        break;
                    }
                }
            }
            if (renameOutput) {
                QSet<QString> uniqueOutputNames;
                foreach(KoFilterEffect *effect, m_effects) {
                    uniqueOutputNames.insert(effect->output());
                }
                int index = 0;
                QString newOutputName;
                do {
                    newOutputName = QString("result%1").arg(index);
                } while(uniqueOutputNames.contains(newOutputName));
                
                // rename soure output
                source.effect()->setOutput(newOutputName);
                // adjust following effects
                for (int i = sourceEffectIndex+1; i < targetEffectIndex; ++i) {
                    KoFilterEffect * effect = m_effects[i];
                    int inputIndex = 0;
                    foreach(const QString &input, effect->inputs()) {
                        if (input.isEmpty() && (i == sourceEffectIndex+1 || input == sourceName)) {
                            effect->removeInput(inputIndex);
                            effect->insertInput(inputIndex, newOutputName);
                        }
                        inputIndex++;
                    }
                    if (sourceName.isEmpty() || effect->output() == sourceName )
                        break;
                }
                sourceName = newOutputName;
            }
        }
        // finally set the input of the target
        if (m_shape)
            m_shape->update();
        target.effect()->removeInput(target.inputIndex());
        target.effect()->insertInput(target.inputIndex(), sourceName);
        if (m_shape)
            m_shape->update();
        
    } else {
        // source is an predefined input image
        QString sourceName = ConnectionSource::typeToString(source.type());
        if (m_shape)
            m_shape->update();
        target.effect()->removeInput(target.inputIndex());
        target.effect()->insertInput(target.inputIndex(), sourceName);
        if (m_shape)
            m_shape->update();
    }
    m_scene->initialize(m_effects);
    fitScene();
}

#include "FilterEffectEditWidget.moc"
