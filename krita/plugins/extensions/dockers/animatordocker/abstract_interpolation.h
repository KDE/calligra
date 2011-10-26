/*
 *  Abstract class for interpolation methods
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


#ifndef ABSTRACT_INTERPOLATION_H
#define ABSTRACT_INTERPOLATION_H

#include "kis_node.h"
#include "kis_clone_layer.h"

class AbstractInterpolation
{
public:
    AbstractInterpolation(){}
    virtual ~AbstractInterpolation(){}
    
public:
    virtual KisCloneLayer* makeLayer(KisNode* from, KisCloneLayer* to, double position);
    virtual void changeLayer(KisCloneLayer* layer, KisNode* from, KisCloneLayer* to, double position) = 0;
};

#endif // ABSTRACT_INTERPOLATION_H
