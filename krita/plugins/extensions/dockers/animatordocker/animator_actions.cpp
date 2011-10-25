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


void AnimatorActions::addAction(QAction* action)
{
    m_actions.append(action);
}

QList< QAction* > AnimatorActions::actions() const
{
    return m_actions;
}

void AnimatorActions::initActions()
{
    QAction* t;
    
    t = new QAction(SmallIcon("system-run"), i18n("Load layers"), this);
    connect(t, SIGNAL(triggered(bool)), this, SLOT(loadLayers()));
    addAction(t);
}


void AnimatorActions::loadLayers()
{
    Q_ASSERT(m_manager);
    m_manager->getLoader()->loadAll();
}
