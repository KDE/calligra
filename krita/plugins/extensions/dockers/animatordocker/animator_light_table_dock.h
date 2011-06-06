/*
 *  Light table docker
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


#ifndef ANIMATOR_LIGHT_TABLE_DOCK_H
#define ANIMATOR_LIGHT_TABLE_DOCK_H

#include <QDockWidget>
#include <QSlider>
#include <QBoxLayout>
#include <QLayoutItem>
#include <QSpinBox>

#include <KoCanvasObserverBase.h>

#include "animator_model.h"
#include "ltslider.h"

class AnimatorLightTableDock : public QDockWidget //, public KoCanvasObserverBase
{
    Q_OBJECT

public:
    AnimatorLightTableDock();
    virtual ~AnimatorLightTableDock();
    
public:
    void setModel(AnimatorModel* model);
    void setLightTable(AnimatorLightTable* table);
    
public slots:
    void slidersUpdate();
    
private:
    AnimatorModel* m_model;
    AnimatorLightTable* m_ltable;
    
private:        // GUI
    QVBoxLayout* m_layout;
    QHBoxLayout* m_sliders_layout;
    QList<LTSlider*> m_sliders;
    QSpinBox* m_near_spinbox;
    
    // Spacers
    QSpacerItem* m_sp_l;
    QSpacerItem* m_sp_r;
};

#endif // ANIMATOR_LIGHT_TABLE_DOCK_H
