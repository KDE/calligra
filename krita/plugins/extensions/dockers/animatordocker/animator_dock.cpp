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

#include <QWidget>
#include <QSpinBox>

#include <KLocale>
#include <KIcon>
#include <KIconLoader>

#include <KoCanvasBase.h>

#include <kis_canvas2.h>
#include <kis_doc2.h>
#include <kis_image.h>
#include <kis_node_manager.h>
#include <kis_view2.h>

#include "animator_dock.h"
#include "animator_model.h"
#include "animator_view.h"

#include <iostream>
#include "animator_light_table_dock.h"

AnimatorDock::AnimatorDock( ) : QDockWidget(i18n("Animator"))
{
    // Setup mode/view
    m_view = new AnimatorView;
    m_model = new AnimatorModel;
    m_light_table = new AnimatorLightTable;
    
    m_model->setLightTable(m_light_table);

    m_document = 0;
    
    // Setup exporter
    m_exporter = new AnimatorExporter(m_model, m_document);
    
    // Setup player
    m_player = new AnimatorPlayer(m_model);
    
    // ----- UI
    
    // Layouts
    m_layout = new QHBoxLayout;
    m_acts_layout = new QVBoxLayout;
    m_lay_acts_layout = new QHBoxLayout;
    
    // Spinbox -- frame
    m_frame_spinbox = new QSpinBox;
    m_frame_spinbox->setPrefix(i18n("frame: "));
    m_frame_spinbox->setMaximum(9999);                                  // MAXIMUM FRAMES NUMBER -- FIXME: move the constant

    connect(m_frame_spinbox, SIGNAL(editingFinished()), this, SLOT(slotSetFrame()));
    connect(this, SIGNAL(setFrame(int)), m_model, SLOT(setFrame(int)));
    connect(m_model, SIGNAL(frameChanged(int)), m_frame_spinbox, SLOT(setValue(int)));
    
    // Spinbox -- frames number
    m_frame_number_spinbox = new QSpinBox;
    m_frame_number_spinbox->setPrefix(i18n("frames number: "));
    m_frame_number_spinbox->setMaximum(9999);                           // MAXIMUM FRAMES NUMBER

    connect(m_frame_number_spinbox, SIGNAL(editingFinished()), this, SLOT(slotSetFNumber()));
    connect(this, SIGNAL(setFNumber(int)), m_model, SLOT(setFramesNumber(int)));
    connect(m_model, SIGNAL(framesNumberChanged(int)), m_frame_number_spinbox, SLOT(setValue(int)));
    
    // Spinbox -- fps
    m_fps_spinbox = new QSpinBox;
    m_fps_spinbox->setRange(1, 100);            // FIXME: move the constant
    m_fps_spinbox->setPrefix(i18n("fps: "));
    connect(m_fps_spinbox, SIGNAL(valueChanged(int)), m_player, SLOT(setFps(int)));
    m_fps_spinbox->setValue(12);
    
    // Export all button
    m_export_button = new QPushButton(KIcon("document-export"), i18n("Export all frames"), this);
    connect(m_export_button, SIGNAL(clicked(bool)), m_exporter, SLOT(exportAll()));
    
    // Layers toolbar
    m_lays_toolbar = new QToolBar(this);
    
    addAction( m_lays_toolbar->addAction(SmallIcon("list-add"), i18n("Add layer"), m_model, SLOT(createLayer())) );
    addAction( m_lays_toolbar->addAction(SmallIcon("list-remove"), i18n("Delete layer"), m_model, SLOT(deleteLayer())) );
    addAction( m_lays_toolbar->addAction(SmallIcon("go-down"), i18n("Move layer down"), m_model, SLOT(layerDown())) );
    addAction( m_lays_toolbar->addAction(SmallIcon("go-up"), i18n("Move layer up"), m_model, SLOT(layerUp())) );
    
    // Frames toolbar
    QToolBar* m_frames_toolbar = new QToolBar(this);
    
    addAction( m_frames_toolbar->addAction(SmallIcon("go-previous"), i18n("Move frame left"), m_model, SLOT(frameLeft())) );
    addAction( m_frames_toolbar->addAction(SmallIcon("go-next"), i18n("Move frame right"), m_model, SLOT(frameRight())) );
    
    // Onion toolbar
    m_onion_toolbar = new QToolBar(this);
    
    QAction* onion_prev = m_onion_toolbar->addAction(SmallIcon("go-previous-view-page"), i18n("Onion previous"));
    onion_prev->setCheckable(true);
    connect(onion_prev, SIGNAL(toggled(bool)), this, SLOT(enableOnionPrevious(bool)));
    addAction( onion_prev );
    
    QAction* onion_next = m_onion_toolbar->addAction(SmallIcon("go-next-view-page"), i18n("Onion next"));
    onion_next->setCheckable(true);
    connect(onion_next, SIGNAL(toggled(bool)), this, SLOT(enableOnionNext(bool)));
    addAction( onion_next );
    
    QAction* ext_light_table = m_onion_toolbar->addAction(SmallIcon("document-edit"), i18n("Extended lighttable (in additional docker)"));
    ext_light_table->setCheckable(true);
    connect(ext_light_table, SIGNAL(toggled(bool)), m_model, SLOT(toogleExtLTable(bool)));
    addAction( ext_light_table );
    
    QAction* show_all = m_onion_toolbar->addAction(SmallIcon("layer-visible-on"), i18n("Show all frames (recommended for shape layers before exit)"));
    show_all->setCheckable(true);
    connect(show_all, SIGNAL(toggled(bool)), m_model, SLOT(setEnabled(bool)));
    addAction( show_all );
    
    // Player toolbar
    m_player_toolbar = new QToolBar(this);
    
    QAction* play_act = m_player_toolbar->addAction(SmallIcon("media-playback-start"), i18n("Play/Pause"));
    play_act->setCheckable(true);
    connect(play_act, SIGNAL(toggled(bool)), m_player, SLOT(play(bool)));
    addAction( play_act );
    
//     // Create new button
//     m_new_button = new QToolButton(this);
//     
//     m_new_button->addAction();
//     
//     m_del_button = new QToolButton(this);
    
    // Add widgets to layout
//     m_lay_acts_layout->addWidget(m_new_button);
//     m_lay_acts_layout->addWidget(m_del_button);
    
    m_acts_layout->addWidget(m_export_button);
    
    m_acts_layout->addWidget(m_frame_spinbox);
    m_acts_layout->addWidget(m_frame_number_spinbox);
    m_acts_layout->addWidget(m_fps_spinbox);
    
//     m_acts_layout->addLayout(m_lay_acts_layout);
    m_acts_layout->addWidget(m_lays_toolbar);
    m_acts_layout->addWidget(m_frames_toolbar);
    m_acts_layout->addWidget(m_onion_toolbar);
    m_acts_layout->addWidget(m_player_toolbar);

    
    m_layout->addWidget(m_view);
    m_layout->addLayout(m_acts_layout);
    
    // Setup widget
    QWidget* main_widget = new QWidget(this);
    main_widget->setLayout(m_layout);
    setWidget(main_widget);
}

