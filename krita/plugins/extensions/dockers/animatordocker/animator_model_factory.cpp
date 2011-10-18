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

#include "animator_model_factory.h"

#include <KGlobal>

#include <iostream>

AnimatorModelFactory::AnimatorModelFactory()
{
    m_instances.clear();
}

AnimatorModelFactory::~AnimatorModelFactory()
{
}

AnimatorModelFactory* AnimatorModelFactory::instance()
{
    K_GLOBAL_STATIC(AnimatorModelFactory, s_instance)
    if (!s_instance.exists()) {
        s_instance->init();
    }
    return s_instance;
}

AnimatorModel* AnimatorModelFactory::getModel(KisImage* image)
{
    if (! m_instances[image])
    {
        m_instances[image] = new AnimatorModel(image);
    }
    
    return m_instances[image];
    
}

void AnimatorModelFactory::init()
{
}
