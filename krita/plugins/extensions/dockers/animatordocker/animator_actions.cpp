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

#include "animator_manager.h"
#include "animator_loader.h"
#include "animator_player.h"
#include "animator_updater.h"

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
    t = new QAction(SmallIcon("system-run"), i18n("Load layers"), this);
    connect(t, SIGNAL(triggered(bool)), SLOT(loadLayers()));
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
}


void AnimatorActions::loadLayers()
{
    Q_ASSERT(m_manager);
    m_manager->getLoader()->loadAll();
    m_manager->getUpdater()->fullUpdate();
    m_manager->getSwitcher()->goFrame(0);
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
