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
    virtual QList<QAction*> actions(const QString& category) const;
    
protected:
    virtual void addAction(const QString& category, QAction* action);
    virtual void initActions();
    
protected slots:
    virtual void loadLayers();
    virtual void exportFrames();
    
    virtual void playPause(bool v);
    virtual void toggleLooping(bool v);
    
    virtual void createNormalLayer();
    virtual void removeLayer();
    
    virtual void renameLayer();
    virtual void doRenameLayer();
    virtual void setRenameString(const QString& string);
    
    virtual void clearFrame();
    virtual void createPaintFrame();
    
    virtual void enableLT(bool v);
    
public slots:
    virtual void setFps(int number);
    
private:
    QMap< QString, QList<QAction*> > m_actions;
    AnimatorManager* m_manager;
    
private:
    QString m_renameString;
    
};

#endif // ANIMATOR_ACTIONS_H
