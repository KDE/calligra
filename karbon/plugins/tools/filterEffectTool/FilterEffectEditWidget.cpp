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
#include "FilterEffectResource.h"
#include "FilterResourceServerProvider.h"
#include "FilterInputChangeCommand.h"
#include "KoGenericRegistryModel.h"
#include "KoFilterEffectRegistry.h"
#include "KoFilterEffect.h"
#include "KoFilterEffectStack.h"
#include "KoShape.h"
#include "KoCanvasBase.h"
#include "KoResourceModel.h"
#include "KoResourceServerAdapter.h"

#include <KDebug>

#include <QtGui/QGraphicsItem>
#include <QtGui/QInputDialog>
#include <QtCore/QSet>

FilterEffectEditWidget::FilterEffectEditWidget(QWidget *parent)
: QWidget(parent), m_scene(new FilterEffectScene(this))
, m_shape(0), m_canvas(0), m_effects(0)
{
    setupUi( this );
    
    FilterResourceServerProvider * serverProvider = FilterResourceServerProvider::instance();
    KoResourceServer<FilterEffectResource> * server = serverProvider->filterEffectServer();
    KoAbstractResourceServerAdapter * adapter = new KoResourceServerAdapter<FilterEffectResource>(server);
    
    KoResourceModel * model = new KoResourceModel(adapter, this);
    model->setColumnCount(1);
    presets->setModel(model);
    
    KoGenericRegistryModel<KoFilterEffectFactory*> * filterEffectModel = new KoGenericRegistryModel<KoFilterEffectFactory*>(KoFilterEffectRegistry::instance());
    
    effectSelector->setModel(filterEffectModel);
    removeEffect->setIcon(KIcon("list-remove"));
    connect(removeEffect, SIGNAL(clicked()), this, SLOT(removeSelectedItem()));
    addEffect->setIcon(KIcon("list-add"));
    connect(addEffect, SIGNAL(clicked()), this, SLOT(addSelectedEffect()));
    
    raiseEffect->setIcon(KIcon("arrow-up"));
    lowerEffect->setIcon(KIcon("arrow-down"));
    
    addPreset->setIcon(KIcon("list-add"));
    connect(addPreset, SIGNAL(clicked()), this, SLOT(addToPresets()));
    
    removePreset->setIcon(KIcon("list-remove"));
    copyPreset->setIcon(KIcon("edit-copy"));
    
    view->setScene(m_scene);
    view->setRenderHint(QPainter::Antialiasing, true);
    view->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    
    connect(m_scene, SIGNAL(connectionCreated(ConnectionSource, ConnectionTarget)),
             this, SLOT(connectionCreated(ConnectionSource, ConnectionTarget)));
}

FilterEffectEditWidget::~FilterEffectEditWidget()
{
    if (!m_shape) {
        delete m_effects;
    }
}

void FilterEffectEditWidget::editShape(KoShape *shape, KoCanvasBase * canvas)
{
    if (!m_shape) {
        delete m_effects;
    }
    
    m_shape = shape;
    m_canvas = canvas;
    
    if (m_shape) {
        m_effects = m_shape->filterEffectStack();
    } else {
        m_effects = new KoFilterEffectStack();
    }
    
    m_scene->initialize(m_effects);
    fitScene();
}

void FilterEffectEditWidget::fitScene()
{
    QRectF bbox = m_scene->itemsBoundingRect();
    m_scene->setSceneRect(bbox);
    bbox.adjust(-25,-25,25,25);
    view->centerOn(bbox.center());
    view->fitInView(bbox, Qt::KeepAspectRatio);
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
    if (!factory)
        return;

    KoFilterEffect * effect = factory->createFilterEffect();
    if (!effect)
        return;
    
    if (m_shape)
        m_shape->update();
    
    m_effects->appendFilterEffect(effect);

    if (m_shape)
        m_shape->update();
    
    m_scene->initialize(m_effects);
    fitScene();
}

