/*
 *  Control docker
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

#include "animator_control_dock.h"

#include <KLocale>

#include <QToolBar>
#include <QLayout>
#include <QSpinBox>

#include <kis_canvas2.h>

#include "animator_manager_factory.h"
#include "animator_loader.h"

AnimatorControlDock::AnimatorControlDock() : QDockWidget(i18n("Animator control"))
{
    m_actions = new AnimatorActions(this);
    setupUI();
}

void AnimatorControlDock::setCanvas(KoCanvasBase* canvas)
{
    KisCanvas2* kcanvas = dynamic_cast<KisCanvas2*>(canvas);
    KisImageSP image = kcanvas->image();
    m_manager = AnimatorManagerFactory::instance()->getManager(image.data(), kcanvas);
    m_actions->setManager(m_manager);
    connect(m_actions, SIGNAL(frameActionsChanges()), SLOT(setupFramesToolbar()));
}

void AnimatorControlDock::unsetCanvas()
{

}

void AnimatorControlDock::setupUI()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    
    QToolBar* tbUtil = new QToolBar(this);
    tbUtil->addActions(m_actions->actions("util"));
    layout->addWidget(tbUtil);
    
    QToolBar* tbLayers = new QToolBar(this);
    tbLayers->addActions(m_actions->actions("layers"));
    layout->addWidget(tbLayers);
    
    m_tbFrames = new QToolBar(this);
    setupFramesToolbar();
    layout->addWidget(m_tbFrames);
    
    QToolBar* tbPlayer = new QToolBar(this);
    tbPlayer->addActions(m_actions->actions("player"));
    QSpinBox* fpsSpin = new QSpinBox(tbPlayer);
    fpsSpin->setPrefix(i18n("fps: "));
    fpsSpin->setValue(12);
    connect(fpsSpin, SIGNAL(valueChanged(int)), m_actions, SLOT(setFps(int)));
    tbPlayer->addWidget(fpsSpin);
    
    layout->addWidget(tbPlayer);
    
    QToolBar* tbLT = new QToolBar(this);
    tbLT->addActions(m_actions->actions("light-table"));
    layout->addWidget(tbLT);
    
    layout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    
    QWidget* mainWidget = new QWidget(this);
    mainWidget->setLayout(layout);
    setWidget(mainWidget);
}

void AnimatorControlDock::setupFramesToolbar()
{
    m_tbFrames->clear();
    m_tbFrames->addActions(m_actions->actions("frames-adding"));
    m_tbFrames->addSeparator();
    m_tbFrames->addActions(m_actions->actions("frames-editing"));
}
