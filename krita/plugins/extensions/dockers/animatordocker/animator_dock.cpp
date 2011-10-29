/*
 *  
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

#include <KLocale>

#include <QBoxLayout>
#include <qlayoutitem.h>
#include <QSlider>

#include <KoCanvasBase.h>

#include <kis_canvas2.h>
#include <kis_doc2.h>
#include <kis_image.h>
#include <kis_node_manager.h>
#include <kis_view2.h>

#include "animator_dock.h"
#include "animator_model.h"
#include "animator_manager_factory.h"
#include "animator_view.h"
#include "animator_switcher.h"

AnimatorDock::AnimatorDock( ) : QDockWidget(i18n("Animator"))
{
    m_mainModel = 0;
    
    setupUI();
}

void AnimatorDock::setupUI()
{
    QWidget* mainWidget = new QWidget(this);
    QVBoxLayout* mLayout = new QVBoxLayout(mainWidget);
    
    m_view = new AnimatorView;
    mLayout->addWidget(m_view);
    
    QHBoxLayout* downLayout = new QHBoxLayout(mainWidget);
    
    QSlider* columnWidthSlider = new QSlider(Qt::Horizontal, mainWidget);
    columnWidthSlider->setRange(8, 64);
    columnWidthSlider->setValue(10);
    columnWidthSlider->setPageStep(8);
    columnWidthSlider->setMaximumWidth(128);
    connect(columnWidthSlider, SIGNAL(valueChanged(int)), SLOT(setFrameWidth(int)));
    
    downLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding));
    downLayout->addWidget(columnWidthSlider);
    
    mLayout->addLayout(downLayout);
    
    mainWidget->setLayout(mLayout);
    
    setWidget(mainWidget);
}

void AnimatorDock::setCanvas(KoCanvasBase* canvas)
{
    KisCanvas2* kcanvas = dynamic_cast<KisCanvas2*>(canvas);
    KisImage* image = kcanvas->image().data();
    
    m_manager = AnimatorManagerFactory::instance()->getManager(image, kcanvas);
    
    m_mainModel = new AnimatorModel(image);
    m_mainModel->setFrameWidth(10);
    connect(m_manager->getSwitcher(), SIGNAL(frameChanged(int,int)), m_mainModel, SLOT(dataChangedSlot(int,int)));
    
    m_view->setModel(m_mainModel);
}

void AnimatorDock::unsetCanvas()
{
    delete m_mainModel;
    m_mainModel = 0;
}


void AnimatorDock::setFrameWidth(int width)
{
    if (m_mainModel)
    {
        m_mainModel->setFrameWidth(width);
        m_view->resizeColumnsToContent();
    }
}

#include "animator_dock.moc"
