/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "FilterEffectEditWidget.h"
#include "FilterAddCommand.h"
#include "FilterEffectResource.h"
#include "FilterInputChangeCommand.h"
#include "FilterRemoveCommand.h"
#include "FilterResourceServerProvider.h"
#include "FilterStackSetCommand.h"
#include "KoCanvasBase.h"
#include "KoFilterEffect.h"
#include "KoFilterEffectConfigWidgetBase.h"
#include "KoFilterEffectRegistry.h"
#include "KoFilterEffectStack.h"
#include "KoGenericRegistryModel.h"
#include "KoResourceModel.h"
#include "KoResourceServerAdapter.h"
#include "KoShape.h"

#include <KoIcon.h>

#include <QDebug>
#include <QInputDialog>

#include <QGraphicsItem>
#include <QSet>

FilterEffectEditWidget::FilterEffectEditWidget(QWidget *parent)
    : QWidget(parent)
    , m_scene(new FilterEffectScene(this))
    , m_shape(nullptr)
    , m_canvas(nullptr)
    , m_effects(nullptr)
{
    setupUi(this);

    FilterResourceServerProvider *serverProvider = FilterResourceServerProvider::instance();
    KoResourceServer<FilterEffectResource> *server = serverProvider->filterEffectServer();
    QSharedPointer<KoAbstractResourceServerAdapter> adapter(new KoResourceServerAdapter<FilterEffectResource>(server));

    presets->setResourceAdapter(adapter);
    presets->setDisplayMode(KoResourceSelector::TextMode);
    presets->setColumnCount(1);

    connect(presets, &KoResourceSelector::resourceSelected, this, &FilterEffectEditWidget::presetSelected);

    connect(presets, &KoResourceSelector::resourceApplied, this, &FilterEffectEditWidget::presetSelected);

    KoGenericRegistryModel<KoFilterEffectFactoryBase *> *filterEffectModel =
        new KoGenericRegistryModel<KoFilterEffectFactoryBase *>(KoFilterEffectRegistry::instance());

    effectSelector->setModel(filterEffectModel);
    removeEffect->setIcon(koIcon("list-remove"));
    connect(removeEffect, &QAbstractButton::clicked, this, &FilterEffectEditWidget::removeSelectedItem);
    addEffect->setIcon(koIcon("list-add"));
    addEffect->setToolTip(i18n("Add effect to current filter stack"));
    connect(addEffect, &QAbstractButton::clicked, this, &FilterEffectEditWidget::addSelectedEffect);

    // TODO: make these buttons do something useful
    raiseEffect->setIcon(koIcon("arrow-up"));
    raiseEffect->hide();
    lowerEffect->setIcon(koIcon("arrow-down"));
    lowerEffect->hide();

    addPreset->setIcon(koIcon("list-add"));
    addPreset->setToolTip(i18n("Add to filter presets"));
    connect(addPreset, &QAbstractButton::clicked, this, &FilterEffectEditWidget::addToPresets);

    removePreset->setIcon(koIcon("list-remove"));
    removePreset->setToolTip(i18n("Remove filter preset"));
    connect(removePreset, &QAbstractButton::clicked, this, &FilterEffectEditWidget::removeFromPresets);

    view->setScene(m_scene);
    view->setRenderHint(QPainter::Antialiasing, true);
    view->setResizeAnchor(QGraphicsView::AnchorViewCenter);

    connect(m_scene, &FilterEffectScene::connectionCreated, this, &FilterEffectEditWidget::connectionCreated);
    connect(m_scene, QOverload<>::of(&FilterEffectScene::selectionChanged), this, &FilterEffectEditWidget::sceneSelectionChanged);

    QSet<ConnectionSource::SourceType> inputs;
    inputs << ConnectionSource::SourceGraphic;
    inputs << ConnectionSource::SourceAlpha;
    inputs << ConnectionSource::BackgroundImage;
    inputs << ConnectionSource::BackgroundAlpha;
    inputs << ConnectionSource::FillPaint;
    inputs << ConnectionSource::StrokePaint;

    m_defaultSourceSelector = new KComboBox(this);
    foreach (ConnectionSource::SourceType source, inputs) {
        m_defaultSourceSelector->addItem(ConnectionSource::typeToString(source));
    }
    m_defaultSourceSelector->hide();
    m_defaultSourceSelector->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
    connect(m_defaultSourceSelector, QOverload<int>::of(&KComboBox::currentIndexChanged), this, &FilterEffectEditWidget::defaultSourceChanged);
}

