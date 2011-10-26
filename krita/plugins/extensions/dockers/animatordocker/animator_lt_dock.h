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


#ifndef ANIMATOR_LT_DOCK_H
#define ANIMATOR_LT_DOCK_H

#include <QDockWidget>
#include <QSlider>
#include <QBoxLayout>
#include <QLayoutItem>
#include <QSpinBox>

#include <KoCanvasObserverBase.h>

#include "animator_lt.h"
#include "lt_slider.h"

class AnimatorLTDock : public QDockWidget //, public KoCanvasObserverBase
{
    Q_OBJECT

public:
    AnimatorLTDock();
    virtual ~AnimatorLTDock();
    
public:
//     void setLT(AnimatorLT* table);
    
public slots:
    void slidersUpdate();
    
private:
    AnimatorLT* m_ltable;
    
private:        // GUI
    QVBoxLayout* m_layout;
    QHBoxLayout* m_slidersLayout;
    QList<LTSlider*> m_sliders;
    QSpinBox* m_nearSpinbox;
    
    // Spacers
    QSpacerItem* m_spL;
    QSpacerItem* m_spR;
};

#endif // ANIMATOR_LT_DOCK_H
