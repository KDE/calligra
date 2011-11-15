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

#include "animator_filtered_lt.h"

#include <kis_adjustment_layer.h>
#include <kis_filter_configuration.h>
#include <kis_selection.h>

AnimatorFilteredLT::AnimatorFilteredLT(KisImage *image) : AnimatorLT()
{
    m_leftFilter = 0;
    m_rightFilter = 0;
    
    m_basicFilter = 0;
    m_usedL = false;
    m_usedR = false;
    
    m_image = image;
    setBasicFilter(new KisAdjustmentLayer(image, "", new KisFilterConfiguration("hsvadjustment", 0), 0));
}

AnimatorFilteredLT::~AnimatorFilteredLT()
{
}


KisAdjustmentLayerSP AnimatorFilteredLT::filter(int relFrame)
{
    if (!m_basicFilter) {
        warnKrita << "cannot use filter";
        return 0;
    }
    
    if (relFrame == 0)
        return 0;
    
    if (relFrame < 0 && !m_usedL)
        return 0;
    
    if (relFrame > 0 && !m_usedR)
        return 0;
    
    KisAdjustmentLayerSP result = relFrame<0 ? m_leftFilter : m_rightFilter;
    if (!result) {
        result = new KisAdjustmentLayer(*m_basicFilter);
        if (relFrame < 0) {
            m_leftFilter = result;
        } else {
            m_rightFilter = result;
        }
    }
    return result;
}


void AnimatorFilteredLT::setBasicFilter(KisAdjustmentLayerSP layer)
{
    m_basicFilter = layer;
}

void AnimatorFilteredLT::setFilterUsed(int relFrame, bool used)
{
    if (relFrame < 0)
        m_usedL = used;
    
    if (relFrame > 0)
        m_usedR = used;
}

void AnimatorFilteredLT::update()
{
    emit fullUpdate();
}
