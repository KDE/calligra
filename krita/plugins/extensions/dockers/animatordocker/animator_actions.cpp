/*
 *  Contains all GUI actions and provide access for them
 *  Copyright (C) 2011 Torio Mlshi <mlshi@lavabit.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "animator_actions.h"

#include <KLocale>
#include <KIconLoader>

#include <KDialog>
#include <KLineEdit>
#include <QLayout>
#include <QLabel>
#include <QSpinBox>

#include "animator_manager.h"
#include "animator_loader.h"
#include "animator_player.h"
#include "animator_exporter.h"
#include "animator_importer.h"
#include "animator_updater.h"
#include "animator_lt_updater.h"

#include "animator_config.h"

#include "normal_animated_layer.h"
#include "control_animated_layer.h"
#include "animator_frame_manager.h"

AnimatorActions::AnimatorActions(QObject* parent) : QObject(parent)
{
    m_manager = 0;
    initActions();
}

AnimatorActions::~AnimatorActions()
{
}

void AnimatorActions::setManager(AnimatorManager* manager)
{
    m_manager = manager;
    connect(m_manager, SIGNAL(animatedLayerActivated(AnimatedLayer*)), SLOT(setupFrameActions(AnimatedLayer*)));
}


void AnimatorActions::addAction(const QString& category, QAction* action)
{
    m_actions[category].append(action);
}

QList< QAction* > AnimatorActions::actions() const
{
    QList<QAction*> list;
    QList<QAction*> sublist;
    foreach (sublist, m_actions.values())
    {
        list.append(sublist);
    }
    return list;
}

QList< QAction* > AnimatorActions::actions(const QString& category) const
{
    if (category == "frames-adding")
        return m_actions[m_frameActionsType];
    if (category == "frames-editing" && m_frameActionsType == "none")
        return QList<QAction*>();
    return m_actions[category];
}

void AnimatorActions::initActions()
{
    QAction* t;
    
    // UTIL
#if !LOAD_ON_START
    t = new QAction(SmallIcon("system-run"), i18n("Load layers"), this);
    connect(t, SIGNAL(triggered(bool)), SLOT(loadLayers()));
    addAction("util", t);
#endif
    
    t = new QAction(SmallIcon("tool-animator"), i18n("Make this file animated"), this);
    connect(t, SIGNAL(triggered(bool)), SLOT(makeAnimated()));
    addAction("util", t);
    
    t = new QAction(SmallIcon("document-import"), i18n("Import from image sequence"), this);
    connect(t, SIGNAL(triggered(bool)), SLOT(importFrames()));
    addAction("util", t);
    
    t = new QAction(SmallIcon("document-export"), i18n("Export to png sequence"), this);
    connect(t, SIGNAL(triggered(bool)), SLOT(exportFrames()));
    addAction("util", t);
    
    // PLAYER
    t = new QAction(SmallIcon("media-playback-start"), i18n("Play/pause"), this);
    t->setCheckable(true);
    t->setChecked(false);
    connect(t, SIGNAL(triggered(bool)), SLOT(playPause(bool)));
    addAction("player", t);
    
    t = new QAction(SmallIcon("task-recurring"), i18n("Toggle player looping"), this);
    t->setCheckable(true);
    t->setChecked(false);
    connect(t, SIGNAL(triggered(bool)), SLOT(toggleLooping(bool)));
    addAction("player", t);
    
    // LAYERS
    t = new QAction(SmallIcon("list-add"), i18n("Add normal layer"), this);
    connect(t, SIGNAL(triggered(bool)), SLOT(createNormalLayer()));
    addAction("layers", t);
    
    t = new QAction(SmallIcon("fork"), i18n("Add control layer (only one layer will work now)"), this);
    connect(t, SIGNAL(triggered(bool)), SLOT(createControlLayer()));
    addAction("layers", t);
    
    t = new QAction(SmallIcon("image-preview"), i18n("Convert current layer to view layer"), this);
    connect(t, SIGNAL(triggered(bool)), SLOT(convertToViewLayer()));
    addAction("layers", t);
    
    t = new QAction(SmallIcon("list-remove"), i18n("Remove layer"), this);
    connect(t, SIGNAL(triggered(bool)), SLOT(removeLayer()));
    addAction("layers", t);
    
    t = new QAction(SmallIcon("edit-rename"), i18n("Rename layer"), this);
    connect(t, SIGNAL(triggered(bool)), SLOT(renameLayer()));
    addAction("layers", t);
    
    t = new QAction(SmallIcon("tools-wizard"), i18n("Calculate layer interpolatation"), this);
    connect(t, SIGNAL(triggered(bool)), SLOT(calculateLayer()));
    addAction("layers", t);
    
    // FRAMES
    t = new QAction(SmallIcon("document-new"), i18n("Create paint frame"), this);
    connect(t, SIGNAL(triggered(bool)), SLOT(createPaintFrame()));
    addAction("frames-adding-normal", t);
    
    t = new QAction(SmallIcon("bookmark-new"), i18n("Create shape frame"), this);
    connect(t, SIGNAL(triggered(bool)), SLOT(createShapeFrame()));
    addAction("frames-adding-normal", t);
    
    t = new QAction(SmallIcon("folder-new"), i18n("Create group frame"), this);
    connect(t, SIGNAL(triggered(bool)), SLOT(createGroupFrame()));
    addAction("frames-adding-normal", t);
    
    t = new QAction(SmallIcon("tools-wizard"), i18n("Interpolate"), this);
    connect(t, SIGNAL(triggered(bool)), SLOT(interpolate()));
    addAction("frames-adding-normal", t);
    
    t = new QAction(SmallIcon("fork"), i18n("Create loop"), this);
    connect(t, SIGNAL(triggered(bool)), SLOT(createLoop()));
    addAction("frames-adding-control", t);
    
    t = new QAction(SmallIcon("edit-clear"), i18n("Clear frame"), this);
    connect(t, SIGNAL(triggered(bool)), SLOT(clearFrame()));
    addAction("frames-edit-one", t);
    
    t = new QAction(SmallIcon("edit-copy"), i18n("Copy previous frame"), this);
    connect(t, SIGNAL(triggered(bool)), SLOT(copyPrevious()));
    addAction("frames-edit-one", t);
    
    t = new QAction(SmallIcon("edit-copy"), i18n("Copy next frame"), this);
    connect(t, SIGNAL(triggered(bool)), SLOT(copyNext()));
    addAction("frames-edit-one", t);
    
    t = new QAction(SmallIcon("go-previous"), i18n("Move frame left"), this);
    connect(t, SIGNAL(triggered(bool)), SLOT(moveLeft()));
    addAction("frames-editing", t);
    
    t = new QAction(SmallIcon("go-next"), i18n("Move frame right"), this);
    connect(t, SIGNAL(triggered(bool)), SLOT(moveRight()));
    addAction("frames-editing", t);
    
    t = new QAction(SmallIcon("edit-table-insert-column-left"), i18n("Insert frame"), this);
    connect(t, SIGNAL(triggered(bool)), SLOT(insertFrame()));
    addAction("frames-editing", t);
    
    t = new QAction(SmallIcon("edit-table-delete-column"), i18n("Remove frame"), this);
    connect(t, SIGNAL(triggered(bool)), SLOT(removeFrame()));
    addAction("frames-editing", t);
    
    // LIGHT TABLE
    t = new QAction(SmallIcon("document-properties"), i18n("Enable/disable light table (see additional docker)"), this);
    t->setCheckable(true);
    t->setChecked(false);
    connect(t, SIGNAL(triggered(bool)), SLOT(enableLT(bool)));
    addAction("light-table", t);
}

void AnimatorActions::setupFrameActions(AnimatedLayer* layer)
{
    if (qobject_cast<NormalAnimatedLayer*>(layer))
        m_frameActionsType = "frames-adding-normal";
    else if (qobject_cast<ControlAnimatedLayer*>(layer))
        m_frameActionsType = "frames-adding-control";
    else
        m_frameActionsType = "none";
    
    emit frameActionsChanges();
}


#if !LOAD_ON_START
void AnimatorActions::loadLayers()
{
    Q_ASSERT(m_manager);
    m_manager->initLayers();
}
#endif

void AnimatorActions::makeAnimated()
{
    Q_ASSERT(m_manager);
    m_manager->setKraMetaInfo();
}

void AnimatorActions::importFrames()
{
    Q_ASSERT(m_manager);
    m_manager->getImporter()->importUser();
}

void AnimatorActions::exportFrames()
{
    Q_ASSERT(m_manager);
    m_manager->getExporter()->exportAll();
}


void AnimatorActions::playPause(bool v)
{
    Q_ASSERT(m_manager);
    if (v)
        m_manager->getPlayer()->play();
    else
        m_manager->getPlayer()->pause();
}

void AnimatorActions::toggleLooping(bool v)
{
    Q_ASSERT(m_manager);
    m_manager->getPlayer()->setLooped(v);
}


void AnimatorActions::createNormalLayer()
{
    Q_ASSERT(m_manager);
    m_manager->createNormalLayer();
}

void AnimatorActions::createControlLayer()
{
    Q_ASSERT(m_manager);
    m_manager->createControlLayer();
}

void AnimatorActions::convertToViewLayer()
{
    Q_ASSERT(m_manager);
    
    KDialog* setLoopDialog = new KDialog();
    setLoopDialog->setModal(true);
    setLoopDialog->setAttribute(Qt::WA_DeleteOnClose);
    setLoopDialog->setButtons(KDialog::Ok | KDialog::Cancel);
    setLoopDialog->setCaption(i18n("Create loop"));
    
    QWidget* mainWidget = new QWidget(setLoopDialog);
    QVBoxLayout* layout = new QVBoxLayout(mainWidget);
    QLabel* label = new QLabel(i18n("Set start and end of new view layer"), mainWidget);
    QHBoxLayout* spinLayout = new QHBoxLayout(mainWidget);
    QSpinBox* fromSpin = new QSpinBox(mainWidget);
    QSpinBox* toSpin = new QSpinBox(mainWidget);
    
    spinLayout->addWidget(fromSpin);
    spinLayout->addWidget(toSpin);
    layout->addWidget(label);
    layout->addLayout(spinLayout);
    
    mainWidget->setLayout(layout);
    
    connect(fromSpin, SIGNAL(valueChanged(int)), SLOT(setConvertFrom(int)));
    connect(toSpin, SIGNAL(valueChanged(int)), SLOT(setConvertTo(int)));
    
    fromSpin->setRange(0, 0xffff);
    fromSpin->setValue(0);
    setConvertFrom(0);
    toSpin->setRange(0, 0xffff);
    toSpin->setValue(0);
    setConvertTo(0);
    
    connect(setLoopDialog, SIGNAL(accepted()), SLOT(doConvertToViewLayer()));
    
    setLoopDialog->setMainWidget(mainWidget);
    setLoopDialog->show();
}

void AnimatorActions::doConvertToViewLayer()
{
    Q_ASSERT(m_manager);
    
    m_manager->convertToViewLayer(m_convertFrom, m_convertTo);
}

void AnimatorActions::setConvertFrom(int from)
{
    m_convertFrom = from;
}

void AnimatorActions::setConvertTo(int to)
{
    m_convertTo = to;
}


void AnimatorActions::removeLayer()
{
    Q_ASSERT(m_manager);
    m_manager->removeLayer();
}

void AnimatorActions::renameLayer()
{
    Q_ASSERT(m_manager);
    KisNode* layer = m_manager->activeLayer();
    QString name;
    
    AnimatedLayer* alayer = qobject_cast<AnimatedLayer*>(layer);
    if (alayer)
        name = alayer->aName();
    else
        name = layer->name();
    
    KDialog* renameDialog = new KDialog();
    renameDialog->setModal(true);
    renameDialog->setAttribute(Qt::WA_DeleteOnClose);
    renameDialog->setButtons(KDialog::Ok | KDialog::Cancel);
    renameDialog->setCaption(i18n("Rename layer"));
    
    QWidget* main_widget = new QWidget(renameDialog);
    QVBoxLayout* layout = new QVBoxLayout(main_widget);
    QLabel* label = new QLabel(i18n("Rename layer"), main_widget);
    KLineEdit* ledit = new KLineEdit(main_widget);
    ledit->setClearButtonShown(true);
    
    setRenameString(name);
    ledit->setText(name);
    ledit->setFocus();
    
    layout->addWidget(label);
    layout->addWidget(ledit);
    
    connect(ledit, SIGNAL(textChanged(QString)), this, SLOT(setRenameString(QString)));
    
    renameDialog->setMainWidget(main_widget);
    
    connect(renameDialog, SIGNAL(accepted()), this, SLOT(doRenameLayer()));
    
    renameDialog->show();
}

void AnimatorActions::setRenameString(const QString& string)
{
    m_renameString = string;
}

void AnimatorActions::doRenameLayer()
{
    m_manager->renameLayer(m_renameString);
}


void AnimatorActions::calculateLayer()
{
    Q_ASSERT(m_manager);
    m_manager->calculateActiveLayer();
}


void AnimatorActions::createPaintFrame()
{
    Q_ASSERT(m_manager);
    m_manager->createFrame("KisPaintLayer");
}

void AnimatorActions::createShapeFrame()
{
    Q_ASSERT(m_manager);
    m_manager->createFrame("KisShapeLayer");
}

void AnimatorActions::createGroupFrame()
{
    Q_ASSERT(m_manager);
    m_manager->createFrame("KisGroupLayer");
}

void AnimatorActions::interpolate()
{
    Q_ASSERT(m_manager);
    m_manager->interpolate();
}


void AnimatorActions::createLoop()
{
    Q_ASSERT(m_manager);
    
    KDialog* setLoopDialog = new KDialog();
    setLoopDialog->setModal(true);
    setLoopDialog->setAttribute(Qt::WA_DeleteOnClose);
    setLoopDialog->setButtons(KDialog::Ok | KDialog::Cancel);
    setLoopDialog->setCaption(i18n("Create loop"));
    
    QWidget* mainWidget = new QWidget(setLoopDialog);
    QVBoxLayout* layout = new QVBoxLayout(mainWidget);
    QLabel* label = new QLabel(i18n("Set loop begining and repeatition number"), mainWidget);
    QHBoxLayout* spinLayout = new QHBoxLayout(mainWidget);
    QSpinBox* targetSpin = new QSpinBox(mainWidget);
    QSpinBox* repeatSpin = new QSpinBox(mainWidget);
    
    spinLayout->addWidget(targetSpin);
    spinLayout->addWidget(repeatSpin);
    layout->addWidget(label);
    layout->addLayout(spinLayout);
    
    mainWidget->setLayout(layout);
    
    connect(targetSpin, SIGNAL(valueChanged(int)), SLOT(setLoopTarget(int)));
    connect(repeatSpin, SIGNAL(valueChanged(int)), SLOT(setLoopRepeat(int)));
    
    targetSpin->setRange(0, 0xffff);
    targetSpin->setValue(0);
    setLoopTarget(0);
    repeatSpin->setRange(-1, 0xff);
    repeatSpin->setValue(-1);
    setLoopRepeat(-1);
    
    connect(setLoopDialog, SIGNAL(accepted()), SLOT(doCreateLoop()));
    
    setLoopDialog->setMainWidget(mainWidget);
    setLoopDialog->show();
}

void AnimatorActions::doCreateLoop()
{
    Q_ASSERT(m_manager);
    
    m_manager->createLoopFrame(m_loopTarget, m_loopRepeat);
}

void AnimatorActions::setLoopRepeat(int repeat)
{
    m_loopRepeat = repeat;
}

void AnimatorActions::setLoopTarget(int target)
{
    m_loopTarget = target;
}


void AnimatorActions::clearFrame()
{
    Q_ASSERT(m_manager);
    m_manager->getFrameManager()->clearRangeActive(1);
}

void AnimatorActions::copyPrevious()
{
    Q_ASSERT(m_manager);
    m_manager->getFrameManager()->copyPreviousKey();
}

void AnimatorActions::copyNext()
{
    Q_ASSERT(m_manager);
    m_manager->getFrameManager()->copyNextKey();
}

void AnimatorActions::moveLeft()
{
    Q_ASSERT(m_manager);
    m_manager->getFrameManager()->moveRangeActive(1, -1);
}

void AnimatorActions::moveRight()
{
    Q_ASSERT(m_manager);
    m_manager->getFrameManager()->moveRangeActive(1, +1);
}

void AnimatorActions::insertFrame()
{
    Q_ASSERT(m_manager);
    m_manager->getFrameManager()->insertRange(1);
}

void AnimatorActions::removeFrame()
{
    Q_ASSERT(m_manager);
    m_manager->getFrameManager()->removeRange(1);
}


void AnimatorActions::setFps(int number)
{
    Q_ASSERT(m_manager);
    m_manager->getPlayer()->setFps(number);
}


void AnimatorActions::enableLT(bool v)
{
    Q_ASSERT(m_manager);
    AnimatorLTUpdater* updater = qobject_cast<AnimatorLTUpdater*>(m_manager->getUpdater());
    if (updater)
        updater->setMode(v ? AnimatorLTUpdater::Normal : AnimatorLTUpdater::Disabled);
}
