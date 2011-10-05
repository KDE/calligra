/*
 *  Docker -- almost all gui is here
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


#ifndef _ANIMATOR_DOCK_H_
#define _ANIMATOR_DOCK_H_

#include <QDockWidget>

#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QToolButton>
#include <QToolBar>

#include "kis_view2.h"
#include "KoCanvasObserverBase.h"
#include "kis_node_model.h"
#include "KoMainWindow.h"

#include "animator_view.h"
#include "animator_model.h"
#include "animator_exporter.h"
#include "animator_player.h"

class AnimatorDock : public QDockWidget, public KoCanvasObserverBase
{
    Q_OBJECT
    
public:
    AnimatorDock();

public slots:
    virtual void setCanvas(KoCanvasBase* canvas);
    virtual void unsetCanvas();
    
    void slotSetFrame();
    void slotSetFNumber();
    
    void framesLeft();
    void framesRight();
    void framesMove(int move);

    void slotFrameInsert();
    void slotFrameDelete();

    void slotCreateLayer();
    
    void enableOnionNext(bool ch);
    void enableOnionPrevious(bool ch);
    
    void slotStatusChanged(bool ns);
    
    void testSlot(int n);
    
//     void exportAll();
    
signals:
    void setFrame(int f);
    void setFNumber(int n);
    
private:
    AnimatorView* m_view;
    AnimatorModel* m_model;
    AnimatorLightTable* m_light_table;
    
    KisNodeModel* m_nodemodel;
    
    KisCanvas2* m_canvas;
    
    KisDoc2* m_document;
    KoMainWindow* m_main_win;
    
    AnimatorExporter* m_exporter;
    AnimatorPlayer* m_player;
    
private:
    // UI elements except view
    QHBoxLayout* m_layout;
    QVBoxLayout* m_acts_layout;
    QHBoxLayout* m_lay_acts_layout;
    
    QSpinBox* m_frame_spinbox;
    QSpinBox* m_frame_number_spinbox;
    QSpinBox* m_fps_spinbox;            // Not used now
    
    QToolBar* m_lays_toolbar;
    QToolBar* m_onion_toolbar;
    
    QToolBar* m_player_toolbar;
    QToolBar* m_frames_toolbar;
};


#endif
