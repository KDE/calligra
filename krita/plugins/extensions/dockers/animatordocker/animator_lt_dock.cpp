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

#include <KLocale>

#include <KoDockRegistry.h>

#include "animator_lt_dock.h"
#include "animator_dock.h"

#include <cstdlib>

AnimatorLTDock::AnimatorLTDock() : QDockWidget(i18n("Light table"))
{
    m_ltable = new AnimatorLT();
    
    m_layout = new QVBoxLayout(this);
    QWidget* main_widget = new QWidget(this);
    main_widget->setLayout(m_layout);
    setWidget(main_widget);
    
    m_nearSpinbox = new QSpinBox(this);
    m_layout->addWidget(m_nearSpinbox);
    
    connect(m_nearSpinbox, SIGNAL(valueChanged(int)), m_ltable, SLOT(setNear(int)));
    connect(m_nearSpinbox, SIGNAL(valueChanged(int)), this, SLOT(slidersUpdate()));
    
    m_slidersLayout = new QHBoxLayout(this);
    
    m_spL = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_spR = new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_slidersLayout->addSpacerItem(m_spL);
    
    m_layout->addLayout(m_slidersLayout);
    
    slidersUpdate();
}

AnimatorLTDock::~AnimatorLTDock()
{

}

void AnimatorLTDock::slidersUpdate()
{
    if (m_ltable)
    {
        int n = m_ltable->getNear();
        int l = m_sliders.length();
//         std::cout << "Hello here" << n << l << std::endl;
        if (n*2 + 2 > l)
        {
            
            for (quint8 i = 0; i < 2*n-l+2; ++i)
            {
//                 std::cout << "jjjjjjj" << std::endl;
                m_sliders.append(new LTSlider(this));
//                 m_sliders.append(new QSlider(this));
            }
        }
        
        for (quint8 j = 0; j < l; ++j)
        {
            m_slidersLayout->removeWidget(m_sliders[j]);
            m_sliders[j]->hide();
            m_sliders[j]->disconnect();
        }
        
        m_slidersLayout->removeItem(m_spL);
        m_slidersLayout->removeItem(m_spR);
        
        m_slidersLayout->addSpacerItem(m_spL);
        
        for (int i = -m_ltable->getNear(); i <= m_ltable->getNear(); ++i)
        {
//             if (i != 0)
//             {
//                 std::cout << std::abs(i)*2+(i < 0) << std::endl;
	    m_slidersLayout->addWidget(m_sliders[std::abs(i)*2+(i < 0)]);
	    m_sliders[std::abs(i)*2+(i < 0)]->show();
	    m_sliders[std::abs(i)*2+(i < 0)]->setNumbner(i);
            m_sliders[std::abs(i)*2+(i < 0)]->setOpacity(m_ltable->getOpacity(i));
            m_sliders[std::abs(i)*2+(i < 0)]->setVisibility(m_ltable->getVisibility(i));
	    connect(m_sliders[std::abs(i)*2+(i < 0)], SIGNAL(opacityChanged(int,int)), m_ltable, SLOT(setOpacity(int,int)));
	    connect(m_sliders[std::abs(i)*2+(i < 0)], SIGNAL(visibilityChanged(int,bool)), m_ltable, SLOT(setVisibility(int,bool)));
//             }
        }
        
        m_slidersLayout->addSpacerItem(m_spR);
    }
}

// void AnimatorLTDock::setLT(AnimatorLT* table)
// {
//     m_ltable = table;
//     connect(m_nearSpinbox, SIGNAL(valueChanged(int)), m_ltable, SLOT(setNear(int)));
//     connect(m_ltable, SIGNAL(nearChanged(int)), this, SLOT(slidersUpdate()));
//     slidersUpdate();
// }