void AnimatorDock::setCanvas(KoCanvasBase* canvas)
{
//     std::cout << "CANVAS CHANGED" << std::endl;
    
    m_canvas = static_cast<KisCanvas2*>(canvas);
    
    KisView2* view = m_canvas->view();
    
    m_nodemodel = new KisNodeModel(this);
    m_nodemodel->setImage(m_canvas->image());
    
    if (! m_nodemodel )
    {
        // Error
        std::cout << "ERROR" << std::endl;
    }
    m_model->setSourceModel(m_nodemodel);
    m_model->setNodeManager(view->nodeManager());
    m_model->setImage(m_canvas->image());
    m_model->setCanvas(m_canvas);
    
    m_view->setModel(m_model);
    
//     static bool t_DELETE_this_variable = false;
//     if (t_DELETE_this_variable)
//         m_model->updateCanvas();
//     else
//         t_DELETE_this_variable = true;
    
    connect(m_canvas, SIGNAL(imageChanged(KisImageWSP)), m_model, SLOT(setImage(KisImageWSP)));
    

    connect(m_nodemodel, SIGNAL(rowsInserted(const QModelIndex&, int, int)), m_model, SLOT(update()));
    connect(m_nodemodel, SIGNAL(rowsRemoved(const QModelIndex&, int, int)), m_model, SLOT(update()));
    connect(m_nodemodel, SIGNAL(rowsMoved(const QModelIndex&, int, int, const QModelIndex&, int)), m_model, SLOT(update()));
    connect(m_nodemodel, SIGNAL(modelReset()), m_model, SLOT(update()));
    
//     m_model->setFrame(1);
    
    m_model->update();
    
    
    // Exporter
    m_document = view->document();
    m_exporter->setDocument(m_document);
    
    
    // Light table docker
    KoMainWindow* win = view->shell();
    QDockWidget* docker;
    foreach (docker, win->dockWidgets())
    {
//         std::cout << docker->metaObject()->className() << std::endl;
        if (docker->inherits("AnimatorLightTableDock"))
        {
//             std::cout << "FOUND" << std::endl;
            AnimatorLightTableDock* ltdock = dynamic_cast<AnimatorLightTableDock*>(docker);
            ltdock->setModel(m_model);
            ltdock->setLightTable(m_light_table);
//             break;
        }
    }
}

void AnimatorDock::unsetCanvas()
{
    // No code now, just to be compilable
}

void AnimatorDock::slotSetFNumber()
{
    int value = m_frame_number_spinbox->value();
    std::cout << "slotSetFNumber(" << value << ")" << std::endl;
    emit setFNumber(value);
}

void AnimatorDock::slotSetFrame()
{
    int value = m_frame_spinbox->value();
    std::cout << "slotSetFrame(" << value << ")" << std::endl;
    emit setFrame(value);
}


void AnimatorDock::enableOnionNext(bool ch)
{
    m_model->setOnionNext(ch?1:0);
}

void AnimatorDock::enableOnionPrevious(bool ch)
{
    m_model->setOnionPrevious(ch?1:0);
}



void AnimatorDock::testSlot(int n)
{
    std::cout << "Signal emmited " << n << std::endl;
}

#include "animator_dock.moc"
