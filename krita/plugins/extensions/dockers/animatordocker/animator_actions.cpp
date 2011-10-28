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

#include "animator_manager.h"
#include "animator_loader.h"
#include "animator_player.h"
#include "animator_exporter.h"
#include "animator_updater.h"
#include "animator_lt_updater.h"

#include "animator_config.h"

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
    t = new QAction(SmallIcon("list-add"), i18n("Add layer"), this);
    connect(t, SIGNAL(triggered(bool)), SLOT(createNormalLayer()));
    addAction("layers", t);
    
    t = new QAction(SmallIcon("list-remove"), i18n("Remove layer"), this);
    connect(t, SIGNAL(triggered(bool)), SLOT(removeLayer()));
    addAction("layers", t);
    
    t = new QAction(SmallIcon("edit-rename"), i18n("Rename layer"), this);
    connect(t, SIGNAL(triggered(bool)), SLOT(renameLayer()));
    addAction("layers", t);
    
    // FRAMES
    t = new QAction(SmallIcon("edit-clear"), i18n("Clear frame"), this);
    connect(t, SIGNAL(triggered(bool)), SLOT(clearFrame()));
    addAction("frames", t);
    
    t = new QAction(SmallIcon("document-new"), i18n("Create paint frame"), this);
    connect(t, SIGNAL(triggered(bool)), SLOT(createPaintFrame()));
    addAction("frames", t);
    
    t = new QAction(SmallIcon("bookmark-new"), i18n("Create shape frame"), this);
    connect(t, SIGNAL(triggered(bool)), SLOT(createShapeFrame()));
    addAction("frames", t);
    
    t = new QAction(SmallIcon("folder-new"), i18n("Create group frame"), this);
    connect(t, SIGNAL(triggered(bool)), SLOT(createGroupFrame()));
    addAction("frames", t);
    
    // LIGHT TABLE
    t = new QAction(SmallIcon("document-properties"), i18n("Enable/disable light table (see additional docker)"), this);
    t->setCheckable(true);
    t->setChecked(false);
    connect(t, SIGNAL(triggered(bool)), SLOT(enableLT(bool)));
    addAction("lighttable", t);
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
        name = alayer->aname();
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


void AnimatorActions::clearFrame()
{
    Q_ASSERT(m_manager);
    m_manager->removeFrame();
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