void FilterEffectEditWidget::removeSelectedItem()
{
    QList<ConnectionSource> selectedItems = m_scene->selectedEffectItems();
    if (!selectedItems.count())
        return;
    
    QList<InputChangeData> changeData;
    QList<KoFilterEffect*> filterEffects = m_effects->filterEffects();
    
    foreach(const ConnectionSource &item, selectedItems) {
        KoFilterEffect * effect = item.effect();
        if (item.type() == ConnectionSource::Effect) {
            int effectIndex = filterEffects.indexOf(effect);
            // adjust inputs of all following effects in the stack
            for (int i = effectIndex+1; i < filterEffects.count(); ++i) {
                KoFilterEffect * nextEffect = filterEffects[i];
                QList<QString> inputs = nextEffect->inputs();
                int inputIndex = 0;
                foreach(const QString &input, inputs) {
                    if(input == effect->output()) {
                        InputChangeData data(nextEffect, inputIndex, input, "");
                        changeData.append(data);
                    }
                }
                // if one of the next effects has the same output name we stop
                if (nextEffect->output() == effect->output())
                    break;
            }
            // remove the effect from the stack
            if (m_shape)
                m_shape->update();
            
            m_effects->removeFilterEffect(effectIndex);
            
            if (m_shape)
                m_shape->update();
        } else {
            QString outputName = ConnectionSource::typeToString(item.type());
            QList<QString> inputs = effect->inputs();
            int inputIndex = 0;
            foreach(const QString &input, inputs) {
                if (input == outputName) {
                    InputChangeData data(effect, inputIndex, input, "");
                    changeData.append(data);
                }
                inputIndex++;
            }
        }
    }
    if (changeData.count()) {
        QUndoCommand * cmd = new FilterInputChangeCommand(changeData, m_shape);
        if (m_canvas) {
            m_canvas->addCommand(cmd);
        } else {
            cmd->redo();
            delete cmd;
        }
    }
    m_scene->initialize(m_effects);
    fitScene();
}

void FilterEffectEditWidget::connectionCreated(ConnectionSource source, ConnectionTarget target)
{
    QList<KoFilterEffect*> filterEffects = m_effects->filterEffects();
    
    int targetEffectIndex = filterEffects.indexOf(target.effect());
    if (targetEffectIndex < 0)
        return;
    
    QList<InputChangeData> changeData;
    QString sourceName;
    
    if (source.type() == ConnectionSource::Effect) {
        sourceName = source.effect()->output();
        int sourceEffectIndex = filterEffects.indexOf(source.effect());
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
                    KoFilterEffect * effect = filterEffects[i];
                    if (effect->output() == sourceName) {
                        renameOutput = true;
                        break;
                    }
                }
            }
            if (renameOutput) {
                QSet<QString> uniqueOutputNames;
                foreach(KoFilterEffect *effect, filterEffects) {
                    uniqueOutputNames.insert(effect->output());
                }
                int index = 0;
                QString newOutputName;
                do {
                    newOutputName = QString("result%1").arg(index);
                } while(uniqueOutputNames.contains(newOutputName));
                
                // rename source output
                source.effect()->setOutput(newOutputName);
                // adjust following effects
                for (int i = sourceEffectIndex+1; i < targetEffectIndex; ++i) {
                    KoFilterEffect * effect = filterEffects[i];
                    int inputIndex = 0;
                    foreach(const QString &input, effect->inputs()) {
                        if (input.isEmpty() && (i == sourceEffectIndex+1 || input == sourceName)) {
                            InputChangeData data(effect, inputIndex, input, newOutputName);
                            changeData.append(data);
                        }
                        inputIndex++;
                    }
                    if (sourceName.isEmpty() || effect->output() == sourceName )
                        break;
                }
                sourceName = newOutputName;
            }
        }
    } else {
        // source is an predefined input image
        sourceName = ConnectionSource::typeToString(source.type());
    }
    
    // finally set the input of the target
    if (target.inputIndex() >= target.effect()->inputs().count()) {
        // insert new input here
        target.effect()->addInput(sourceName);
    } else {
        QString oldInput = target.effect()->inputs()[target.inputIndex()];
        InputChangeData data(target.effect(), target.inputIndex(), oldInput, sourceName);
        changeData.append(data);
    }
    
    if (changeData.count()) {
        QUndoCommand * cmd = new FilterInputChangeCommand(changeData, m_shape);
        if (m_canvas) {
            m_canvas->addCommand(cmd);
        } else {
            cmd->redo();
            delete cmd;
        }
    }
    m_scene->initialize(m_effects);
    fitScene();
}

void FilterEffectEditWidget::addToPresets()
{
    if (!m_effects)
        return;

    bool ok = false;
    QString effectName = QInputDialog::getText(this,
                                               i18n("Effect name"), 
                                               i18n("Please enter a name for the filter effect"),
                                               QLineEdit::Normal,
                                               QString::null,
                                               &ok);
    if (!ok)
        return;
    
    FilterEffectResource * resource = FilterEffectResource::fromFilterEffectStack(m_effects);
    if (!resource)
        return;
    
    resource->setName(effectName);
    
    FilterResourceServerProvider * serverProvider = FilterResourceServerProvider::instance();
    KoResourceServer<FilterEffectResource> * server = serverProvider->filterEffectServer();
    
    QString savePath = server->saveLocation();
    
    int i = 1;
    QFileInfo fileInfo;
    
    do {
        fileInfo.setFile(savePath + QString("%1.svg").arg(i++, 4, 10, QChar('0')));
    }
    while(fileInfo.exists());
    
    resource->setFilename(fileInfo.filePath());
    resource->setValid(true);
    
    if (!server->addResource(resource))
        delete resource;
}

#include "FilterEffectEditWidget.moc"
