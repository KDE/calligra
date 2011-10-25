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

#ifndef ANIMATOR_ACTIONS_H
#define ANIMATOR_ACTIONS_H

#include <QObject>
#include <QList>
#include <QAction>

#include "animator_manager.h"

class AnimatorActions : public QObject
{
    Q_OBJECT

public:
    AnimatorActions(QObject* parent = 0);
    virtual ~AnimatorActions();
    
public:
    virtual void setManager(AnimatorManager* manager);
    
public:
    virtual QList<QAction*> actions() const;
    
protected:
    virtual void addAction(QAction* action);
    virtual void initActions();
    
protected slots:
    void loadLayers();
    
private:
    QList<QAction*> m_actions;
    AnimatorManager* m_manager;
};

#endif // ANIMATOR_ACTIONS_H
