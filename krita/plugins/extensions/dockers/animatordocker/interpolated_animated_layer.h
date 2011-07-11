/*
 *  Interpolated layer: allows several ways of interpolating
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


#ifndef INTERPOLATED_ANIMATED_LAYER_H
#define INTERPOLATED_ANIMATED_LAYER_H

#include <QSet>

#include "kis_clone_layer.h"
#include "kis_node.h"

#include "simple_animated_layer.h"

class InterpolatedAnimatedLayer : public SimpleAnimatedLayer
{
    Q_OBJECT
    
public:
    InterpolatedAnimatedLayer(const KisGroupLayer& source);
    
public:
    virtual void update();
//     virtual void frameUpdate();
    virtual KisNode* getKeyFrameLayer(int num);
    virtual KisNode* getFrameLayer(int num);
    
    virtual KisNode* getCachedFrame();
    
    virtual KisNode* setFrameNumber(int num);
    
protected:
    virtual KisCloneLayer* interpolate(KisNode* from, KisCloneLayer* to, double percent);
    virtual int getSCurrentKey(int num);
    
private:
    QSet<int> m_keys;
    QSet<int> m_non_keys;
    
    int m_to_cache;
    KisCloneLayer* m_cached_frame;
    
    bool m_gfl_works;
};

#endif // INTERPOLATED_ANIMATED_LAYER_H