FilterEffectEditWidget::~FilterEffectEditWidget()
{
    if (!m_shape) {
        delete m_effects;
    }
}

void FilterEffectEditWidget::editShape(KoShape *shape, KoCanvasBase *canvas)
{
    if (!m_shape) {
        delete m_effects;
        m_effects = nullptr;
    }

    m_shape = shape;
    m_canvas = canvas;

    if (m_shape) {
        m_effects = m_shape->filterEffectStack();
    }
    if (!m_effects) {
        m_effects = new KoFilterEffectStack();
    }

    m_scene->initialize(m_effects);
    fitScene();
}

void FilterEffectEditWidget::fitScene()
{
    QRectF bbox = m_scene->itemsBoundingRect();
    m_scene->setSceneRect(bbox);
    bbox.adjust(-25, -25, 25, 25);
    view->centerOn(bbox.center());
    view->fitInView(bbox, Qt::KeepAspectRatio);
}

void FilterEffectEditWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    fitScene();
}

void FilterEffectEditWidget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    fitScene();
}

void FilterEffectEditWidget::addSelectedEffect()
{
    KoFilterEffectRegistry *registry = KoFilterEffectRegistry::instance();
    KoFilterEffectFactoryBase *factory = registry->values()[effectSelector->currentIndex()];
    if (!factory)
        return;

    KoFilterEffect *effect = factory->createFilterEffect();
    if (!effect)
        return;

    if (m_shape) {
        if (!m_shape->filterEffectStack()) {
            m_effects->appendFilterEffect(effect);
            m_canvas->addCommand(new FilterStackSetCommand(m_effects, m_shape));
        } else {
            m_canvas->addCommand(new FilterAddCommand(effect, m_shape));
        }
    } else {
        m_effects->appendFilterEffect(effect);
    }

    m_scene->initialize(m_effects);
    fitScene();
}

void FilterEffectEditWidget::removeSelectedItem()
{
    QList<ConnectionSource> selectedItems = m_scene->selectedEffectItems();
    if (!selectedItems.count())
        return;

    QList<InputChangeData> changeData;
    QList<KoFilterEffect *> filterEffects = m_effects->filterEffects();
    int effectIndexToDelete = -1;

    const ConnectionSource &item = selectedItems.first();
    KoFilterEffect *effect = item.effect();
    if (item.type() == ConnectionSource::Effect) {
        int effectIndex = filterEffects.indexOf(effect);
        // adjust inputs of all following effects in the stack
        for (int i = effectIndex + 1; i < filterEffects.count(); ++i) {
            KoFilterEffect *nextEffect = filterEffects[i];
            QList<QString> inputs = nextEffect->inputs();
            int inputIndex = 0;
            foreach (const QString &input, inputs) {
                if (input == effect->output()) {
                    InputChangeData data(nextEffect, inputIndex, input, "");
                    changeData.append(data);
                }
            }
            // if one of the next effects has the same output name we stop
            if (nextEffect->output() == effect->output())
                break;
        }
        effectIndexToDelete = effectIndex;
    } else {
        QString outputName = ConnectionSource::typeToString(item.type());
        QList<QString> inputs = effect->inputs();
        int inputIndex = 0;
        foreach (const QString &input, inputs) {
            if (input == outputName) {
                InputChangeData data(effect, inputIndex, input, "");
                changeData.append(data);
            }
            inputIndex++;
        }
    }

    KUndo2Command *cmd = new KUndo2Command();
    if (changeData.count()) {
        KUndo2Command *subCmd = new FilterInputChangeCommand(changeData, m_shape, cmd);
        cmd->setText(subCmd->text());
    }
    if (effectIndexToDelete >= 0) {
        KUndo2Command *subCmd = new FilterRemoveCommand(effectIndexToDelete, m_effects, m_shape, cmd);
        cmd->setText(subCmd->text());
    }
    if (m_canvas && m_shape) {
        m_canvas->addCommand(cmd);
    } else {
        cmd->redo();
        delete cmd;
    }
    m_scene->initialize(m_effects);
    fitScene();
}

