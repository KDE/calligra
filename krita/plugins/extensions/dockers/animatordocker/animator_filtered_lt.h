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

#ifndef ANIMATOR_FILTERED_LT_H
#define ANIMATOR_FILTERED_LT_H

#include "animator_lt.h"
#include <kis_types.h>
#include <kis_image.h>


class AnimatorFilteredLT : public AnimatorLT
{
    Q_OBJECT
    
public:
    AnimatorFilteredLT(KisImage *image);
    virtual ~AnimatorFilteredLT();
    
public:
    virtual KisAdjustmentLayerSP filter(int relFrame);
    
    virtual void setBasicFilter(KisAdjustmentLayerSP layer);
    virtual void setFilterUsed(int relFrame, bool used);
    
private:
    KisAdjustmentLayerSP m_leftFilter;
    KisAdjustmentLayerSP m_rightFilter;
   
    bool m_usedL;
    bool m_usedR;
    
    KisAdjustmentLayerSP m_basicFilter;
    
    KisImage *m_image;
};

#endif // ANIMATOR_FILTERED_LT_H