void FilterEffectEditWidget::connectionCreated(ConnectionSource source, ConnectionTarget target)
{
    QList<KoFilterEffect *> filterEffects = m_effects->filterEffects();

    int targetEffectIndex = filterEffects.indexOf(target.effect());
    if (targetEffectIndex < 0)
        return;

    QList<InputChangeData> changeData;
    QString sourceName;

    if (source.type() == ConnectionSource::Effect) {
        sourceName = source.effect()->output();
        int sourceEffectIndex = filterEffects.indexOf(source.effect());
        if (targetEffectIndex - sourceEffectIndex > 1) {
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
                for (int i = sourceEffectIndex + 1; i < targetEffectIndex; ++i) {
                    KoFilterEffect *effect = filterEffects[i];
                    if (effect->output() == sourceName) {
                        renameOutput = true;
                        break;
                    }
                }
            }
            if (renameOutput) {
                QSet<QString> uniqueOutputNames;
                foreach (KoFilterEffect *effect, filterEffects) {
                    uniqueOutputNames.insert(effect->output());
                }
                int index = 0;
                QString newOutputName;
                do {
                    newOutputName = QString("result%1").arg(index);
                } while (uniqueOutputNames.contains(newOutputName));

                // rename source output
                source.effect()->setOutput(newOutputName);
                // adjust following effects
                for (int i = sourceEffectIndex + 1; i < targetEffectIndex; ++i) {
                    KoFilterEffect *effect = filterEffects[i];
                    int inputIndex = 0;
                    foreach (const QString &input, effect->inputs()) {
                        if (input.isEmpty() && (i == sourceEffectIndex + 1 || input == sourceName)) {
                            InputChangeData data(effect, inputIndex, input, newOutputName);
                            changeData.append(data);
                        }
                        inputIndex++;
                    }
                    if (sourceName.isEmpty() || effect->output() == sourceName)
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
        KUndo2Command *cmd = new FilterInputChangeCommand(changeData, m_shape);
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
    QString effectName = QInputDialog::getText(this, i18n("Effect name"), i18n("Please enter a name for the filter effect"), QLineEdit::Normal, QString(), &ok);
    if (!ok)
        return;

    FilterEffectResource *resource = FilterEffectResource::fromFilterEffectStack(m_effects);
    if (!resource)
        return;

    resource->setName(effectName);

    FilterResourceServerProvider *serverProvider = FilterResourceServerProvider::instance();
    KoResourceServer<FilterEffectResource> *server = serverProvider->filterEffectServer();

    QString savePath = server->saveLocation();

    int i = 1;
    QFileInfo fileInfo;

    do {
        fileInfo.setFile(savePath + QString("%1.svg").arg(i++, 4, 10, QChar('0')));
    } while (fileInfo.exists());

    resource->setFilename(fileInfo.filePath());
    resource->setValid(true);

    if (!server->addResource(resource))
        delete resource;
}

void FilterEffectEditWidget::removeFromPresets()
{
    if (!presets->count()) {
        return;
    }
    FilterResourceServerProvider *serverProvider = FilterResourceServerProvider::instance();
    if (!serverProvider) {
        return;
    }
    KoResourceServer<FilterEffectResource> *server = serverProvider->filterEffectServer();
    if (!server) {
        return;
    }

    FilterEffectResource *resource = server->resources().at(presets->currentIndex());
    if (!resource) {
        return;
    }

    server->removeResourceAndBlacklist(resource);
}

void FilterEffectEditWidget::presetSelected(KoResource *resource)
{
    FilterEffectResource *effectResource = dynamic_cast<FilterEffectResource *>(resource);
    if (!effectResource)
        return;

    KoFilterEffectStack *filterStack = effectResource->toFilterStack();
    if (!filterStack)
        return;

    if (m_shape) {
        KUndo2Command *cmd = new FilterStackSetCommand(filterStack, m_shape);
        if (m_canvas) {
            m_canvas->addCommand(cmd);
        } else {
            cmd->redo();
            delete cmd;
        }
    } else {
        delete m_effects;
    }
    m_effects = filterStack;

    m_scene->initialize(m_effects);
    fitScene();
}

void FilterEffectEditWidget::addWidgetForItem(ConnectionSource item)
{
    // get the filter effect from the item
    KoFilterEffect *filterEffect = item.effect();
    if (item.type() != ConnectionSource::Effect)
        filterEffect = nullptr;

    KoFilterEffect *currentEffect = m_currentItem.effect();
    if (m_currentItem.type() != ConnectionSource::Effect)
        currentEffect = nullptr;

    m_defaultSourceSelector->hide();

    // remove current widget if new effect is zero or effect type has changed
    if (!filterEffect || !currentEffect || (filterEffect->id() != currentEffect->id())) {
        while (configStack->count())
            configStack->removeWidget(configStack->widget(0));
    }

    m_currentItem = item;

    KoFilterEffectConfigWidgetBase *currentPanel = nullptr;

    if (!filterEffect) {
        if (item.type() != ConnectionSource::Effect) {
            configStack->insertWidget(0, m_defaultSourceSelector);
            m_defaultSourceSelector->blockSignals(true);
            m_defaultSourceSelector->setCurrentIndex(item.type() - 1);
            m_defaultSourceSelector->blockSignals(false);
            m_defaultSourceSelector->show();
        }
    } else if (!currentEffect || currentEffect->id() != filterEffect->id()) {
        // when a shape is set and is differs from the previous one
        // get the config widget and insert it into the option widget

        KoFilterEffectRegistry *registry = KoFilterEffectRegistry::instance();
        KoFilterEffectFactoryBase *factory = registry->value(filterEffect->id());
        if (!factory)
            return;

        currentPanel = factory->createConfigWidget();
        if (!currentPanel)
            return;

        configStack->insertWidget(0, currentPanel);
        connect(currentPanel, &KoFilterEffectConfigWidgetBase::filterChanged, this, &FilterEffectEditWidget::filterChanged);
    }

    currentPanel = qobject_cast<KoFilterEffectConfigWidgetBase *>(configStack->widget(0));
    if (currentPanel)
        currentPanel->editFilterEffect(filterEffect);
}

void FilterEffectEditWidget::filterChanged()
{
    if (m_shape)
        m_shape->update();
}

void FilterEffectEditWidget::sceneSelectionChanged()
{
    QList<ConnectionSource> selectedItems = m_scene->selectedEffectItems();
    if (!selectedItems.count()) {
        addWidgetForItem(ConnectionSource());
    } else {
        addWidgetForItem(selectedItems.first());
    }
}

void FilterEffectEditWidget::defaultSourceChanged(int index)
{
    if (m_currentItem.type() == ConnectionSource::Effect)
        return;

    KoFilterEffect *filterEffect = m_currentItem.effect();
    if (!filterEffect)
        return;

    QString oldInput = ConnectionSource::typeToString(m_currentItem.type());
    QString newInput = m_defaultSourceSelector->itemText(index);

    const QString defInput = "SourceGraphic";
    int effectIndex = m_effects->filterEffects().indexOf(filterEffect);

    InputChangeData data;
    int inputIndex = 0;
    foreach (const QString &input, filterEffect->inputs()) {
        if (input == oldInput || (effectIndex == 0 && oldInput == defInput)) {
            data = InputChangeData(filterEffect, inputIndex, input, newInput);
            break;
        }
        inputIndex++;
    }
    KUndo2Command *cmd = new FilterInputChangeCommand(data, m_shape);
    if (m_canvas && m_shape) {
        m_canvas->addCommand(cmd);
    } else {
        cmd->redo();
        delete cmd;
    }

    m_scene->initialize(m_effects);
    fitScene();
}
